# platform

Header-only C++11 platform detection. Four independent headers, no
dependencies, no build step.

| Header | Kind | Answers |
|---|---|---|
| `<platform/os.hpp>` | compile-time | Which OS: Windows, macOS, iOS, Linux, Android, OpenHarmony, the BSDs, Solaris, AIX, Fuchsia, Haiku, QNX |
| `<platform/compiler.hpp>` | compile-time | Which compiler and version: GCC, Clang, Apple Clang, MSVC, MinGW, Emscripten |
| `<platform/arch.hpp>` | compile-time | Which ISA: x86, x86_64, arm, arm64/aarch64, riscv32/64, loongarch64, wasm32 |
| `<platform/simd.hpp>` | runtime | Which SIMD extensions the current machine supports: SSE2..AVX-512F, NEON, SVE, SVE2, dotprod, FP16, I8MM, BF16 |

## Quick start

Add this repository's `include/` directory to your include path, or use the
CMake target:

```cmake
add_subdirectory(platform)
target_link_libraries(my_app PRIVATE platform)

# or, after installing:
find_package(platform CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE platform::platform)
```

```cpp
#include <platform/os.hpp>
#include <platform/arch.hpp>

#if OS_WINDOWS
    // preprocessing-time selection
#endif

if constexpr (arch::is_arm64) {
    // compile-time selection
}
```

Runtime SIMD detection is a separate call, because it asks the machine the
program is running on:

```cpp
#include <platform/simd.hpp>
#include <iostream>

const simd::features& f = simd::cached(); // detect once, thread-safe
if (f.avx2) {
    // call the AVX2 kernel
}
std::cout << simd::to_string(f) << '\n'; // "sse2 sse4.2 avx2 ..."
```

`if constexpr` requires C++17; the headers themselves only require C++11, so
on C++11 use the `#if OS_*` form for preprocessing-time selection.

## Compile-time vs runtime

- `OS_*`, `COMPILER_*`, `ARCH_*` are always defined to `0` or `1`. Use
  `#if OS_MACOS`, **not** `#ifdef OS_MACOS`.
- `os::is_*`, `compiler::is_*`, `arch::is_*` are `static constexpr bool`, for
  `if constexpr` and `static_assert`.
- `simd::detect()` / `simd::cached()` answer what the **current CPU** supports
  at runtime. They are not constants and cannot be used in `#if` or
  `if constexpr`.
- `simd.hpp` must be compiled for the baseline ISA. A binary built with
  `-mavx2` cannot run on a CPU without AVX2, so it can never report the truth.

Writing SIMD code is still done with the compiler's macros
(`__AVX2__`, `__ARM_NEON`, ...); `simd.hpp` only chooses which kernel to call.

## Status

Tested at runtime:

- macOS arm64 (Apple Silicon)
- macOS x86_64 (Rosetta)

CI is configured to build and run the tests on Linux x86_64, macOS, and
Windows x64.

Compile-checked only:

- Cross-target compiles of `os.hpp` / `arch.hpp` for i386, x86_64, armv7,
  aarch64, riscv32/64, loongarch64 and wasm32
- The Android/OpenHarmony ARM branches of `simd.hpp` were syntax-checked
  with a stub `<sys/auxv.h>`, not with the real NDK/OpenHarmony toolchains

Not yet validated at runtime: Windows ARM64, Android, OpenHarmony. The
detection code is there, but treat it as untested until CI or a device says
otherwise.

## Feature notes

- `arch.hpp` covers instruction set families; ARMv7 vs ARMv8 and x86-64
  levels are not modelled. Use `__ARM_ARCH` / `-march` macros when you need
  that granularity.
- `simd.hpp` reports `avx512f` only; BW/DQ/VL are not exposed.
- SVE vector length is not reported: it is runtime-variable unless the
  compiler fixes it with `-msve-vector-bits`.
- Hexagon, RISC-V vector, LoongArch LASX and wasm SIMD are not detected.
- `OS_APPLE` is true on every Apple platform; `OS_MACOS` and `OS_IOS` narrow
  it down. Mac Catalyst is intentionally neither.

## Build and test

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

`add_subdirectory()` and `install()` are both supported. The install
interface puts the headers under `include/platform/`.

## Design notes

- Headers are independent: none includes another, so you only pay for what
  you use.
- The macro names (`OS_*`, `COMPILER_*`, `ARCH_*`) are global and intentionally
  short. If you embed this into a larger codebase, watch for collisions.
- This is a detection library, not a platform abstraction. It deliberately
  has no threads, files, dynamic loading, or networking.
- The namespace names (`os`, `compiler`, `arch`, `simd`) are also short. If you
  publish this as a dependency, a project-specific prefix is worth considering.
- Pre-1.0: the API is small and may still change.

## Author

Zhuo Zhang <imzhuo@foxmail.com>

## License

MIT. See [LICENSE](LICENSE).
