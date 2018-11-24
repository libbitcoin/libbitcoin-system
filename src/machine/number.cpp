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
#include <bitcoin/system/machine/number.hpp>

#include <cstdint>

namespace libbitcoin {
namespace system {
namespace machine {

const uint8_t number::negative_1 = negative_mask | positive_1;
const uint8_t number::negative_0 = negative_mask | positive_0;
const uint8_t number::positive_0 = 0;
const uint8_t number::positive_1 = 1;
const uint8_t number::positive_2 = 2;
const uint8_t number::positive_3 = 3;
const uint8_t number::positive_4 = 4;
const uint8_t number::positive_5 = 5;
const uint8_t number::positive_6 = 6;
const uint8_t number::positive_7 = 7;
const uint8_t number::positive_8 = 8;
const uint8_t number::positive_9 = 9;
const uint8_t number::positive_10 = 10;
const uint8_t number::positive_11 = 11;
const uint8_t number::positive_12 = 12;
const uint8_t number::positive_13 = 13;
const uint8_t number::positive_14 = 14;
const uint8_t number::positive_15 = 15;
const uint8_t number::positive_16 = 16;
const uint8_t number::negative_mask = 0x80;

} // namespace machine
} // namespace system
} // namespace libbitcoin
