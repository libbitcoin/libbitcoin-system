/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

// External: Requires that <bitcoin/system.hpp> be included before boost.
// Current alphabetical sort of system.hpp ensures this appears before boost.
// Internal: Placeholders are not currently used in this system library.
// Avoid namespace conflict between boost::placeholders and std::placeholders.
// This arises when including <functional>, which declares std::placeholders.
// This results in an declared symbol when boost includes use placeholders.
// So bracket those includes with an undefine and redefine of this symbol.
#define BOOST_BIND_NO_PLACEHOLDERS

// These are ADL free functions for use with boost-json.
#define DECLARE_JSON_VALUE_CONVERTORS(name) \
BC_API name tag_invoke(boost::json::value_to_tag<name>, \
    const boost::json::value& value) noexcept; \
BC_API void tag_invoke(boost::json::value_from_tag, \
    boost::json::value& value, const name& instance) noexcept

// Define so we can have better visibility of lcov exclusion ranges.
#define LCOV_EXCL_START(text)
#define LCOV_EXCL_STOP()

#ifdef _MSC_VER
    // Suppress C4706: assignment within conditional expression (we allow).
    #pragma warning(disable:4706)

    // Suppress C4459: declaration of 'one' hides global declaration.
    // This arises from boost templates  defining 'one' as a local variable,
    // in their own namespaces. Have found no narrower way to exclude it.
    #pragma warning(disable:4459)
#endif

#ifdef _MSC_VER
    // vs2017 and earlier do not support _Pragma, though it allows macros to
    // parameterize the warning value. Degrade to broader exclusion in vs2017.
    #ifdef BC_VS2019
        #define PRAGMA(pragma) _Pragma(#pragma)
        #define BC_PUSH_MSVC_WARNING(value) \
            PRAGMA(warning(push)) \
            PRAGMA(warning(disable:value))
        #define BC_POP_MSVC_WARNING() \
            PRAGMA(warning(pop))
    #else
        #define BC_PUSH_MSVC_WARNING(value) \
            __pragma(warning(push)) \
            __pragma(warning(disable:value))
        #define BC_POP_MSVC_WARNING() \
            __pragma(warning(pop))
    #endif

#else
    #define BC_PUSH_MSVC_WARNING(warning)
    #define BC_POP_MSVC_WARNING()
#endif

#endif
