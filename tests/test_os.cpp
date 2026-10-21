#include <platform/os.hpp>
#include <platform/os.hpp> // pragma once

#include <iostream>

// The macros and the constants must never disagree.
#define CHECK(lower, upper) \
    static_assert(os::is_##lower == (OS_##upper != 0), #upper)

CHECK(windows, WINDOWS);
CHECK(apple, APPLE);
CHECK(macos, MACOS);
CHECK(ios, IOS);
CHECK(ios_simulator, IOS_SIMULATOR);
CHECK(linux, LINUX);
CHECK(android, ANDROID);
CHECK(ohos, OHOS);
CHECK(freebsd, FREEBSD);
CHECK(netbsd, NETBSD);
CHECK(openbsd, OPENBSD);
CHECK(dragonfly, DRAGONFLY);
CHECK(solaris, SOLARIS);
CHECK(aix, AIX);
CHECK(fuchsia, FUCHSIA);
CHECK(haiku, HAIKU);
CHECK(qnx, QNX);

#undef CHECK

// Preprocessing-time selection: this is what the macros are for.
#if OS_IOS_SIMULATOR
static constexpr const char* name = "ios-simulator";
#elif OS_IOS
static constexpr const char* name = "ios";
#elif OS_MACOS
static constexpr const char* name = "macos";
#elif OS_WINDOWS
static constexpr const char* name = "windows";
#elif OS_LINUX
static constexpr const char* name = "linux";
#elif OS_ANDROID
static constexpr const char* name = "android";
#elif OS_OHOS
static constexpr const char* name = "ohos";
#elif OS_FREEBSD
static constexpr const char* name = "freebsd";
#elif OS_NETBSD
static constexpr const char* name = "netbsd";
#elif OS_OPENBSD
static constexpr const char* name = "openbsd";
#elif OS_DRAGONFLY
static constexpr const char* name = "dragonfly";
#elif OS_SOLARIS
static constexpr const char* name = "solaris";
#elif OS_AIX
static constexpr const char* name = "aix";
#elif OS_FUCHSIA
static constexpr const char* name = "fuchsia";
#elif OS_HAIKU
static constexpr const char* name = "haiku";
#elif OS_QNX
static constexpr const char* name = "qnx";
#else
static constexpr const char* name = "unknown";
#endif

int main()
{
    std::cout << std::boolalpha
              << "windows        " << os::is_windows        << "  OS_WINDOWS        " << OS_WINDOWS << '\n'
              << "apple          " << os::is_apple          << "  OS_APPLE          " << OS_APPLE << '\n'
              << "macos          " << os::is_macos          << "  OS_MACOS          " << OS_MACOS << '\n'
              << "ios            " << os::is_ios            << "  OS_IOS            " << OS_IOS << '\n'
              << "ios_simulator  " << os::is_ios_simulator  << "  OS_IOS_SIMULATOR  " << OS_IOS_SIMULATOR << '\n'
              << "linux          " << os::is_linux          << "  OS_LINUX          " << OS_LINUX << '\n'
              << "android        " << os::is_android        << "  OS_ANDROID        " << OS_ANDROID << '\n'
              << "ohos           " << os::is_ohos           << "  OS_OHOS           " << OS_OHOS << '\n'
              << "freebsd        " << os::is_freebsd        << "  OS_FREEBSD        " << OS_FREEBSD << '\n'
              << "netbsd         " << os::is_netbsd         << "  OS_NETBSD         " << OS_NETBSD << '\n'
              << "openbsd        " << os::is_openbsd        << "  OS_OPENBSD        " << OS_OPENBSD << '\n'
              << "dragonfly      " << os::is_dragonfly      << "  OS_DRAGONFLY      " << OS_DRAGONFLY << '\n'
              << "solaris        " << os::is_solaris        << "  OS_SOLARIS        " << OS_SOLARIS << '\n'
              << "aix            " << os::is_aix            << "  OS_AIX            " << OS_AIX << '\n'
              << "fuchsia        " << os::is_fuchsia        << "  OS_FUCHSIA        " << OS_FUCHSIA << '\n'
              << "haiku          " << os::is_haiku          << "  OS_HAIKU          " << OS_HAIKU << '\n'
              << "qnx            " << os::is_qnx            << "  OS_QNX            " << OS_QNX << '\n'
              << "name           " << name << '\n';
    return 0;
}
