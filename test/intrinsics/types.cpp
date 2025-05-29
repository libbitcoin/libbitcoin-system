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

BOOST_AUTO_TEST_SUITE(intrinsics_types_tests)

// extended types
// ----------------------------------------------------------------------------

// Extended integrals are always defined and return the same size_of.
static_assert(size_of<xint128_t>() == 16u);
static_assert(size_of<xint256_t>() == 32u);
static_assert(size_of<xint512_t>() == 64u);

// Extended integrals are neither c++ integrals nor stdlib integers.
static_assert(!std::is_integral_v<xint128_t>);
static_assert(!std::is_integral_v<xint256_t>);
static_assert(!std::is_integral_v<xint512_t>);
static_assert(!std::numeric_limits<xint128_t>::is_integer);
static_assert(!std::numeric_limits<xint256_t>::is_integer);
static_assert(!std::numeric_limits<xint512_t>::is_integer);

// to_extended
// ----------------------------------------------------------------------------

////static_assert(is_same_type<to_extended<uint16_t, 65>, xint512_t>);
static_assert(is_same_type<to_extended<uint8_t, 64>, xint512_t>); // full
static_assert(is_same_type<to_extended<uint8_t, 63>, xint512_t>);
static_assert(is_same_type<to_extended<uint8_t, 33>, xint512_t>);
static_assert(is_same_type<to_extended<uint8_t, 32>, xint256_t>); // full
static_assert(is_same_type<to_extended<uint8_t, 31>, xint256_t>);
static_assert(is_same_type<to_extended<uint8_t, 17>, xint256_t>);
static_assert(is_same_type<to_extended<uint8_t, 16>, xint128_t>); // full
static_assert(is_same_type<to_extended<uint8_t, 15>, xint128_t>);
static_assert(is_same_type<to_extended<uint8_t,  9>, xint128_t>);
static_assert(is_same_type<to_extended<uint8_t,  8>, uint64_t>);  // full
static_assert(is_same_type<to_extended<uint8_t,  7>, uint64_t>);
static_assert(is_same_type<to_extended<uint8_t,  5>, uint64_t>);
static_assert(is_same_type<to_extended<uint8_t,  4>, uint32_t>);  // full
static_assert(is_same_type<to_extended<uint8_t,  3>, uint32_t>);
static_assert(is_same_type<to_extended<uint8_t,  2>, uint16_t>);  // full
static_assert(is_same_type<to_extended<uint8_t,  1>, uint8_t>);   // full
////static_assert(is_same_type<to_extended<uint8_t,  0>, uint16_t>);

////static_assert(is_same_type<to_extended<uint16_t, 33>, xint512_t>);
static_assert(is_same_type<to_extended<uint16_t, 32>, xint512_t>); // full
static_assert(is_same_type<to_extended<uint16_t, 31>, xint512_t>);
static_assert(is_same_type<to_extended<uint16_t, 17>, xint512_t>);
static_assert(is_same_type<to_extended<uint16_t, 16>, xint256_t>); // full
static_assert(is_same_type<to_extended<uint16_t, 15>, xint256_t>);
static_assert(is_same_type<to_extended<uint16_t,  9>, xint256_t>);
static_assert(is_same_type<to_extended<uint16_t,  8>, xint128_t>); // full
static_assert(is_same_type<to_extended<uint16_t,  7>, xint128_t>);
static_assert(is_same_type<to_extended<uint16_t,  5>, xint128_t>);
static_assert(is_same_type<to_extended<uint16_t,  4>, uint64_t>);  // full
static_assert(is_same_type<to_extended<uint16_t,  3>, uint64_t>);
static_assert(is_same_type<to_extended<uint16_t,  2>, uint32_t>);  // full
static_assert(is_same_type<to_extended<uint16_t,  1>, uint16_t>);  // full
////static_assert(is_same_type<to_extended<uint16_t,  0>, uint16_t>);

////static_assert(is_same_type<to_extended<uint32_t, 17>, xint512_t>);
static_assert(is_same_type<to_extended<uint32_t, 16>, xint512_t>); // full
static_assert(is_same_type<to_extended<uint32_t, 15>, xint512_t>);
static_assert(is_same_type<to_extended<uint32_t,  9>, xint512_t>);
static_assert(is_same_type<to_extended<uint32_t,  8>, xint256_t>); // full
static_assert(is_same_type<to_extended<uint32_t,  7>, xint256_t>);
static_assert(is_same_type<to_extended<uint32_t,  5>, xint256_t>);
static_assert(is_same_type<to_extended<uint32_t,  4>, xint128_t>); // full
static_assert(is_same_type<to_extended<uint32_t,  3>, xint128_t>);
static_assert(is_same_type<to_extended<uint32_t,  2>, uint64_t>);  // full
static_assert(is_same_type<to_extended<uint32_t,  1>, uint32_t>);  // full
////static_assert(is_same_type<to_extended<uint32_t,  0>, uint32_t>);

////static_assert(is_same_type<to_extended<uint64_t, 9>, xint512_t>);
static_assert(is_same_type<to_extended<uint64_t, 8>, xint512_t>); // full
static_assert(is_same_type<to_extended<uint64_t, 7>, xint512_t>);
static_assert(is_same_type<to_extended<uint64_t, 5>, xint512_t>);
static_assert(is_same_type<to_extended<uint64_t, 4>, xint256_t>); // full
static_assert(is_same_type<to_extended<uint64_t, 3>, xint256_t>);
static_assert(is_same_type<to_extended<uint64_t, 2>, xint128_t>); // full
static_assert(is_same_type<to_extended<uint64_t, 1>, uint64_t>);  // full
////static_assert(is_same_type<to_extended<uint64_t, 0>, uint64_t>);

// is_extended
// ----------------------------------------------------------------------------

// helper
template <typename>
constexpr bool is_defined = true;

// is_extended is true even with mock type.
static_assert(!is_extended<uint32_t>);
static_assert(is_extended<xint128_t>);
static_assert(is_extended<xint256_t>);
static_assert(is_extended<xint512_t>);
static_assert(is_defined<if_extended<xint128_t>>);
static_assert(is_defined<if_extended<xint256_t>>);
static_assert(is_defined<if_extended<xint512_t>>);

// have
// ----------------------------------------------------------------------------

static_assert(have<xint512_t> == have_512);
static_assert(have<xint256_t> == have_256);
static_assert(have<xint128_t> == have_128);

// have_lanes
// ----------------------------------------------------------------------------

static_assert(have_lanes<uint64_t, 8> == have_512);
static_assert(have_lanes<uint32_t, 16> == have_512);
static_assert(have_lanes<uint16_t, 32> == have_512);
static_assert(!have_lanes<uint64_t, 7>);
static_assert(!have_lanes<uint32_t, 15>);
static_assert(!have_lanes<uint16_t, 31>);
static_assert(!have_lanes<uint8_t, 63>);

static_assert(have_lanes<uint64_t, 4> == have_256);
static_assert(have_lanes<uint32_t, 8> == have_256);
static_assert(have_lanes<uint16_t, 16> == have_256);
static_assert(have_lanes<uint8_t, 32> == have_256);
static_assert(!have_lanes<uint64_t, 3>);
static_assert(!have_lanes<uint32_t, 7>);
static_assert(!have_lanes<uint16_t, 15>);
static_assert(!have_lanes<uint8_t, 31>);

static_assert(have_lanes<uint64_t, 2> == have_128);
static_assert(have_lanes<uint32_t, 4> == have_128);
static_assert(have_lanes<uint16_t, 8> == have_128);
static_assert(have_lanes<uint8_t, 16> == have_128);
static_assert(!have_lanes<uint64_t, 1>);
static_assert(!have_lanes<uint32_t, 3>);
static_assert(!have_lanes<uint16_t, 7>);
static_assert(!have_lanes<uint8_t, 15>);

// capacity [for extended integers]
// ----------------------------------------------------------------------------

static_assert(capacity<xint128_t, uint8_t>    == 16); // required to fill
static_assert(capacity<xint128_t, uint8_t, 1> == 16); // full
static_assert(capacity<xint128_t, uint8_t, 2> ==  8); // full
static_assert(capacity<xint128_t, uint8_t, 3> ==  5);
static_assert(capacity<xint128_t, uint8_t, 4> ==  4); // full
static_assert(capacity<xint128_t, uint8_t, 5> ==  3);
static_assert(capacity<xint128_t, uint8_t, 6> ==  2);
static_assert(capacity<xint128_t, uint8_t, 7> ==  2);
static_assert(capacity<xint128_t, uint8_t, 8> ==  2); // full
static_assert(capacity<xint128_t, uint8_t, 9> ==  1);
static_assert(capacity<xint128_t, uint8_t, 10> == 1);
static_assert(capacity<xint128_t, uint8_t, 11> == 1);
static_assert(capacity<xint128_t, uint8_t, 12> == 1);
static_assert(capacity<xint128_t, uint8_t, 13> == 1);
static_assert(capacity<xint128_t, uint8_t, 14> == 1);
static_assert(capacity<xint128_t, uint8_t, 15> == 1);
static_assert(capacity<xint128_t, uint8_t, 16> == 1); // full
static_assert(capacity<xint128_t, uint8_t, 17> == 0);
////static_assert(capacity<xint128_t, uint8_t, 0> == 0);

static_assert(capacity<xint128_t, uint16_t>    == 8); // required to fill
static_assert(capacity<xint128_t, uint16_t, 1> == 8);
static_assert(capacity<xint128_t, uint16_t, 2> == 4); // full
static_assert(capacity<xint128_t, uint16_t, 3> == 2);
static_assert(capacity<xint128_t, uint16_t, 4> == 2); // full
static_assert(capacity<xint128_t, uint16_t, 5> == 1);
static_assert(capacity<xint128_t, uint16_t, 6> == 1);
static_assert(capacity<xint128_t, uint16_t, 7> == 1);
static_assert(capacity<xint128_t, uint16_t, 8> == 1); // full
static_assert(capacity<xint128_t, uint16_t, 9> == 0);
////static_assert(capacity<xint128_t, uint16_t, 0> == 0);

static_assert(capacity<xint128_t, uint32_t>    == 4); // required to fill (lanes)
static_assert(capacity<xint128_t, uint32_t, 1> == 4);
static_assert(capacity<xint128_t, uint32_t, 2> == 2); // full
static_assert(capacity<xint128_t, uint32_t, 3> == 1);
static_assert(capacity<xint128_t, uint32_t, 4> == 1); // full
static_assert(capacity<xint128_t, uint32_t, 5> == 0);
////static_assert(capacity<xint128_t, uint32_t, 0> == 0);

static_assert(capacity<xint128_t, uint64_t>    == 2); // required to fill (lanes)
static_assert(capacity<xint128_t, uint64_t, 1> == 2);
static_assert(capacity<xint128_t, uint64_t, 2> == 1); // full
static_assert(capacity<xint128_t, uint64_t, 3> == 0);
////static_assert(capacity<xint128_t, uint64_t, 0> == 0);

static_assert(capacity<xint256_t, uint8_t>        == 32); // required to fill
static_assert(capacity<xint256_t, uint8_t, 1 * 2> == 16); // full
static_assert(capacity<xint256_t, uint8_t, 2 * 2> ==  8); // full
static_assert(capacity<xint256_t, uint8_t, 3 * 2> ==  5);
static_assert(capacity<xint256_t, uint8_t, 4 * 2> ==  4); // full
static_assert(capacity<xint256_t, uint8_t, 5 * 2> ==  3);
static_assert(capacity<xint256_t, uint8_t, 6 * 2> ==  2);
static_assert(capacity<xint256_t, uint8_t, 7 * 2> ==  2);
static_assert(capacity<xint256_t, uint8_t, 8 * 2> ==  2); // full
static_assert(capacity<xint256_t, uint8_t, 9 * 2> ==  1);
static_assert(capacity<xint256_t, uint8_t, 10 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 11 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 12 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 13 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 14 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 15 * 2> == 1);
static_assert(capacity<xint256_t, uint8_t, 16 * 2> == 1); // full
static_assert(capacity<xint256_t, uint8_t, 17 * 2> == 0);
////static_assert(capacity<xint256_t, uint8_t, 0> == 0);

static_assert(capacity<xint256_t, uint16_t>        == 16); // required to fill
static_assert(capacity<xint256_t, uint16_t, 1 * 2> == 8);
static_assert(capacity<xint256_t, uint16_t, 2 * 2> == 4); // full
static_assert(capacity<xint256_t, uint16_t, 3 * 2> == 2);
static_assert(capacity<xint256_t, uint16_t, 4 * 2> == 2); // full
static_assert(capacity<xint256_t, uint16_t, 5 * 2> == 1);
static_assert(capacity<xint256_t, uint16_t, 6 * 2> == 1);
static_assert(capacity<xint256_t, uint16_t, 7 * 2> == 1);
static_assert(capacity<xint256_t, uint16_t, 8 * 2> == 1); // full
static_assert(capacity<xint256_t, uint16_t, 9 * 2> == 0);
////static_assert(capacity<xint256_t, uint16_t, 0> == 0);

static_assert(capacity<xint256_t, uint32_t>        == 8); // required to fill (lanes)
static_assert(capacity<xint256_t, uint32_t, 1 * 2> == 4);
static_assert(capacity<xint256_t, uint32_t, 2 * 2> == 2); // full
static_assert(capacity<xint256_t, uint32_t, 3 * 2> == 1);
static_assert(capacity<xint256_t, uint32_t, 4 * 2> == 1); // full
static_assert(capacity<xint256_t, uint32_t, 5 * 2> == 0);
////static_assert(capacity<xint256_t, uint32_t, 0> == 0);

static_assert(capacity<xint256_t, uint64_t>        == 4); // required to fill (lanes)
static_assert(capacity<xint256_t, uint64_t, 1 * 2> == 2);
static_assert(capacity<xint256_t, uint64_t, 2 * 2> == 1); // full
static_assert(capacity<xint256_t, uint64_t, 3 * 2> == 0);
////static_assert(capacity<xint256_t, uint64_t, 0> == 0);

static_assert(capacity<xint512_t, uint8_t>        == 64); // required to fill
static_assert(capacity<xint512_t, uint8_t, 1 * 4> == 16); // full
static_assert(capacity<xint512_t, uint8_t, 2 * 4> ==  8); // full
static_assert(capacity<xint512_t, uint8_t, 3 * 4> ==  5);
static_assert(capacity<xint512_t, uint8_t, 4 * 4> ==  4); // full
static_assert(capacity<xint512_t, uint8_t, 5 * 4> ==  3);
static_assert(capacity<xint512_t, uint8_t, 6 * 4> ==  2);
static_assert(capacity<xint512_t, uint8_t, 7 * 4> ==  2);
static_assert(capacity<xint512_t, uint8_t, 8 * 4> ==  2); // full
static_assert(capacity<xint512_t, uint8_t, 9 * 4> ==  1);
static_assert(capacity<xint512_t, uint8_t, 10 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 11 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 12 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 13 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 14 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 15 * 4> == 1);
static_assert(capacity<xint512_t, uint8_t, 16 * 4> == 1); // full
static_assert(capacity<xint512_t, uint8_t, 17 * 4> == 0);
////static_assert(capacity<xint512_t, uint8_t, 0> == 0);

static_assert(capacity<xint512_t, uint16_t>        == 32); // required to fill
static_assert(capacity<xint512_t, uint16_t, 1 * 4> == 8);
static_assert(capacity<xint512_t, uint16_t, 2 * 4> == 4); // full
static_assert(capacity<xint512_t, uint16_t, 3 * 4> == 2);
static_assert(capacity<xint512_t, uint16_t, 4 * 4> == 2); // full
static_assert(capacity<xint512_t, uint16_t, 5 * 4> == 1);
static_assert(capacity<xint512_t, uint16_t, 6 * 4> == 1);
static_assert(capacity<xint512_t, uint16_t, 7 * 4> == 1);
static_assert(capacity<xint512_t, uint16_t, 8 * 4> == 1); // full
static_assert(capacity<xint512_t, uint16_t, 9 * 4> == 0);
////static_assert(capacity<xint512_t, uint16_t, 0> == 0);

static_assert(capacity<xint512_t, uint32_t>        == 16); // required to fill (lanes)
static_assert(capacity<xint512_t, uint32_t, 1 * 4> == 4);
static_assert(capacity<xint512_t, uint32_t, 2 * 4> == 2); // full
static_assert(capacity<xint512_t, uint32_t, 3 * 4> == 1);
static_assert(capacity<xint512_t, uint32_t, 4 * 4> == 1); // full
static_assert(capacity<xint512_t, uint32_t, 5 * 4> == 0);
////static_assert(capacity<xint512_t, uint32_t, 0> == 0);

static_assert(capacity<xint512_t, uint64_t>        == 8); // required to fill (lanes)
static_assert(capacity<xint512_t, uint64_t, 1 * 4> == 2);
static_assert(capacity<xint512_t, uint64_t, 2 * 4> == 1); // full
static_assert(capacity<xint512_t, uint64_t, 3 * 4> == 0);
////static_assert(capacity<xint512_t, uint64_t, 0> == 0);

BOOST_AUTO_TEST_SUITE_END()
