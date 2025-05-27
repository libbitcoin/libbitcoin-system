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

namespace libbitcoin
{
#if defined(NDEBUG)
    constexpr auto build_checked = false;
#else
    constexpr auto build_checked = true;
#endif


#if defined(HAVE_X32) || defined(HAVE_ARM32)
    constexpr auto have_32b = true;
#else
    constexpr auto have_32b = false;
#endif

#if defined(HAVE_X64) || defined(HAVE_ARM64)
    constexpr auto have_64b = true;
#else
    constexpr auto have_64b = false;
#endif

#ifdef HAVE_XCPU
    constexpr auto have_xcpu = true;
#else
    constexpr auto have_xcpu = false;
#endif

#ifdef HAVE_ARM
    constexpr auto have_arm = true;
#else
    constexpr auto have_arm = false;
#endif

#if defined(HAVE_128)
    constexpr auto have_128 = true;
#else
    constexpr auto have_128 = false;
#endif
    
#if defined(HAVE_256)
    constexpr auto have_256 = true;
#else
    constexpr auto have_256 = false;
#endif

#if defined(HAVE_512)
    constexpr auto have_512 = true;
#else
    constexpr auto have_512 = false;
#endif

#if defined(HAVE_SHA)
    constexpr auto have_sha = true;
#else
    constexpr auto have_sha = false;
#endif

} // namespace libbitcoin

/// Create bc namespace alias.
namespace bc = libbitcoin;

#endif
