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
#ifndef LIBBITCOIN_SYSTEM_MATH_BYTES_HPP
#define LIBBITCOIN_SYSTEM_MATH_BYTES_HPP

#include <cstdint>
#include <bitcoin/system/constraints.hpp>

namespace libbitcoin {
namespace system {

/// Widths.

/// See also std::bit_width (C++20).
/// The number of bytes required to store value, byte_width(0) returns zero.
/// A negative value always returns sizeof(Value).
template <typename Value, if_unsigned_integer<Value> = true>
constexpr size_t byte_width(Value value) noexcept;
template <typename Value, if_signed_integer<Value> = true>
constexpr size_t byte_width(Value value) noexcept;

/// Get the high order bit of high order non-zero byte.
template <typename Integer, if_integer<Integer> = true>
constexpr bool is_negated(Integer value) noexcept;

/// Set high order bit of the value.
template <typename Integer, if_integer<Integer> = true>
constexpr Integer to_negated(Integer value) noexcept;

/// Clear high order bit of high order non-zero byte.
template <typename Integer, if_signed_integer<Integer> = true>
constexpr Integer to_unnegated(Integer value) noexcept;

/// Endianness.

/// Convert a native integral integer to big-endian.
template <typename Integer,
    if_big_endian_integral_integer<Integer> = true>
constexpr Integer to_big_end(Integer from) noexcept;

/// Convert a native integral integer to big-endian.
template <typename Integer,
    if_little_endian_integral_integer<Integer> = true>
constexpr Integer to_big_end(Integer from) noexcept;

/// Convert a native integral integer to little-endian.
template <typename Integer,
    if_little_endian_integral_integer<Integer> = true>
constexpr Integer to_little_end(Integer from) noexcept;

/// Convert a native integral integer to little-endian.
template <typename Integer,
    if_big_endian_integral_integer<Integer> = true>
constexpr Integer to_little_end(Integer from) noexcept;

/// C++23: std::byteswap implementation (but also allows signed integrals).

template <typename Integer,
    if_integral_integer<Integer> = true,
    if_size_of<Integer, sizeof(uint8_t)> = true,
    if_unique_object_representations<Integer> = true>
constexpr Integer byteswap(Integer value) noexcept;

template <typename Integer,
    if_integral_integer<Integer> = true,
    if_size_of<Integer, sizeof(uint16_t)> = true,
    if_unique_object_representations<Integer> = true>
constexpr Integer byteswap(Integer value) noexcept;

template <typename Integer,
    if_integral_integer<Integer> = true,
    if_size_of<Integer, sizeof(uint32_t)> = true,
    if_unique_object_representations<Integer> = true>
constexpr Integer byteswap(Integer value) noexcept;

template <typename Integer,
    if_integral_integer<Integer> = true,
    if_size_of<Integer, sizeof(uint64_t)> = true,
    if_unique_object_representations<Integer> = true>
constexpr Integer byteswap(Integer value) noexcept;

/// Bits to bytes utilities.

template <size_t Bits, if_bytes_width<Bits> = true>
constexpr size_t to_bytes() noexcept;

template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr Integer to_ceilinged_bytes(Integer bits) noexcept;

template <typename Integer, if_unsigned_integer<Integer> = true>
constexpr Integer to_floored_bytes(Integer bits) noexcept;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/math/bytes.ipp>

#endif
