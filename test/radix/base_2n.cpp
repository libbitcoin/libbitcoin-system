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

// zero => zero.
// small values are left-shifted with negative (1..2) exponent.
// 3 byte values values are not shifted zero exponent (3) exponent.
// > 3 byte values are right-shifted with positive (4..32) exponent, losing the precision of all bytes of order > 3.
// A shift of >32 cannot be produced, since there are only 32 bytes of input.

// Half byte steps (leading bit unset).
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000000"))) == 0x00000000ul);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000007"))) == 0x01070000ul);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000007b"))) == 0x017b0000ul);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000007bc"))) == 0x0207bc00ul);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000007bcd"))) == 0x027bcd00ul);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000007bcde"))) == 0x0307bcdeul);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000007bcdef"))) == 0x037bcdeful);

// Full byte steps (leading bit unset).
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000007bcdefaa"))) == 0x047bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000007bcdefa7bb"))) == 0x057bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000007bcdefa7bbcc"))) == 0x067bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000007bcdefa7bbccdd"))) == 0x077bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000007bcdefa7bbccddee"))) == 0x087bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000007bcdefa7bbccddeeff"))) == 0x097bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000007bcdefa7bbccddeeff00"))) == 0x0a7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000007bcdefa7bbccddeeff0011"))) == 0x0b7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000007bcdefa7bbccddeeff001122"))) == 0x0c7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000007bcdefa7bbccddeeff00112233"))) == 0x0d7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000007bcdefa7bbccddeeff0011223344"))) == 0x0e7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000007bcdefa7bbccddeeff001122334455"))) == 0x0f7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000007bcdefa7bbccddeeff00112233445566"))) == 0x107bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000007bcdefa7bbccddeeff0011223344556677"))) == 0x117bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000007bcdefa7bbccddeeff001122334455667788"))) == 0x127bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000007bcdefa7bbccddeeff00112233445566778899"))) == 0x137bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000007bcdefa7bbccddeeff00112233445566778899aa"))) == 0x147bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000007bcdefa7bbccddeeff00112233445566778899a7bb"))) == 0x157bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000007bcdefa7bbccddeeff00112233445566778899a7bbcc"))) == 0x167bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccdd"))) == 0x177bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddee"))) == 0x187bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff"))) == 0x197bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00"))) == 0x1a7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff0011"))) == 0x1b7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff001122"))) == 0x1c7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00112233"))) == 0x1d7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff0011223344"))) == 0x1e7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff001122334455"))) == 0x1f7bcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("7bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00112233445566"))) == 0x207bcdeful);

// Half byte steps (leading bit set).
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000000"))) == 0x00000000ul);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000000a"))) == 0x010a0000ul);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000000ab"))) == 0x01ab0000ul);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000abc"))) == 0x020abc00ul);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000abcd"))) == 0x02abcd00ul);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000abcde"))) == 0x030abcdeul);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000abcdef"))) == 0x03abcdeful);

// Full byte steps (leading bit set).
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000abcdefaa"))) == 0x04abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000abcdefaabb"))) == 0x05abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000abcdefaabbcc"))) == 0x06abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000abcdefaabbccdd"))) == 0x07abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000abcdefaabbccddee"))) == 0x08abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000abcdefaabbccddeeff"))) == 0x09abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000abcdefaabbccddeeff00"))) == 0x0aabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000abcdefaabbccddeeff0011"))) == 0x0babcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000abcdefaabbccddeeff001122"))) == 0x0cabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000000000abcdefaabbccddeeff00112233"))) == 0x0dabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000000000abcdefaabbccddeeff0011223344"))) == 0x0eabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000000000abcdefaabbccddeeff001122334455"))) == 0x0fabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000000000abcdefaabbccddeeff00112233445566"))) == 0x10abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000000000abcdefaabbccddeeff0011223344556677"))) == 0x11abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000000000abcdefaabbccddeeff001122334455667788"))) == 0x12abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000000000abcdefaabbccddeeff00112233445566778899"))) == 0x13abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000000000abcdefaabbccddeeff00112233445566778899aa"))) == 0x14abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000000000abcdefaabbccddeeff00112233445566778899aabb"))) == 0x15abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000000000abcdefaabbccddeeff00112233445566778899aabbcc"))) == 0x16abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000000000abcdefaabbccddeeff00112233445566778899aabbccdd"))) == 0x17abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000000000abcdefaabbccddeeff00112233445566778899aabbccddee"))) == 0x18abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff"))) == 0x19abcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff00"))) == 0x1aabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff0011"))) == 0x1babcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00000000abcdefaabbccddeeff00112233445566778899aabbccddeeff001122"))) == 0x1cabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("000000abcdefaabbccddeeff00112233445566778899aabbccddeeff00112233"))) == 0x1dabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("0000abcdefaabbccddeeff00112233445566778899aabbccddeeff0011223344"))) == 0x1eabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("00abcdefaabbccddeeff00112233445566778899aabbccddeeff001122334455"))) == 0x1fabcdeful);
static_assert(base256e::compress(to_uintx(base16_hash("abcdefaabbccddeeff00112233445566778899aabbccddeeff00112233445566"))) == 0x20abcdeful);

// 0 returns 0.
static_assert(base256e::expand(0) == 0u);
static_assert(base256e::compress(0) == 0u);
static_assert(base256e::expand(base256e::compress(0)) == 0u);
static_assert(base256e::compress(base256e::expand(0)) == 0u);

// -3 (0 - 3) exponent zeroes the mantissa.
constexpr auto precision1 = 3u;
constexpr auto exponent1 = 0u;
constexpr auto mantissa1 = 0x00ffffffu;
constexpr auto compressed1 = (exponent1 << 24u) | mantissa1;
constexpr auto lower1 = precision1 - exponent1;
constexpr auto expanded1 = uint256_t{ mantissa1 } >> to_bits(lower1);
static_assert(expanded1 == 0u);
static_assert(base256e::expand(compressed1) == 0u);
static_assert(base256e::compress(expanded1) == 0u);

// 0 (3 - 3) exponent reproduces the mantissa.
constexpr auto precision2 = 3u;
constexpr auto exponent2 = precision2;
constexpr auto mantissa2 = 0x00ffffffu;
constexpr auto compressed2 = (exponent2 << 24u) | mantissa2;
constexpr auto expanded2 = uint256_t{ mantissa2 } << 0u;
static_assert(base256e::expand(compressed2) == expanded2);
static_assert(base256e::compress(expanded2) == compressed2);

// 29 (32 - 3) exponent zeroes the mantissa.
constexpr auto precision3 = 3u;
constexpr auto exponent3 = 32u;
constexpr auto mantissa3 = 0x00ffffffu;
constexpr auto compressed3 = (exponent3 << 24u) | mantissa3;
constexpr auto raise3 = exponent3 - precision3;
constexpr auto expanded3 = uint256_t{ mantissa3 } << to_bits(raise3);
static_assert(base256e::expand(compressed3) == expanded3);
static_assert(base256e::compress(expanded3) == compressed3);

// 0 value always produces 0 (regardless of exponent).
constexpr auto exponent4 = 0u;
constexpr auto mantissa4 = 0x00000000u;
constexpr auto compressed4 = (exponent4 << 24u) | mantissa4;
constexpr auto minimum4 = uint256_t{ 0 };
static_assert(base256e::expand(compressed4) == minimum4);
static_assert(base256e::compress(minimum4) == compressed4);

// maximal value always produces a maximal value (with maximal exponent).
constexpr auto exponent5 = 32u;
constexpr auto mantissa5 = 0x00ffffffu;
constexpr auto compressed5 = (exponent5 << 24u) | mantissa5;
constexpr auto maximum5 = bit_all<uint256_t>;
static_assert(base256e::compress(maximum5) == compressed5);
