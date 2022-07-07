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

// What we are.
#ifdef _MSC_VER
    #define HAVE_MSC
    #define MSC_VERSION _MSC_VER
#elif defined(__GNUC__)
    #define HAVE_GNUC
#elif defined(__CYGWIN__)
    #define HAVE_CYGWIN
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
#endif

// ISO predefined constant for C++ version.
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

// MSFT predefined constant for VS version.
#if MSC_VERSION >= 1800
    #define HAVE_VS2013
#endif
#if MSC_VERSION >= 1900
    #define HAVE_VS2015
#endif
#if MSC_VERSION >= 1910
    #define HAVE_VS2017
#endif
#if MSC_VERSION >= 1920
    #define HAVE_VS2019
#endif
#if MSC_VERSION >= 1930
    #define HAVE_VS2022
#endif

// ISO predefined constant for targeted CPU architecture.
#if defined _M_IX86
    #define HAVE_X86
#elif defined _M_X64
    #define HAVE_X64
#elif defined _M_IA64
    #define HAVE_ITANIUM
#endif

// Other platforms not as far with C++ 20.
#if defined(HAVE_MSC) && defined(HAVE_CPP20)
    #define HAVE_RANGES
    #define HAVE_CONSTEVAL
    #define HAVE_STRING_CONSTEXPR
    #define HAVE_VECTOR_CONSTEXPR
#endif

// TODO: define warning suppressions for other platforms.
#if defined(HAVE_MSC)
    #define HAVE_PRAGMA_WARNING
#endif

// Things we configure to have (available on all platforms).

// disable noexcept to capture stack trace.
#define HAVE_NOEXCEPT

// deprecated is noisy, turn on to find dependencies.
////#define HAVE_DEPRECATED

// have a portable build (no intrinsics).
////#define HAVE_PORTABLE

#endif
