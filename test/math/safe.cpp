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
#include "../test.hpp"
#include <cstddef>
#include <cstdint>

BOOST_AUTO_TEST_SUITE(limits_tests)

constexpr size_t minimum = 0;
constexpr size_t maximum = max_size_t;
constexpr size_t half = maximum / 2;

// Down-casting a negative loses the sign bit.
static_assert(static_cast<int32_t>(min_int64) == 0);

constexpr auto right = narrow_cast<int32_t>(min_int64);
static_assert(right == min_int64);
static_assert(narrow_cast<int32_t>(min_int64) != min_int64);

// Verify compiler "usual arithmetic conversion" expectations.
// ----------------------------------------------------------------------------
// Shift works like a unary operator (right operand is not incorporated).
// Order of operands does not affect (other) binary operators.
// std::common_type does not support unary operators.
// std::common_type<Left, Right>::type == decltype(left - right)
// std::common_type<Value>::type != decltype(-value)

// Compiler warns on bool safety for -b, ~b, <<b, >>b, /b, %b, b/, b% (assertions disabled).
using integer = decltype(-'a');
using character = std::common_type_t<char>;
static_assert(!std::is_same_v<integer, character>);

// negate
static_assert(std::is_same<decltype(-char{0}),                   int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(-int8_t{0}),                 int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(-int16_t{0}),                int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(-int32_t{0}),                int32_t>::value,  "unpromoted (>=32");
static_assert(std::is_same<decltype(-int64_t{0}),                int64_t>::value,  "unpromoted (>=32");
////static_assert(std::is_same<decltype(-bool{false}),               int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(-uint8_t{0}),                int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(-uint16_t{0}),               int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(-uint32_t{0}),               uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(-uint64_t{0}),               uint64_t>::value, "unpromoted (>=32");

// ones_complement
static_assert(std::is_same<decltype(~char{0}),                   int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~int8_t{0}),                 int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~int16_t{0}),                int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~int32_t{0}),                int32_t>::value,  "unpromoted (>=32");
static_assert(std::is_same<decltype(~int64_t{0}),                int64_t>::value,  "unpromoted (>=32");
////static_assert(std::is_same<decltype(~bool{false}),               int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~uint8_t{0}),                int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~uint16_t{0}),               int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(~uint32_t{0}),               uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(~uint64_t{0}),               uint64_t>::value, "unpromoted (>=32");

// shift_left
static_assert(std::is_same<decltype(uint8_t{0}  << char{0}),     int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  << uint8_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  << uint16_t{0}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint16_t{0} << uint32_t{0}), int >::value,     "promoted   (<32)");
static_assert(std::is_same<decltype(uint32_t{0} << uint64_t{0}), uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(uint64_t{0} << uint64_t{0}), uint64_t>::value, "unpromoted (>=32");
////static_assert(std::is_same<decltype(uint8_t{0}  << bool{false}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(bool{false} << uint32_t{0}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  << int8_t{0}),   int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  << int16_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint16_t{0} << int32_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint32_t{0} << int64_t{0}),  uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(int64_t{0}  << uint32_t{0}), int64_t>::value,  "unpromoted (>=32");

// shift_right
static_assert(std::is_same<decltype(uint8_t{0}  >> char{0}),     int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  >> uint8_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  >> uint16_t{0}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint16_t{0} >> uint32_t{0}), int >::value,     "promoted   (<32)");
static_assert(std::is_same<decltype(uint32_t{0} >> uint64_t{0}), uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(uint64_t{0} >> uint64_t{0}), uint64_t>::value, "unpromoted (>=32");
////static_assert(std::is_same<decltype(uint8_t{0}  >> bool{false}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(bool{false} >> uint32_t{0}), int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  >> int8_t{0}),   int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint8_t{0}  >> int16_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint16_t{0} >> int32_t{0}),  int>::value,      "promoted   (<32)");
static_assert(std::is_same<decltype(uint32_t{0} >> int64_t{0}),  uint32_t>::value, "unpromoted (>=32");
static_assert(std::is_same<decltype(int64_t{0}  >> uint32_t{0}), int64_t>::value,  "unpromoted (>=32");

// bit_and
static_assert(std::is_same<decltype(uint8_t{0}  & char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  & uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  & uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} & uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} & uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} & uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  & bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  & int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  & int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} & int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} & int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  & uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// bit_or
static_assert(std::is_same<decltype(uint8_t{0}  | char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  | uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  | uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} | uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} | uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} | uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  | bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  | int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  | int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} | int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} | int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  | uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// bit_xor
static_assert(std::is_same<decltype(uint8_t{0}  ^ char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} ^ uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} ^ uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} ^ uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  ^ int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} ^ int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} ^ int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  ^ uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// add
static_assert(std::is_same<decltype(uint8_t{0}  + char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  + uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  + uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} + uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} + uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} + uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  + bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  + int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  + int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} + int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} + int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  + uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// subtract
static_assert(std::is_same<decltype(uint8_t{0}  - char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} - uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} - uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} - uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  - bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} - int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} - int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  - uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// subtract (reversed)
static_assert(std::is_same<decltype(char{0}     - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint32_t{0} - uint16_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint64_t{0} - uint32_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} - uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(bool{false} - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int8_t{0}   - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int16_t{0}  - uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int32_t{0}  - uint16_t{0}), int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(int64_t{0}  - uint32_t{0}), int64_t>::value,  "unsigned (two >=32, s-u)");
static_assert(std::is_same<decltype(uint32_t{0} - int64_t{0}),  int64_t>::value,  "unsigned (two >=32, u-s)");

// multiply
static_assert(std::is_same<decltype(uint8_t{0}  * char{0}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  * uint8_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  * uint16_t{0}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} * uint32_t{0}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} * uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} * uint64_t{0}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint8_t{0}  * bool{false}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  * int8_t{0}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  * int16_t{0}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} * int32_t{0}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} * int64_t{0}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  * uint32_t{0}), int64_t>::value,  "unsigned (two >=32, u/s)");

// divide
static_assert(std::is_same<decltype(uint8_t{0}  / char{1}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / uint16_t{1}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} / uint32_t{1}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} / uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} / uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
////static_assert(std::is_same<decltype(uint8_t{0}  / bool{true}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / int8_t{1}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / int16_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} / int32_t{1}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} / int64_t{1}),  int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(int64_t{0}  / uint32_t{1}), int64_t>::value,  "unsigned (two >=32, u/s)");

// divide (reversed)
static_assert(std::is_same<decltype(char{0}     / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint32_t{0} / uint16_t{1}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint64_t{0} / uint32_t{1}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} / uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
////static_assert(std::is_same<decltype(bool{false} / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int8_t{0}   / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int16_t{0}  / uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int32_t{0}  / uint16_t{1}), int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(int64_t{0}  / uint32_t{1}), int64_t>::value,  "unsigned (two >=32, s/u)");
static_assert(std::is_same<decltype(uint32_t{0} / int64_t{1}),  int64_t>::value,  "unsigned (two >=32, u/s)");

// modulo
static_assert(std::is_same<decltype(uint8_t{0}  % char{1}),     int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % uint16_t{1}), int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} % uint32_t{1}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} % uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} % uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
////static_assert(std::is_same<decltype(uint8_t{0}  % bool{true}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % int8_t{1}),   int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % int16_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} % int32_t{1}),  int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(uint32_t{0} % int64_t{1}),  int64_t>::value,  "unsigned (two >=32, s%u)");
static_assert(std::is_same<decltype(int64_t{0}  % uint32_t{1}), int64_t>::value,  "unsigned (two >=32, u%s)");

// modulo (reversed)
static_assert(std::is_same<decltype(char{0}     % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint8_t{0}  % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint16_t{0} % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(uint32_t{0} % uint16_t{1}), uint32_t>::value, "widest   (one >=32)");
static_assert(std::is_same<decltype(uint64_t{0} % uint32_t{1}), uint64_t>::value, "widest   (two >=32)");
static_assert(std::is_same<decltype(uint64_t{0} % uint64_t{1}), uint64_t>::value, "widest   (two >=32)");
////static_assert(std::is_same<decltype(bool{false} % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int8_t{0}   % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int16_t{0}  % uint8_t{1}),  int>::value,      "promoted (two <32)");
static_assert(std::is_same<decltype(int32_t{0}  % uint16_t{1}), int32_t>::value,  "widest   (one >=32)");
static_assert(std::is_same<decltype(int64_t{0}  % uint32_t{1}), int64_t>::value,  "unsigned (two >=32, s%u)");
static_assert(std::is_same<decltype(uint32_t{0} % int64_t{1}),  int64_t>::value,  "unsigned (two >=32, u%s)");

// safe_multiply

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_minimum_times_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_maximum_times_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_multiply(maximum, maximum), overflow_exception);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_minimum_times_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(minimum, maximum), minimum);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_maximum_times_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(maximum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_half_times_2__maximum_minus_1)
{
    // The maximum of an unsigned integer is always odd, so half is always rounded down.
    // Therefore 2 * half is always one less than the unsigned integer maximum.
    BOOST_REQUIRE_EQUAL(safe_multiply(half, size_t(2)), sub1(maximum));
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_2_times_half_plus_1__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_multiply(size_t(2), add1(half)), overflow_exception);
}

// safe_add

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_minimum_plus_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_add(minimum, minimum), minimum);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_maximum_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(maximum, maximum), overflow_exception);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_minimum_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_add(minimum, maximum), maximum);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_maximum_plus_minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_add(maximum, minimum), maximum);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_half_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(half, maximum), overflow_exception);
}

BOOST_AUTO_TEST_SUITE_END()
