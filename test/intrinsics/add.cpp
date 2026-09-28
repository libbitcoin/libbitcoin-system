/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

BOOST_AUTO_TEST_SUITE(intrinsics_add_tests)

// add_carry
// ----------------------------------------------------------------------------

template <typename Unsigned>
constexpr Unsigned add_carry_sum(Unsigned left, Unsigned right, bool carry) NOEXCEPT
{
    Unsigned sum{};
    add_carry(sum, left, right, carry);
    return sum;
}

template <typename Unsigned>
constexpr bool add_carry_out(Unsigned left, Unsigned right, bool carry) NOEXCEPT
{
    Unsigned sum{};
    return add_carry(sum, left, right, carry);
}

static_assert(add_carry_sum(0_u64, 0_u64, false) == 0_u64);
static_assert(!add_carry_out(0_u64, 0_u64, false));
static_assert(add_carry_sum(0_u64, 0_u64, true) == 1_u64);
static_assert(!add_carry_out(0_u64, 0_u64, true));
static_assert(add_carry_sum(max_uint64, 1_u64, false) == 0_u64);
static_assert(add_carry_out(max_uint64, 1_u64, false));
static_assert(add_carry_sum(max_uint64, 0_u64, true) == 0_u64);
static_assert(add_carry_out(max_uint64, 0_u64, true));
static_assert(add_carry_sum(max_uint64, max_uint64, true) == max_uint64);
static_assert(add_carry_out(max_uint64, max_uint64, true));
static_assert(add_carry_sum(max_uint64, max_uint64, false) == sub1(max_uint64));
static_assert(add_carry_out(max_uint64, max_uint64, false));
static_assert(add_carry_sum(0x8000000000000000_u64, 0x7fffffffffffffff_u64, false) == max_uint64);
static_assert(!add_carry_out(0x8000000000000000_u64, 0x7fffffffffffffff_u64, false));
static_assert(add_carry_sum(0x8000000000000000_u64, 0x7fffffffffffffff_u64, true) == 0_u64);
static_assert(add_carry_out(0x8000000000000000_u64, 0x7fffffffffffffff_u64, true));
static_assert(add_carry_sum(0xff_u8, 0x01_u8, false) == 0x00_u8);
static_assert(add_carry_out(0xff_u8, 0x01_u8, false));
static_assert(add_carry_sum(0xfe_u8, 0x00_u8, true) == 0xff_u8);
static_assert(!add_carry_out(0xfe_u8, 0x00_u8, true));
static_assert(add_carry_sum(0xffffffff_u32, 0xffffffff_u32, true) == 0xffffffff_u32);
static_assert(add_carry_out(0xffffffff_u32, 0xffffffff_u32, true));

static bool add_carry64_matches_nominal(uint64_t left, uint64_t right, bool carry) NOEXCEPT
{
    uint64_t sum{}, nominal{};
    const auto out = add_carry64(sum, left, right, carry);
    const auto nominal_out = add_carry64_nominal(nominal, left, right, carry);
    return sum == nominal && out == nominal_out;
}

BOOST_AUTO_TEST_CASE(intrinsics__add_carry64__always__matches_nominal)
{
    BOOST_CHECK(add_carry64_matches_nominal(0_u64, 0_u64, false));
    BOOST_CHECK(add_carry64_matches_nominal(0_u64, 0_u64, true));
    BOOST_CHECK(add_carry64_matches_nominal(max_uint64, 1_u64, false));
    BOOST_CHECK(add_carry64_matches_nominal(max_uint64, 0_u64, true));
    BOOST_CHECK(add_carry64_matches_nominal(max_uint64, max_uint64, false));
    BOOST_CHECK(add_carry64_matches_nominal(max_uint64, max_uint64, true));
    BOOST_CHECK(add_carry64_matches_nominal(0x8000000000000000_u64, 0x7fffffffffffffff_u64, false));
    BOOST_CHECK(add_carry64_matches_nominal(0x8000000000000000_u64, 0x7fffffffffffffff_u64, true));
    BOOST_CHECK(add_carry64_matches_nominal(0x123456789abcdef0_u64, 0xfedcba9876543210_u64, true));
}

BOOST_AUTO_TEST_CASE(intrinsics__add_carry__runtime__expected)
{
    uint64_t sum{};
    BOOST_CHECK(add_carry(sum, max_uint64, max_uint64, true));
    BOOST_CHECK_EQUAL(sum, max_uint64);
    BOOST_CHECK(!add_carry(sum, 1_u64, 2_u64, true));
    BOOST_CHECK_EQUAL(sum, 4_u64);
}

BOOST_AUTO_TEST_SUITE_END()
