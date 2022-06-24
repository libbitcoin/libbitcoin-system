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

BOOST_AUTO_TEST_SUITE(base_256e_tests)

using namespace base256e;

BOOST_AUTO_TEST_CASE(base_256e__expand__zero__expected)
{
    BOOST_CHECK_EQUAL(expand(0), 0u);
}

BOOST_AUTO_TEST_CASE(base_256e__compress__zero__expected)
{
    BOOST_CHECK_EQUAL(compress(0), 0u);
}

BOOST_AUTO_TEST_CASE(base_256e__compress_expand___zero__expected)
{
    BOOST_CHECK_EQUAL(expand(compress(0)), 0u);
}

BOOST_AUTO_TEST_CASE(base_256e__expand_compress___zero__expected)
{
    BOOST_CHECK_EQUAL(compress(expand(0)), 0u);
}

BOOST_AUTO_TEST_CASE(base_256e__both__minimum_exponent__expected)
{
    constexpr auto precision = 3u;
    constexpr auto exponent = 0u;
    constexpr auto mantissa = 0x00ffffffu;
    constexpr auto compressed = (exponent << 24u) | mantissa;
    constexpr auto lower = precision - exponent;

    uint256_t expanded{ mantissa };
    expanded >>= to_bits(lower);

    // A -3 (0 - 3) exponent zeroes the mantissa.
    BOOST_CHECK_EQUAL(expanded, 0u);
    BOOST_CHECK_EQUAL(expand(compressed), 0u);
    BOOST_CHECK_EQUAL(compress(expanded), 0u);
}

BOOST_AUTO_TEST_CASE(base_256e__both__zero_exponent__expected)
{
    constexpr auto precision = 3u;
    constexpr auto exponent = precision;
    constexpr auto mantissa = 0x00ffffffu;
    constexpr auto compressed = (exponent << 24u) | mantissa;
    ////constexpr auto raise = exponent - precision;

    // This shift is a no-op.
    const uint256_t expanded{ mantissa };
    ////expanded <<= to_bits(raise);

    // A 0 (3 - 3) exponent reporduces the mantissa.
    BOOST_CHECK_EQUAL(expand(compressed), expanded);
    BOOST_CHECK_EQUAL(compress(expanded), compressed);
}

BOOST_AUTO_TEST_CASE(base_256e__both__maximum_exponent__expected)
{
    constexpr auto precision = 3u;
    constexpr auto exponent = 32u;
    constexpr auto mantissa = 0x00ffffffu;
    constexpr auto compressed = (exponent << 24u) | mantissa;
    constexpr auto raise = exponent - precision;

    uint256_t expanded{ mantissa };
    expanded <<= to_bits(raise);

    // A 29 (32 - 3) exponent zeroes the mantissa.
    BOOST_CHECK_EQUAL(expand(compressed), expanded);
    BOOST_CHECK_EQUAL(compress(expanded), compressed);
}

BOOST_AUTO_TEST_CASE(base_256e__base256e_compress__minimum_value__expected)
{
    ////constexpr auto precision = 3u;
    constexpr auto exponent = 0u;
    constexpr auto mantissa = 0x00000000u;
    constexpr auto compressed = (exponent << 24u) | mantissa;
    ////constexpr auto raise = exponent - precision;

    // no bits set (min value)
    const auto minimum = uint256_t{ 0 };

    // A zero value always produces zero (regardless of exponent).
    BOOST_CHECK_EQUAL(expand(compressed), minimum);
    BOOST_CHECK_EQUAL(compress(minimum), compressed);
}

BOOST_AUTO_TEST_CASE(base_256e__base256e_compress__maximum_value__expected)
{
    ////constexpr auto precision = 3u;
    constexpr auto exponent = 32u;
    constexpr auto mantissa = 0x00ffffffu;
    constexpr auto compressed = (exponent << 24u) | mantissa;
    ////constexpr auto raise = exponent - precision;

    // all bits set (max value)
    const auto maximum = ~uint256_t{ 0 };

    // A maximal value always produces a maximal value (with maximal exponent).
    BOOST_CHECK_EQUAL(compress(maximum), compressed);
}

BOOST_AUTO_TEST_CASE(base_256e__base256e_compress__values__expected)
{
    // zero => zero.
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000000"))), 0x00000000ul);

    // small values are left-shifted with negative (1..2) exponent.
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000000a"))), 0x010a0000ul);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000000ab"))), 0x01ab0000ul);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000abc"))), 0x020abc00ul);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000abcd"))), 0x02abcd00ul);

    // 3 byte values values are not shifted zero exponent (3) exponent.
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000abcde"))), 0x030abcdeul);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000abcdef"))), 0x03abcdeful);


    // > 3 byte values are right-shifted with positive (4..32) exponent, losing the precision of all bytes of order > 3.
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000abcdefaa"))), 0x04abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000abcdefaabb"))), 0x05abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000abcdefaabbcc"))), 0x06abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000abcdefaabbccdd"))), 0x07abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000abcdefaabbccddee"))), 0x08abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000abcdefaabbccddeeff"))), 0x09abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000abcdefaabbccddeeff00"))), 0x0aabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000000000000000000000000000abcdefaabbccddeeff0011"))), 0x0babcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000000000000000000000abcdefaabbccddeeff001122"))), 0x0cabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000000000000000000000abcdefaabbccddeeff00112233"))), 0x0dabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000000000000000000000abcdefaabbccddeeff0011223344"))), 0x0eabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000000000000000abcdefaabbccddeeff001122334455"))), 0x0fabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000000000000000abcdefaabbccddeeff00112233445566"))), 0x10abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000000000000000abcdefaabbccddeeff0011223344556677"))), 0x11abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000000000abcdefaabbccddeeff001122334455667788"))), 0x12abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000000000abcdefaabbccddeeff00112233445566778899"))), 0x13abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000000000abcdefaabbccddeeff00112233445566778899aa"))), 0x14abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000000000abcdefaabbccddeeff00112233445566778899aabb"))), 0x15abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000000000abcdefaabbccddeeff00112233445566778899aabbcc"))), 0x16abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000000000abcdefaabbccddeeff00112233445566778899aabbccdd"))), 0x17abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000000000abcdefaabbccddeeff00112233445566778899aabbccddee"))), 0x18abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff"))), 0x19abcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff00"))), 0x1aabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff0011"))), 0x1babcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00000000abcdefaabbccddeeff00112233445566778899aabbccddeeff001122"))), 0x1cabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("000000abcdefaabbccddeeff00112233445566778899aabbccddeeff00112233"))), 0x1dabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("0000abcdefaabbccddeeff00112233445566778899aabbccddeeff0011223344"))), 0x1eabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("00abcdefaabbccddeeff00112233445566778899aabbccddeeff001122334455"))), 0x1fabcdeful);
    BOOST_CHECK_EQUAL(compress(to_uintx(base16_hash("abcdefaabbccddeeff00112233445566778899aabbccddeeff00112233445566"))), 0x20abcdeful);

    // A shift of >32 cannot be produced, since there are only 32 bytes of input.
}

BOOST_AUTO_TEST_SUITE_END()
