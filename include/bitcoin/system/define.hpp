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
#ifndef LIBBITCOIN_SYSTEM_DEFINE_HPP
#define LIBBITCOIN_SYSTEM_DEFINE_HPP

/// Standard includes (do not include directly).
/// All except <array> are included here by include ancestory.
#include <array>            // TODO: purge
#include <cstddef>          // purged
#include <cstdint>          // purged
#include <functional>       // purged
#include <iostream>         // TODO: purge
#include <limits>           // TODO: purge
#include <stdexcept>        // TODO: purge
#include <string>           // TODO: purge
#include <type_traits>      // TODO: purge
#include <vector>           // TODO: purge

// Pulls in all /system headers (except settings.hpp).
#include <bitcoin/system/constraints.hpp>

#if defined(HAVE_MSC)
    #include <windows.h>
#endif

/// Create bc namespace alias.
namespace libbitcoin{ namespace system {} }
namespace bc = libbitcoin;

/// NDEBUG (conditional exclusion).
/// ---------------------------------------------------------------------------

#if defined(NDEBUG)
    namespace libbitcoin { constexpr auto build_checked = false; };
#else
    namespace libbitcoin { constexpr auto build_checked = true; };
#endif

/// Major platform.
/// ---------------------------------------------------------------------------

#if defined(HAVE_X32)
    namespace libbitcoin { constexpr auto build_x32 = true; };
#else
    namespace libbitcoin { constexpr auto build_x32 = false; };
#endif

#if defined(HAVE_X64)
    namespace libbitcoin { constexpr auto build_x64 = true; };
#else
    namespace libbitcoin { constexpr auto build_x64 = false; };
#endif

#ifdef HAVE_XCPU
    namespace libbitcoin { constexpr auto with_xcpu = true; };
#else
    namespace libbitcoin { constexpr auto with_xcpu = false; };
#endif

#ifdef HAVE_ARM
    namespace libbitcoin { constexpr auto with_arm = true; };
#else
    namespace libbitcoin { constexpr auto with_arm = false; };
#endif

/// Workarounds for C++ noncompliance.
/// ---------------------------------------------------------------------------

/// C++17 (partial), see HAVE_EXECUTION.
#if defined(HAVE_EXECUTION)
    namespace libbitcoin { constexpr auto par_unseq = std::execution::par_unseq; }
    namespace libbitcoin { constexpr auto seq = std::execution::seq; }
#else
    namespace libbitcoin { constexpr auto par_unseq = false; }
    namespace libbitcoin { constexpr auto seq = false; }
#endif

#endif
