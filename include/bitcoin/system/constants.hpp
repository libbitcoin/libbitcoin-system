/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

#include <bit>
#include <bitcoin/system/types.hpp>

namespace libbitcoin {

#if defined(NDEBUG)
    constexpr auto build_checked = false;
#else
    constexpr auto build_checked = true;
#endif

#if defined(HAVE_X32) || defined(HAVE_ARM32)
    constexpr auto have_32b = true;
    constexpr auto have_64b = false;
#else
    constexpr auto have_32b = false;
    constexpr auto have_64b = true;
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

#if defined(HAVE_FAST_MATH)
    constexpr auto have_fast_math = true;
#else
    constexpr auto have_fast_math = false;
#endif

/// C++20: all signed types require two's complement negative representation.

/// Use zero, one, two when the unsigned value is required.
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

/// Floating point (iec559 requires correctly-rounded arithmetic).
constexpr auto is_iec559_float = std::numeric_limits<float>::is_iec559;
constexpr auto is_iec559_double = std::numeric_limits<double>::is_iec559;
constexpr auto is_iec559 = is_iec559_float && is_iec559_double;
static_assert(is_iec559, "unsupported floating point representation");

/// Fast math discards correct rounding (iec559 remains reported).
static_assert(!have_fast_math, "unsupported floating point arithmetic");

/// Signed max.
constexpr int8_t  max_int8  = std::numeric_limits<int8_t>::max();
constexpr int16_t max_int16 = std::numeric_limits<int16_t>::max();
constexpr int32_t max_int32 = std::numeric_limits<int32_t>::max();
constexpr int64_t max_int64 = std::numeric_limits<int64_t>::max();
constexpr signed_size_t max_signed_size_t =
std::numeric_limits<signed_size_t>::max();

/// Signed min.
constexpr int8_t  min_int8  = std::numeric_limits<int8_t>::min();
constexpr int16_t min_int16 = std::numeric_limits<int16_t>::min();
constexpr int32_t min_int32 = std::numeric_limits<int32_t>::min();
constexpr int64_t min_int64 = std::numeric_limits<int64_t>::min();
constexpr signed_size_t min_signed_size_t =
std::numeric_limits<signed_size_t>::min();

/// Unsigned max.
constexpr uint8_t  max_uint8  = std::numeric_limits<uint8_t>::max();
constexpr uint16_t max_uint16 = std::numeric_limits<uint16_t>::max();
constexpr uint32_t max_uint32 = std::numeric_limits<uint32_t>::max();
constexpr uint64_t max_uint64 = std::numeric_limits<uint64_t>::max();
constexpr size_t   max_size_t = std::numeric_limits<size_t>::max();

/// Unsigned min.
constexpr uint8_t  min_uint8  = std::numeric_limits<uint8_t>::min();
constexpr uint16_t min_uint16 = std::numeric_limits<uint16_t>::min();
constexpr uint32_t min_uint32 = std::numeric_limits<uint32_t>::min();
constexpr uint64_t min_uint64 = std::numeric_limits<uint64_t>::min();
constexpr size_t   min_size_t = std::numeric_limits<size_t>::min();

} // namespace libbitcoin

#endif
