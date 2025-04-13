/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitset>

BOOST_AUTO_TEST_SUITE(bloom_tests)

////#define HAVE_SLOW_TESTS

template <size_t m, size_t k>
class accessor
  : public bloom<m, k>
{
public:
    using base = bloom<m, k>;
    using type = base::type;

    using base::empty;
    using base::is_empty;
    using base::is_saturated;
};

// Ensure default/nop behavior.
static_assert( accessor<0, 0>::is_screened(accessor<0, 0>::empty, 42));
static_assert( accessor<1, 0>::is_screened(accessor<1, 0>::empty, 42));
static_assert(!accessor<1, 1>::is_screened(accessor<1, 1>::empty, 42));

BOOST_AUTO_TEST_CASE(bloom__screened__forward__expected)
{
    constexpr size_t m = 32;
    constexpr size_t k = 4;
    using bloom_t = accessor<m, k>;

    // This generation is based on the manually-generated matrix.
    constexpr std_array<uint32_t, m> forward
    {
        0b00000000000000000000000000000000_u32, // 0
        0b00000000000000000000000000000001_u32, // 1
        0b00000000000000000000000000000010_u32, // 2
        0b00000000000000000000000000000011_u32, // 3
        0b00000000000000000000000000000100_u32, // 4
        0b00000000000000000000000000000101_u32, // 5
        0b00000000000000000000000000000110_u32, // 6
        0b00000000000000000000000000000111_u32, // 7
        0b00000000000000000000000000001000_u32, // 8
        0b00000000000000000000000000001001_u32, // 9
        0b00000000000000000000000000001010_u32, // 10
        0b00000000000000000000000000001011_u32, // 11
        0b00000000000000000000000000001100_u32, // 12
        0b00000000000000000000000000001101_u32, // 13
        0b00000000000000000000000000001110_u32, // 14
        0b00000000000000000000000000001111_u32, // 15
        0b00000000000000000000000000010000_u32, // 16
        0b00000000000000000000000000010001_u32, // 17
        0b00000000000000000000000000010010_u32, // 18
        0b00000000000000000000000000010011_u32, // 19
        0b00000000000000000000000000010100_u32, // 20
        0b00000000000000000000000000010101_u32, // 21
        0b00000000000000000000000000010110_u32, // 22
        0b00000000000000000000000000010111_u32, // 23
        0b00000000000000000000000000011000_u32, // 24
        0b00000000000000000000000000011001_u32, // 25
        0b00000000000000000000000000011010_u32, // 26
        0b00000000000000000000000000011011_u32, // 27
        0b00000000000000000000000000011100_u32, // 28
        0b00000000000000000000000000011101_u32, // 29
        0b00000000000000000000000000011110_u32, // 30
        0b00000000000000000000000000011111_u32  // 31
    };

    // none screened
    auto value = bloom_t::empty;
    BOOST_CHECK( bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111111111111u);

    // 0 screened
    value = bloom_t::screen(value, forward[0]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111111111110u);

    // 1 screened
    value = bloom_t::screen(value, forward[1]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111111111100u);

    // 2 screened
    value = bloom_t::screen(value, forward[2]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111111111000u);

    // 3 screened
    value = bloom_t::screen(value, forward[3]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111111110000u);

    // 4 screened
    value = bloom_t::screen(value, forward[4]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111111100000u);

    // 5 screened
    value = bloom_t::screen(value, forward[5]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111111000000u);

    // 6 screened
    value = bloom_t::screen(value, forward[6]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111110000000u);

    // 7 screened
    value = bloom_t::screen(value, forward[7]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111100000000u);

    // 8 screened
    value = bloom_t::screen(value, forward[8]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111000000000u);

    // 9 screened
    value = bloom_t::screen(value, forward[9]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111110000000000u);

    // 10 screened
    value = bloom_t::screen(value, forward[10]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111100000000000u);

    // 11 screened
    value = bloom_t::screen(value, forward[11]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111000000000000u);

    // 12 screened
    value = bloom_t::screen(value, forward[12]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111110000000000000u);

    // 13 screened
    value = bloom_t::screen(value, forward[13]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111100000000000000u);

    // 14 screened
    value = bloom_t::screen(value, forward[14]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111000000000000000u);

    // 15 screened
    value = bloom_t::screen(value, forward[15]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, forward[0]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[1]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[2]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[3]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[4]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[5]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[6]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[7]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[8]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[9]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[10]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[11]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[12]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[13]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[14]));
    BOOST_CHECK( bloom_t::is_screened(value, forward[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111110000000000000000u);

    // Not saturated (already screened).
    value = bloom_t::screen(value, forward[15]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));

    // Saturated (not already screened).
    value = bloom_t::screen(value, forward[16]);
    value = bloom_t::screen(value, forward[17]);
    value = bloom_t::screen(value, forward[18]);
    value = bloom_t::screen(value, forward[19]);
    value = bloom_t::screen(value, forward[20]);
    value = bloom_t::screen(value, forward[21]);
    value = bloom_t::screen(value, forward[22]);
    value = bloom_t::screen(value, forward[23]);
    value = bloom_t::screen(value, forward[24]);
    value = bloom_t::screen(value, forward[25]);
    value = bloom_t::screen(value, forward[26]);
    value = bloom_t::screen(value, forward[27]);
    value = bloom_t::screen(value, forward[28]);
    value = bloom_t::screen(value, forward[29]);
    value = bloom_t::screen(value, forward[30]);
    value = bloom_t::screen(value, forward[31]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK( bloom_t::is_saturated(value));
}

BOOST_AUTO_TEST_CASE(bloom__screened__reverse__expected)
{
    constexpr size_t m = 32;
    constexpr size_t k = 4;
    using bloom_t = accessor<m, k>;

    // This generation is based on the manually-generated matrix.
    constexpr std_array<uint32_t, m> reverse
    {
        0b11111111111111111111111111111111_u32, // 0
        0b11111111111111111111111111111110_u32, // 1
        0b11111111111111111111111111111101_u32, // 2
        0b11111111111111111111111111111100_u32, // 3
        0b11111111111111111111111111111011_u32, // 4
        0b11111111111111111111111111111010_u32, // 5
        0b11111111111111111111111111111001_u32, // 6
        0b11111111111111111111111111111000_u32, // 7
        0b11111111111111111111111111110111_u32, // 8
        0b11111111111111111111111111110110_u32, // 9
        0b11111111111111111111111111110101_u32, // 10
        0b11111111111111111111111111110100_u32, // 11
        0b11111111111111111111111111110011_u32, // 12
        0b11111111111111111111111111110010_u32, // 13
        0b11111111111111111111111111110001_u32, // 14
        0b11111111111111111111111111110000_u32, // 15
        0b11111111111111111111111111101111_u32, // 16
        0b11111111111111111111111111101110_u32, // 17
        0b11111111111111111111111111101101_u32, // 18
        0b11111111111111111111111111101100_u32, // 19
        0b11111111111111111111111111101011_u32, // 20
        0b11111111111111111111111111101010_u32, // 21
        0b11111111111111111111111111101001_u32, // 22
        0b11111111111111111111111111101000_u32, // 23
        0b11111111111111111111111111100111_u32, // 24
        0b11111111111111111111111111100110_u32, // 25
        0b11111111111111111111111111100101_u32, // 26
        0b11111111111111111111111111100100_u32, // 27
        0b11111111111111111111111111100011_u32, // 28
        0b11111111111111111111111111100010_u32, // 29
        0b11111111111111111111111111100001_u32, // 30
        0b11111111111111111111111111100000_u32  // 31
    };

    // none screened
    auto value = bloom_t::empty;
    BOOST_CHECK( bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b11111111111111111111111111111111u);

    // 0 screened
    value = bloom_t::screen(value, reverse[0]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b01111111111111111111111111111111u);

    // 1 screened
    value = bloom_t::screen(value, reverse[1]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00111111111111111111111111111111u);

    // 2 screened
    value = bloom_t::screen(value, reverse[2]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00011111111111111111111111111111u);

    // 3 screened
    value = bloom_t::screen(value, reverse[3]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00001111111111111111111111111111u);

    // 4 screened
    value = bloom_t::screen(value, reverse[4]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000111111111111111111111111111u);

    // 5 screened
    value = bloom_t::screen(value, reverse[5]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000011111111111111111111111111u);

    // 6 screened
    value = bloom_t::screen(value, reverse[6]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000001111111111111111111111111u);

    // 7 screened
    value = bloom_t::screen(value, reverse[7]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000111111111111111111111111u);

    // 8 screened
    value = bloom_t::screen(value, reverse[8]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000011111111111111111111111u);

    // 9 screened
    value = bloom_t::screen(value, reverse[9]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000001111111111111111111111u);

    // 10 screened
    value = bloom_t::screen(value, reverse[10]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000000111111111111111111111u);

    // 11 screened
    value = bloom_t::screen(value, reverse[11]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000000011111111111111111111u);

    // 12 screened
    value = bloom_t::screen(value, reverse[12]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000000001111111111111111111u);

    // 13 screened
    value = bloom_t::screen(value, reverse[13]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000000000111111111111111111u);

    // 14 screened
    value = bloom_t::screen(value, reverse[14]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000000000011111111111111111u);

    // 15 screened
    value = bloom_t::screen(value, reverse[15]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[0]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[1]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[2]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[3]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[4]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[5]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[6]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[7]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[8]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[9]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[10]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[11]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[12]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[13]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[14]));
    BOOST_CHECK( bloom_t::is_screened(value, reverse[15]));
    BOOST_CHECK_EQUAL(value, 0b00000000000000001111111111111111u);

    // Not saturated (already screened).
    value = bloom_t::screen(value, reverse[15]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));

    // Saturated (not already screened).
    value = bloom_t::screen(value, reverse[16]);
    value = bloom_t::screen(value, reverse[17]);
    value = bloom_t::screen(value, reverse[18]);
    value = bloom_t::screen(value, reverse[19]);
    value = bloom_t::screen(value, reverse[20]);
    value = bloom_t::screen(value, reverse[21]);
    value = bloom_t::screen(value, reverse[22]);
    value = bloom_t::screen(value, reverse[23]);
    value = bloom_t::screen(value, reverse[24]);
    value = bloom_t::screen(value, reverse[25]);
    value = bloom_t::screen(value, reverse[26]);
    value = bloom_t::screen(value, reverse[27]);
    value = bloom_t::screen(value, reverse[28]);
    value = bloom_t::screen(value, reverse[29]);
    value = bloom_t::screen(value, reverse[30]);
    value = bloom_t::screen(value, reverse[31]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK( bloom_t::is_saturated(value));
}

BOOST_AUTO_TEST_CASE(bloom__screen__full__expected_saturation)
{
    constexpr size_t m = 32;
    constexpr size_t k = 4;
    using bloom_t = accessor<m, k>;

    // This generation is based on the manually-generated matrix.
    constexpr std_array<uint32_t, m> forward
    {
        0b00000000000000000000000000000000_u32, // 0
        0b00000000000000000000000000000001_u32, // 1
        0b00000000000000000000000000000010_u32, // 2
        0b00000000000000000000000000000011_u32, // 3
        0b00000000000000000000000000000100_u32, // 4
        0b00000000000000000000000000000101_u32, // 5
        0b00000000000000000000000000000110_u32, // 6
        0b00000000000000000000000000000111_u32, // 7
        0b00000000000000000000000000001000_u32, // 8
        0b00000000000000000000000000001001_u32, // 9
        0b00000000000000000000000000001010_u32, // 10
        0b00000000000000000000000000001011_u32, // 11
        0b00000000000000000000000000001100_u32, // 12
        0b00000000000000000000000000001101_u32, // 13
        0b00000000000000000000000000001110_u32, // 14
        0b00000000000000000000000000001111_u32, // 15
        0b00000000000000000000000000010000_u32, // 16
        0b00000000000000000000000000010001_u32, // 17
        0b00000000000000000000000000010010_u32, // 18
        0b00000000000000000000000000010011_u32, // 19
        0b00000000000000000000000000010100_u32, // 20
        0b00000000000000000000000000010101_u32, // 21
        0b00000000000000000000000000010110_u32, // 22
        0b00000000000000000000000000010111_u32, // 23
        0b00000000000000000000000000011000_u32, // 24
        0b00000000000000000000000000011001_u32, // 25
        0b00000000000000000000000000011010_u32, // 26
        0b00000000000000000000000000011011_u32, // 27
        0b00000000000000000000000000011100_u32, // 28
        0b00000000000000000000000000011101_u32, // 29
        0b00000000000000000000000000011110_u32, // 30
        0b00000000000000000000000000011111_u32  // 31
    };

    bloom_t::type previous{};
    auto value = bloom_t::empty;
    BOOST_CHECK( bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));
    BOOST_CHECK(!bloom_t::is_screened(value, forward[0]));

    value = bloom_t::screen(previous = value, forward[0]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[1]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[2]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[3]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[4]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[5]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[6]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[7]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[8]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[9]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[10]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[11]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[12]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[13]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[14]);
    BOOST_CHECK(value != previous);

    value = bloom_t::screen(previous = value, forward[15]);
    BOOST_CHECK(value != previous);

    // Already screened, not bloom change.
    value = bloom_t::screen(previous = value, forward[15]);
    BOOST_CHECK(value == previous);

    // Full but not saturated.
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK(!bloom_t::is_saturated(value));

    // Saturate.
    value = bloom_t::screen(value, forward[16]);
    value = bloom_t::screen(value, forward[17]);
    value = bloom_t::screen(value, forward[18]);
    value = bloom_t::screen(value, forward[19]);
    value = bloom_t::screen(value, forward[20]);
    value = bloom_t::screen(value, forward[21]);
    value = bloom_t::screen(value, forward[22]);
    value = bloom_t::screen(value, forward[23]);
    value = bloom_t::screen(value, forward[24]);
    value = bloom_t::screen(value, forward[25]);
    value = bloom_t::screen(value, forward[26]);
    value = bloom_t::screen(value, forward[27]);
    value = bloom_t::screen(value, forward[28]);
    value = bloom_t::screen(value, forward[29]);
    value = bloom_t::screen(value, forward[30]);
    value = bloom_t::screen(value, forward[31]);
    BOOST_CHECK(!bloom_t::is_empty(value));
    BOOST_CHECK( bloom_t::is_saturated(value));

#if defined(HAVE_SLOW_TESTS)
    // All values must be screened once saturated (full and overflow).
    for (uint32_t finger{}; finger < max_uint32; ++finger)
    {
        if (!bloom_t::is_screened(value, finger))
        {
            BOOST_REQUIRE(false);
        }
    }
#endif
}

#if defined(HAVE_SLOW_TESTS)

BOOST_AUTO_TEST_CASE(bloom__screen__4_bits_forward__16_screens)
{
    constexpr size_t m = 32;
    constexpr size_t k = 4;
    constexpr size_t limit = power2(m);
    using bloom_t = accessor<m, k>;
    auto next = bloom_t::empty;
    size_t count{};

    // This exhausts the full uint32_t domain and returns exactly limit values.
    // The count < limit condition just speeds up the test, still slow though.
    for (uint32_t finger{}; count < limit; ++finger)
    {
        const auto previous = next;
        next = bloom_t::screen(next, finger);
        if (!bloom_t::is_collision(previous, next))
        {
            std::cout << "0b" << std::bitset<m>(finger)
                << "_u32, // " << count << std::endl;
            count++;
        }
    }

    BOOST_CHECK_EQUAL(count, limit);
}

BOOST_AUTO_TEST_CASE(bloom__screen__4_bits_reverse__16_screens)
{
    constexpr size_t m = 32;
    constexpr size_t k = 4;
    constexpr size_t limit = power2(m);
    using bloom_t = accessor<m, k>;
    auto next = bloom_t::empty;
    size_t count{};

    for (auto finger = max_uint32; count < limit; --finger)
    {
        const auto previous = next;
        next = bloom_t::screen(next, finger);
        if (!bloom_t::is_collision(previous, next))
        {
            std::cout << "0b" << std::bitset<m>(finger)
                << "_u32, // " << count << std::endl;
            count++;
        }
    }

    BOOST_CHECK_EQUAL(count, limit);
}

#endif // HAVE_SLOW_TESTS

BOOST_AUTO_TEST_SUITE_END()
