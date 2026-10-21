#include <platform/compiler.hpp>
#include <platform/compiler.hpp> // pragma once

#include <iostream>

// The macros and the constants must never disagree.
#define CHECK(lower, upper) \
    static_assert(compiler::is_##lower == (COMPILER_##upper != 0), #upper)

CHECK(gcc, GCC);
CHECK(clang, CLANG);
CHECK(apple_clang, APPLE_CLANG);
CHECK(msvc, MSVC);
CHECK(mingw, MINGW);
CHECK(emscripten, EMSCRIPTEN);

#undef CHECK

static_assert(compiler::version_major == COMPILER_VERSION_MAJOR, "");
static_assert(compiler::version_minor == COMPILER_VERSION_MINOR, "");
static_assert(compiler::version_patch == COMPILER_VERSION_PATCH, "");

// Specific flags first: Apple Clang and Emscripten are also Clang, and
// MinGW is also GCC.
#if COMPILER_APPLE_CLANG
static constexpr const char* name = "apple-clang";
#elif COMPILER_EMSCRIPTEN
static constexpr const char* name = "emscripten";
#elif COMPILER_CLANG
static constexpr const char* name = "clang";
#elif COMPILER_MINGW
static constexpr const char* name = "mingw";
#elif COMPILER_GCC
static constexpr const char* name = "gcc";
#elif COMPILER_MSVC
static constexpr const char* name = "msvc";
#else
static constexpr const char* name = "unknown";
#endif

int main()
{
    std::cout << std::boolalpha
              << "gcc            " << compiler::is_gcc            << "  COMPILER_GCC            " << COMPILER_GCC << '\n'
              << "clang          " << compiler::is_clang          << "  COMPILER_CLANG          " << COMPILER_CLANG << '\n'
              << "apple_clang    " << compiler::is_apple_clang    << "  COMPILER_APPLE_CLANG    " << COMPILER_APPLE_CLANG << '\n'
              << "msvc           " << compiler::is_msvc           << "  COMPILER_MSVC           " << COMPILER_MSVC << '\n'
              << "mingw          " << compiler::is_mingw          << "  COMPILER_MINGW          " << COMPILER_MINGW << '\n'
              << "emscripten     " << compiler::is_emscripten     << "  COMPILER_EMSCRIPTEN     " << COMPILER_EMSCRIPTEN << '\n'
              << "version        " << compiler::version_major << '.'
                                   << compiler::version_minor << '.'
                                   << compiler::version_patch << '\n'
              << "name           " << name << '\n';
    return 0;
}
