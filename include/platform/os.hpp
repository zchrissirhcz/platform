// SPDX-License-Identifier: MIT
// Copyright (c) 2026 Zhuo Zhang

#pragma once

// Compile-time OS target detection.
//
// The same facts are exposed twice, derived from one set of conditions:
//
//   OS_*      preprocessor macros, always defined to 0 or 1.  Use them in
//             `#if OS_WINDOWS` when the decision must happen before
//             compilation (selecting an #include, a declaration, ...).
//             They are always defined, so test `#if OS_X`, not `#ifdef`.
//
//   os::is_*  constexpr bools.  Use them in `if constexpr`; they are
//             type-checked and scoped, which is why they are preferable
//             whenever preprocessing is not actually required.
//
// Flags: OS_WINDOWS, OS_APPLE, OS_MACOS, OS_IOS, OS_IOS_SIMULATOR,
//        OS_LINUX, OS_ANDROID, OS_OHOS, OS_FREEBSD, OS_NETBSD,
//        OS_OPENBSD, OS_DRAGONFLY, OS_SOLARIS, OS_AIX, OS_FUCHSIA,
//        OS_HAIKU, OS_QNX
//
// Not modelled on purpose:
//   - ChromeOS, SteamOS, Tizen: a normal toolchain reports plain Linux.
//   - Mac Catalyst, tvOS, watchOS, visionOS: only OS_APPLE is 1.
//   - Console SDKs (PlayStation, Xbox, Switch): vendor macros, not portable.
//   - RTOS other than QNX (FreeRTOS, Zephyr, ...): toolchain-specific.
//   - WebAssembly targets (Emscripten, WASI): target environments, not OSes.
//   - Legacy systems (OS/2, Symbian, Windows Phone, ...): not in active use.
//
// Terminology:
//   OS_APPLE          - any Apple platform (family flag, not an alternative)
//   OS_MACOS          - macOS proper, not Mac Catalyst
//   OS_IOS            - iPhone/iPad, device or Simulator, not Catalyst
//   OS_IOS_SIMULATOR  - the target environment is the iOS Simulator
//   OS_LINUX          - Linux-based OS other than Android and OpenHarmony
//   OS_SOLARIS        - Solaris and its derivatives (illumos, OpenIndiana)

#if defined(__APPLE__)
#  include <TargetConditionals.h>
#endif

// ------------------------------------------------------------ OS_* macros --

// ----------------------------------------------------------------- Windows --
// _WIN32 is defined by both 32- and 64-bit MSVC/clang-cl toolchains.
#if defined(_WIN32)
#  define OS_WINDOWS 1
#else
#  define OS_WINDOWS 0
#endif

// ------------------------------------------------------------------- Apple --
// __APPLE__ is true on every Apple platform, iPhone included.  The real
// discriminator is TargetConditionals.h.
#if defined(__APPLE__)
#  define OS_APPLE 1
#else
#  define OS_APPLE 0
#endif

// macOS proper.  Mac Catalyst is deliberately 0: it runs on macOS but is
// built against the iOS SDK.
#if OS_APPLE && defined(TARGET_OS_OSX) && TARGET_OS_OSX
#  define OS_MACOS 1
#else
#  define OS_MACOS 0
#endif

// iPadOS has no macro of its own and reports TARGET_OS_IOS.  Catalyst also
// sets TARGET_OS_IOS, hence the exclusion.
#if OS_APPLE && defined(TARGET_OS_IOS) && TARGET_OS_IOS && \
    !(defined(TARGET_OS_MACCATALYST) && TARGET_OS_MACCATALYST)
#  define OS_IOS 1
#else
#  define OS_IOS 0
#endif

// A Simulator is a target environment, not an OS, so it is qualified by the
// platform it simulates.  OS_IOS is already 1 there; a physical device is
// `OS_IOS && !OS_IOS_SIMULATOR`.
#if OS_IOS && defined(TARGET_OS_SIMULATOR) && TARGET_OS_SIMULATOR
#  define OS_IOS_SIMULATOR 1
#else
#  define OS_IOS_SIMULATOR 0
#endif

// -------------------------------------------------------- Linux / Android --
// The NDK defines __ANDROID__ (not __android__), and __linux__ is also
// defined on Android, so the two tests exclude each other.
#if defined(__ANDROID__)
#  define OS_ANDROID 1
#else
#  define OS_ANDROID 0
#endif

#if defined(__OHOS__)
#  define OS_OHOS 1
#else
#  define OS_OHOS 0
#endif

// Linux-based OS in general (desktop, server, embedded distros).  Android and
// OpenHarmony are separate OSes built on the Linux kernel and are excluded.
#if defined(__linux__) && !OS_ANDROID && !OS_OHOS
#  define OS_LINUX 1
#else
#  define OS_LINUX 0
#endif

// --------------------------------------------------------------------- BSD --
#if defined(__FreeBSD__)
#  define OS_FREEBSD 1
#else
#  define OS_FREEBSD 0
#endif

#if defined(__NetBSD__)
#  define OS_NETBSD 1
#else
#  define OS_NETBSD 0
#endif

#if defined(__OpenBSD__)
#  define OS_OPENBSD 1
#else
#  define OS_OPENBSD 0
#endif

#if defined(__DragonFly__)
#  define OS_DRAGONFLY 1
#else
#  define OS_DRAGONFLY 0
#endif

// ------------------------------------------------------- Solaris and AIX --
// __sun covers Solaris and its derivatives.  illumos additionally defines
// __illumos__ if the two ever need to be told apart.
#if defined(__sun)
#  define OS_SOLARIS 1
#else
#  define OS_SOLARIS 0
#endif

#if defined(_AIX)
#  define OS_AIX 1
#else
#  define OS_AIX 0
#endif

// ------------------------------------------------------ Other native OSes --
#if defined(__Fuchsia__)
#  define OS_FUCHSIA 1
#else
#  define OS_FUCHSIA 0
#endif

#if defined(__HAIKU__)
#  define OS_HAIKU 1
#else
#  define OS_HAIKU 0
#endif

#if defined(__QNX__)
#  define OS_QNX 1
#else
#  define OS_QNX 0
#endif

// --------------------------------------------------------- os::is_* flags --
// Derived from the macros above, so the conditions exist in exactly one
// place.

namespace os {

static constexpr bool is_windows = OS_WINDOWS != 0;
static constexpr bool is_apple = OS_APPLE != 0;
static constexpr bool is_macos = OS_MACOS != 0;
static constexpr bool is_ios = OS_IOS != 0;
static constexpr bool is_ios_simulator = OS_IOS_SIMULATOR != 0;
static constexpr bool is_linux = OS_LINUX != 0;
static constexpr bool is_android = OS_ANDROID != 0;
static constexpr bool is_ohos = OS_OHOS != 0;
static constexpr bool is_freebsd = OS_FREEBSD != 0;
static constexpr bool is_netbsd = OS_NETBSD != 0;
static constexpr bool is_openbsd = OS_OPENBSD != 0;
static constexpr bool is_dragonfly = OS_DRAGONFLY != 0;
static constexpr bool is_solaris = OS_SOLARIS != 0;
static constexpr bool is_aix = OS_AIX != 0;
static constexpr bool is_fuchsia = OS_FUCHSIA != 0;
static constexpr bool is_haiku = OS_HAIKU != 0;
static constexpr bool is_qnx = OS_QNX != 0;

} // namespace os
