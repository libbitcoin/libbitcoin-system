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
#ifndef LIBBITCOIN_SYSTEM_DEFINE_HPP
#define LIBBITCOIN_SYSTEM_DEFINE_HPP

// Other #define declarations here.
#include <bitcoin/system/version.hpp>
#include <bitcoin/system/warnings.hpp>

// Create bc namespace alias.
namespace libbitcoin {
namespace system {
} // namespace system
} // namespace libbitcoin

namespace bc = libbitcoin;

#define BC_USER_AGENT "/libbitcoin:" LIBBITCOIN_SYSTEM_VERSION "/"

#ifdef NDEBUG
    #define BC_ASSERT(expression)
    #define BC_ASSERT_MSG(expression, text)
    #define BC_DEBUG_ONLY(expression)
#else
    #include <cassert>
    #define BC_ASSERT(expression) assert(expression)
    #define BC_ASSERT_MSG(expression, text) assert((expression)&&(text))
    #define BC_DEBUG_ONLY(expression) expression
#endif

// See gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
// GNU visibilty attribute overrides compiler flag `fvisibility`.
#if defined _MSC_VER || defined __CYGWIN__
    #define BC_HELPER_DLL_IMPORT __declspec(dllimport)
    #define BC_HELPER_DLL_EXPORT __declspec(dllexport)
    #define BC_HELPER_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define BC_HELPER_DLL_IMPORT __attribute__((visibility("default")))
        #define BC_HELPER_DLL_EXPORT __attribute__((visibility("default")))
        #define BC_HELPER_DLL_LOCAL  __attribute__((visibility("internal")))
    #else
        #define BC_HELPER_DLL_IMPORT
        #define BC_HELPER_DLL_EXPORT
        #define BC_HELPER_DLL_LOCAL
    #endif
#endif

// Now we use the generic helper definitions above to define BC_API
// and BC_INTERNAL. BC_API is used for the public API symbols. It either DLL
// imports or DLL exports (or does nothing for static build) BC_INTERNAL is
// used for non-api symbols.

#if defined BC_STATIC
    #define BC_API
    #define BC_INTERNAL
#elif defined BC_DLL
    #define BC_API      BC_HELPER_DLL_EXPORT
    #define BC_INTERNAL BC_HELPER_DLL_LOCAL
#else
    #define BC_API      BC_HELPER_DLL_IMPORT
    #define BC_INTERNAL BC_HELPER_DLL_LOCAL
#endif

// Tag to deprecate functions and methods.
// Gives a compiler warning when they are used.
#if defined _MSC_VER || defined __CYGWIN__
    #define BC_DEPRECATED __declspec(deprecated)
#else
    #if __GNUC__ >= 4
        #define BC_DEPRECATED __attribute__((deprecated))
    #else
        #define BC_DEPRECATED
    #endif
#endif

// ISO predefined constant for targeted CPU architecture.
#if defined _M_IX86
    #define BC_X86_BUILD
#elif defined _M_X64
    #define BC_X64_BUILD
#elif defined _M_IA64
    #define BC_ITANIUM_BUILD
#endif

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

#if !defined(BC_CPP_20)
    static_assert(false, "C++20 minimum required.");
#endif

// MSFT predefined constant for VS version.
#if _MSC_VER >= 1800
    #define BC_VS2013
#endif
#if _MSC_VER >= 1900
    #define BC_VS2015
#endif
#if _MSC_VER >= 1910
    #define BC_VS2017
#endif
#if _MSC_VER >= 1920
    #define BC_VS2019
#endif
#if _MSC_VER >= 1930
    #define BC_VS2022
#endif

#if defined(_MSC_VER) && !defined(BC_VS2022)
    static_assert(false, "Visual Studio 2022 minimum required.");
#endif

// clang does not yet have consteval.
#if defined(BC_VS2022) && defined(BC_CPP_20)
    #define CONSTEVAL consteval
    #define HAVE_CONSTEVAL
#else
    #define CONSTEVAL constexpr
#endif

#define NOEXCEPT noexcept
#define THROWS noexcept(false)

// Define so we can have better visibility of lcov exclusion ranges.
#define LCOV_EXCL_START(text)
#define LCOV_EXCL_STOP()

#endif
