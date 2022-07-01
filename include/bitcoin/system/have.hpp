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

// Things we have.

#ifdef _MSC_VER
    #define HAVE_MSC
    #define HAVE_NOEXCEPT
    #define HAVE_CONSTEVAL
    #define HAVE_PRAGMA_WARNING
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

// clang does not yet have consteval.
// Remove this when all platforms support consteval.
// RUNTIME_LITERALS may be used to exclude test evaluation (until consteval).
#if defined(HAVE_CONSTEVAL)
    #define CONSTEVAL consteval
#else
    #define CONSTEVAL constexpr
    #define RUNTIME_LITERALS
#endif

#if defined(HAVE_NOEXCEPT)
    #define NOEXCEPT noexcept
#else
    #define NOEXCEPT
#endif

#define THROWS noexcept(false)

// ISO predefined constant for C++ version.
#if __cplusplus >= 199711L
    #define BC_CPP_03
#endif
#if __cplusplus >= 201103L
    #define BC_CPP_11
#endif
#if __cplusplus >= 201402L
    #define BC_CPP_14
#endif
#if __cplusplus >= 201703L
    #define BC_CPP_17
#endif
#if __cplusplus >= 202002L
    #define BC_CPP_20
#endif

// MSFT predefined constant for VS version.
#if MSC_VERSION >= 1800
    #define BC_VS2013
#endif
#if MSC_VERSION >= 1900
    #define BC_VS2015
#endif
#if MSC_VERSION >= 1910
    #define BC_VS2017
#endif
#if MSC_VERSION >= 1920
    #define BC_VS2019
#endif
#if MSC_VERSION >= 1930
    #define BC_VS2022
#endif

#if defined(HAVE_PRAGMA_WARNING)
    #define BC_DISABLE_WARNING(value) \
        __pragma(warning(disable:value))
    #define BC_PUSH_WARNING(value) \
        __pragma(warning(push)) \
        __pragma(warning(disable:value))
    #define BC_POP_WARNING() \
        __pragma(warning(pop))
#else
    #define BC_PUSH_WARNING(value)
    #define BC_POP_WARNING()
#endif

// ISO predefined constant for targeted CPU architecture.
#if defined _M_IX86
    #define BC_X86_BUILD
#elif defined _M_X64
    #define BC_X64_BUILD
#elif defined _M_IA64
    #define BC_ITANIUM_BUILD
#endif

#endif
