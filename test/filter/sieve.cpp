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

BOOST_AUTO_TEST_SUITE(sieve_tests)

////#define HAVE_SLOW_TESTS

template <size_t m, size_t k>
class accessor
  : public sieve<m, k>
{
public:
    using base = sieve<m, k>;
    using type = base::type;

    using base::empty;
    using base::is_empty;
    using base::is_saturated;
};

// Ensure default/nop behavior.
static_assert( accessor<0, 0>::is_screened(accessor<0, 0>::empty, 42));
static_assert( accessor<1, 0>::is_screened(accessor<1, 0>::empty, 42));
static_assert(!accessor<1, 1>::is_screened(accessor<1, 1>::empty, 42));

BOOST_AUTO_TEST_CASE(sieve__screened__forward__expected)
{
    constexpr size_t sieve_bits = 32;
    constexpr size_t select_bits = 4;
    constexpr size_t limit = power2(select_bits);
    using sieve_t = accessor<sieve_bits, select_bits>;

    // This generation is based on the manually-generated matrix.
    constexpr std_array<uint32_t, limit> forward
    {
        0b00000000000000000000000000000000_u32, // 0
        0b00000000000000000000000000000001_u32, // 1
        0b00000000000000000100000000000000_u32, // 2
        0b00000000000001000000000000100000_u32, // 3
        0b00000000001000000000000010000000_u32, // 4
        0b00000000010000000000000100000000_u32, // 5
        0b00000000100000000000001000000000_u32, // 6
        0b00000001000000000000010000000000_u32, // 7
        0b00000001000000000000010000000001_u32, // 8
        0b00000001000000001000100000000001_u32, // 9
        0b00000010000000001000100000000001_u32, // 10
        0b00000010000000001000100000000010_u32, // 11
        0b00000010000000001000100000000110_u32, // 12
        0b00000010000010010000100000000110_u32, // 13
        0b00000100000010010001000000000110_u32, // 14
        0b00000100000010010001100000000110_u32  // 15
    };

    // none screened
    auto value = sieve_t::empty;
    BOOST_CHECK( sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 0 screened
    value = sieve_t::screen(value, forward[0]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 1 screened
    value = sieve_t::screen(value, forward[1]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 2 screened
    value = sieve_t::screen(value, forward[2]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 3 screened
    value = sieve_t::screen(value, forward[3]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 4 screened
    value = sieve_t::screen(value, forward[4]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 5 screened
    value = sieve_t::screen(value, forward[5]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 6 screened
    value = sieve_t::screen(value, forward[6]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 7 screened
    value = sieve_t::screen(value, forward[7]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 8 screened
    value = sieve_t::screen(value, forward[8]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 9 screened
    value = sieve_t::screen(value, forward[9]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 10 screened
    value = sieve_t::screen(value, forward[10]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 11 screened
    value = sieve_t::screen(value, forward[11]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 12 screened
    value = sieve_t::screen(value, forward[12]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 13 screened
    value = sieve_t::screen(value, forward[13]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 14 screened
    value = sieve_t::screen(value, forward[14]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[15]));

    // 15 screened
    value = sieve_t::screen(value, forward[15]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, forward[0]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[1]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[2]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[3]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[4]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[5]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[6]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[7]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[8]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[9]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[10]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[11]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[12]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[13]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[14]));
    BOOST_CHECK( sieve_t::is_screened(value, forward[15]));

    // Not saturated (already screened).
    value = sieve_t::screen(value, forward[15]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));

    // Saturated (not already screened).
    value = sieve_t::screen(value, 0b0000'0100000011011001100000000110);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK( sieve_t::is_saturated(value));
}

BOOST_AUTO_TEST_CASE(sieve__screened__reverse__expected)
{
    constexpr size_t sieve_bits = 32;
    constexpr size_t select_bits = 4;
    constexpr size_t limit = power2(select_bits);
    using sieve_t = accessor<sieve_bits, select_bits>;

    // This generation is based on the manually-generated matrix.
    constexpr std_array<uint32_t, limit> reverse
    {
        0b11111111111111111111111111111111_u32, // 0
        0b11111111111111111111111111111110_u32, // 1
        0b11111111111111111011111111111111_u32, // 2
        0b11111111111110111111111111011111_u32, // 3
        0b11111111110111111111111101111111_u32, // 4
        0b11111111101111111111111011111111_u32, // 5
        0b11111111011111111111110111111111_u32, // 6
        0b11111110111111111111101111111111_u32, // 7
        0b11111110111111111111101111111110_u32, // 8
        0b11111110111111110111011111111110_u32, // 9
        0b11111101111111110111011111111110_u32, // 10
        0b11111101111111110111011111111101_u32, // 11
        0b11111101111111110111011111111001_u32, // 12
        0b11111101111101101111011111111001_u32, // 13
        0b11111011111101101110111111111001_u32, // 14
        0b11111011111101101110011111111001_u32  // 15
    };

    // none screened
    auto value = sieve_t::empty;
    BOOST_CHECK( sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 0 screened
    value = sieve_t::screen(value, reverse[0]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 1 screened
    value = sieve_t::screen(value, reverse[1]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 2 screened
    value = sieve_t::screen(value, reverse[2]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 3 screened
    value = sieve_t::screen(value, reverse[3]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 4 screened
    value = sieve_t::screen(value, reverse[4]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 5 screened
    value = sieve_t::screen(value, reverse[5]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 6 screened
    value = sieve_t::screen(value, reverse[6]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 7 screened
    value = sieve_t::screen(value, reverse[7]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 8 screened
    value = sieve_t::screen(value, reverse[8]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 9 screened
    value = sieve_t::screen(value, reverse[9]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 10 screened
    value = sieve_t::screen(value, reverse[10]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 11 screened
    value = sieve_t::screen(value, reverse[11]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 12 screened
    value = sieve_t::screen(value, reverse[12]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 13 screened
    value = sieve_t::screen(value, reverse[13]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 14 screened
    value = sieve_t::screen(value, reverse[14]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK(!sieve_t::is_screened(value, reverse[15]));

    // 15 screened
    value = sieve_t::screen(value, reverse[15]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[0]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[1]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[2]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[3]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[4]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[5]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[6]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[7]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[8]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[9]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[10]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[11]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[12]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[13]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[14]));
    BOOST_CHECK( sieve_t::is_screened(value, reverse[15]));

    // Not saturated (already screened).
    value = sieve_t::screen(value, reverse[15]);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));

    // Saturated (not already screened).
    value = sieve_t::screen(value, 0b0000'0010000000000000001110000000);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK( sieve_t::is_saturated(value));
}

BOOST_AUTO_TEST_CASE(sieve__screen__full__expected_saturation)
{
    constexpr size_t sieve_bits = 32;
    constexpr size_t select_bits = 4;
    constexpr size_t limit = power2(select_bits);
    using sieve_t = accessor<sieve_bits, select_bits>;

    // This generation is based on the manually-generated matrix.
    constexpr std_array<uint32_t, limit> forward
    {
        0b00000000000000000000000000000000_u32, // 0
        0b00000000000000000000000000000001_u32, // 1
        0b00000000000000000100000000000000_u32, // 2
        0b00000000000001000000000000100000_u32, // 3
        0b00000000001000000000000010000000_u32, // 4
        0b00000000010000000000000100000000_u32, // 5
        0b00000000100000000000001000000000_u32, // 6
        0b00000001000000000000010000000000_u32, // 7
        0b00000001000000000000010000000001_u32, // 8
        0b00000001000000001000100000000001_u32, // 9
        0b00000010000000001000100000000001_u32, // 10
        0b00000010000000001000100000000010_u32, // 11
        0b00000010000000001000100000000110_u32, // 12
        0b00000010000010010000100000000110_u32, // 13
        0b00000100000010010001000000000110_u32, // 14
        0b00000100000010010001100000000110_u32  // 15
    };

    sieve_t::type previous{};
    auto value = sieve_t::empty;
    BOOST_CHECK( sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));
    BOOST_CHECK(!sieve_t::is_screened(value, forward[0]));

    value = sieve_t::screen(previous = value, forward[0]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[1]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[2]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[3]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[4]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[5]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[6]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[7]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[8]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[9]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[10]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[11]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[12]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[13]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[14]);
    BOOST_CHECK(value != previous);

    value = sieve_t::screen(previous = value, forward[15]);
    BOOST_CHECK(value != previous);

    // Already screened, not sieve change.
    value = sieve_t::screen(previous = value, forward[15]);
    BOOST_CHECK(value == previous);

    // Full but not saturated.
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK(!sieve_t::is_saturated(value));

    // Saturate.
    value = sieve_t::screen(value, 0b0000'0100000011011001100000000110);
    BOOST_CHECK(!sieve_t::is_empty(value));
    BOOST_CHECK( sieve_t::is_saturated(value));

#if defined(HAVE_SLOW_TESTS)
    // All values must be screened once saturated (full and overflow).
    for (uint32_t finger{}; finger < max_uint32; ++finger)
    {
        if (!sieve_t::is_screened(value, finger))
        {
            BOOST_REQUIRE(false);
        }
    }
#endif
}

#if defined(HAVE_SLOW_TESTS)

BOOST_AUTO_TEST_CASE(sieve__screen__4_bits_forward__16_screens)
{
    constexpr size_t sieve_bits = 32;
    constexpr size_t select_bits = 4;
    constexpr size_t limit = power2(sieve_bits);
    using sieve_t = accessor<sieve_bits, select_bits>;
    auto next = sieve_t::empty;
    size_t count{};

    for (uint32_t finger{}; count < limit; ++finger)
    {
        const auto previous = next;
        next = sieve_t::screen(next, finger);
        if (!sieve_t::is_collision(previous, next))
        {
            std::cout << "0b" << std::bitset<sieve_bits>(finger)
                << "_u32, // " << count << std::endl;
            count++;
        }
    }

    BOOST_CHECK_EQUAL(count, limit);
}

BOOST_AUTO_TEST_CASE(sieve__screen__4_bits_reverse__16_screens)
{
    constexpr size_t sieve_bits = 32;
    constexpr size_t select_bits = 4;
    constexpr size_t limit = power2(sieve_bits);
    using sieve_t = accessor<sieve_bits, select_bits>;
    auto next = sieve_t::empty;
    size_t count{};

    for (auto finger = max_uint32; count < limit; --finger)
    {
        const auto previous = next;
        next = sieve_t::screen(next, finger);
        if (!sieve_t::is_collision(previous, next))
        {
            std::cout << "0b" << std::bitset<sieve_bits>(finger)
                << "_u32, // " << count << std::endl;
            count++;
        }

        if (is_zero(finger))
            break;
    }

    BOOST_CHECK_EQUAL(count, limit);
}

#endif // HAVE_SLOW_TESTS

BOOST_AUTO_TEST_SUITE_END()

#if defined(DISABLED)

void print_table() const NOEXCEPT
{
    for (size_t r = 0; r < screens; ++r)
    {
        std::cout << "Row " << (r + 1) << ":\n";
        for (size_t c = 0; c < screens; ++c)
        {
            if (c <= r)
            {
                const auto output = std::bitset<screen_bits>(masks(r, c));

                std::cout << "  Col " << c << ": " << output << " ("
                    << std::popcount(masks(r, c)) << " bits)\n";
            }
        }
    }
}

static constexpr masks_t masks_
{
    // 1
    0b0000'1111111111111111111111111111,

    // 2
    0b0000'1111111111111100000000000000,
    0b0000'0000000000000011111111111111,

    // 3
    0b0000'1111111111000000000000000000,
    0b0000'0000000000000011111111100000,
    0b0000'0000000000111100000000011111,

    // 4
    0b0000'1111111000000000000000000000,
    0b0000'0000000000000011111110000000,
    0b0000'0000000000111100000000011100,
    0b0000'0000000111000000000001100011,

    // 5
    0b0000'1111110000000000000000000000,
    0b0000'0000000000000011111100000000,
    0b0000'0000000000111100000000011000,
    0b0000'0000000111000000000001100000,
    0b0000'0000001000000000000010000111,

    // 6
    0b0000'1111100000000000000000000000,
    0b0000'0000000000000011111000000000,
    0b0000'0000000000111100000000010000,
    0b0000'0000000111000000000001100000,
    0b0000'0000001000000000000010000110,
    0b0000'0000010000000000000100001001,

    // 7
    0b0000'1111000000000000000000000000,
    0b0000'0000000000000011110000000000,
    0b0000'0000000000111100000000000000,
    0b0000'0000000111000000000001000000,
    0b0000'0000001000000000000010000110,
    0b0000'0000010000000000000100001001,
    0b0000'0000100000000000001000110000,

    // 8
    0b0000'1111000000000000000000000000,
    0b0000'0000000000000011110000000000,
    0b0000'0000000000111100000000000000,
    0b0000'0000000111000000000001000000,
    0b0000'0000001000000000000010000100,
    0b0000'0000010000000000000100001000,
    0b0000'0000100000000000001000100000,
    0b0000'0000000000000000000000010011,

    // 9
    0b0000'1111000000000000000000000000,
    0b0000'0000000000000011100000000000,
    0b0000'0000000000111000000000000000,
    0b0000'0000000111000000000000000000,
    0b0000'0000001000000000000010000100,
    0b0000'0000010000000000000100001000,
    0b0000'0000100000000000001000100000,
    0b0000'0000000000000000000000010011,
    0b0000'0000000000000100010001000000,

    // 10
    0b0000'1110000000000000000000000000,
    0b0000'0000000000000011100000000000,
    0b0000'0000000000111000000000000000,
    0b0000'0000000111000000000000000000,
    0b0000'0000001000000000000010000100,
    0b0000'0000010000000000000100001000,
    0b0000'0000100000000000001000100000,
    0b0000'0000000000000000000000010011,
    0b0000'0000000000000100010000000000,
    0b0000'0001000000000000000001000000,

    // 11
    0b0000'1110000000000000000000000000,
    0b0000'0000000000000011100000000000,
    0b0000'0000000000111000000000000000,
    0b0000'0000000111000000000000000000,
    0b0000'0000001000000000000010000100,
    0b0000'0000010000000000000100001000,
    0b0000'0000100000000000001000000000,
    0b0000'0000000000000000000000010010,
    0b0000'0000000000000100010000000000,
    0b0000'0001000000000000000001000000,
    0b0000'0000000000000000000000100001,

    // 12
    0b0000'1110000000000000000000000000,
    0b0000'0000000000000011100000000000,
    0b0000'0000000000111000000000000000,
    0b0000'0000000111000000000000000000,
    0b0000'0000001000000000000010000000,
    0b0000'0000010000000000000100000000,
    0b0000'0000100000000000001000000000,
    0b0000'0000000000000000000000010010,
    0b0000'0000000000000100010000000000,
    0b0000'0001000000000000000001000000,
    0b0000'0000000000000000000000100001,
    0b0000'0000000000000000000000001100,

    // 13
    0b0000'1110000000000000000000000000,
    0b0000'0000000000000011100000000000,
    0b0000'0000000000110000000000000000,
    0b0000'0000000110000000000000000000,
    0b0000'0000001000000000000010000000,
    0b0000'0000010000000000000100000000,
    0b0000'0000100000000000001000000000,
    0b0000'0000000000000000000000010010,
    0b0000'0000000000000100010000000000,
    0b0000'0001000000000000000001000000,
    0b0000'0000000000000000000000100001,
    0b0000'0000000000000000000000001100,
    0b0000'0000000001001000000000000000,

    // 14
    0b0000'1100000000000000000000000000,
    0b0000'0000000000000011000000000000,
    0b0000'0000000000110000000000000000,
    0b0000'0000000110000000000000000000,
    0b0000'0000001000000000000010000000,
    0b0000'0000010000000000000100000000,
    0b0000'0000100000000000001000000000,
    0b0000'0000000000000000000000010010,
    0b0000'0000000000000100010000000000,
    0b0000'0001000000000000000001000000,
    0b0000'0000000000000000000000100001,
    0b0000'0000000000000000000000001100,
    0b0000'0000000001001000000000000000,
    0b0000'0010000000000000100000000000,

    // 15
    0b0000'1100000000000000000000000000,
    0b0000'0000000000000011000000000000,
    0b0000'0000000000110000000000000000,
    0b0000'0000000110000000000000000000,
    0b0000'0000001000000000000010000000,
    0b0000'0000010000000000000100000000,
    0b0000'0000100000000000001000000000,
    0b0000'0000000000000000000000010010,
    0b0000'0000000000000100010000000000,
    0b0000'0001000000000000000001000000,
    0b0000'0000000000000000000000100001,
    0b0000'0000000000000000000000001100,
    0b0000'0000000001001000000000000000,
    0b0000'0010000000000000000000000000,
    0b0000'0000000000000000100000000000,

    // 16
    // High order screen bit must be zero to avoid saturated sentinel conflict.
    0b0000'0100000000000000000000000000,
    0b0000'0000000000000011000000000000,
    0b0000'0000000000110000000000000000,
    0b0000'0000000110000000000000000000,
    0b0000'0000001000000000000010000000,
    0b0000'0000010000000000000100000000,
    0b0000'0000100000000000001000000000,
    0b0000'0000000000000000000000010010,
    0b0000'0000000000000100010000000000,
    0b0000'0001000000000000000001000000,
    0b0000'0000000000000000000000100001,
    0b0000'0000000000000000000000001100,
    0b0000'0000000001000000000000000000,
    0b0000'0010000000000000000000000000,
    0b0000'0000000000000000100000000000,
    0b0000'0000000000001000000000000000
};

#endif // DISABLED
