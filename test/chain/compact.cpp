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

BOOST_AUTO_TEST_SUITE(compact_tests)

using namespace system::chain;

BOOST_AUTO_TEST_CASE(compact__compress__positive_values__expected)
{
    // zero => zero.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000000"))), 0x00000000ul);

    // small values are left-shifted with negative (1..2) exponent.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000007"))), 0x01070000ul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000007b"))), 0x017b0000ul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000007bc"))), 0x0207bc00ul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000007bcd"))), 0x027bcd00ul);

    // 3 byte values values are not shifted zero exponent (3) exponent.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000007bcde"))), 0x0307bcdeul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000007bcdef"))), 0x037bcdeful);


    // > 3 byte values are right-shifted with positive (4..32) exponent, losing the precision of all bytes of order > 3.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000007bcdefaa"))), 0x047bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000007bcdefa7bb"))), 0x057bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000007bcdefa7bbcc"))), 0x067bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000007bcdefa7bbccdd"))), 0x077bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000007bcdefa7bbccddee"))), 0x087bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000007bcdefa7bbccddeeff"))), 0x097bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000007bcdefa7bbccddeeff00"))), 0x0a7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000007bcdefa7bbccddeeff0011"))), 0x0b7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000007bcdefa7bbccddeeff001122"))), 0x0c7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000007bcdefa7bbccddeeff00112233"))), 0x0d7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000007bcdefa7bbccddeeff0011223344"))), 0x0e7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000007bcdefa7bbccddeeff001122334455"))), 0x0f7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000007bcdefa7bbccddeeff00112233445566"))), 0x107bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000007bcdefa7bbccddeeff0011223344556677"))), 0x117bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000007bcdefa7bbccddeeff001122334455667788"))), 0x127bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000007bcdefa7bbccddeeff00112233445566778899"))), 0x137bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000007bcdefa7bbccddeeff00112233445566778899aa"))), 0x147bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000007bcdefa7bbccddeeff00112233445566778899a7bb"))), 0x157bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000007bcdefa7bbccddeeff00112233445566778899a7bbcc"))), 0x167bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccdd"))), 0x177bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddee"))), 0x187bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff"))), 0x197bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00"))), 0x1a7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff0011"))), 0x1b7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff001122"))), 0x1c7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00112233"))), 0x1d7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff0011223344"))), 0x1e7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff001122334455"))), 0x1f7bcdeful);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("7bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00112233445566"))), 0x207bcdeful);

    // A shift of >32 cannot be produced, since there are only 32 bytes of input and no negative shift.
}

BOOST_AUTO_TEST_CASE(compact__compress__negative_values__expected)
{
    // zero => zero.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000000"))), 0x00000000ul);

    // small values are left-shifted with negative (1..2) exponent.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000000a"))), 0x010a0000ul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000000ab"))), 0x0200ab00ul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000abc"))), 0x020abc00ul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000abcd"))), 0x0300abcdul);

    // 3 byte values values are not shifted zero exponent (3) exponent.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000abcde"))), 0x030abcdeul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000abcdef"))), 0x0400abcdul);


    // > 3 byte values are right-shifted with positive (4..32) exponent, losing the precision of all bytes of order > 3.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000abcdefaa"))), 0x0500abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000abcdefaabb"))), 0x0600abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000abcdefaabbcc"))), 0x0700abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000abcdefaabbccdd"))), 0x0800abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000abcdefaabbccddee"))), 0x0900abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000abcdefaabbccddeeff"))), 0x0a00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000abcdefaabbccddeeff00"))), 0x0b00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000abcdefaabbccddeeff0011"))), 0x0c00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000abcdefaabbccddeeff001122"))), 0x0d00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000abcdefaabbccddeeff00112233"))), 0x0e00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000abcdefaabbccddeeff0011223344"))), 0x0f00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000abcdefaabbccddeeff001122334455"))), 0x1000abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000abcdefaabbccddeeff00112233445566"))), 0x1100abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000000000abcdefaabbccddeeff0011223344556677"))), 0x1200abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000000000abcdefaabbccddeeff001122334455667788"))), 0x1300abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000000000abcdefaabbccddeeff00112233445566778899"))), 0x1400abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000000000abcdefaabbccddeeff00112233445566778899aa"))), 0x1500abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000000000abcdefaabbccddeeff00112233445566778899aabb"))), 0x1600abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000000000abcdefaabbccddeeff00112233445566778899aabbcc"))), 0x1700abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000000000abcdefaabbccddeeff00112233445566778899aabbccdd"))), 0x1800abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000000000abcdefaabbccddeeff00112233445566778899aabbccddee"))), 0x1900abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff"))), 0x1a00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff00"))), 0x1b00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff0011"))), 0x1c00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00000000abcdefaabbccddeeff00112233445566778899aabbccddeeff001122"))), 0x1d00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("000000abcdefaabbccddeeff00112233445566778899aabbccddeeff00112233"))), 0x1e00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("0000abcdefaabbccddeeff00112233445566778899aabbccddeeff0011223344"))), 0x1f00abcdul);
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("00abcdefaabbccddeeff00112233445566778899aabbccddeeff001122334455"))), 0x2000abcdul);

    // A shift of 33 is produced from the negative shift.
    BOOST_CHECK_EQUAL(compact::compress(to_uintx(base16_hash("abcdefaabbccddeeff00112233445566778899aabbccddeeff00112233445566"))), 0x2100abcdul);

    // A shift of >33 cannot be produced, since there are only 32 bytes of input, with one possible negative shift.
}

constexpr uint32_t factory(int32_t logical_exponent, bool negative,
    uint32_t mantissa) noexcept
{
    // The exponent of a non-zero mantissa is valid from -3 to +29.
    BC_ASSERT(logical_exponent >= -3 && logical_exponent <= 252);

    // The mantissa may not intrude on the sign bit or the exponent.
    BC_ASSERT(is_zero(mantissa & 0xff800000));

    // The logical 0 exponent is represented as 3 (decimal point).
    const uint32_t exponent = logical_exponent + 3;

    // Construct the non-normalized compact value.
    return (exponent << 24) | ((negative ? 1 : 0) << 23) | mantissa;
}

// expand

BOOST_AUTO_TEST_CASE(compact__expand__negative3_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, true, 0x007fffff))), 0x00000000u);

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-3, false, 0x007fffff))), 0x00000000u);
}

BOOST_AUTO_TEST_CASE(compact__expand__negative2_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, true, 0x007fffff))), 0x00000000u);

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-2, false, 0x007fffff))), 0x017f0000u);
}

BOOST_AUTO_TEST_CASE(compact__expand__negative1_exponent_normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, true, 0x007fffff))), 0x00000000u);

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0x0000ffff))), 0x0200ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(-1, false, 0x007fffff))), 0x027fff00u);
}

BOOST_AUTO_TEST_CASE(compact__expand__zero_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, true, 0x007fffff))), 0x00000000u);

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0x000000ff))), 0x0200ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0x0000ffff))), 0x0300ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(0, false, 0x007fffff))), 0x037fffffu);
}

BOOST_AUTO_TEST_CASE(compact__expand__positive1_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, true, 0x007fffff))), 0x00000000u);

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0x000000ff))), 0x0300ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0x0000ffff))), 0x0400ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(1, false, 0x007fffff))), 0x047fffffu);
}

BOOST_AUTO_TEST_CASE(compact__expand__positive29_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, true, 0x007fffff))), 0x00000000u);

    // positive
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0x000000ff))), 0x1f00ff00u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0x0000ffff))), 0x2000ffffu);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(29, false, 0x007fffff))), 0x207fffffu);
}

BOOST_AUTO_TEST_CASE(compact__expand__positive30_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, true, 0x007fffff))), 0x00000000u);

    // positive, overflow above 0x0000ffff
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0x000000ff))), 0x00000000u); // strict
////BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0x000000ff))), 0x2000ff00u); // lax
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0x0000ffff))), 0x00000000u); // n^32->0
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(30, false, 0x007fffff))), 0x00000000u);
}

BOOST_AUTO_TEST_CASE(compact__expand__positive31_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, true, 0x007fffff))), 0x00000000u);

    // positive, overflow above 0x000000ff
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0x000000ff))), 0x00000000u); // strict
////BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0x000000ff))), 0x2100ff00u); // lax
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(31, false, 0x007fffff))), 0x00000000u);
}

BOOST_AUTO_TEST_CASE(compact__expand__positive32_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, true, 0x007fffff))), 0x00000000u);

    // positive, always overflow
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(32, false, 0x007fffff))), 0x00000000u);
}

BOOST_AUTO_TEST_CASE(compact__expand__positive252_exponent__normalizes_as_expected)
{
    // negative, always zero
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, true, 0x007fffff))), 0x00000000u);

    // positive, always overflow.
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0x00000000))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0x000000ff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0x0000ffff))), 0x00000000u);
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(factory(252, false, 0x007fffff))), 0x00000000u);
}

// expand/compress

BOOST_AUTO_TEST_CASE(compact__expand_compress__proof_of_work_limit__round_trips)
{
    const auto pow_limit = settings(chain::selection::mainnet).proof_of_work_limit;
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(pow_limit)), pow_limit);
}

BOOST_AUTO_TEST_CASE(compact__expand_compress__no_retarget_proof_of_work_limit__round_trips)
{
    const auto no_pow_limit = settings(chain::selection::regtest).proof_of_work_limit;
    BOOST_CHECK_EQUAL(compact::compress(compact::expand(no_pow_limit)), no_pow_limit);
}

// compress/expand

BOOST_AUTO_TEST_CASE(compact__compress_expand__zero__round_trips)
{
    BOOST_CHECK_EQUAL(uint256_t(0), compact::expand(compact::compress(uint256_t(0))));
}

BOOST_AUTO_TEST_CASE(compact__expand_expand__42__round_trips)
{
    BOOST_CHECK_EQUAL(uint256_t(42), compact::expand(compact::compress(uint256_t(42))));
}

BOOST_AUTO_TEST_SUITE_END()

// Satoshi: for any exponent [0x00..0x000000ff] and mantissa [0x000000..0x007fffff].
//bool overflow =
//(
//    (exponent > 34 && mantissa > 0x000000) || // invalid
//    (exponent > 33 && mantissa > 0x0000ff) || // lax    (34 with 1)
//    (exponent > 32 && mantissa > 0x00ffff)    // strict (33 with 2)
//)

// > 0x0000 <= 0x00ff (overflow if exponent > 31 and ceilinged_log256(mantissa) > 1)
static_assert(ceilinged_log256(0x00000001ul) == 1);
static_assert(ceilinged_log256(0x000000fful) == 1);

// > 0x00ff <= 0x0000ffff (eoverflow if exponent > 30 and ceilinged_log256(mantissa) > 2)
static_assert(ceilinged_log256(0x00000100ul) == 2);
static_assert(ceilinged_log256(0x0000fffful) == 2);

// > 0x0000ffff (<= 0x007ffffful) (overflow if exponent > 29 and ceilinged_log256(mantissa) > 3)
static_assert(ceilinged_log256(0x00010000ul) == 3);
static_assert(ceilinged_log256(0x007ffffful) == 3);
