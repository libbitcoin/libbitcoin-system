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
#ifndef LIBBITCOIN_SYSTEM_MATH_BYTES_IPP
#define LIBBITCOIN_SYSTEM_MATH_BYTES_IPP

#include <cstdint>
#include <type_traits>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/external/byte_swap.hpp>

namespace libbitcoin {
namespace system {

template <typename Integer,
    if_integral_integer<Integer>,
    if_size_of<Integer, sizeof(uint8_t)>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    // no-op for calling consistency.
    return value;
}

template <typename Integer,
    if_integral_integer<Integer>,
    if_size_of<Integer, sizeof(uint16_t)>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    // Compiles away to direct API call for non-constexpr.
    return possible_sign_cast<Integer>(std::is_constant_evaluated() ?
        byte_swap16_native(to_unsigned(value)) :
        byte_swap16(to_unsigned(value)));
}

template <typename Integer,
    if_integral_integer<Integer>,
    if_size_of<Integer, sizeof(uint32_t)>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    // Compiles away to direct API call for non-constexpr.
    return possible_sign_cast<Integer>(std::is_constant_evaluated() ?
        byte_swap32_native(to_unsigned(value)) :
        byte_swap32(to_unsigned(value)));
}

template <typename Integer,
    if_integral_integer<Integer>,
    if_size_of<Integer, sizeof(uint64_t)>,
    if_unique_object_representations<Integer>>
constexpr Integer byteswap(Integer value) noexcept
{
    // Compiles away to direct API call for non-constexpr.
    return possible_sign_cast<Integer>(std::is_constant_evaluated() ?
        byte_swap64_native(to_unsigned(value)) :
        byte_swap64(to_unsigned(value)));
}

} // namespace system
} // namespace libbitcoin

#endif
