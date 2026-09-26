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

BOOST_AUTO_TEST_SUITE(intrinsics_multiply_tests)

// mul_wide
// ----------------------------------------------------------------------------

template <typename Unsigned>
constexpr Unsigned mul_wide_hi(Unsigned left, Unsigned right) NOEXCEPT
{
    Unsigned hi{}, lo{};
    mul_wide(hi, lo, left, right);
    return hi;
}

template <typename Unsigned>
constexpr Unsigned mul_wide_lo(Unsigned left, Unsigned right) NOEXCEPT
{
    Unsigned hi{}, lo{};
    mul_wide(hi, lo, left, right);
    return lo;
}

static_assert(mul_wide_hi(0_u8, 0_u8) == 0_u8);
static_assert(mul_wide_lo(0_u8, 0_u8) == 0_u8);
static_assert(mul_wide_hi(0xff_u8, 0xff_u8) == 0xfe_u8);
static_assert(mul_wide_lo(0xff_u8, 0xff_u8) == 0x01_u8);
static_assert(mul_wide_hi(0xffff_u16, 0xffff_u16) == 0xfffe_u16);
static_assert(mul_wide_lo(0xffff_u16, 0xffff_u16) == 0x0001_u16);
static_assert(mul_wide_hi(0x1234_u16, 0x5678_u16) == 0x0626_u16);
static_assert(mul_wide_lo(0x1234_u16, 0x5678_u16) == 0x0060_u16);
static_assert(mul_wide_hi(0xffffffff_u32, 0xffffffff_u32) == 0xfffffffe_u32);
static_assert(mul_wide_lo(0xffffffff_u32, 0xffffffff_u32) == 0x00000001_u32);
static_assert(mul_wide_hi(max_uint64, max_uint64) == 0xfffffffffffffffe_u64);
static_assert(mul_wide_lo(max_uint64, max_uint64) == 0x0000000000000001_u64);
static_assert(mul_wide_hi(max_uint64, 0_u64) == 0_u64);
static_assert(mul_wide_lo(max_uint64, 0_u64) == 0_u64);
static_assert(mul_wide_hi(max_uint64, 1_u64) == 0_u64);
static_assert(mul_wide_lo(max_uint64, 1_u64) == max_uint64);
static_assert(mul_wide_hi(0x0000000100000000_u64, 0x0000000100000000_u64) == 1_u64);
static_assert(mul_wide_lo(0x0000000100000000_u64, 0x0000000100000000_u64) == 0_u64);
static_assert(mul_wide_hi(0x123456789abcdef0_u64, 0x0fedcba987654321_u64) == 0x0121fa00ad77d742_u64);
static_assert(mul_wide_lo(0x123456789abcdef0_u64, 0x0fedcba987654321_u64) == 0x2236d88fe5618cf0_u64);

static bool mul_wide64_matches_nominal(uint64_t left, uint64_t right) NOEXCEPT
{
    uint64_t hi{}, lo{}, nominal_hi{}, nominal_lo{};
    mul_wide64(hi, lo, left, right);
    mul_wide64_nominal(nominal_hi, nominal_lo, left, right);
    return hi == nominal_hi && lo == nominal_lo;
}

BOOST_AUTO_TEST_CASE(intrinsics__mul_wide64__always__matches_nominal)
{
    BOOST_CHECK(mul_wide64_matches_nominal(0_u64, 0_u64));
    BOOST_CHECK(mul_wide64_matches_nominal(0_u64, max_uint64));
    BOOST_CHECK(mul_wide64_matches_nominal(1_u64, max_uint64));
    BOOST_CHECK(mul_wide64_matches_nominal(2_u64, max_uint64));
    BOOST_CHECK(mul_wide64_matches_nominal(max_uint64, max_uint64));
    BOOST_CHECK(mul_wide64_matches_nominal(0xffffffff_u64, 0xffffffff_u64));
    BOOST_CHECK(mul_wide64_matches_nominal(0xffffffff_u64, 0x0000000100000000_u64));
    BOOST_CHECK(mul_wide64_matches_nominal(0x0000000100000000_u64, 0x0000000100000000_u64));
    BOOST_CHECK(mul_wide64_matches_nominal(0x0000000100000000_u64, max_uint64));
    BOOST_CHECK(mul_wide64_matches_nominal(0x123456789abcdef0_u64, 0x0fedcba987654321_u64));
    BOOST_CHECK(mul_wide64_matches_nominal(0x0fedcba987654321_u64, 0x123456789abcdef0_u64));
    BOOST_CHECK(mul_wide64_matches_nominal(0x123456789abcdef0_u64, max_uint64));
    BOOST_CHECK(mul_wide64_matches_nominal(0x8000000000000000_u64, 0x8000000000000000_u64));
    BOOST_CHECK(mul_wide64_matches_nominal(0x8000000000000000_u64, 2_u64));
}

BOOST_AUTO_TEST_CASE(intrinsics__mul_wide__runtime__expected)
{
    uint64_t hi64{}, lo64{};
    mul_wide(hi64, lo64, max_uint64, max_uint64);
    BOOST_CHECK_EQUAL(hi64, 0xfffffffffffffffe_u64);
    BOOST_CHECK_EQUAL(lo64, 0x0000000000000001_u64);

    mul_wide(hi64, lo64, 0x123456789abcdef0_u64, 0x0fedcba987654321_u64);
    BOOST_CHECK_EQUAL(hi64, 0x0121fa00ad77d742_u64);
    BOOST_CHECK_EQUAL(lo64, 0x2236d88fe5618cf0_u64);

    uint32_t hi32{}, lo32{};
    mul_wide(hi32, lo32, 0xffffffff_u32, 0xffffffff_u32);
    BOOST_CHECK_EQUAL(hi32, 0xfffffffe_u32);
    BOOST_CHECK_EQUAL(lo32, 0x00000001_u32);

    uint16_t hi16{}, lo16{};
    mul_wide(hi16, lo16, 0x1234_u16, 0x5678_u16);
    BOOST_CHECK_EQUAL(hi16, 0x0626_u16);
    BOOST_CHECK_EQUAL(lo16, 0x0060_u16);

    uint8_t hi8{}, lo8{};
    mul_wide(hi8, lo8, 0xff_u8, 0xff_u8);
    BOOST_CHECK_EQUAL(hi8, 0xfe_u8);
    BOOST_CHECK_EQUAL(lo8, 0x01_u8);
}

// madd52lo/madd52hi
// ----------------------------------------------------------------------------

constexpr auto max52 = 0x000fffffffffffff_u64;

// (2^52 - 1)^2 = 2^104 - 2^53 + 1, low 52 bits are 1, high 52 bits are 2^52 - 2.
static_assert(madd52lo(0_u64, max52, max52) == 1_u64);
static_assert(madd52hi(0_u64, max52, max52) == 0x000ffffffffffffe_u64);
static_assert(madd52lo(7_u64, max52, max52) == 8_u64);
static_assert(madd52hi(7_u64, max52, max52) == 0x000ffffffffffffe_u64 + 7_u64);

// Bits above 52 of the multiplicands are ignored.
static_assert(madd52lo(1_u64, 0x8000000000000003_u64, 5_u64) == 16_u64);
static_assert(madd52hi(1_u64, 0x8000000000000003_u64, 5_u64) == 1_u64);
static_assert(madd52lo(0_u64, 0xfff0000000000000_u64, max52) == 0_u64);
static_assert(madd52hi(0_u64, 0xfff0000000000000_u64, max52) == 0_u64);

// Accumulation wraps at 64 bits.
static_assert(madd52lo(max_uint64, 1_u64, 2_u64) == 1_u64);
static_assert(madd52hi(max_uint64, max52, 2_u64) == 0_u64);

// Zero multiplicand passes the accumulator through.
static_assert(madd52lo(42_u64, 0_u64, max52) == 42_u64);
static_assert(madd52hi(42_u64, 0_u64, max52) == 42_u64);

BOOST_AUTO_TEST_CASE(intrinsics__madd52__runtime__expected)
{
    BOOST_CHECK_EQUAL(madd52lo(7_u64, max52, max52), 8_u64);
    BOOST_CHECK_EQUAL(madd52hi(7_u64, max52, max52), 0x000ffffffffffffe_u64 + 7_u64);
    BOOST_CHECK_EQUAL(madd52lo(1_u64, 0x8000000000000003_u64, 5_u64), 16_u64);
    BOOST_CHECK_EQUAL(madd52hi(1_u64, 0x8000000000000003_u64, 5_u64), 1_u64);
    BOOST_CHECK_EQUAL(madd52lo(max_uint64, 1_u64, 2_u64), 1_u64);
    BOOST_CHECK_EQUAL(madd52hi(max_uint64, max52, 2_u64), 0_u64);
}

BOOST_AUTO_TEST_SUITE_END()
