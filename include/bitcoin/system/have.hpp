/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_HAVE_HPP
#define LIBBITCOIN_SYSTEM_HAVE_HPP

#include <bitcoin/system/version.hpp>

///////////////////////////////////////////////////////////////////////////////
// Maintainers: update corresponding diagnostic HAVE emissions in define.cpp.
///////////////////////////////////////////////////////////////////////////////

/// Plaform: architecture, compiler, and standard libraries.
/// ---------------------------------------------------------------------------

#if defined(__linux__)
    #define HAVE_LINUX
#elif defined(__APPLE__)
    #define HAVE_APPLE
#elif defined(__FreeBSD__)
    #define HAVE_FREEBSD
#elif defined(__OpenBSD__)
    #define HAVE_OPENBSD
#elif defined(__NetBSD__)
    #define HAVE_NETBSD
#elif defined(__CYGWIN__)
    #define HAVE_CYGWIN
#endif

/// stackoverflow.com/questions/38499462/how-to-tell-clang-to-stop-pretending-
/// to-be-other-compilers
#if defined(__clang__)
    #define HAVE_CLANG
#endif
#if defined(__APPLE__) && defined(HAVE_CLANG) && defined(__apple_build_version__)
    #define HAVE_XCODE
#endif
#if defined(__GNUC__) && !defined(HAVE_CLANG)
    #define HAVE_GNUC
#endif
#if defined(_MSC_VER) && !defined(HAVE_CLANG)
    #define HAVE_MSC
#endif

/// Determines linker defines (Windows vs. Unix/Linux).
#if defined(HAVE_CLANG) || defined(HAVE_GNUC)
    #define HAVE_NX_LIBS
#elif defined(_MSC_VER) || defined(__CYGWIN__)
    #define HAVE_WINDOWS_LIBS
#endif

/// GNU/MSC defines for targeted CPU architecture.
/// sourceforge.net/p/predef/wiki/Architectures
/// docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros
#if defined(__i386__) || defined(_M_IX86)
    #define HAVE_X32
    #define HAVE_XCPU
#elif defined(__amd64__) || defined(_M_AMD64)
    #define HAVE_X64
    #define HAVE_XCPU
#elif defined(__arm__) || defined(_M_ARM)
    #define HAVE_ARM32
    #define HAVE_ARM
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define HAVE_ARM64
    #define HAVE_ARM
#endif

/// Platform features derived.
/// ---------------------------------------------------------------------------

/// XCPU architecture intrinsics _xgetbv, _cpuid, __cpuidex/__cpuid_count.
/// Always enabled on XCPU, as the binary is not portable to other CPUs.
#if defined(HAVE_XCPU)
    // TODO: CLANG/GCC compile test XCPU and always set -mxsave.
    // TODO: these these defines can be enabled unconditionally.
    #if defined(HAVE_CLANG)
        // Clang13: __cpuid_count/_cpuid/_xgetbv.
        ////#define HAVE_XGETBV
        ////#define HAVE_XCPUID_COUNT
    #endif
    #if defined(HAVE_GNUC)
        // GCC11: __cpuidex/_cpuid/_xgetbv.
        ////#define HAVE_XGETBV
        ////#define HAVE_XCPUIDEX
    #endif
    #if defined(HAVE_MSC)
        // Always available without platform test or build configuration.
        // docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex
        // docs.microsoft.com/en-us/cpp/intrinsics/x86-intrinsics-list
        #define HAVE_XGETBV
        #define HAVE_XCPUIDEX
    #endif
#endif

/// XCPU architecture inline assembly.
#if defined(HAVE_XCPU) && !(defined(HAVE_MSC) && defined(HAVE_X32))
    #define HAVE_XCPU_ASSEMBLY
#endif

/// ARM architecture inline assembly.
#if defined(HAVE_ARM) && !(defined(HAVE_MSC) && defined(HAVE_X32))
    #define HAVE_ARM_ASSEMBLY
#endif

/// Map standard XCPU defines for intrinsics usage.
/// vc++: SHA and SSE41 not independently configurable, all cross compilable.
#if defined(HAVE_XCPU)
    // -msha
    // vc++: SHANI not independently configurable.
    #if defined(__SHA__)
        #define HAVE_SHANI
        #define HAVE_SSE41
        ////#define HAVE_SHA
        ////#define HAVE_128
    #endif
    // -mavx512bw
    // vc++: Advanced Vector Extensions 512 (X86/X64) (/arch:AVX512)
    #if defined(__AVX512BW__)
        #define HAVE_AVX512
        #define HAVE_AVX2
        ////#define HAVE_512
        ////#define HAVE_256
        ////#define HAVE_128
    #endif
    // -mavx2
    // vc++: Advanced Vector Extensions 2 (X86/X64) (/arch:AVX2)
    #if defined(__AVX2__)
        #define HAVE_AVX2
        #define HAVE_SSE41
        ////#define HAVE_256
        ////#define HAVE_128
    #endif
    // -msse4.1
    // vc++: SSE41 not independently configurable.
    #if defined(__SSE4_1__)
        #define HAVE_SSE41
        ////#define HAVE_128
    #endif
#endif

/// Map standard ARM defines for intrinsics usage. 
#if defined(HAVE_ARM)
    // -march=armv8-a+crypto
    // vc++: not independently configurable, cross compilable.
    #if defined(__ARM_FEATURE_CRYPTO) || defined(HAVE_MSC)
        #define HAVE_CRYPTO
        #define HAVE_NEON
        ////#define HAVE_SHA
        ////#define HAVE_128
    #endif
    // vc++: notsupported.
    #if defined(__ARM_FEATURE_SVE) 
        #define HAVE_SVE
        #define HAVE_NEON
        ////#define HAVE_512
        ////#define HAVE_256
        ////#define HAVE_128
    #endif
    // -march=armv8-a (64), march=armv7-a -mfpu=neon -mfloat-abi=hard (32)
    // vc++: not independently configurable, cross compilable.
    #if defined(__ARM_NEON) || defined(HAVE_MSC)
        #define HAVE_NEON
        ////#define HAVE_128
    #endif
#endif

/// WITH_ build symbols.
/// ---------------------------------------------------------------------------

/// XCPU architecture intrinsics.
#if defined(HAVE_XCPU)
    #if defined(WITH_SHANI)
        #define HAVE_SHANI
        #define HAVE_SSE41
        ////#define HAVE_SHA
        ////#define HAVE_128
    #endif
    #if defined(WITH_AVX512)
        #define HAVE_AVX512
        #define HAVE_AVX2
        #define HAVE_SSE41
        ////#define HAVE_512
        ////#define HAVE_256
        ////#define HAVE_128
    #endif
    #if defined(WITH_AVX2)
        #define HAVE_AVX2
        #define HAVE_SSE41
        ////#define HAVE_256
        ////#define HAVE_128
    #endif
    #if defined(WITH_SSE41)
        #define HAVE_SSE41
        ////#define HAVE_128
    #endif
#endif

/// ARM architecture intrinsics (SVE dynamic length covers both 256 and 512).
#if defined(HAVE_ARM)
    #if defined(WITH_SHANI)
        #define HAVE_CRYPTO
        #define HAVE_NEON
        ////#define HAVE_SHA
        ////#define HAVE_128
    #endif
    #if defined(WITH_AVX512) && !defined(HAVE_MSC)
        // vc++: SVE not supported.
        #define HAVE_SVE
        #define HAVE_NEON
        ////#define HAVE_512
        ////#define HAVE_256
        ////#define HAVE_128
    #endif
    #if defined(WITH_AVX2) && !defined(HAVE_MSC)
        // vc++: SVE not supported.
        #define HAVE_SVE
        #define HAVE_NEON
        ////#define HAVE_256
        ////#define HAVE_128
    #endif
    #if defined(WITH_SSE41)
        #define HAVE_NEON
        ////#define HAVE_128
    #endif
#endif

// NEON DISABLED
#undef HAVE_NEON

/// Build configured (always available on msvc).
#if defined(HAVE_MSC) || defined(WITH_ICU)
    #define HAVE_ICU
#endif

/// MSC predefined constant for Visual Studio version (exclusive).
/// ---------------------------------------------------------------------------

#if defined(HAVE_MSC)
    #if   _MSC_VER >= 1930
        #define HAVE_VS2022
    #elif _MSC_VER >= 1920
        #define HAVE_VS2019
    #elif _MSC_VER >= 1910
        #define HAVE_VS2017
    #elif _MSC_VER >= 1900
        #define HAVE_VS2015
    #elif _MSC_VER >= 1800
        #define HAVE_VS2013
    #endif
#endif

/// C/C++ language and support by platform.
/// ---------------------------------------------------------------------------

/// ISO predefined constant for C++ version (inclusive).
#if __cplusplus >= 199711L
    #define HAVE_CPP03
#endif
#if __cplusplus >= 201103L
    #define HAVE_CPP11
#endif
#if __cplusplus >= 201402L
    #define HAVE_CPP14
#endif
#if __cplusplus >= 201703L
    #define HAVE_CPP17
#endif
#if __cplusplus >= 202002L
    #define HAVE_CPP20
#endif

// Support for C++20 features we use varies.
// en.cppreference.com/w/cpp/compiler_support
// These are based on the compiler versions we test with, shown below.
#if defined(HAVE_CPP20)
    // All versions.
    #if defined(HAVE_MSC)
        #define HAVE_CONSTEVAL
    #elif defined(HAVE_CLANG)
        // Apple clang version 15
        #if defined(HAVE_APPLE)
            #define HAVE_CONSTEVAL
        // Ubuntu clang version 16
        #else
            ////#define HAVE_CONSTEVAL (v17)
        #endif
    // gcc version 12
    #elif defined(HAVE_GNUC)
        #define HAVE_CONSTEVAL
    #endif
#endif

/// These are manually configured here.
/// ---------------------------------------------------------------------------

/// Disable to suppress pragma messages.
#define HAVE_MESSAGES

/// Disable to unsuppress warnings.
#define HAVE_SUPPRESSION

/// Disable to emit all suppressed warnings.
#define HAVE_WARNINGS

// Deprecated is noisy, turn on to find dependencies.
////#define HAVE_DEPRECATED

#endif
