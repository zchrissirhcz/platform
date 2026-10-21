#include <platform/arch.hpp>
#include <platform/arch.hpp> // pragma once

#include <iostream>

// The macros and the constants must never disagree.
#define CHECK(lower, upper) \
    static_assert(arch::is_##lower == (ARCH_##upper != 0), #upper)

CHECK(x86, X86);
CHECK(x86_64, X86_64);
CHECK(arm, ARM);
CHECK(arm64, ARM64);
CHECK(aarch64, AARCH64);
CHECK(riscv, RISCV);
CHECK(riscv32, RISCV32);
CHECK(riscv64, RISCV64);
CHECK(loongarch, LOONGARCH);
CHECK(loongarch64, LOONGARCH64);
CHECK(wasm32, WASM32);

#undef CHECK

// Aliases must stay aliases.
static_assert(arch::is_aarch64 == arch::is_arm64, "");
static_assert(ARCH_AARCH64 == ARCH_ARM64, "");

// 64-bit variants first where the family flag exists.
#if ARCH_X86_64
static constexpr const char* name = "x86_64";
#elif ARCH_X86
static constexpr const char* name = "x86";
#elif ARCH_ARM64
static constexpr const char* name = "arm64";
#elif ARCH_ARM
static constexpr const char* name = "arm";
#elif ARCH_RISCV64
static constexpr const char* name = "riscv64";
#elif ARCH_RISCV32
static constexpr const char* name = "riscv32";
#elif ARCH_LOONGARCH64
static constexpr const char* name = "loongarch64";
#elif ARCH_WASM32
static constexpr const char* name = "wasm32";
#else
static constexpr const char* name = "unknown";
#endif

int main()
{
    std::cout << std::boolalpha
              << "x86            " << arch::is_x86            << "  ARCH_X86            " << ARCH_X86 << '\n'
              << "x86_64         " << arch::is_x86_64         << "  ARCH_X86_64         " << ARCH_X86_64 << '\n'
              << "arm            " << arch::is_arm            << "  ARCH_ARM            " << ARCH_ARM << '\n'
              << "arm64          " << arch::is_arm64          << "  ARCH_ARM64          " << ARCH_ARM64 << '\n'
              << "aarch64        " << arch::is_aarch64        << "  ARCH_AARCH64        " << ARCH_AARCH64 << '\n'
              << "riscv          " << arch::is_riscv          << "  ARCH_RISCV          " << ARCH_RISCV << '\n'
              << "riscv32        " << arch::is_riscv32        << "  ARCH_RISCV32        " << ARCH_RISCV32 << '\n'
              << "riscv64        " << arch::is_riscv64        << "  ARCH_RISCV64        " << ARCH_RISCV64 << '\n'
              << "loongarch      " << arch::is_loongarch      << "  ARCH_LOONGARCH      " << ARCH_LOONGARCH << '\n'
              << "loongarch64    " << arch::is_loongarch64    << "  ARCH_LOONGARCH64    " << ARCH_LOONGARCH64 << '\n'
              << "wasm32         " << arch::is_wasm32         << "  ARCH_WASM32         " << ARCH_WASM32 << '\n'
              << "name           " << name << '\n';
    return 0;
}
