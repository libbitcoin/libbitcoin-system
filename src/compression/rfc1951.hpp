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
#ifndef LIBBITCOIN_SYSTEM_SRC_COMPRESSION_RFC1951_HPP
#define LIBBITCOIN_SYSTEM_SRC_COMPRESSION_RFC1951_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// rfc-editor.org/rfc/rfc1951.txt

namespace libbitcoin {
namespace system {
namespace rfc1951 {

// Block types (rfc1951 3.2.3).
constexpr uint32_t stored_block = 0;
constexpr uint32_t fixed_block = 1;
constexpr uint32_t dynamic_block = 2;

// Code space (rfc1951 3.2.5, 3.2.7).
constexpr size_t maximum_bits = 15;
constexpr size_t maximum_length_bits = 7;
constexpr size_t literal_codes = 288;
constexpr size_t literal_limit = 286;
constexpr size_t distance_codes = 30;
constexpr size_t length_codes = 19;
constexpr uint16_t end_of_block = 256;
constexpr uint16_t first_length = 257;
constexpr uint16_t repeat_previous = 16;
constexpr uint16_t repeat_zeros = 17;
constexpr uint16_t repeat_long_zeros = 18;

// Header field widths and minimums (rfc1951 3.2.7).
constexpr size_t literal_count_bits = 5;
constexpr size_t distance_count_bits = 5;
constexpr size_t length_count_bits = 4;
constexpr size_t code_length_bits = 3;
constexpr size_t minimum_length_count = 4;

// Match bounds (rfc1951 3.2.5).
constexpr size_t minimum_match = 3;
constexpr size_t maximum_match = 258;
constexpr size_t window_size = 32768;

// Stored block length limit (rfc1951 3.2.4).
constexpr size_t maximum_stored = max_uint16;

// Lengths 257..285 (rfc1951 3.2.5).
constexpr std_array<uint16_t, 29> length_base
{
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51,
    59, 67, 83, 99, 115, 131, 163, 195, 227, 258
};

constexpr std_array<uint8_t, 29> length_extra
{
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4,
    4, 5, 5, 5, 5, 0
};

// Distances 0..29 (rfc1951 3.2.5).
constexpr std_array<uint16_t, distance_codes> distance_base
{
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385,
    513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385,
    24577
};

constexpr std_array<uint8_t, distance_codes> distance_extra
{
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10,
    10, 11, 11, 12, 12, 13, 13
};

// Code length repeat extra bits and minimums (rfc1951 3.2.7).
constexpr std_array<uint8_t, 3> repeat_extra{ 2, 3, 7 };
constexpr std_array<uint8_t, 3> repeat_base{ 3, 3, 11 };

// Order of the code length code lengths (rfc1951 3.2.7).
constexpr std_array<uint8_t, length_codes> length_order
{
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

// Fixed literal code lengths by symbol range (rfc1951 3.2.6).
constexpr std_array<uint16_t, 4> fixed_ends{ 144, 256, 280, 288 };
constexpr std_array<uint8_t, 4> fixed_lengths{ 8, 9, 7, 8 };
constexpr uint8_t fixed_distance_length = 5;

} // namespace rfc1951
} // namespace system
} // namespace libbitcoin

#endif
