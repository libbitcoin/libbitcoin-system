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
#ifndef LIBBITCOIN_SYSTEM_PREPROCESSOR_HPP
#define LIBBITCOIN_SYSTEM_PREPROCESSOR_HPP

// Pulls in all /system headers (except settings.hpp).
#include <bitcoin/system/have.hpp>

#define BC_USER_AGENT "/libbitcoin:" LIBBITCOIN_SYSTEM_VERSION "/"

/// LCOV (known symbols for code coverage exclusion).
/// ---------------------------------------------------------------------------
#define LCOV_EXCL_START(comment)
#define LCOV_EXCL_STOP()

/// Debugging.
/// ---------------------------------------------------------------------------

/// Enables ETW tracing for VS memory profiler.
#if defined(HAVE_MSC)
    #define ALLOCATOR __declspec(allocator)
#else
    #define ALLOCATOR
#endif

/// learn.microsoft.com/en-us/cpp/c-runtime-library/
/// find-memory-leaks-using-the-crt-library
/// _CRTDBG_MAP_ALLOC must be prior to the includes, so defined in props.
#if defined(HAVE_MSC) && defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC)
    #include <stdlib.h>
    #include <crtdbg.h>
    ////_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

/// NDEBUG (conditional exclusion).
#if defined(NDEBUG)
    #define BC_ASSERT(expression)
    #define BC_ASSERT_MSG(expression, text)
    #define BC_DEBUG_ONLY(expression)
#else
    #include <cassert>
    #define BC_ASSERT(expression) assert(expression)
    #define BC_ASSERT_MSG(expression, text) assert((expression)&&(text))
    #define BC_DEBUG_ONLY(expression) expression
#endif

/// Messages.
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

/// Symbols.
/// ---------------------------------------------------------------------------

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

/// Inlines.
/// ---------------------------------------------------------------------------

/// A stronger compiler hint for inlining.
/// May use prior to 'constexpr' or in place of 'inline'.
/// Disable msc/x32 due to high level of rejection (warning volume).
#if defined(HAVE_MSC) && !defined(HAVE_X32)
    #define INLINE __forceinline
#elif defined(HAVE_GNUC) || defined(HAVE_CLANG)
    #define INLINE __attribute__((always_inline)) inline
#else
    #define INLINE inline
#endif

/// Class helpers.
/// ---------------------------------------------------------------------------
/// Virtual concrete classes should define public virtual destructor.
/// Abstract base classes may define protected non-virtual destructor.
/// Non-virtual (final) classes may define public non-virtual destructor.
/// Pure virtual base classes do not define destructor.

#define DELETE_COPY(name) \
    name(const name&) = delete; \
    name& operator=(const name&) = delete

#define DELETE_MOVE(name) \
    name(name&&) = delete; \
    name& operator=(name&&) = delete

#define DELETE_COPY_MOVE(name) \
    DELETE_COPY(name); \
    DELETE_MOVE(name)

#define DEFAULT_COPY(name) \
    name(const name&) = default; \
    name& operator=(const name&) = default

#define DEFAULT_MOVE(name) \
    name(name&&) = default; \
    name& operator=(name&&) = default

#define DEFAULT_COPY_MOVE(name) \
    DEFAULT_COPY(name); \
    DEFAULT_MOVE(name)

#define DELETE_COPY_MOVE_DESTRUCT(name) \
    DELETE_COPY_MOVE(name); \
    virtual ~name() = default

#define DEFAULT_COPY_MOVE_DESTRUCT(name) \
    DEFAULT_COPY_MOVE(name); \
    virtual ~name() = default

/// Minimums.
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
/// None on xcode.
/// Requires link with -ltbb on gcc (v9).
/// Experimental on clang (libcxx.llvm.org/Status/PSTL.html), requires:
/// -fexperimental-library
/// using poolstl, do not include <execution>.
#include <bitcoin/system/execution.hpp>

/// C++20 (partial)
#if defined(HAVE_CONSTEVAL)
    #define CONSTEVAL consteval
#else
    #define CONSTEVAL constexpr
#endif

#endif
