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

// Verify compiler "usual arithmetic conversion" expectations.
// ----------------------------------------------------------------------------
// Shift works like a unary op (right operand is not incorporated).
// Order of operands does not affect (other) binary ops.
// self-assigning (e.g. >>=, /=) ops obviously do not (cannot) promote.
// ! and comparison are not mathematical ops.

// Unary ops (+/-/~) change to unsigned upon width promotion, not otherwise.
// + is a unary op and produces promotion.
// std::common_type does not support unary operators.
// std::common_type<Left, Right>::type == decltype(left - right)
// std::common_type<Value>::type != decltype(-value)
using integer = decltype(-'a');
using character = std::common_type_t<char>;
using common_character = std::common_type_t<char, char>;
static_assert(!is_same_type<integer, character>);
static_assert(!is_same_type<integer, common_character>);

// Compiler warns on bool safety for -b, ~b, <<b, >>b, /b, %b, b/, b% (assertions disabled).

// invert
static_assert(is_same_type<decltype(!char{0}),                   bool>, "boolean");
static_assert(is_same_type<decltype(!int8_t{0}),                 bool>, "boolean");
static_assert(is_same_type<decltype(!int16_t{0}),                bool>, "boolean");
static_assert(is_same_type<decltype(!int32_t{0}),                bool>, "boolean");
static_assert(is_same_type<decltype(!int64_t{0}),                bool>, "boolean");
static_assert(is_same_type<decltype(!bool{false}),               bool>, "boolean");
static_assert(is_same_type<decltype(!uint8_t{0}),                bool>, "boolean");
static_assert(is_same_type<decltype(!uint16_t{0}),               bool>, "boolean");
static_assert(is_same_type<decltype(!uint32_t{0}),               bool>, "boolean");
static_assert(is_same_type<decltype(!uint64_t{0}),               bool>, "boolean");

// comparison
static_assert(is_same_type<decltype(uint8_t{0}  == char{0}),     bool>, "boolean");
static_assert(is_same_type<decltype(uint8_t{0}  != uint8_t{0}),  bool>, "boolean");
static_assert(is_same_type<decltype(uint8_t{0}  >= uint16_t{0}), bool>, "boolean");
static_assert(is_same_type<decltype(uint16_t{0} <= uint32_t{0}), bool>, "boolean");
static_assert(is_same_type<decltype(uint32_t{0} == uint64_t{0}), bool>, "boolean");
static_assert(is_same_type<decltype(uint64_t{0} == uint64_t{0}), bool>, "boolean");
static_assert(is_same_type<decltype(uint8_t{0}  == bool{false}), bool>, "boolean");
static_assert(is_same_type<decltype(bool{false} == uint32_t{0}), bool>, "boolean");
static_assert(is_same_type<decltype(uint8_t{0}  == int8_t{0}),   bool>, "boolean");
static_assert(is_same_type<decltype(uint8_t{0}  == int16_t{0}),  bool>, "boolean");
static_assert(is_same_type<decltype(uint16_t{0} == int32_t{0}),  bool>, "boolean");
static_assert(is_same_type<decltype(uint32_t{0} == int64_t{0}),  bool>, "boolean");
static_assert(is_same_type<decltype(int64_t{0}  == uint32_t{0}), bool>, "boolean");

// positive
static_assert(is_same_type<decltype(+char{0}),                   int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(+int8_t{0}),                 int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(+int16_t{0}),                int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(+int32_t{0}),                int32_t>,  "unpromoted (>=32");
static_assert(is_same_type<decltype(+int64_t{0}),                int64_t>,  "unpromoted (>=32");
static_assert(is_same_type<decltype(+bool{false}),               int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(+uint8_t{0}),                int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(+uint16_t{0}),               int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(+uint32_t{0}),               uint32_t>, "unpromoted (>=32");
static_assert(is_same_type<decltype(+uint64_t{0}),               uint64_t>, "unpromoted (>=32");

// negate
static_assert(is_same_type<decltype(-char{0}),                   int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(-int8_t{0}),                 int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(-int16_t{0}),                int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(-int32_t{0}),                int32_t>,  "unpromoted (>=32");
static_assert(is_same_type<decltype(-int64_t{0}),                int64_t>,  "unpromoted (>=32");
////static_assert(is_same_type<decltype(-bool{false}),               int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(-uint8_t{0}),                int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(-uint16_t{0}),               int>,      "promoted   (<32)");
////static_assert(is_same_type<decltype(-uint32_t{0}),               uint32_t>, "unpromoted (>=32");
////static_assert(is_same_type<decltype(-uint64_t{0}),               uint64_t>, "unpromoted (>=32");

// ones_complement
static_assert(is_same_type<decltype(~char{0}),                   int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(~int8_t{0}),                 int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(~int16_t{0}),                int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(~int32_t{0}),                int32_t>,  "unpromoted (>=32");
static_assert(is_same_type<decltype(~int64_t{0}),                int64_t>,  "unpromoted (>=32");
////static_assert(is_same_type<decltype(~bool{false}),               int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(~uint8_t{0}),                int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(~uint16_t{0}),               int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(~uint32_t{0}),               uint32_t>, "unpromoted (>=32");
static_assert(is_same_type<decltype(~uint64_t{0}),               uint64_t>, "unpromoted (>=32");

// shift_left
static_assert(is_same_type<decltype(uint8_t{0}  << char{0}),     int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint8_t{0}  << uint8_t{0}),  int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint8_t{0}  << uint16_t{0}), int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint16_t{0} << uint32_t{0}), int >,     "promoted   (<32)");
static_assert(is_same_type<decltype(uint32_t{0} << uint64_t{0}), uint32_t>, "unpromoted (>=32");
static_assert(is_same_type<decltype(uint64_t{0} << uint64_t{0}), uint64_t>, "unpromoted (>=32");
////static_assert(is_same_type<decltype(uint8_t{0}  << bool{false}), int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(bool{false} << uint32_t{0}), int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint8_t{0}  << int8_t{0}),   int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint8_t{0}  << int16_t{0}),  int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint16_t{0} << int32_t{0}),  int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint32_t{0} << int64_t{0}),  uint32_t>, "unpromoted (>=32");
static_assert(is_same_type<decltype(int64_t{0}  << uint32_t{0}), int64_t>,  "unpromoted (>=32");

// shift_right
static_assert(is_same_type<decltype(uint8_t{0}  >> char{0}),     int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint8_t{0}  >> uint8_t{0}),  int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint8_t{0}  >> uint16_t{0}), int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint16_t{0} >> uint32_t{0}), int >,     "promoted   (<32)");
static_assert(is_same_type<decltype(uint32_t{0} >> uint64_t{0}), uint32_t>, "unpromoted (>=32");
static_assert(is_same_type<decltype(uint64_t{0} >> uint64_t{0}), uint64_t>, "unpromoted (>=32");
////static_assert(is_same_type<decltype(uint8_t{0}  >> bool{false}), int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(bool{false} >> uint32_t{0}), int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint8_t{0}  >> int8_t{0}),   int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint8_t{0}  >> int16_t{0}),  int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint16_t{0} >> int32_t{0}),  int>,      "promoted   (<32)");
static_assert(is_same_type<decltype(uint32_t{0} >> int64_t{0}),  uint32_t>, "unpromoted (>=32");
static_assert(is_same_type<decltype(int64_t{0}  >> uint32_t{0}), int64_t>,  "unpromoted (>=32");

// bit_and
static_assert(is_same_type<decltype(uint8_t{0}  & char{0}),     int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  & uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  & uint16_t{0}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} & uint32_t{0}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} & uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} & uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint8_t{0}  & bool{false}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  & int8_t{0}),   int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  & int16_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} & int32_t{0}),  int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} & int64_t{0}),  int64_t>,  "unsigned (two >=32, s/u)");
static_assert(is_same_type<decltype(int64_t{0}  & uint32_t{0}), int64_t>,  "unsigned (two >=32, u/s)");

// bit_or
static_assert(is_same_type<decltype(uint8_t{0}  | char{0}),     int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  | uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  | uint16_t{0}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} | uint32_t{0}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} | uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} | uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint8_t{0}  | bool{false}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  | int8_t{0}),   int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  | int16_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} | int32_t{0}),  int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} | int64_t{0}),  int64_t>,  "unsigned (two >=32, s/u)");
static_assert(is_same_type<decltype(int64_t{0}  | uint32_t{0}), int64_t>,  "unsigned (two >=32, u/s)");

// bit_xor
static_assert(is_same_type<decltype(uint8_t{0}  ^ char{0}),     int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  ^ uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  ^ uint16_t{0}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} ^ uint32_t{0}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} ^ uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} ^ uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint8_t{0}  ^ bool{false}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  ^ int8_t{0}),   int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  ^ int16_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} ^ int32_t{0}),  int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} ^ int64_t{0}),  int64_t>,  "unsigned (two >=32, s/u)");
static_assert(is_same_type<decltype(int64_t{0}  ^ uint32_t{0}), int64_t>,  "unsigned (two >=32, u/s)");

// add
static_assert(is_same_type<decltype(uint8_t{0}  + char{0}),     int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  + uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  + uint16_t{0}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} + uint32_t{0}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} + uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} + uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint8_t{0}  + bool{false}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  + int8_t{0}),   int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  + int16_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} + int32_t{0}),  int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} + int64_t{0}),  int64_t>,  "unsigned (two >=32, s/u)");
static_assert(is_same_type<decltype(int64_t{0}  + uint32_t{0}), int64_t>,  "unsigned (two >=32, u/s)");

// subtract
static_assert(is_same_type<decltype(uint8_t{0}  - char{0}),     int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  - uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  - uint16_t{0}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} - uint32_t{0}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} - uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} - uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint8_t{0}  - bool{false}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  - int8_t{0}),   int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  - int16_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} - int32_t{0}),  int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} - int64_t{0}),  int64_t>,  "unsigned (two >=32, s/u)");
static_assert(is_same_type<decltype(int64_t{0}  - uint32_t{0}), int64_t>,  "unsigned (two >=32, u/s)");

// subtract (reversed)
static_assert(is_same_type<decltype(char{0}     - uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  - uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} - uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint32_t{0} - uint16_t{0}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint64_t{0} - uint32_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} - uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(bool{false} - uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int8_t{0}   - uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int16_t{0}  - uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int32_t{0}  - uint16_t{0}), int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(int64_t{0}  - uint32_t{0}), int64_t>,  "unsigned (two >=32, s-u)");
static_assert(is_same_type<decltype(uint32_t{0} - int64_t{0}),  int64_t>,  "unsigned (two >=32, u-s)");

// multiply
static_assert(is_same_type<decltype(uint8_t{0}  * char{0}),     int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  * uint8_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  * uint16_t{0}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} * uint32_t{0}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} * uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} * uint64_t{0}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint8_t{0}  * bool{false}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  * int8_t{0}),   int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  * int16_t{0}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} * int32_t{0}),  int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} * int64_t{0}),  int64_t>,  "unsigned (two >=32, s/u)");
static_assert(is_same_type<decltype(int64_t{0}  * uint32_t{0}), int64_t>,  "unsigned (two >=32, u/s)");

// divide
static_assert(is_same_type<decltype(uint8_t{0}  / char{1}),     int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  / uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  / uint16_t{1}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} / uint32_t{1}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} / uint64_t{1}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} / uint64_t{1}), uint64_t>, "widest   (two >=32)");
////static_assert(is_same_type<decltype(uint8_t{0}  / bool{true}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  / int8_t{1}),   int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  / int16_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} / int32_t{1}),  int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} / int64_t{1}),  int64_t>,  "unsigned (two >=32, s/u)");
static_assert(is_same_type<decltype(int64_t{0}  / uint32_t{1}), int64_t>,  "unsigned (two >=32, u/s)");

// divide (reversed)
static_assert(is_same_type<decltype(char{0}     / uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  / uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} / uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint32_t{0} / uint16_t{1}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint64_t{0} / uint32_t{1}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} / uint64_t{1}), uint64_t>, "widest   (two >=32)");
////static_assert(is_same_type<decltype(bool{false} / uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int8_t{0}   / uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int16_t{0}  / uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int32_t{0}  / uint16_t{1}), int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(int64_t{0}  / uint32_t{1}), int64_t>,  "unsigned (two >=32, s/u)");
static_assert(is_same_type<decltype(uint32_t{0} / int64_t{1}),  int64_t>,  "unsigned (two >=32, u/s)");

// modulo
static_assert(is_same_type<decltype(uint8_t{0}  % char{1}),     int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  % uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  % uint16_t{1}), int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} % uint32_t{1}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} % uint64_t{1}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} % uint64_t{1}), uint64_t>, "widest   (two >=32)");
////static_assert(is_same_type<decltype(uint8_t{0}  % bool{true}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  % int8_t{1}),   int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  % int16_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} % int32_t{1}),  int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(uint32_t{0} % int64_t{1}),  int64_t>,  "unsigned (two >=32, s%u)");
static_assert(is_same_type<decltype(int64_t{0}  % uint32_t{1}), int64_t>,  "unsigned (two >=32, u%s)");

// modulo (reversed)
static_assert(is_same_type<decltype(char{0}     % uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint8_t{0}  % uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint16_t{0} % uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(uint32_t{0} % uint16_t{1}), uint32_t>, "widest   (one >=32)");
static_assert(is_same_type<decltype(uint64_t{0} % uint32_t{1}), uint64_t>, "widest   (two >=32)");
static_assert(is_same_type<decltype(uint64_t{0} % uint64_t{1}), uint64_t>, "widest   (two >=32)");
////static_assert(is_same_type<decltype(bool{false} % uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int8_t{0}   % uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int16_t{0}  % uint8_t{1}),  int>,      "promoted (two <32)");
static_assert(is_same_type<decltype(int32_t{0}  % uint16_t{1}), int32_t>,  "widest   (one >=32)");
static_assert(is_same_type<decltype(int64_t{0}  % uint32_t{1}), int64_t>,  "unsigned (two >=32, s%u)");
static_assert(is_same_type<decltype(uint32_t{0} % int64_t{1}),  int64_t>,  "unsigned (two >=32, u%s)");

BOOST_AUTO_TEST_SUITE(safe_tests)

// TODO: constrained static_cast.
// depromote
// narrow_cast
// sign_cast
// narrow_sign_cast
// possible_narrow_cast
// possible_sign_cast
// possible_narrow_sign_cast
// possible_sign_narrow_cast
// possible_narrow_and_sign_cast
// possible_wide_cast

// TODO: constrained reinterpret_cast.
// pointer_cast
// possible_pointer_cast

// byte_cast
// ----------------------------------------------------------------------------

// array_cast
// ----------------------------------------------------------------------------

using array8x2 = std::array<uint8_t, 2>;
using array16x1 = std::array<uint16_t, 1>;
constexpr array8x2 const_data8x2{ 1_u8, 2_u8 };
constexpr array16x1 const_data16x1{ 1_u16 };
auto data8x2 = const_data8x2;
auto data16x1 = const_data16x1;

// reinterpret_cast(nullptr) at array size zero is safe, so empty array narrowing is allowed.
static_assert(std::array<uint8_t, 0>{}.data() == nullptr);
using array8x0 = std::array<uint8_t, 0>;
constexpr array8x0 const_data8x0{};
static_assert(is_same_type<decltype(array_cast<uint8_t>(const_data8x0)), const array8x0&>);

static_assert(is_same_type<decltype(array_cast<uint16_t>(data8x2)), array16x1&>);
static_assert(is_same_type<decltype(array_cast<uint8_t>(data16x1)), array8x2&>);
static_assert(is_same_type<decltype(array_cast<uint16_t>(data16x1)), array16x1&>);

static_assert(is_same_type<decltype(array_cast<uint16_t>(const_data8x2)), const array16x1&>);
static_assert(is_same_type<decltype(array_cast<uint8_t>(const_data16x1)), const array8x2&>);
static_assert(is_same_type<decltype(array_cast<uint16_t>(const_data16x1)), const array16x1&>);

// Since _u8 data is cast into integrals its above order is preserved.
// The endianness of the _u32 integrals affects their bytewise interpretation.
BOOST_AUTO_TEST_CASE(safe__array_cast__const__expected)
{
    constexpr std::array<uint8_t, 2> data8x2{ 0x01_u8, 0x02_u8 };

    const auto& value16x1 = array_cast<uint16_t>(data8x2);
    BOOST_REQUIRE_EQUAL(value16x1[0], native_to_little_end(0x0201_u16));

    const auto& value8x2 = array_cast<uint8_t>(data8x2);
    BOOST_REQUIRE_EQUAL(value8x2[0], data8x2[0]);
    BOOST_REQUIRE_EQUAL(value8x2[1], data8x2[1]);
}

// Since _u8 data is cast into integrals its above order is preserved.
// The endianness of the _u32 integrals affects their bytewise interpretation.
BOOST_AUTO_TEST_CASE(safe__array_cast__non_const__expected)
{
    std::array<uint8_t, 2> data8x2{ 0x01_u8, 0x02_u8 };

    auto& value16x1 = array_cast<uint16_t>(data8x2);
    BOOST_REQUIRE_EQUAL(value16x1[0], native_to_little_end(0x0201_u16));

    auto& value8x2 = array_cast<uint8_t>(data8x2);
    BOOST_REQUIRE_EQUAL(value8x2[0], data8x2[0]);
    BOOST_REQUIRE_EQUAL(value8x2[1], data8x2[1]);

    // Demonstrate that this is a cast, not a copy.
    value8x2[0] = 0x42_u8;
    value8x2[1] = 0x24_u8;
    BOOST_REQUIRE_EQUAL(value16x1[0], native_to_little_end(0x4224_u16));
}

// narrowing_array_cast
// ----------------------------------------------------------------------------

using array16x5 = std::array<uint16_t, 5>;
using array64x3 = std::array<uint64_t, 3>;
constexpr array16x5 const_data16x5{ 1_u16, 2_u16, 3_u16, 4_u16, 5_u16 };
constexpr array64x3 const_data64x3{ 1_u64, 2_u64, 3_u64 };
auto data16x5 = const_data16x5;
auto data64x3 = const_data64x3;
static_assert(is_same_type<decltype(narrowing_array_cast<uint16_t, 5>(data64x3)), array16x5&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint16_t, 5>(const_data64x3)), const array16x5&>);

// reinterpret_cast(nullptr) at array size zero is safe, so empty array narrowing is allowed.
static_assert(std::array<uint8_t, 0>{}.data() == nullptr);

static_assert(is_same_type<decltype(narrowing_array_cast<uint8_t, 0>(const_data64x3)), const std::array<uint8_t, 0>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint8_t, 1>(const_data64x3)), const std::array<uint8_t, 1>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint8_t, 16>(const_data64x3)), const std::array<uint8_t, 16>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint8_t, 23>(const_data64x3)), const std::array<uint8_t, 23>&>);
////static_assert(is_same_type<decltype(narrowing_array_cast<uint8_t, 24>(const_data64x3)), const std::array<uint8_t, 24>&>);
////static_assert(array_cast<uint8_t>(const_data64x3).size() == 24);

static_assert(is_same_type<decltype(narrowing_array_cast<uint16_t, 0>(const_data64x3)), const std::array<uint16_t, 0>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint16_t, 1>(const_data64x3)), const std::array<uint16_t, 1>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint16_t, 8>(const_data64x3)), const std::array<uint16_t, 8>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint16_t, 11>(const_data64x3)), const std::array<uint16_t, 11>&>);
////static_assert(is_same_type<decltype(narrowing_array_cast<uint16_t, 12>(const_data64x3)), const std::array<uint16_t, 12>&>);
////static_assert(array_cast<uint16_t>(const_data64x3).size() == 12);

static_assert(is_same_type<decltype(narrowing_array_cast<uint32_t, 0>(const_data64x3)), const std::array<uint32_t, 0>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint32_t, 1>(const_data64x3)), const std::array<uint32_t, 1>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint32_t, 2>(const_data64x3)), const std::array<uint32_t, 2>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint32_t, 5>(const_data64x3)), const std::array<uint32_t, 5>&>);
////static_assert(is_same_type<decltype(narrowing_array_cast<uint32_t, 6>(const_data64x3)), const std::array<uint32_t, 6>&>);
////static_assert(array_cast<uint32_t>(const_data64x3).size() == 6);

static_assert(is_same_type<decltype(narrowing_array_cast<uint64_t, 0>(const_data64x3)), const std::array<uint64_t, 0>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint64_t, 1>(const_data64x3)), const std::array<uint64_t, 1>&>);
static_assert(is_same_type<decltype(narrowing_array_cast<uint64_t, 2>(const_data64x3)), const std::array<uint64_t, 2>&>);
////static_assert(is_same_type<decltype(narrowing_array_cast<uint64_t, 5>(const_data64x3)), const std::array<uint64_t, 3>&>);
////static_assert(array_cast<uint64_t>(const_data64x3).size() == 3);

BOOST_AUTO_TEST_CASE(safe__narrowing_array_cast__const__expected)
{
    constexpr std::array<uint8_t, 2> data8x2{ 0x01_u8, 0x02_u8 };

    const auto& value8x1 = narrowing_array_cast<uint8_t, 1>(data8x2);
    BOOST_REQUIRE_EQUAL(value8x1[0], 0x01_u8);
}

BOOST_AUTO_TEST_CASE(safe__narrowing_array_cast__non_const__expected)
{
    std::array<uint8_t, 2> data8x2{ 0x01_u8, 0x02_u8 };

    auto& value8x1 = narrowing_array_cast<uint8_t, 1>(data8x2);
    BOOST_REQUIRE_EQUAL(value8x1[0], 0x01_u8);

    // Demonstrate that this is a cast, not a copy.
    value8x1[0] = 0x42_u8;
    BOOST_REQUIRE_EQUAL(data8x2[0], 0x42_u8);
}

// unsafe_array_cast
// ----------------------------------------------------------------------------

// 3 lls to 12 shorts
constexpr auto llongs16 = 3_size;
constexpr auto llints16 = (llongs16 * sizeof(uint64_t)) / sizeof(uint16_t);
constexpr uint64_t const_llongs16[llongs16]{ 42_u64, 43_u64, 44_u64 };
using llong_array16 = decltype(unsafe_array_cast<uint16_t, llints16>(const_llongs16));
static_assert(is_same_type<llong_array16, const std::array<uint16_t, llints16>&>);
static_assert(!is_same_type<llong_array16, std::array<uint16_t, llints16>&>);

// 8 bytes to 4 shorts
constexpr auto bytes16 = 8_size;
constexpr auto ints16 = (bytes16 * sizeof(uint8_t)) / sizeof(uint16_t);
constexpr uint8_t const_data16[bytes16]{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };
using const_array16 = decltype(unsafe_array_cast<uint16_t, ints16>(const_data16));
static_assert(is_same_type<const_array16, const std::array<uint16_t, ints16>&>);
static_assert(!is_same_type<const_array16, std::array<uint16_t, ints16>&>);

// 8 bytes to 2 ints
constexpr auto bytes32 = 8_size;
constexpr auto ints32 = (bytes32 * sizeof(uint8_t)) / sizeof(uint32_t);
constexpr uint8_t const_data32[bytes32]{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };
using const_array32 = decltype(unsafe_array_cast<uint32_t, ints32>(const_data32));
static_assert(is_same_type<const_array32, const std::array<uint32_t, ints32>&>);
static_assert(!is_same_type<const_array32, std::array<uint32_t, ints32>&>);

// Type (including signedness) and array size must correspond.
static_assert(!is_same_type<const_array32, const std::array<int32_t, ints32>&>);
static_assert(!is_same_type<const_array32, const std::array<uint16_t, ints32>&>);
static_assert(!is_same_type<const_array32, const std::array<uint32_t, ints32 * 2_size>&>);

// But, a data buffer of any size can be cast to an array of any size.
// This is the unsafe nature of a reinterpret_cast (and why it cannot be constexpr).
// ----------------------------------------------------------------------------
// 1 byte to 1 int (out of bounds)
constexpr uint8_t data1[1]{ 1_u8 };
using const_array1 = decltype(unsafe_array_cast<uint32_t, 1>(data1));
static_assert(is_same_type<const_array1, const std::array<uint32_t, 1>&>);
// ----------------------------------------------------------------------------

// Values cannot be tested here, as reinterpret_cast is not constexpr.
uint8_t non_const_data32[bytes32]{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };
using non_const_array32 = decltype(unsafe_array_cast<uint32_t, ints32>(non_const_data32));
static_assert(is_same_type<non_const_array32, std::array<uint32_t, ints32>&>);
static_assert(!is_same_type<non_const_array32, const std::array<uint32_t, ints32>&>);

// Since _u8 data is cast into integrals its above byte order is preserved.
// The endianness of the _u32 integrals affects their numerical interpretation.
BOOST_AUTO_TEST_CASE(safe__unsafe_array_cast__const__expected)
{
    constexpr uint8_t data8x8[8]{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };

    const auto& value32x2 = unsafe_array_cast<uint32_t, 2>(&data8x8[0]);
    BOOST_REQUIRE_EQUAL(value32x2[0], native_to_little_end(0x04030201_u32));
    BOOST_REQUIRE_EQUAL(value32x2[1], native_to_little_end(0x08070605_u32));
}

// Since _u8 data is cast into integrals its above byte order is preserved.
// The endianness of the _u32 integrals affects their numerical interpretation.
BOOST_AUTO_TEST_CASE(safe__unsafe_array_cast__non_const__expected)
{
    uint8_t data8x8[8]{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };

    auto& value32x2 = unsafe_array_cast<uint32_t, 2>(&data8x8[0]);
    BOOST_REQUIRE_EQUAL(value32x2[0], native_to_little_end(0x04030201_u32));
    BOOST_REQUIRE_EQUAL(value32x2[1], native_to_little_end(0x08070605_u32));

    // Demonstrate that this is a cast, not a copy.
    value32x2[0] = native_to_big_end(0x12345678_u32);
    value32x2[1] = native_to_big_end(0x0abcdef0_u32);
    BOOST_REQUIRE_EQUAL(data8x8[0], 0x12_u8);
    BOOST_REQUIRE_EQUAL(data8x8[1], 0x34_u8);
    BOOST_REQUIRE_EQUAL(data8x8[2], 0x56_u8);
    BOOST_REQUIRE_EQUAL(data8x8[3], 0x78_u8);
    BOOST_REQUIRE_EQUAL(data8x8[4], 0x0a_u8);
    BOOST_REQUIRE_EQUAL(data8x8[5], 0xbc_u8);
    BOOST_REQUIRE_EQUAL(data8x8[6], 0xde_u8);
    BOOST_REQUIRE_EQUAL(data8x8[7], 0xf0_u8);
}

// Throwing functions.
// ----------------------------------------------------------------------------

// parameters
constexpr size_t minimal = 0;
constexpr size_t maximal = max_size_t;
constexpr size_t half = to_half(maximal);

// safe_multiply
static_assert(safe_multiply(minimal, minimal) == minimal);
static_assert(safe_multiply(minimal, maximal) == minimal);
static_assert(safe_multiply(maximal, minimal) == minimal);
static_assert(safe_multiply(half, 2_size)     == sub1(maximal));
static_assert(safe_multiply(2_size, half)     == sub1(maximal));
////static_assert(safe_multiply(maximal, maximal) == 0u);

// safe_add
static_assert(safe_add(minimal, minimal)  == minimal);
static_assert(safe_add(minimal, maximal)  ==  maximal);
static_assert(safe_add(maximal, minimal)  == maximal);
////static_assert(safe_add(half, maximal) == maximal);
////static_assert(safe_add(maximal, maximal) == maximal);

// safe_negate
static_assert(safe_negate(0) == 0);
static_assert(safe_negate(1) == -1);
static_assert(safe_negate(-1) == +1);
static_assert(safe_negate(-42) == +42);
static_assert(safe_negate(max_int32) == add1(min_int32));
static_assert(safe_negate(add1(min_int32)) == max_int32);
////static_assert(safe_negate(min_int32) == 0u);

// TODO: boost test is not catching these thrown exceptions (related to disabled exceptions?).
#if defined(DISABLED)

// safe_multiply

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_minimum_times_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(minimal, minimal), minimal);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_minimum_times_maximum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(minimal, maximal), minimal);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_maximum_times_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(maximal, minimal), minimal);
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_half_times_2__maximum_minus_1)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(half, size_t(2)), sub1(maximal));
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_2_times_half__maximum_minus_1)
{
    BOOST_REQUIRE_EQUAL(safe_multiply(size_t(2), half), sub1(maximal));
}

BOOST_AUTO_TEST_CASE(safe__safe_multiply__size_t_maximum_times_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_multiply(maximal, maximal), overflow_exception);
}

// safe_add

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_minimum_plus_minimum__minimum)
{
    BOOST_REQUIRE_EQUAL(safe_add(minimal, minimal), minimal);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_minimum_plus_maximum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_add(minimal, maximal), maximal);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_maximum_plus_minimum__maximum)
{
    BOOST_REQUIRE_EQUAL(safe_add(maximal, minimal), maximal);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_half_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(half, maximal), overflow_exception);
}

BOOST_AUTO_TEST_CASE(safe__safe_add__size_t_maximum_plus_maximum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_add(maximal, maximal), overflow_exception);
}

// safe_negate

BOOST_AUTO_TEST_CASE(safe__safe_negate__int32_minimum_plus_one__expected)
{
    BOOST_REQUIRE_EQUAL(safe_negate(add1(min_int32)), max_int32);
}

BOOST_AUTO_TEST_CASE(safe__safe_negate__int32_minimum__throws_overflow)
{
    BOOST_REQUIRE_THROW(safe_negate(min_int32), overflow_exception);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
