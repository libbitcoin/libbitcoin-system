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

BOOST_AUTO_TEST_SUITE(functional_tests)

// mul_wide
// ----------------------------------------------------------------------------

template <typename Word>
constexpr Word mul_wide_hi(Word a, Word b) NOEXCEPT
{
    Word hi{}, lo{};
    f::mul_wide(hi, lo, a, b);
    return hi;
}

template <typename Word>
constexpr Word mul_wide_lo(Word a, Word b) NOEXCEPT
{
    Word hi{}, lo{};
    f::mul_wide(hi, lo, a, b);
    return lo;
}

static_assert(mul_wide_hi(0xff_u8, 0xff_u8) == 0xfe_u8);
static_assert(mul_wide_lo(0xff_u8, 0xff_u8) == 0x01_u8);
static_assert(mul_wide_hi(0xffffffff_u32, 0xffffffff_u32) == 0xfffffffe_u32);
static_assert(mul_wide_lo(0xffffffff_u32, 0xffffffff_u32) == 0x00000001_u32);
static_assert(mul_wide_hi(max_uint64, max_uint64) == 0xfffffffffffffffe_u64);
static_assert(mul_wide_lo(max_uint64, max_uint64) == 0x0000000000000001_u64);
static_assert(mul_wide_hi(0x123456789abcdef0_u64, 0x0fedcba987654321_u64) == 0x0121fa00ad77d742_u64);
static_assert(mul_wide_lo(0x123456789abcdef0_u64, 0x0fedcba987654321_u64) == 0x2236d88fe5618cf0_u64);

BOOST_AUTO_TEST_CASE(functional__mul_wide__runtime__expected)
{
    uint64_t hi{}, lo{};
    f::mul_wide(hi, lo, 0x123456789abcdef0_u64, 0x0fedcba987654321_u64);
    BOOST_CHECK_EQUAL(hi, 0x0121fa00ad77d742_u64);
    BOOST_CHECK_EQUAL(lo, 0x2236d88fe5618cf0_u64);
}

// madd52lo/madd52hi
// ----------------------------------------------------------------------------

constexpr auto max52 = 0x000fffffffffffff_u64;

static_assert(f::madd52lo(7_u64, max52, max52) == 8_u64);
static_assert(f::madd52hi(7_u64, max52, max52) == 0x000ffffffffffffe_u64 + 7_u64);
static_assert(f::madd52lo(1_u64, 0x8000000000000003_u64, 5_u64) == 16_u64);
static_assert(f::madd52hi(1_u64, 0x8000000000000003_u64, 5_u64) == 1_u64);

BOOST_AUTO_TEST_CASE(functional__madd52__runtime__expected)
{
    BOOST_CHECK_EQUAL(f::madd52lo(7_u64, max52, max52), 8_u64);
    BOOST_CHECK_EQUAL(f::madd52hi(7_u64, max52, max52), 0x000ffffffffffffe_u64 + 7_u64);
}

// sub
// ----------------------------------------------------------------------------

static_assert(f::sub(5_u64, 3_u64) == 2_u64);
static_assert(f::sub(0_u64, 1_u64) == max_uint64);
static_assert(f::sub<64>(0x0010000000000000_u64, 1_u64) == 0x000fffffffffffff_u64);
static_assert(f::sub(0_u8, 1_u8) == max_uint8);
static_assert(f::sub(0x42_u8, 0x02_u8) == 0x40_u8);

BOOST_AUTO_TEST_CASE(functional__sub__runtime__expected)
{
    BOOST_CHECK_EQUAL(f::sub(0_u64, 1_u64), max_uint64);
    BOOST_CHECK_EQUAL(f::sub<64>(0x0010000000000000_u64, 1_u64), 0x000fffffffffffff_u64);
}

// andnot/eq/select/any
// ----------------------------------------------------------------------------

static_assert(f::andnot(0xf0f0_u64, 0xffff_u64) == 0x0f0f_u64);
static_assert(f::andnot(0xffff_u64, 0x0f0f_u64) == 0x0000_u64);
static_assert(f::andnot(0x00_u8, 0xff_u8) == 0xff_u8);
static_assert(f::andnot(0xff_u8, 0xff_u8) == 0x00_u8);

static_assert(f::eq(1_u64, 1_u64) == max_uint64);
static_assert(f::eq(1_u64, 2_u64) == 0_u64);
static_assert(f::eq<64>(max_uint64, max_uint64) == max_uint64);
static_assert(f::eq(0x42_u8, 0x42_u8) == 0xff_u8);
static_assert(f::eq(0x42_u8, 0x24_u8) == 0x00_u8);

static_assert(f::select(max_uint64, 10_u64, 20_u64) == 10_u64);
static_assert(f::select(0_u64, 10_u64, 20_u64) == 20_u64);
static_assert(f::select(0x0f_u8, 0xaa_u8, 0x55_u8) == 0x5a_u8);

static_assert(!f::any(0_u64));
static_assert(f::any(1_u64));
static_assert(f::any(0x8000000000000000_u64));
static_assert(!f::any(0_u8));
static_assert(f::any(0x80_u8));

// gather
// ----------------------------------------------------------------------------

constexpr std_array<uint64_t, 4> table{ 100, 101, 102, 103 };
static_assert(f::gather(table.data(), 0_u64) == 100_u64);
static_assert(f::gather(table.data(), 3_u64) == 103_u64);

BOOST_AUTO_TEST_CASE(functional__gather__runtime__expected)
{
    BOOST_CHECK_EQUAL(f::gather(table.data(), 1_u64), 101_u64);
    BOOST_CHECK_EQUAL(f::gather(table.data(), 2_u64), 102_u64);
}

BOOST_AUTO_TEST_SUITE_END()
