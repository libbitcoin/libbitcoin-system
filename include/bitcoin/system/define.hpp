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
/// All except 'array' are included here by include ancestory.
#include <array>            // purged
#include <cstddef>          // purged
#include <cstdint>          // purged
#include <functional>       // purged
#include <istream>          // purged
#include <ostream>          // purged
#include <iostream>         // purged
#include <limits>           // purged
#include <stdexcept>        // purged
#include <string>           // purged
#include <type_traits>      // purged
#include <vector>           // purged

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

#endif
