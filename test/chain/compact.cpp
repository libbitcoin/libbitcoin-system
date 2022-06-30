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

using namespace system::chain;

// zero => zero.
// small values are left-shifted with negative (1..2) exponent.
// 3 byte values values are not shifted zero exponent (3) exponent.
// > 3 byte values are right-shifted with positive (4..32) exponent, losing the precision of all bytes of order > 3.
// A shift of >32 cannot be produced, since there are only 32 bytes of input and no negative shift.

// Half byte steps (leading bit unset).
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000000"))) == 0x00000000ul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000007"))) == 0x01070000ul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000007b"))) == 0x017b0000ul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000007bc"))) == 0x0207bc00ul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000007bcd"))) == 0x027bcd00ul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000007bcde"))) == 0x0307bcdeul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000007bcdef"))) == 0x037bcdeful);

// Full byte steps (leading bit unset).
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000007bcdefaa"))) == 0x047bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000007bcdefa7bb"))) == 0x057bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000007bcdefa7bbcc"))) == 0x067bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000007bcdefa7bbccdd"))) == 0x077bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000007bcdefa7bbccddee"))) == 0x087bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000007bcdefa7bbccddeeff"))) == 0x097bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000007bcdefa7bbccddeeff00"))) == 0x0a7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000007bcdefa7bbccddeeff0011"))) == 0x0b7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000007bcdefa7bbccddeeff001122"))) == 0x0c7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000007bcdefa7bbccddeeff00112233"))) == 0x0d7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000007bcdefa7bbccddeeff0011223344"))) == 0x0e7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000007bcdefa7bbccddeeff001122334455"))) == 0x0f7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000007bcdefa7bbccddeeff00112233445566"))) == 0x107bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000007bcdefa7bbccddeeff0011223344556677"))) == 0x117bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000007bcdefa7bbccddeeff001122334455667788"))) == 0x127bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000007bcdefa7bbccddeeff00112233445566778899"))) == 0x137bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000007bcdefa7bbccddeeff00112233445566778899aa"))) == 0x147bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000007bcdefa7bbccddeeff00112233445566778899a7bb"))) == 0x157bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000007bcdefa7bbccddeeff00112233445566778899a7bbcc"))) == 0x167bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccdd"))) == 0x177bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddee"))) == 0x187bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("000000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff"))) == 0x197bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00"))) == 0x1a7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff0011"))) == 0x1b7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("000000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff001122"))) == 0x1c7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("0000007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00112233"))) == 0x1d7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("00007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff0011223344"))) == 0x1e7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("007bcdefa7bbccddeeff00112233445566778899a7bbccddeeff001122334455"))) == 0x1f7bcdeful);
static_assert(compact::compress(to_uintx(base16_hash("7bcdefa7bbccddeeff00112233445566778899a7bbccddeeff00112233445566"))) == 0x207bcdeful);

// zero => zero.
// small values are left-shifted with negative (1..2) exponent.
// 3 byte values values are not shifted zero exponent (3) exponent.
// > 3 byte values are right-shifted with positive (4..32) exponent, losing the precision of all bytes of order > 3.
// A shift of 33 is produced from the negative shift.
// A shift of >33 cannot be produced, since there are only 32 bytes of input, with one possible negative shift.

// Half byte steps (leading bit set).
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000000"))) == 0x00000000ul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000000a"))) == 0x010a0000ul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000000ab"))) == 0x0200ab00ul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000000abc"))) == 0x020abc00ul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000000000abcd"))) == 0x0300abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000000abcde"))) == 0x030abcdeul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000000000abcdef"))) == 0x0400abcdul);

// Full byte steps (leading bit set).
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000000000abcdefaa"))) == 0x0500abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000000000abcdefaabb"))) == 0x0600abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000000000abcdefaabbcc"))) == 0x0700abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000000000abcdefaabbccdd"))) == 0x0800abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000000000abcdefaabbccddee"))) == 0x0900abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000000000abcdefaabbccddeeff"))) == 0x0a00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000000000abcdefaabbccddeeff00"))) == 0x0b00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000000000abcdefaabbccddeeff0011"))) == 0x0c00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000000000abcdefaabbccddeeff001122"))) == 0x0d00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000000000abcdefaabbccddeeff00112233"))) == 0x0e00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000000000abcdefaabbccddeeff0011223344"))) == 0x0f00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000000000abcdefaabbccddeeff001122334455"))) == 0x1000abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000000000abcdefaabbccddeeff00112233445566"))) == 0x1100abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000000000abcdefaabbccddeeff0011223344556677"))) == 0x1200abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000000000abcdefaabbccddeeff001122334455667788"))) == 0x1300abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000000000abcdefaabbccddeeff00112233445566778899"))) == 0x1400abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000000000abcdefaabbccddeeff00112233445566778899aa"))) == 0x1500abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000000000abcdefaabbccddeeff00112233445566778899aabb"))) == 0x1600abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000000000abcdefaabbccddeeff00112233445566778899aabbcc"))) == 0x1700abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000000000abcdefaabbccddeeff00112233445566778899aabbccdd"))) == 0x1800abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000000000abcdefaabbccddeeff00112233445566778899aabbccddee"))) == 0x1900abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff"))) == 0x1a00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff00"))) == 0x1b00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000000000abcdefaabbccddeeff00112233445566778899aabbccddeeff0011"))) == 0x1c00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00000000abcdefaabbccddeeff00112233445566778899aabbccddeeff001122"))) == 0x1d00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("000000abcdefaabbccddeeff00112233445566778899aabbccddeeff00112233"))) == 0x1e00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("0000abcdefaabbccddeeff00112233445566778899aabbccddeeff0011223344"))) == 0x1f00abcdul);
static_assert(compact::compress(to_uintx(base16_hash("00abcdefaabbccddeeff00112233445566778899aabbccddeeff001122334455"))) == 0x2000abcdul);
static_assert(compact::compress(to_uintx(base16_hash("abcdefaabbccddeeff00112233445566778899aabbccddeeff00112233445566"))) == 0x2100abcdul);

constexpr uint32_t factory(int32_t logical_exponent, bool negative, uint32_t mantissa) NOEXCEPT
{
    return ((logical_exponent + 3) << 24) | ((negative ? 1 : 0) << 23) | mantissa;
}

// expand

// negative, always zero
static_assert(compact::compress(compact::expand(factory(-3, true, 0x00000000))) == 0x00000000u);    // negative...
static_assert(compact::compress(compact::expand(factory(-3, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(-3, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(-3, true, 0x007fffff))) == 0x00000000u);

// positive
static_assert(compact::compress(compact::expand(factory(-3, false, 0x00000000))) == 0x00000000u);   // zero
static_assert(compact::compress(compact::expand(factory(-3, false, 0x000000ff))) == 0x00000000u);   // shift to zero...
static_assert(compact::compress(compact::expand(factory(-3, false, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(-3, false, 0x007fffff))) == 0x00000000u);

// negative, always zero
static_assert(compact::compress(compact::expand(factory(-2, true, 0x00000000))) == 0x00000000u);    // negative...
static_assert(compact::compress(compact::expand(factory(-2, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(-2, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(-2, true, 0x007fffff))) == 0x00000000u);

// positive
static_assert(compact::compress(compact::expand(factory(-2, false, 0x00000000))) == 0x00000000u);   // zero
static_assert(compact::compress(compact::expand(factory(-2, false, 0x000000ff))) == 0x00000000u);   // shift to zero...
static_assert(compact::compress(compact::expand(factory(-2, false, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(-2, false, 0x007fffff))) == 0x017f0000u);   // valid

// negative, always zero
static_assert(compact::compress(compact::expand(factory(-1, true, 0x00000000))) == 0x00000000u);    // negative...
static_assert(compact::compress(compact::expand(factory(-1, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(-1, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(-1, true, 0x007fffff))) == 0x00000000u);

// positive
static_assert(compact::compress(compact::expand(factory(-1, false, 0x00000000))) == 0x00000000u);   // zero
static_assert(compact::compress(compact::expand(factory(-1, false, 0x000000ff))) == 0x00000000u);   // shift to zero
static_assert(compact::compress(compact::expand(factory(-1, false, 0x0000ffff))) == 0x0200ff00u);   // valid...
static_assert(compact::compress(compact::expand(factory(-1, false, 0x007fffff))) == 0x027fff00u);

// negative, always zero
static_assert(compact::compress(compact::expand(factory(0, true, 0x00000000))) == 0x00000000u);     // negative...
static_assert(compact::compress(compact::expand(factory(0, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(0, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(0, true, 0x007fffff))) == 0x00000000u);

// positive
static_assert(compact::compress(compact::expand(factory(0, false, 0x00000000))) == 0x00000000u);    // zero
static_assert(compact::compress(compact::expand(factory(0, false, 0x000000ff))) == 0x0200ff00u);    // valid...
static_assert(compact::compress(compact::expand(factory(0, false, 0x0000ffff))) == 0x0300ffffu);
static_assert(compact::compress(compact::expand(factory(0, false, 0x007fffff))) == 0x037fffffu);

// negative, always zero
static_assert(compact::compress(compact::expand(factory(1, true, 0x00000000))) == 0x00000000u);     // negative...
static_assert(compact::compress(compact::expand(factory(1, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(1, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(1, true, 0x007fffff))) == 0x00000000u);

// positive
static_assert(compact::compress(compact::expand(factory(1, false, 0x00000000))) == 0x00000000u);    // zero
static_assert(compact::compress(compact::expand(factory(1, false, 0x000000ff))) == 0x0300ff00u);    // valid...
static_assert(compact::compress(compact::expand(factory(1, false, 0x0000ffff))) == 0x0400ffffu);
static_assert(compact::compress(compact::expand(factory(1, false, 0x007fffff))) == 0x047fffffu);

// negative, always zero
static_assert(compact::compress(compact::expand(factory(29, true, 0x00000000))) == 0x00000000u);    // negative...
static_assert(compact::compress(compact::expand(factory(29, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(29, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(29, true, 0x007fffff))) == 0x00000000u);

// positive
static_assert(compact::compress(compact::expand(factory(29, false, 0x00000000))) == 0x00000000u);   // zero
static_assert(compact::compress(compact::expand(factory(29, false, 0x000000ff))) == 0x1f00ff00u);   // valid...
static_assert(compact::compress(compact::expand(factory(29, false, 0x0000ffff))) == 0x2000ffffu);
static_assert(compact::compress(compact::expand(factory(29, false, 0x007fffff))) == 0x207fffffu);

// negative, always zero
static_assert(compact::compress(compact::expand(factory(30, true, 0x00000000))) == 0x00000000u);    // negative...
static_assert(compact::compress(compact::expand(factory(30, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(30, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(30, true, 0x007fffff))) == 0x00000000u);

// positive, overflow above 0x0000ffff
static_assert(compact::compress(compact::expand(factory(30, false, 0x00000000))) == 0x00000000u);   // zero
static_assert(compact::compress(compact::expand(factory(30, false, 0x000000ff))) == 0x00000000u);   // requires lax (^33|1)
static_assert(compact::compress(compact::expand(factory(30, false, 0x0000ffff))) == 0x2100ffffu);   // strict       (^33|2)
static_assert(compact::compress(compact::expand(factory(30, false, 0x007fffff))) == 0x00000000u);   // overflow

// negative, always zero
static_assert(compact::compress(compact::expand(factory(31, true, 0x00000000))) == 0x00000000u);    // negative...
static_assert(compact::compress(compact::expand(factory(31, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(31, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(31, true, 0x007fffff))) == 0x00000000u);

// positive, overflow above 0x000000ff
static_assert(compact::compress(compact::expand(factory(31, false, 0x00000000))) == 0x00000000u);   // zero
static_assert(compact::compress(compact::expand(factory(31, false, 0x000000ff))) == 0x00000000u);   // requires lax (^34|1)
static_assert(compact::compress(compact::expand(factory(31, false, 0x0000ffff))) == 0x00000000u);   // overflow...
static_assert(compact::compress(compact::expand(factory(31, false, 0x007fffff))) == 0x00000000u);

// negative, always zero
static_assert(compact::compress(compact::expand(factory(32, true, 0x00000000))) == 0x00000000u);    // negative...
static_assert(compact::compress(compact::expand(factory(32, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(32, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(32, true, 0x007fffff))) == 0x00000000u);

// positive, always overflow
static_assert(compact::compress(compact::expand(factory(32, false, 0x00000000))) == 0x00000000u);   // overflow...
static_assert(compact::compress(compact::expand(factory(32, false, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(32, false, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(32, false, 0x007fffff))) == 0x00000000u);

// negative, always zero
static_assert(compact::compress(compact::expand(factory(252, true, 0x00000000))) == 0x00000000u);   // negative...
static_assert(compact::compress(compact::expand(factory(252, true, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(252, true, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(252, true, 0x007fffff))) == 0x00000000u);

// positive, always overflow.
static_assert(compact::compress(compact::expand(factory(252, false, 0x00000000))) == 0x00000000u);  // overflow...
static_assert(compact::compress(compact::expand(factory(252, false, 0x000000ff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(252, false, 0x0000ffff))) == 0x00000000u);
static_assert(compact::compress(compact::expand(factory(252, false, 0x007fffff))) == 0x00000000u);

// expand/compress

constexpr auto mainnet = 0x1d00ffff;
static_assert(compact::compress(compact::expand(mainnet)) == mainnet);

constexpr auto regtest = 0x207fffff;
static_assert(compact::compress(compact::expand(regtest)) == regtest);

// compress/expand

static_assert(compact::expand(compact::compress(uint256_t(0))) == uint256_t(0));
static_assert(compact::expand(compact::compress(uint256_t(42))) == uint256_t(42));

// Satoshi: for any exponent [0x00..0x000000ff] and mantissa [0x000000..0x007fffff].
//bool overflow =
//(
//    (exponent > 34 && mantissa > 0x000000) || // invalid (35 with 0)
//    (exponent > 33 && mantissa > 0x0000ff) || // lax     (34 with 0|1)
//    (exponent > 32 && mantissa > 0x00ffff)    // strict  (33 with 1|2)
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
