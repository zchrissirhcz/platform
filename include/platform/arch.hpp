// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Zhuo Zhang

#pragma once

// Compile-time CPU architecture detection.
//
// Same shape as os.hpp and compiler.hpp:
//
//   ARCH_*       preprocessor macros, always defined to 0 or 1.  Use them in
//                `#if ARCH_ARM64` when the decision must happen before
//                compilation.
//
//   arch::is_*   constexpr bools, for `if constexpr`.
//
// Scope: what general-purpose C++ projects actually build for today.
// Kept: x86, x86_64, arm, arm64, riscv32/riscv64, loongarch64, wasm32.
//
// Naming: ARCH_X86 and ARCH_ARM are the 32-bit variants; the 64-bit variants
// add _64.  RISC-V and LoongArch use a family flag plus width-specific flags:
// ARCH_RISCV + ARCH_RISCV32/ARCH_RISCV64, ARCH_LOONGARCH +
// ARCH_LOONGARCH64.
//
// AArch64 is the architectural name for ARM64 (GNU/Linux and LLVM say
// aarch64, Apple and MSVC say arm64).  ARCH_AARCH64 / is_aarch64 are aliases
// for ARCH_ARM64 / is_arm64, not a separate architecture.
//
// WebAssembly is an instruction set, so wasm32 lives here rather than in
// os.hpp.
//
// Not modelled on purpose:
//   - s390x, PowerPC, MIPS, SPARC: still maintained, but niche outside their
//     vendors' ecosystems; they would double the flag list for very little
//     use.
//   - wasm64: not widely deployed; only wasm32 is modelled.
//   - 32-bit LoongArch (LA64 is the variant in use)
//   - ARMv7-A / ARMv8-A and the A/R/M profiles: sub-architectures, not
//     top-level flags.  When a version or profile branch is really needed,
//     test __ARM_ARCH / __ARM_ARCH_PROFILE directly.
//   - Itanium, Alpha, PA-RISC, m68k, SuperH, e2k
//   - Microcontrollers: AVR, Xtensa, MSP430, ARC, ...
//   - GPU ISAs (NVPTX, AMDGPU): not CPU architectures
//   - Pointer width is not an architecture flag: use sizeof(void*)

// ------------------------------------------------------------ ARCH_* flags --

// ---------------------------------------------------------------- x86 / x64 --
#if defined(__i386__) || defined(__i386) || defined(_M_IX86)
#  define ARCH_X86 1
#else
#  define ARCH_X86 0
#endif

#if defined(__x86_64__) || defined(__amd64__) || \
    defined(_M_X64) || defined(_M_AMD64)
#  define ARCH_X86_64 1
#else
#  define ARCH_X86_64 0
#endif

// -------------------------------------------------------------------- ARM --
// Apple defines both __aarch64__ and __arm64__; MSVC uses _M_ARM64.
// ARM64EC is ARM64 code with an x64-compatible ABI, so it counts as ARM64
// here and is not given a separate flag.
#if defined(__arm__) || defined(_M_ARM)
#  define ARCH_ARM 1
#else
#  define ARCH_ARM 0
#endif

#if defined(__aarch64__) || defined(__arm64__) || \
    defined(_M_ARM64) || defined(_M_ARM64EC)
#  define ARCH_ARM64 1
#else
#  define ARCH_ARM64 0
#endif

// Same architecture, the spelling used by GNU/Linux and LLVM.
#define ARCH_AARCH64 ARCH_ARM64

// ---------------------------------------------------------------- RISC-V --
#if defined(__riscv)
#  define ARCH_RISCV 1
#else
#  define ARCH_RISCV 0
#endif

#if ARCH_RISCV && defined(__riscv_xlen) && __riscv_xlen == 32
#  define ARCH_RISCV32 1
#else
#  define ARCH_RISCV32 0
#endif

#if ARCH_RISCV && defined(__riscv_xlen) && __riscv_xlen == 64
#  define ARCH_RISCV64 1
#else
#  define ARCH_RISCV64 0
#endif

// ------------------------------------------------------------- LoongArch --
#if defined(__loongarch__)
#  define ARCH_LOONGARCH 1
#else
#  define ARCH_LOONGARCH 0
#endif

#if ARCH_LOONGARCH && \
    (defined(__loongarch64) || \
     (defined(__loongarch_grlen) && __loongarch_grlen == 64))
#  define ARCH_LOONGARCH64 1
#else
#  define ARCH_LOONGARCH64 0
#endif

// ----------------------------------------------------------- WebAssembly --
#if defined(__wasm32__)
#  define ARCH_WASM32 1
#else
#  define ARCH_WASM32 0
#endif

// ------------------------------------------------------- arch::is_* flags --
// Derived from the macros above, so the conditions exist in exactly one
// place.

namespace arch {

static constexpr bool is_x86 = ARCH_X86 != 0;
static constexpr bool is_x86_64 = ARCH_X86_64 != 0;
static constexpr bool is_arm = ARCH_ARM != 0;
static constexpr bool is_arm64 = ARCH_ARM64 != 0;
static constexpr bool is_aarch64 = is_arm64; // alias, same architecture
static constexpr bool is_riscv = ARCH_RISCV != 0;
static constexpr bool is_riscv32 = ARCH_RISCV32 != 0;
static constexpr bool is_riscv64 = ARCH_RISCV64 != 0;
static constexpr bool is_loongarch = ARCH_LOONGARCH != 0;
static constexpr bool is_loongarch64 = ARCH_LOONGARCH64 != 0;
static constexpr bool is_wasm32 = ARCH_WASM32 != 0;

} // namespace arch
