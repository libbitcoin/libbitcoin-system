/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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

/// Plaform: architecture, compiler, and standard libraries.
/// ---------------------------------------------------------------------------

#if defined(__GNUC__)
    #define HAVE_GNUC
#elif defined(__linux__)
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
#elif defined(_MSC_VER)
    #define HAVE_MSC
#endif

/// ISO defines for targeted CPU architecture.
#if defined _M_IX86
    #define HAVE_X86
#elif defined _M_X64
    #define HAVE_X64
#elif defined _M_IA64
    #define HAVE_ITANIUM
#endif

/// Use intrinsics for Intel CPU features with ISO defines.
#if defined(HAVE_X86) || defined(HAVE_X64)  || defined(HAVE_ITANIUM)
    #define HAVE_ISO_INTEL
#endif

/// Use __asm__ for Intel CPU features if non-ISO interface.
#if defined(__x86_64__) || defined(__amd64__) || defined(__i386__)
    #define HAVE_GNU_INTEL
#endif

/// Intel CPU architecture (__m128i/__m256i defined).
#if defined(HAVE_ISO_INTEL) || defined(HAVE_GNU_INTEL)
    #define HAVE_INTEL
#endif

/// MSC inline assembly (via __asm, no __asm__ support) does not support ARM
/// and x64. Since we cross compile to x64 we consider this lack of support
/// prohibitive for __asm. github.com/MicrosoftDocs/cpp-docs/blob/main/docs/
/// assembler/inline/inline-assembler.md
#if defined(HAVE_INTEL) && !defined(HAVE_MSC)
    #define HAVE_INTEL_ASM
#endif

/// ARM CPU architecture.
#if defined(__arm__) || defined(__arm64__) || defined(_M_ARM)
    #define HAVE_ARM
#endif

/// ARM Neon intrinsics defined.
#if defined(HAVE_ARM)
#if defined(HAVE_GNUC) || defined(__ARM_NEON) || defined(HAVE_MSC)
    #define HAVE_NEON_INTRINSICS
#endif
#endif

/// MSC predefined constant for Visual Studio version.
#if defined(HAVE_MSC)
    #if _MSC_VER >= 1800
        #define HAVE_VS2013
    #endif
    #if _MSC_VER >= 1900
        #define HAVE_VS2015
    #endif
    #if _MSC_VER >= 1910
        #define HAVE_VS2017
    #endif
    #if _MSC_VER >= 1920
        #define HAVE_VS2019
    #endif
    #if _MSC_VER >= 1930
        #define HAVE_VS2022
    #endif
#endif

/// C/C++ language and support by platform.
/// ---------------------------------------------------------------------------

/// ISO predefined constant for C++ version.
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

/// Other platforms not as far with C++ 20.
#if defined(HAVE_MSC) && defined(HAVE_CPP20)
    #define HAVE_RANGES
    #define HAVE_CONSTEVAL
    #define HAVE_STRING_CONSTEXPR
    #define HAVE_VECTOR_CONSTEXPR
#endif

/// TODO: define warning suppressions for other platforms.
#if defined(HAVE_MSC)
    #define HAVE_PRAGMA_WARNING
#endif

/// WITH_ indicates build symbol.
/// ---------------------------------------------------------------------------

/// Build configured (always available on msvc).
#if defined(HAVE_MSC) || defined(WITH_ICU)
    #define HAVE_ICU
#endif

/// These are manually configured here.
/// ---------------------------------------------------------------------------

/// Disable noexcept to capture stack trace.
#define HAVE_NOEXCEPT

// Deprecated is noisy, turn on to find dependencies.
////#define HAVE_DEPRECATED

/// Have slow test execution (scrypt is slow by design).
////#define HAVE_SLOW_TESTS

#endif
