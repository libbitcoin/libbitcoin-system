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

/// Standard includes (do not include directly).
/// All except <array> are included here by include ancestory.
#include <array>            // TODO: purge
#include <cstddef>          // purged
#include <cstdint>          // purged
#include <exception>        // TODO: purge
#include <iostream>         // TODO: purge
#include <limits>           // TODO: purge
#include <stdexcept>        // TODO: purge
#include <string>           // TODO: purge
#include <type_traits>      // TODO: purge
#include <vector>           // TODO: purge

// Pulls chains in all /system headers (except settings.hpp).
#include <bitcoin/system/constraints.hpp>

#if defined(HAVE_MSC)
    #include <windows.h>
#endif

/// Create bc namespace alias.
namespace libbitcoin{ namespace system {} }
namespace bc = libbitcoin;

#define BC_USER_AGENT "/libbitcoin:" LIBBITCOIN_SYSTEM_VERSION "/"

/// LCOV (known symbols for code coverage exclusion).
/// ---------------------------------------------------------------------------
#define LCOV_EXCL_START(comment)
#define LCOV_EXCL_STOP()

/// NDEBUG (conditional exclusion).
/// ---------------------------------------------------------------------------

#if defined(NDEBUG)
    namespace libbitcoin { constexpr auto checked_build = false; };
    #define BC_ASSERT(expression)
    #define BC_ASSERT_MSG(expression, text)
    #define BC_DEBUG_ONLY(expression)
#else
    #include <cassert>
    namespace libbitcoin { constexpr auto checked_build = true; };
    #define BC_RUNTIME(evaluate) if (!std::is_constant_evaluated()) { evaluate; }
    #define BC_ASSERT(expression) BC_RUNTIME(assert(expression))
    #define BC_ASSERT_MSG(expression, text) BC_RUNTIME(assert((expression)&&(text)))
    #define BC_DEBUG_ONLY(expression) expression
#endif

/// Attributes (for platform independence).
/// ---------------------------------------------------------------------------

/// Emit messages from .cpp during compilation.
#if defined(HAVE_MESSAGES)
    #if defined(HAVE_MSC)
        #define DEFINED(text) __pragma(message(text))
    #elif defined(HAVE_GNUC)
        // TODO: This is not working.
        ////#define DO_PRAGMA(text) _Pragma (#text)
        ////#define DEFINED(text) DO_PRAGMA(message (#text))
        #define DEFINED(text)
    #elif defined(HAVE_CLANG)
        // TODO: implement.
        #define DEFINED(text)
    #else
        #define DEFINED(text)
    #endif
#else
    #define DEFINED(text)
#endif

/// See gcc.gnu.org/wiki/Visibility
/// Generic helper definitions for shared library support
/// GNU visibilty attribute overrides compiler flag `fvisibility`.
#if defined(HAVE_WINDOWS_LIBS)
    #define BC_HELPER_DLL_IMPORT __declspec(dllimport)
    #define BC_HELPER_DLL_EXPORT __declspec(dllexport)
    #define BC_HELPER_DLL_LOCAL
#elif defined(HAVE_NX_LIBS)
    #define BC_HELPER_DLL_IMPORT __attribute__((visibility("default")))
    #define BC_HELPER_DLL_EXPORT __attribute__((visibility("default")))
    #define BC_HELPER_DLL_LOCAL  __attribute__((visibility("internal")))
#else
    #define BC_HELPER_DLL_IMPORT
    #define BC_HELPER_DLL_EXPORT
    #define BC_HELPER_DLL_LOCAL
#endif

/// Now we use the generic helper definitions above to define BC_API
/// and BC_INTERNAL. BC_API is used for the public API symbols. It either DLL
/// imports or DLL exports (or does nothing for static build) BC_INTERNAL is
/// used for non-api symbols.
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

/// A stronger compiler hint for inlining.
/// May use prior to 'constexpr' or in place of 'inline'.
/// Do not use in conjunction with with XCONSTEXPR macros, as this will result
/// in a double inline specification in the case of XCONSTEXPR default.
#if defined(HAVE_MSC)
    #define INLINE __forceinline
#elif defined(HAVE_GNUC) || defined(HAVE_CLANG)
    #define INLINE __attribute__((always_inline)) inline
#else
    #define INLINE inline
#endif

/// Class helpers
/// ---------------------------------------------------------------------------

#define DEFAULT5(class_name) \
    class_name(class_name&&) = default; \
    class_name(const class_name&) = default; \
    class_name& operator=(class_name&&) = default; \
    class_name& operator=(const class_name&) = default; \
    ~class_name() = default

/// Destructor must be defaulted.
#define DELETE5(class_name) \
    class_name(class_name&&) = delete; \
    class_name(const class_name&) = delete; \
    class_name& operator=(class_name&&) = delete; \
    class_name& operator=(const class_name&) = delete; \
    inline ~class_name() = default

/// Minimums
/// ---------------------------------------------------------------------------

#if !defined(HAVE_CPP20)
    static_assert(false, "C++20 minimum required.");
#endif

#if defined(HAVE_MSC) && !defined(HAVE_VS2022)
    static_assert(false, "Visual Studio 2022 minimum required.");
#endif

/// Required to access ARM intrinsics in msvc.
#if defined(HAVE_MSC) && defined(HAVE_NEON)
    #define _ARM_USE_NEW_NEON_INTRINSICS
#endif

/// Workarounds for C++ noncompliance.
/// ---------------------------------------------------------------------------

/// C++11 (full)
#if defined(HAVE_NOEXCEPT)
    #define NOEXCEPT noexcept
    #define THROWS noexcept(false)
#else
    #define NOEXCEPT
    #define THROWS
#endif

/// C++14 (full)
#if defined(HAVE_DEPRECATED)
    #define DEPRECATED [[deprecated]]
#else
    #define DEPRECATED
#endif

/// C++17 (full)
#define NODISCARD [[nodiscard]]

/// C++17 (full)
#define FALLTHROUGH [[fallthrough]]

/// C++17 (partial)
#if defined(HAVE_EXECUTION)
    #include <execution>
    #define std_for_each(p, b, e, l) std::for_each((p), (b), (e), (l))
    #define std_transform(p, b, e, t, l) std::transform((p), (b), (e), (t), (l))
    namespace libbitcoin { constexpr auto par_unseq = std::execution::par_unseq; }
    namespace libbitcoin { constexpr auto seq = std::execution::seq; }
#else
    #define std_for_each(p, b, e, l) std::for_each((b), (e), (l))
    #define std_transform(p, b, e, t, l) std::transform((b), (e), (t), (l))
    namespace libbitcoin { constexpr auto par_unseq = false; }
    namespace libbitcoin { constexpr auto seq = false; }
#endif

/// C++20 (partial)
#if defined(HAVE_CONSTEVAL)
    #define CONSTEVAL consteval
#else
    #define CONSTEVAL constexpr
#endif

/// C++20 (partial)
#if defined(HAVE_RANGES)
    #define RCONSTEXPR constexpr
#else
    #define RCONSTEXPR inline
#endif

/// C++20 (partial)
#if defined(HAVE_VECTOR_CONSTEXPR)
    #define VCONSTEXPR constexpr
#else
    #define VCONSTEXPR inline
#endif

/// C++20 (partial)
#if defined(HAVE_STRING_CONSTEXPR)
    #define SCONSTEXPR constexpr
#else
    #define SCONSTEXPR inline
#endif

/// C++20 (partial)
#if defined(HAVE_STRING_CONSTEXPR) && defined(HAVE_RANGES)
    #define SRCONSTEXPR constexpr
#else
    #define SRCONSTEXPR inline
#endif

/// C++20 (partial)
#if defined(HAVE_STRING_CONSTEXPR) && defined(HAVE_VECTOR_CONSTEXPR)
    #define SVCONSTEXPR constexpr
#else
    #define SVCONSTEXPR inline
#endif

#endif
