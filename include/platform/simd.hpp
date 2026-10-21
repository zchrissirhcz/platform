// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Zhuo Zhang

#pragma once

// Runtime SIMD feature detection for the machine the program is running on.
//
// This is deliberately NOT a header of constexpr flags: compile-time macros
// (__AVX2__, __ARM_NEON, ...) describe what the compiler is allowed to emit
// into this binary, which is a different question from what the current CPU
// supports.  Use the macros to decide what you may compile, and this header
// to decide which kernel to call at runtime.
//
//   #if defined(__AVX2__)
//       // this translation unit may contain AVX2 code
//   #endif
//
//   if (simd::cached().avx2) { /* call the AVX2 kernel */ }
//
// Detection must be built for the baseline ISA: a binary compiled with
// -mavx2 cannot run on a CPU without AVX2, so it can never report the truth.
//
// Covered:
//   x86 (SSE2..AVX-512F) on Windows, macOS, Linux, Android and OpenHarmony.
//   ARM NEON on those platforms (ARM64 on Windows), plus SVE/SVE2/dotprod/
//   FP16/I8MM/BF16 where the OS exposes them: HWCAP on Linux/Android/OHOS,
//   sysctl on macOS, IsProcessorFeaturePresent on Windows ARM64.
//
// AVX-512 is reported as AVX512F only; the BW/DQ/VL subsets are not exposed.
// SVE vector length is not reported: it is runtime-variable unless the
// compiler fixes it with -msve-vector-bits.
//
// Other architectures (RISC-V vector, LoongArch LASX, wasm SIMD) are not
// detected here.

#include <cstddef>
#include <cstdint>
#include <string>

#if defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64) || \
    defined(__i386__) || defined(_M_IX86)
#  define SIMD_DETAIL_X86 1
#else
#  define SIMD_DETAIL_X86 0
#endif

#if defined(__aarch64__) || defined(__arm64__) || \
    defined(_M_ARM64) || defined(__arm__) || defined(_M_ARM)
#  define SIMD_DETAIL_ARM 1
#else
#  define SIMD_DETAIL_ARM 0
#endif

#if SIMD_DETAIL_X86
#  if defined(_MSC_VER)
#    include <immintrin.h>
#    include <intrin.h>
#  else
#    include <cpuid.h>
#  endif
#endif

#if defined(_WIN32) && SIMD_DETAIL_ARM
#  include <windows.h>
#endif

#if defined(__APPLE__) && SIMD_DETAIL_ARM
#  include <sys/sysctl.h>
#endif

#if (defined(__linux__) || defined(__ANDROID__) || defined(__OHOS__)) && \
    SIMD_DETAIL_ARM
#  include <sys/auxv.h>
#endif

namespace simd {

struct features {
    // x86
    bool sse2 = false;
    bool sse3 = false;
    bool ssse3 = false;
    bool sse4_1 = false;
    bool sse4_2 = false;
    bool avx = false;
    bool avx2 = false;
    bool avx512f = false;

    // ARM
    bool neon = false;
    bool sve = false;
    bool sve2 = false;
    bool dotprod = false;
    bool fp16 = false;
    bool i8mm = false;
    bool bf16 = false;
};

namespace detail {

#if SIMD_DETAIL_X86

inline void cpuid(std::uint32_t leaf, std::uint32_t subleaf,
                  std::uint32_t out[4]) noexcept
{
#if defined(_MSC_VER)
    int regs[4];
    __cpuidex(regs, static_cast<int>(leaf), static_cast<int>(subleaf));
    for (int i = 0; i < 4; ++i)
        out[i] = static_cast<std::uint32_t>(regs[i]);
#else
    __get_cpuid_count(leaf, subleaf, &out[0], &out[1], &out[2], &out[3]);
#endif
}

// XCR0, only valid when CPUID.1:ECX.OSXSAVE is set.
inline std::uint64_t xgetbv0() noexcept
{
#if defined(_MSC_VER)
    return _xgetbv(0);
#else
    std::uint32_t eax = 0;
    std::uint32_t edx = 0;
    __asm__ __volatile__("xgetbv" : "=a"(eax), "=d"(edx) : "c"(0));
    return (static_cast<std::uint64_t>(edx) << 32) | eax;
#endif
}

inline void detect_x86(features& f) noexcept
{
    std::uint32_t regs[4] = {0, 0, 0, 0};
    cpuid(0, 0, regs);
    const std::uint32_t max_leaf = regs[0];

    if (max_leaf < 1)
        return;

    cpuid(1, 0, regs);

    f.sse2 = (regs[3] & (1u << 26)) != 0;
    f.sse3 = (regs[2] & (1u << 0)) != 0;
    f.ssse3 = (regs[2] & (1u << 9)) != 0;
    f.sse4_1 = (regs[2] & (1u << 19)) != 0;
    f.sse4_2 = (regs[2] & (1u << 20)) != 0;

    const bool osxsave = (regs[2] & (1u << 27)) != 0;
    const bool avx_cpu = (regs[2] & (1u << 28)) != 0;

    std::uint64_t xcr0 = 0;
    if (osxsave)
        xcr0 = xgetbv0();

    // XCR0 bits 1 and 2: SSE state and AVX (YMM) state enabled by the OS.
    f.avx = avx_cpu && ((xcr0 & 0x6) == 0x6);

    if (max_leaf < 7)
        return;

    cpuid(7, 0, regs);

    f.avx2 = f.avx && (regs[1] & (1u << 5)) != 0;

    // XCR0 bits 1,2,5,6,7: SSE, AVX, opmask, ZMM_Hi256, Hi16_ZMM.
    f.avx512f = f.avx && ((xcr0 & 0xE6) == 0xE6) &&
                (regs[1] & (1u << 16)) != 0;
}

#endif // SIMD_DETAIL_X86

#if SIMD_DETAIL_ARM

#if defined(__APPLE__)

inline bool sysctl_flag(const char* name) noexcept
{
    int value = 0;
    std::size_t size = sizeof(value);
    return sysctlbyname(name, &value, &size, nullptr, 0) == 0 && value != 0;
}

inline void detect_apple_arm(features& f) noexcept
{
#if defined(__aarch64__) || defined(__arm64__) || defined(_M_ARM64)
    // NEON is mandatory in AArch64.
    f.neon = true;
#else
    // Every 32-bit ARM iOS device is ARMv7 with NEON.
    f.neon = true;
#endif

    f.dotprod = sysctl_flag("hw.optional.arm.FEAT_DotProd");
    f.fp16 = sysctl_flag("hw.optional.arm.FEAT_FP16");
    f.i8mm = sysctl_flag("hw.optional.arm.FEAT_I8MM");
    f.bf16 = sysctl_flag("hw.optional.arm.FEAT_BF16");

    // Apple does not implement SVE/SVE2.
}

#endif // __APPLE__

#if defined(_WIN32)

inline void detect_windows_arm(features& f) noexcept
{
#if defined(_M_ARM64) || defined(__aarch64__)
    // NEON is mandatory in AArch64.
    f.neon = true;

    // PROCESSOR_FEATURE_ID values.  The names are missing from older SDKs,
    // so the documented numbers are used directly; unknown IDs simply
    // report false on systems that predate them.
    constexpr DWORD kArmV82Dp = 43;
    constexpr DWORD kArmSve = 46;
    constexpr DWORD kArmSve2 = 47;
    constexpr DWORD kArmV82I8mm = 66;
    constexpr DWORD kArmV82Fp16 = 67;
    constexpr DWORD kArmV86Bf16 = 68;

    f.dotprod = IsProcessorFeaturePresent(kArmV82Dp) != 0;
    f.fp16 = IsProcessorFeaturePresent(kArmV82Fp16) != 0;
    f.i8mm = IsProcessorFeaturePresent(kArmV82I8mm) != 0;
    f.bf16 = IsProcessorFeaturePresent(kArmV86Bf16) != 0;
    f.sve = IsProcessorFeaturePresent(kArmSve) != 0;
    f.sve2 = IsProcessorFeaturePresent(kArmSve2) != 0;
#else
    (void)f;
#endif
}

#endif // _WIN32

#if defined(__linux__) || defined(__ANDROID__) || defined(__OHOS__)

inline void detect_linux_arm(features& f) noexcept
{
    // AT_HWCAP/AT_HWCAP2 are enum values in some libcs, so #ifdef cannot
    // detect them reliably.
#if defined(AT_HWCAP)
#  define SIMD_DETAIL_AT_HWCAP AT_HWCAP
#else
#  define SIMD_DETAIL_AT_HWCAP 16
#endif
#if defined(AT_HWCAP2)
#  define SIMD_DETAIL_AT_HWCAP2 AT_HWCAP2
#else
#  define SIMD_DETAIL_AT_HWCAP2 26
#endif

    const unsigned long hwcap = getauxval(SIMD_DETAIL_AT_HWCAP);

#if defined(__aarch64__) || defined(__arm64__)
    const unsigned long hwcap2 = getauxval(SIMD_DETAIL_AT_HWCAP2);

    // NEON (ASIMD) is mandatory in AArch64; a failed getauxval must not
    // hide it.
    f.neon = true;
    f.sve = (hwcap & (1UL << 22)) != 0;    // SVE
    f.dotprod = (hwcap & (1UL << 20)) != 0; // ASIMDDP
    f.fp16 = (hwcap & (1UL << 10)) != 0;   // ASIMDHP

    f.sve2 = (hwcap2 & (1UL << 1)) != 0;   // SVE2
    f.i8mm = (hwcap2 & (1UL << 13)) != 0;  // I8MM
    f.bf16 = (hwcap2 & (1UL << 14)) != 0;  // BF16
#else
    // Linux arm (32-bit) HWCAP bit for NEON.
    f.neon = (hwcap & (1UL << 12)) != 0;
#endif

#undef SIMD_DETAIL_AT_HWCAP
#undef SIMD_DETAIL_AT_HWCAP2
}

#endif // linux/android/ohos

#endif // SIMD_DETAIL_ARM

} // namespace detail

inline features detect()
{
    features f;

#if SIMD_DETAIL_X86
    detail::detect_x86(f);
#elif SIMD_DETAIL_ARM
#  if defined(_WIN32)
    detail::detect_windows_arm(f);
#  elif defined(__APPLE__)
    detail::detect_apple_arm(f);
#  elif defined(__linux__) || defined(__ANDROID__) || defined(__OHOS__)
    detail::detect_linux_arm(f);
#  endif
#endif

    return f;
}

// Detect once; thread-safe local static since C++11.
inline const features& cached()
{
    static const features value = detect();
    return value;
}

inline std::string to_string(const features& f)
{
    std::string out;
    auto add = [&out](bool on, const char* name) {
        if (!on)
            return;
        if (!out.empty())
            out.push_back(' ');
        out += name;
    };

    add(f.sse2, "sse2");
    add(f.sse3, "sse3");
    add(f.ssse3, "ssse3");
    add(f.sse4_1, "sse4.1");
    add(f.sse4_2, "sse4.2");
    add(f.avx, "avx");
    add(f.avx2, "avx2");
    add(f.avx512f, "avx512f");

    add(f.neon, "neon");
    add(f.sve, "sve");
    add(f.sve2, "sve2");
    add(f.dotprod, "dotprod");
    add(f.fp16, "fp16");
    add(f.i8mm, "i8mm");
    add(f.bf16, "bf16");

    return out;
}

} // namespace simd

#undef SIMD_DETAIL_X86
#undef SIMD_DETAIL_ARM
