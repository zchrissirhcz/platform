// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Zhuo Zhang

#pragma once

// Compile-time compiler detection.
//
// Same shape as os.hpp:
//
//   COMPILER_*    preprocessor macros, always defined to 0 or 1.  Use them
//                 in `#if COMPILER_MSVC` when the decision must happen
//                 before compilation.
//
//   compiler::is_*  constexpr bools, for `if constexpr`.
//
//   compiler::version_major / version_minor / version_patch
//                 normalized version of the active compiler, 0.0.0 when
//                 the compiler is unknown.
//
// Family flags can overlap on purpose:
//   - is_clang is also true for Apple Clang, Emscripten and clang-cl.
//   - is_gcc   is also true for MinGW.
//   Use the specific flag (is_apple_clang, is_mingw, ...) to narrow down.
//   is_msvc is false for clang-cl, which reports is_clang instead.
//
// The version is the upstream compiler version.  For Apple Clang that is the
// LLVM version it is based on; __apple_build_version__ is Apple's own build
// number and is deliberately not exposed here.
//
// Not modelled on purpose (add them when the build matrix needs them):
//   - Intel oneAPI / classic: __INTEL_LLVM_COMPILER / __INTEL_COMPILER
//   - CUDA / HIP: __CUDACC__ / __HIPCC__
//   - clang-cl as a distinct flag (it reports is_clang)
//   - TCC and other small compilers

// ------------------------------------------------------- COMPILER_* flags --

// MSVC proper.  clang-cl also defines _MSC_VER, hence the exclusion.
#if defined(_MSC_VER) && !defined(__clang__)
#  define COMPILER_MSVC 1
#else
#  define COMPILER_MSVC 0
#endif

#if defined(__clang__)
#  define COMPILER_CLANG 1
#else
#  define COMPILER_CLANG 0
#endif

// Apple's clang.  Upstream LLVM built on macOS does not define this.
#if defined(__apple_build_version__)
#  define COMPILER_APPLE_CLANG 1
#else
#  define COMPILER_APPLE_CLANG 0
#endif

// GCC family.  Clang defines __GNUC__ for compatibility, hence the exclusion.
// MinGW is GCC, so is_gcc is true there as well.
#if defined(__GNUC__) && !defined(__clang__)
#  define COMPILER_GCC 1
#else
#  define COMPILER_GCC 0
#endif

#if defined(__MINGW32__) || defined(__MINGW64__)
#  define COMPILER_MINGW 1
#else
#  define COMPILER_MINGW 0
#endif

#if defined(__EMSCRIPTEN__)
#  define COMPILER_EMSCRIPTEN 1
#else
#  define COMPILER_EMSCRIPTEN 0
#endif

// ------------------------------------------------------- version numbers --
// MSVC encodes its version in _MSC_VER as MMmm (e.g. 1940 => 19.40) and has
// no patch level there, so version_patch is 0 for MSVC.

#if COMPILER_MSVC
#  define COMPILER_VERSION_MAJOR (_MSC_VER / 100)
#  define COMPILER_VERSION_MINOR (_MSC_VER % 100)
#  define COMPILER_VERSION_PATCH 0
#elif COMPILER_CLANG
#  define COMPILER_VERSION_MAJOR __clang_major__
#  define COMPILER_VERSION_MINOR __clang_minor__
#  define COMPILER_VERSION_PATCH __clang_patchlevel__
#elif COMPILER_GCC
#  define COMPILER_VERSION_MAJOR __GNUC__
#  define COMPILER_VERSION_MINOR __GNUC_MINOR__
#  define COMPILER_VERSION_PATCH __GNUC_PATCHLEVEL__
#else
#  define COMPILER_VERSION_MAJOR 0
#  define COMPILER_VERSION_MINOR 0
#  define COMPILER_VERSION_PATCH 0
#endif

// ---------------------------------------------------- compiler::is_* flags --
// Derived from the macros above, so the conditions exist in exactly one
// place.

namespace compiler {

static constexpr bool is_gcc = COMPILER_GCC != 0;
static constexpr bool is_clang = COMPILER_CLANG != 0;
static constexpr bool is_apple_clang = COMPILER_APPLE_CLANG != 0;
static constexpr bool is_msvc = COMPILER_MSVC != 0;
static constexpr bool is_mingw = COMPILER_MINGW != 0;
static constexpr bool is_emscripten = COMPILER_EMSCRIPTEN != 0;

static constexpr int version_major = COMPILER_VERSION_MAJOR;
static constexpr int version_minor = COMPILER_VERSION_MINOR;
static constexpr int version_patch = COMPILER_VERSION_PATCH;

} // namespace compiler
