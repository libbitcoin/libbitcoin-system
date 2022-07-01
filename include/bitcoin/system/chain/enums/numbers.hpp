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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_ENUMS_NUMBERS_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_ENUMS_NUMBERS_HPP

/// DELETECSTDINT
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

// These are STACK representations of the numbers -1..16.
// This differs from integral numbers only for negative_1.
enum numbers : uint8_t
{
    // b10000001 : -1 is represented as 0x01 | 0x80 sign bit.
    negative_1 = bit_or<uint8_t>(0x80, 0x01),       // [0x81]
    number_0 = 0,                                   // [0x00]
    positive_1 = 1,                                 // [0x01]
    positive_2 = 2,                                 // [0x02]
    positive_3 = 3,                                 // [0x03]
    positive_4 = 4,                                 // [0x04]
    positive_5 = 5,                                 // [0x05]
    positive_6 = 6,                                 // [0x06]
    positive_7 = 7,                                 // [0x07]
    positive_8 = 8,                                 // [0x08]
    positive_9 = 9,                                 // [0x09]
    positive_10 = 10,                               // [0x0a]
    positive_11 = 11,                               // [0x0b]
    positive_12 = 12,                               // [0x0c]
    positive_13 = 13,                               // [0x0d]
    positive_14 = 14,                               // [0x0e]
    positive_15 = 15,                               // [0x0f]
    positive_16 = 16,                               // [0x10]
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
