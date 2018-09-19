/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_COMPAT_HPP
#define LIBBITCOIN_COMPAT_HPP

#include <cstdint>
#include <limits>

// CTP_Nov2013 implements noexcept but unfortunately VC12 and CTP_Nov2013
// both identify as _MSC_VER = 1800, otherwise we could include CTP_Nov2013.
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
    #define BC_NOEXCEPT _NOEXCEPT
    #define BC_CONSTEXPR const
    #define BC_CONSTFUNC inline
    #define BC_CONSTCTOR
#else
    #define BC_NOEXCEPT noexcept
    #define BC_CONSTEXPR constexpr
    #define BC_CONSTFUNC constexpr
    #define BC_CONSTCTOR constexpr
#endif

// TODO: prefix names with BC_
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
    #define MIN_INT64 INT64_MIN
    #define MAX_INT64 INT64_MAX
    #define MIN_INT32 INT32_MIN
    #define MAX_INT32 INT32_MAX
    #define MAX_UINT64 UINT64_MAX
    #define MAX_UINT32 UINT32_MAX
    #define MAX_UINT16 UINT16_MAX
    #define MAX_UINT8 UINT8_MAX
    #define BC_MAX_SIZE SIZE_MAX
#else
    #define MIN_INT64 std::numeric_limits<int64_t>::min()
    #define MAX_INT64 std::numeric_limits<int64_t>::max()
    #define MIN_INT32 std::numeric_limits<int32_t>::min()
    #define MAX_INT32 std::numeric_limits<int32_t>::max()
    #define MAX_UINT64 std::numeric_limits<uint64_t>::max()
    #define MAX_UINT32 std::numeric_limits<uint32_t>::max()
    #define MAX_UINT16 std::numeric_limits<uint16_t>::max()
    #define MAX_UINT8 std::numeric_limits<uint8_t>::max()
    #define BC_MAX_SIZE std::numeric_limits<size_t>::max()
#endif

#endif
