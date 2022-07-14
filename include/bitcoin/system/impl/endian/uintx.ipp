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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_UINTX_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_UINTX_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/minimal.hpp>
#include <bitcoin/system/endian/nominal.hpp>

namespace libbitcoin {
namespace system {

// data => uintx
// uintx => data_chunk

// Overload for uintx, as from_big_endian<0> reads zero bytes and uintx is a
// signed type (though otherwise would be declared as uintx_t<0>).
// data size determined by uintx (vs. type).

inline uintx from_big_endian(const data_chunk& data) NOEXCEPT
{
    return from_big_chunk<uintx>(data.size(), data);
}

inline uintx from_little_endian(const data_chunk& data) NOEXCEPT
{
    return from_little_chunk<uintx>(data.size(), data);
}

inline data_chunk to_big_endian(const uintx& value) NOEXCEPT
{
    // minimal (chunk - required by unitx variable size).
    // to_big_endian_size<> implies array, otherwise chunk.
    return to_big_endian_size(value);
}

inline data_chunk to_little_endian(const uintx& value) NOEXCEPT
{
    // minimal (chunk - required by unitx variable size).
    // to_little_endian_size<> implies array, otherwise chunk.
    return to_little_endian_size(value);
}

} // namespace system
} // namespace libbitcoin

#endif
