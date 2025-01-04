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

BOOST_AUTO_TEST_SUITE(intrinsics_tests)

#if defined (DISABLED)

// Types work as integrals.
static_assert(is_same_type<xint128_t, __m128i>);
static_assert(is_same_type<xint256_t, __m256i>);
static_assert(is_same_type<xint512_t, __m512i>);

// Extended integrals are neither c++ integrals nor stdlib integers.
static_assert(!std::is_integral_v<xint128_t>);
static_assert(!std::is_integral_v<xint256_t>);
static_assert(!std::is_integral_v<xint512_t>);
static_assert(!std::numeric_limits<xint128_t>::is_integer);
static_assert(!std::numeric_limits<xint256_t>::is_integer);
static_assert(!std::numeric_limits<xint512_t>::is_integer);

// sizeof() works as expected.
static_assert(to_bits(sizeof(xint128_t)) == 128u);
static_assert(to_bits(sizeof(xint256_t)) == 256u);
static_assert(to_bits(sizeof(xint512_t)) == 512u);

#endif

BOOST_AUTO_TEST_SUITE_END()
