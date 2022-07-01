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

// Standard includes (do not include directly).
// All except <array> are included here by include ancestory.
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
#include <type_traits>

// Pulls chains in all /system headers (except settings.hpp).
#include <bitcoin/system/constraints.hpp>

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
#if defined(HAVE_MSC) || defined(HAVE_CYGWIN)
    #define BC_HELPER_DLL_IMPORT __declspec(dllimport)
    #define BC_HELPER_DLL_EXPORT __declspec(dllexport)
    #define BC_HELPER_DLL_LOCAL
#elif defined(HAVE_GNUC)
    #define BC_HELPER_DLL_IMPORT __attribute__((visibility("default")))
    #define BC_HELPER_DLL_EXPORT __attribute__((visibility("default")))
    #define BC_HELPER_DLL_LOCAL  __attribute__((visibility("internal")))
#else
    #define BC_HELPER_DLL_IMPORT
    #define BC_HELPER_DLL_EXPORT
    #define BC_HELPER_DLL_LOCAL
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

#if defined(HAVE_MSC) && !defined(BC_VS2022)
    static_assert(false, "Visual Studio 2022 minimum required.");
#endif

#if !defined(BC_CPP_20)
    static_assert(false, "C++20 minimum required.");
#endif
    
// deprecated is noisy, turn on to find dependencies.
#define DEPRECATED
////#define DEPRECATED [[deprecated]]
#define NODISCARD [[nodiscard]]
#define FALLTHROUGH [[fallthrough]]

// Define so we can have better visibility of lcov exclusion ranges.
#define LCOV_EXCL_START(text)
#define LCOV_EXCL_STOP()

#endif
