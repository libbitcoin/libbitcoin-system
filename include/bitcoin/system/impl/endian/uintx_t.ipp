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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_UINTX_T_IPP
#define LIBBITCOIN_SYSTEM_ENDIAN_UINTX_T_IPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/minimal.hpp>
#include <bitcoin/system/endian/nominal.hpp>

namespace libbitcoin {
namespace system {

// data<Bytes> <=> uintx_t<Bits>
// ----------------------------------------------------------------------------

template <uintx_size_t Bits>
constexpr data_array<to_ceilinged_bytes(Bits)>
from_uintx(const uintx_t<Bits>& value) NOEXCEPT
{
    return to_little_endian_size<to_ceilinged_bytes(Bits)>(value);
}

template <size_t Bytes>
constexpr uintx_t<to_bits<uintx_size_t>(Bytes)>
to_uintx(const data_array<Bytes>& hash) NOEXCEPT
{
    return uintx_from_little_endian_array<Bytes>(hash);
}

// data<Bytes> => uintx_t<to_bits(Bytes)>
// ----------------------------------------------------------------------------

template <size_t Size>
constexpr uintx_t<to_bits(Size)> uintx_from_big_endian_array(
    const data_array<Size>& data) NOEXCEPT
{
    return from_big_array<uintx_t<to_bits(Size)>>(data);
}

template <size_t Size>
constexpr uintx_t<to_bits(Size)> uintx_from_little_endian_array(
    const data_array<Size>& data) NOEXCEPT
{
    return from_little_array<uintx_t<to_bits(Size)>>(data);
}

template <size_t Size>
VCONSTEXPR uintx_t<to_bits(Size)> uintx_from_big_endian_chunk(
    const data_chunk& data) NOEXCEPT
{
    return from_big_chunk<uintx_t<to_bits(Size)>>(Size, data);
}

template <size_t Size>
VCONSTEXPR uintx_t<to_bits(Size)> uintx_from_little_endian_chunk(
    const data_chunk& data) NOEXCEPT
{
    return from_little_chunk<uintx_t<to_bits(Size)>>(Size, data);
}

} // namespace system
} // namespace libbitcoin

#endif
