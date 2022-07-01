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
#ifndef LIBBITCOIN_SYSTEM_CONSTANTS_HPP
#define LIBBITCOIN_SYSTEM_CONSTANTS_HPP

/// DELETECSTDDEF
/// DELETECSTDINT
#include <limits>
#include <bitcoin/system/types.hpp>

namespace libbitcoin {

/// C++20: all signed types require two's complement negative representation.

/// Use zero, one, two when the unsigned value is required.
/// Literals are used below to prevent sign (unsigned) promotion.
constexpr size_t zero = 0;
constexpr size_t one = 1;
constexpr size_t two = 2;

/// The number of bits in a byte (uint8_t).
constexpr uint8_t byte_bits = 8;

/// Use negative_one when returning negative as a sentinel value.
constexpr int8_t negative_one = -1;

/// Variable integer prefix sentinels.
constexpr uint8_t varint_two_bytes = 0xfd;
constexpr uint8_t varint_four_bytes = 0xfe;
constexpr uint8_t varint_eight_bytes = 0xff;

/// Endianness.
constexpr auto is_big_endian = std::endian::native == std::endian::big;
constexpr auto is_little_endian = std::endian::native == std::endian::little;
constexpr auto is_unknown_endian = !is_big_endian && !is_little_endian;
static_assert(!is_unknown_endian, "unsupported integer representation");

/// Signed max.
constexpr auto max_signed_size_t = std::numeric_limits<signed_size_t>::max();
constexpr auto max_int64 = std::numeric_limits<int64_t>::max();
constexpr auto max_int32 = std::numeric_limits<int32_t>::max();
constexpr auto max_int16 = std::numeric_limits<int16_t>::max();
constexpr auto max_int8 = std::numeric_limits<int8_t>::max();

/// Signed min.
constexpr auto min_signed_size_t = std::numeric_limits<signed_size_t>::min();
constexpr auto min_int64 = std::numeric_limits<int64_t>::min();
constexpr auto min_int32 = std::numeric_limits<int32_t>::min();
constexpr auto min_int16 = std::numeric_limits<int16_t>::min();
constexpr auto min_int8 = std::numeric_limits<int8_t>::min();

/// Unsigned max.
constexpr auto max_size_t = std::numeric_limits<size_t>::max();
constexpr auto max_uint64 = std::numeric_limits<uint64_t>::max();
constexpr auto max_uint32 = std::numeric_limits<uint32_t>::max();
constexpr auto max_uint16 = std::numeric_limits<uint16_t>::max();
constexpr auto max_uint8 = std::numeric_limits<uint8_t>::max();

/// Unsigned min.
constexpr auto min_size_t = std::numeric_limits<size_t>::min();
constexpr auto min_uint64 = std::numeric_limits<uint64_t>::min();
constexpr auto min_uint32 = std::numeric_limits<uint32_t>::min();
constexpr auto min_uint16 = std::numeric_limits<uint16_t>::min();
constexpr auto min_uint8 = std::numeric_limits<uint8_t>::min();

} // namespace libbitcoin

#endif
