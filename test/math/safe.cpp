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
/// DELETECSTDDEF
/// DELETECSTDINT

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

// TODO: boost is not catching these exceptions (related to disabled exceptions?).
#if defined(DISABLED)

BOOST_AUTO_TEST_SUITE(limits_tests)

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

BOOST_AUTO_TEST_SUITE_END()

#endif