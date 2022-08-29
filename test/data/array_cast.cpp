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

BOOST_AUTO_TEST_SUITE(array_cast_tests)

template <typename>
constexpr bool is_defined = true;

// sequence
static_assert(sequence<uint32_t, 42>[41] == 41_u32);
static_assert(sequence<uint32_t, 42>[0] == 0_u32);
static_assert(std::is_same_v<decltype(sequence<uint32_t, 42>), const std_array<uint32_t, 42>>);

// to_sequence
static_assert(to_sequence<uint32_t, 42>()[0] == 0_u32);
static_assert(to_sequence<uint32_t, 42>()[41] == 41_u32);
static_assert(std::is_same_v<decltype(to_sequence<uint32_t, 42>()), std_array<uint32_t, 42>>);

// is_proportional<Left, Right>(left_count)
// ----------------------------------------------------------------------------
// (left_count * Left) == (n * Right)

static_assert( is_proportional<uint8_t,  uint8_t >(1u)); // n = 1
static_assert( is_proportional<uint16_t, uint8_t >(1u)); // n = 2
static_assert(!is_proportional<uint8_t,  uint16_t>(1u)); // n = 0.5
static_assert( is_proportional<uint8_t,  uint16_t>(2u)); // n = 1
static_assert(!is_proportional<uint8_t,  uint16_t>(3u)); // n = 1.5
static_assert( is_proportional<uint8_t,  uint64_t>(8u)); // n = 1
static_assert( is_proportional<uint16_t, uint64_t>(8u)); // n = 2
static_assert( is_proportional<uint32_t, uint64_t>(8u)); // n = 4
static_assert( is_proportional<uint64_t, uint64_t>(8u)); // n = 8

static_assert(!is_proportional<uint8_t, uint64_t>(max_size_t)); // overflow

// if_proportional<size_t, Integral, Integral>
// ----------------------------------------------------------------------------

static_assert( is_defined<if_proportional<1, uint8_t,  uint8_t >>); // n = 1
static_assert( is_defined<if_proportional<1, uint16_t, uint8_t >>); // n = 2
////static_assert(!is_defined<if_proportional<1, uint8_t,  uint16_t>>); // n = 0.5
static_assert( is_defined<if_proportional<2, uint8_t,  uint16_t>>); // n = 1
////static_assert(!is_defined<if_proportional<3, uint8_t,  uint16_t>>); // n = 1.5
static_assert( is_defined<if_proportional<8, uint8_t,  uint64_t>>); // n = 1
static_assert( is_defined<if_proportional<8, uint16_t, uint64_t>>); // n = 2
static_assert( is_defined<if_proportional<8, uint32_t, uint64_t>>); // n = 4
static_assert( is_defined<if_proportional<8, uint64_t, uint64_t>>); // n = 8

// proportion<size_t, Integral, Integral>
// ----------------------------------------------------------------------------
// proportion is guarded by if_proportional type constraint.

static_assert( proportion<1, uint8_t,  uint8_t > == 1u); // n = 1
static_assert( proportion<1, uint16_t, uint8_t > == 2u); // n = 2
////static_assert(!proportion<1, uint8_t,  uint16_t> == 0u); // n = 0.5 (0)
////static_assert(!proportion<1, uint8_t,  uint16_t> == 1u); // n = 0.5 (1)
static_assert( proportion<2, uint8_t,  uint16_t> == 1u); // n = 1
////static_assert(!proportion<3, uint8_t,  uint16_t> == 1u); // n = 1.5 (1)
////static_assert(!proportion<3, uint8_t,  uint16_t> == 2u); // n = 1.5 (2)
static_assert( proportion<8, uint8_t,  uint64_t> == 1u); // n = 1
static_assert( proportion<8, uint16_t, uint64_t> == 2u); // n = 2
static_assert( proportion<8, uint32_t, uint64_t> == 4u); // n = 4
static_assert( proportion<8, uint64_t, uint64_t> == 8u); // n = 8

static_assert(proportion<0, std_array<uint8_t,   0>, std_array<uint8_t,   1>> == 0);
static_assert(proportion<1, std_array<uint8_t,   1>, std_array<uint8_t,   1>> == 1);
static_assert(proportion<0, std_array<uint8_t, 128>, std_array<uint8_t,  64>> == 0);
static_assert(proportion<1, std_array<uint8_t, 128>, std_array<uint8_t,  64>> == 2);
static_assert(proportion<2, std_array<uint8_t, 128>, std_array<uint8_t,  64>> == 4);
static_assert(proportion<3, std_array<uint8_t, 128>, std_array<uint8_t,  64>> == 6);
static_assert(proportion<4, std_array<uint8_t,  64>, std_array<uint8_t, 128>> == 2);
static_assert(proportion<6, std_array<uint8_t,  64>, std_array<uint8_t, 128>> == 3);
static_assert(proportion<8, std_array<uint8_t,  64>, std_array<uint8_t, 128>> == 4);

// is_portional<Integral, Integral>(size_t, size_t)
// ----------------------------------------------------------------------------
// (left_count * Left) <= (right_count * Right)

static_assert(is_portional<uint8_t,  uint8_t>(0, 0));
static_assert(is_portional<uint8_t,  uint8_t>(1, 1));
static_assert(is_portional<uint8_t,  uint8_t>(1, 2));
static_assert(is_portional<uint8_t,  uint8_t>(2, 4));
static_assert(is_portional<uint8_t, uint16_t>(0, 0));
static_assert(is_portional<uint8_t, uint16_t>(1, 1));
static_assert(is_portional<uint8_t, uint16_t>(2, 1));
static_assert(is_portional<uint8_t, uint16_t>(2, 2));
static_assert(is_portional<uint8_t, uint16_t>(2, 3));
static_assert(is_portional<uint8_t, uint16_t>(3, 3));

// equal proportions (portional)
static_assert(is_portional<uint8_t,  uint8_t >(1u, 1u));
static_assert(is_portional<uint16_t, uint8_t >(1u, 2u));
static_assert(is_portional<uint8_t,  uint16_t>(2u, 1u));
static_assert(is_portional<uint8_t,  uint64_t>(8u, 1u));
static_assert(is_portional<uint16_t, uint64_t>(8u, 2u));
static_assert(is_portional<uint32_t, uint64_t>(8u, 4u));
static_assert(is_portional<uint64_t, uint64_t>(8u, 8u));

// lesser proportions (portional)
static_assert(is_portional<uint8_t,  uint8_t >(1u, 2u));
static_assert(is_portional<uint16_t, uint8_t >(1u, 3u));
static_assert(is_portional<uint8_t,  uint16_t>(1u, 1u));
static_assert(is_portional<uint8_t,  uint16_t>(2u, 2u));
static_assert(is_portional<uint8_t,  uint16_t>(3u, 2u));
static_assert(is_portional<uint8_t,  uint64_t>(8u, 2u));
static_assert(is_portional<uint16_t, uint64_t>(8u, 3u));
static_assert(is_portional<uint32_t, uint64_t>(8u, 5u));
static_assert(is_portional<uint64_t, uint64_t>(8u, 9u));

// greater proportions (not portional)
static_assert(!is_portional<uint8_t,  uint8_t >(2u, 1u));
static_assert(!is_portional<uint16_t, uint8_t >(2u, 2u));
static_assert(!is_portional<uint8_t,  uint16_t>(2u, 0u));
static_assert(!is_portional<uint8_t,  uint16_t>(3u, 1u));
static_assert(!is_portional<uint8_t,  uint16_t>(4u, 1u));
static_assert(!is_portional<uint8_t,  uint64_t>(9u, 1u));
static_assert(!is_portional<uint16_t, uint64_t>(9u, 2u));
static_assert(!is_portional<uint32_t, uint64_t>(9u, 4u));
static_assert(!is_portional<uint64_t, uint64_t>(9u, 8u));

// arrays
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint8_t,  1>>(0, 0));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint8_t,  1>>(1, 1));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint8_t,  1>>(1, 2));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint8_t,  1>>(2, 4));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 1>>(0, 0));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 1>>(1, 1));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 1>>(2, 1));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 1>>(2, 2));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 1>>(2, 3));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 1>>(3, 3));

static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint8_t,  4>>(0, 0));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint8_t,  4>>(1, 1));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint8_t,  4>>(1, 2));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint8_t,  4>>(2, 4));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 4>>(0, 0));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 4>>(1, 1));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 4>>(2, 1));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 4>>(2, 2));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 4>>(2, 3));
static_assert( is_portional<std_array<uint8_t, 1>, std_array<uint16_t, 4>>(3, 3));

static_assert( is_portional<std_array<uint8_t, 4>, std_array<uint8_t,  1>>(0, 0));
static_assert(!is_portional<std_array<uint8_t, 4>, std_array<uint8_t,  1>>(1, 1));
static_assert(!is_portional<std_array<uint8_t, 4>, std_array<uint8_t,  1>>(1, 2));
static_assert(!is_portional<std_array<uint8_t, 4>, std_array<uint8_t,  1>>(2, 4));
static_assert( is_portional<std_array<uint8_t, 4>, std_array<uint16_t, 1>>(0, 0));
static_assert(!is_portional<std_array<uint8_t, 4>, std_array<uint16_t, 1>>(1, 1));
static_assert(!is_portional<std_array<uint8_t, 4>, std_array<uint16_t, 1>>(2, 1));
static_assert(!is_portional<std_array<uint8_t, 4>, std_array<uint16_t, 1>>(2, 2));
static_assert(!is_portional<std_array<uint8_t, 4>, std_array<uint16_t, 1>>(2, 3)); 
static_assert(!is_portional<std_array<uint8_t, 4>, std_array<uint16_t, 1>>(3, 3));
static_assert( is_portional<std_array<uint8_t, 4>, std_array<uint16_t, 1>>(2, 4));

// overflows
static_assert( is_portional<uint8_t,  uint8_t >(1u, max_size_t));         // maximum Right
static_assert(!is_portional<uint8_t,  uint8_t >(max_size_t, 1u));         // maximum Left (not portional)
static_assert( is_portional<uint8_t,  uint8_t >(max_size_t, max_size_t)); // maximum both
static_assert(!is_portional<uint16_t, uint16_t>(1u, max_size_t));         // overflow Right
static_assert(!is_portional<uint16_t, uint16_t>(max_size_t, 1u));         // overflow Left
static_assert(!is_portional<uint16_t, uint16_t>(max_size_t, max_size_t)); // overflow both

// if_portional<size_t, size_t, Integral, Integral>
// ----------------------------------------------------------------------------

// equal proportions (portional)
static_assert(is_defined<if_portional<1u, uint8_t,  1u, uint8_t>>);
static_assert(is_defined<if_portional<1u, uint16_t, 2u, uint8_t >>);
////static_assert(!is_defined<if_portional<1u, uint8_t,  0u, uint16_t>>);
static_assert(is_defined<if_portional<2u, uint8_t,  1u, uint16_t>>);
////static_assert(!is_defined<if_portional<3u, uint8_t,  1u, uint16_t>>);
static_assert(is_defined<if_portional<8u, uint8_t,  1u, uint64_t>>);
static_assert(is_defined<if_portional<8u, uint16_t, 2u, uint64_t>>);
static_assert(is_defined<if_portional<8u, uint32_t, 4u, uint64_t>>);
static_assert(is_defined<if_portional<8u, uint64_t, 8u, uint64_t>>);

// lesser proportions (portional)
static_assert(is_defined<if_portional<1u, uint8_t,  2u, uint8_t>>);
static_assert(is_defined<if_portional<1u, uint16_t, 3u, uint8_t >>);
static_assert(is_defined<if_portional<1u, uint8_t,  1u, uint16_t>>);
static_assert(is_defined<if_portional<2u, uint8_t,  2u, uint16_t>>);
static_assert(is_defined<if_portional<3u, uint8_t,  2u, uint16_t>>);
static_assert(is_defined<if_portional<8u, uint8_t,  2u, uint64_t>>);
static_assert(is_defined<if_portional<8u, uint16_t, 3u, uint64_t>>);
static_assert(is_defined<if_portional<8u, uint32_t, 5u, uint64_t>>);
static_assert(is_defined<if_portional<8u, uint64_t, 9u, uint64_t>>);

// greater proportions (not portional)
////static_assert(!is_defined<if_portional<2u, uint8_t,  1u, uint8_t>>);
////static_assert(!is_defined<if_portional<2u, uint16_t, 2u, uint8_t >>);
////static_assert(!is_defined<if_portional<2u, uint8_t,  0u, uint16_t>>);
////static_assert(!is_defined<if_portional<3u, uint8_t,  1u, uint16_t>>);
////static_assert(!is_defined<if_portional<4u, uint8_t,  1u, uint16_t>>);
////static_assert(!is_defined<if_portional<9u, uint8_t,  1u, uint64_t>>);
////static_assert(!is_defined<if_portional<9u, uint16_t, 2u, uint64_t>>);
////static_assert(!is_defined<if_portional<9u, uint32_t, 4u, uint64_t>>);
////static_assert(!is_defined<if_portional<9u, uint64_t, 8u, uint64_t>>);

// array_cast (array to array)
// ----------------------------------------------------------------------------

// rule out rounding in type constraint.
using array16x128 = std::array<uint16_t, 256 / 2>;
using array16x127 = std::array<uint16_t, 127>;
using array16x126 = std::array<uint16_t, 254 / 2>;
using array8x256 = std::array<uint8_t, 256>;
using array8x255 = std::array<uint8_t, 255>; // cannot match either 127 or 128.
using array8x254 = std::array<uint8_t, 254>;
constexpr array8x256 data8x256{};
////constexpr array8x255 data8x255{};
constexpr array8x254 data8x254{};
static_assert(std::is_same_v<decltype(array_cast<uint16_t>(data8x256)), const array16x128&>);
////static_assert(std::is_same_v<decltype(array_cast<uint16_t>(data8x255)), const array16x128&>);
////static_assert(std::is_same_v<decltype(array_cast<uint16_t>(data8x255)), const array16x127&>);
static_assert(std::is_same_v<decltype(array_cast<uint16_t>(data8x254)), const array16x127&>);

using array8x2 = data_array<2>;
using array16x1 = std::array<uint16_t, 1>;
constexpr array8x2 const_data8x2{ 1_u8, 2_u8 };
constexpr array16x1 const_data16x1{ 1_u16 };
auto data8x2 = const_data8x2;
auto data16x1 = const_data16x1;

// reinterpret_cast(nullptr) at array size zero is safe, so empty array narrowing is allowed.
static_assert(data_array<0>{}.data() == nullptr);
using array8x0 = data_array<0>;
constexpr array8x0 const_data8x0{};
static_assert(std::is_same_v<decltype(array_cast<uint8_t>(const_data8x0)), const array8x0&>);

static_assert(std::is_same_v<decltype(array_cast<uint16_t>(data8x2)), array16x1&>);
static_assert(std::is_same_v<decltype(array_cast<uint8_t>(data16x1)), array8x2&>);
static_assert(std::is_same_v<decltype(array_cast<uint16_t>(data16x1)), array16x1&>);

static_assert(std::is_same_v<decltype(array_cast<uint16_t>(const_data8x2)), const array16x1&>);
static_assert(std::is_same_v<decltype(array_cast<uint8_t>(const_data16x1)), const array8x2&>);
static_assert(std::is_same_v<decltype(array_cast<uint16_t>(const_data16x1)), const array16x1&>);

// Since _u8 data is cast into integrals its above order is preserved.
// The endianness of the _u32 integrals affects their numerical interpretation.
BOOST_AUTO_TEST_CASE(cast__array_cast__const__expected)
{
    constexpr data_array<2> data8x2{ 0x01_u8, 0x02_u8 };

    const auto& value16x1 = array_cast<uint16_t>(data8x2);
    BOOST_REQUIRE_EQUAL(value16x1[0], native_to_little_end(0x0201_u16));

    const auto& value8x2 = array_cast<uint8_t>(data8x2);
    BOOST_REQUIRE_EQUAL(value8x2[0], data8x2[0]);
    BOOST_REQUIRE_EQUAL(value8x2[1], data8x2[1]);
}

// Since _u8 data is cast into integrals its above order is preserved.
// The endianness of the _u32 integrals affects their numerical interpretation.
BOOST_AUTO_TEST_CASE(cast__array_cast__non_const__expected)
{
    data_array<2> data8x2{ 0x01_u8, 0x02_u8 };

    auto& value16x1 = array_cast<uint16_t>(data8x2);
    BOOST_REQUIRE_EQUAL(value16x1[0], native_to_little_end(0x0201_u16));

    auto& value8x2 = array_cast<uint8_t>(data8x2);
    BOOST_REQUIRE_EQUAL(value8x2[0], data8x2[0]);
    BOOST_REQUIRE_EQUAL(value8x2[1], data8x2[1]);

    // Demonstrate that this is a cast, not a copy.
    value8x2[0] = 0x42_u8;
    value8x2[1] = 0x24_u8;
    BOOST_REQUIRE_EQUAL(value16x1[0], native_to_little_end(0x2442_u16));
}

// array_cast (array<integral> to array<integral, size>)
// ----------------------------------------------------------------------------

using array16x5 = std::array<uint16_t, 5>;
using array64x3 = std::array<uint64_t, 3>;
constexpr array16x5 const_data16x5{ 1_u16, 2_u16, 3_u16, 4_u16, 5_u16 };
constexpr array64x3 const_data64x3{ 1_u64, 2_u64, 3_u64 };
auto data16x5 = const_data16x5;
auto data64x3 = const_data64x3;
static_assert(std::is_same_v<decltype(array_cast<uint16_t, 5>(data64x3)), array16x5&>);
static_assert(std::is_same_v<decltype(array_cast<uint16_t, 5>(const_data64x3)), const array16x5&>);

// reinterpret_cast(nullptr) [at array size zero] is safe, so empty array narrowing is allowed.
static_assert(data_array<0>{}.data() == nullptr);

static_assert(std::is_same_v<decltype(array_cast<uint8_t, 0>(const_data64x3)), const data_array<0>&>);
static_assert(std::is_same_v<decltype(array_cast<uint8_t, 1>(const_data64x3)), const data_array<1>&>);
static_assert(std::is_same_v<decltype(array_cast<uint8_t, 16>(const_data64x3)), const data_array<16>&>);
static_assert(std::is_same_v<decltype(array_cast<uint8_t, 23>(const_data64x3)), const data_array<23>&>);
////static_assert(std::is_same_v<decltype(array_cast<uint8_t, 24>(const_data64x3)), const data_array<24>&>);
////static_assert(array_cast<uint8_t>(const_data64x3).size() == 24);

static_assert(std::is_same_v<decltype(array_cast<uint16_t, 0>(const_data64x3)), const std::array<uint16_t, 0>&>);
static_assert(std::is_same_v<decltype(array_cast<uint16_t, 1>(const_data64x3)), const std::array<uint16_t, 1>&>);
static_assert(std::is_same_v<decltype(array_cast<uint16_t, 8>(const_data64x3)), const std::array<uint16_t, 8>&>);
static_assert(std::is_same_v<decltype(array_cast<uint16_t, 11>(const_data64x3)), const std::array<uint16_t, 11>&>);
////static_assert(std::is_same_v<decltype(array_cast<uint16_t, 12>(const_data64x3)), const std::array<uint16_t, 12>&>);
////static_assert(array_cast<uint16_t>(const_data64x3).size() == 12);

static_assert(std::is_same_v<decltype(array_cast<uint32_t, 0>(const_data64x3)), const std::array<uint32_t, 0>&>);
static_assert(std::is_same_v<decltype(array_cast<uint32_t, 1>(const_data64x3)), const std::array<uint32_t, 1>&>);
static_assert(std::is_same_v<decltype(array_cast<uint32_t, 2>(const_data64x3)), const std::array<uint32_t, 2>&>);
static_assert(std::is_same_v<decltype(array_cast<uint32_t, 5>(const_data64x3)), const std::array<uint32_t, 5>&>);
////static_assert(std::is_same_v<decltype(array_cast<uint32_t, 6>(const_data64x3)), const std::array<uint32_t, 6>&>);
////static_assert(array_cast<uint32_t>(const_data64x3).size() == 6);

static_assert(std::is_same_v<decltype(array_cast<uint64_t, 0>(const_data64x3)), const std::array<uint64_t, 0>&>);
static_assert(std::is_same_v<decltype(array_cast<uint64_t, 1>(const_data64x3)), const std::array<uint64_t, 1>&>);
static_assert(std::is_same_v<decltype(array_cast<uint64_t, 2>(const_data64x3)), const std::array<uint64_t, 2>&>);
////static_assert(std::is_same_v<decltype(array_cast<uint64_t, 5>(const_data64x3)), const std::array<uint64_t, 3>&>);
////static_assert(array_cast<uint64_t>(const_data64x3).size() == 3);

BOOST_AUTO_TEST_CASE(cast__array_cast__const_narrowing__expected)
{
    constexpr data_array<2> data8x2{ 0x01_u8, 0x02_u8 };

    const auto& value8x1 = array_cast<uint8_t, 1>(data8x2);
    BOOST_REQUIRE_EQUAL(value8x1[0], 0x01_u8);
}

BOOST_AUTO_TEST_CASE(cast__array_cast__non_const_narrowing__expected)
{
    data_array<2> data8x2{ 0x01_u8, 0x02_u8 };

    auto& value8x1 = array_cast<uint8_t, 1>(data8x2);
    BOOST_REQUIRE_EQUAL(value8x1[0], 0x01_u8);

    // Demonstrate that this is a cast, not a copy.
    value8x1[0] = 0x42_u8;
    BOOST_REQUIRE_EQUAL(data8x2[0], 0x42_u8);
}

// array_cast (array<integral> to array<integral, size,  offset>)
// ----------------------------------------------------------------------------

// array_cast (array<array>    to array<integral, size, [offset]>)
// ----------------------------------------------------------------------------

// array_cast (array<integral> to array<array,    size, [offset]>)
// ----------------------------------------------------------------------------

// unsafe_array_cast (array of integrals)
// ----------------------------------------------------------------------------

// 3 lls to 12 shorts
constexpr auto llongs16 = 3_size;
constexpr auto llints16 = (llongs16 * sizeof(uint64_t)) / sizeof(uint16_t);
constexpr uint64_t const_llongs16[llongs16]{ 42_u64, 43_u64, 44_u64 };
using llong_array16 = decltype(unsafe_array_cast<uint16_t, llints16>(const_llongs16));
static_assert(std::is_same_v<llong_array16, const std::array<uint16_t, llints16>&>);
static_assert(!std::is_same_v<llong_array16, std::array<uint16_t, llints16>&>);

// 8 bytes to 4 shorts
constexpr auto bytes16 = 8_size;
constexpr auto ints16 = (bytes16 * sizeof(uint8_t)) / sizeof(uint16_t);
constexpr uint8_t const_data16[bytes16]{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };
using const_array16 = decltype(unsafe_array_cast<uint16_t, ints16>(const_data16));
static_assert(std::is_same_v<const_array16, const std::array<uint16_t, ints16>&>);
static_assert(!std::is_same_v<const_array16, std::array<uint16_t, ints16>&>);

// 8 bytes to 2 ints
constexpr auto bytes32 = 8_size;
constexpr auto ints32 = (bytes32 * sizeof(uint8_t)) / sizeof(uint32_t);
constexpr uint8_t const_data32[bytes32]{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };
using const_array32 = decltype(unsafe_array_cast<uint32_t, ints32>(const_data32));
static_assert(std::is_same_v<const_array32, const std::array<uint32_t, ints32>&>);
static_assert(!std::is_same_v<const_array32, std::array<uint32_t, ints32>&>);

// Type (including signedness) and array size must correspond.
static_assert(!std::is_same_v<const_array32, const std::array<int32_t, ints32>&>);
static_assert(!std::is_same_v<const_array32, const std::array<uint16_t, ints32>&>);
static_assert(!std::is_same_v<const_array32, const std::array<uint32_t, ints32 * 2_size>&>);

// But, a data buffer of any size can be cast to an array of any size.
// This is the unsafe nature of a reinterpret_cast (and why it cannot be constexpr).
// ============================================================================
// 1 byte to 1 int (out of bounds)
constexpr uint8_t data1[1]{ 1_u8 };
using const_array1 = decltype(unsafe_array_cast<uint32_t, 1>(data1));
static_assert(std::is_same_v<const_array1, const std::array<uint32_t, 1>&>);
// ============================================================================

// Values cannot be tested here, as reinterpret_cast is not constexpr.
uint8_t non_const_data32[bytes32]{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };
using non_const_array32 = decltype(unsafe_array_cast<uint32_t, ints32>(non_const_data32));
static_assert(std::is_same_v<non_const_array32, std::array<uint32_t, ints32>&>);
static_assert(!std::is_same_v<non_const_array32, const std::array<uint32_t, ints32>&>);

BOOST_AUTO_TEST_CASE(cast__unsafe_array_cast__const__expected)
{
    constexpr data_array<8> data8x8{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };

    const auto& value32x2 = unsafe_array_cast<uint32_t, 2>(data8x8.data());
    BOOST_REQUIRE_EQUAL(value32x2[0], native_to_little_end(0x04030201_u32));
    BOOST_REQUIRE_EQUAL(value32x2[1], native_to_little_end(0x08070605_u32));
}

BOOST_AUTO_TEST_CASE(cast__unsafe_array_cast__non_const__expected)
{
    data_array<8> data8x8{ 1_u8, 2_u8, 3_u8, 4_u8, 5_u8, 6_u8, 7_u8, 8_u8 };

    auto& value32x2 = unsafe_array_cast<uint32_t, 2>(data8x8.data());
    BOOST_REQUIRE_EQUAL(value32x2[0], native_to_little_end(0x04030201_u32));
    BOOST_REQUIRE_EQUAL(value32x2[1], native_to_little_end(0x08070605_u32));

    // Demonstrate that this is a cast, not a copy.
    value32x2[0] = native_to_big_end(0x12345678_u32);
    value32x2[1] = native_to_big_end(0x0abcdef0_u32);
    BOOST_REQUIRE_EQUAL(data8x8[0], 0x12_u8);
    BOOST_REQUIRE_EQUAL(data8x8[1], 0x34_u8);
    BOOST_REQUIRE_EQUAL(data8x8[2], 0x56_u8);
    BOOST_REQUIRE_EQUAL(data8x8[3], 0x78_u8);
    BOOST_REQUIRE_EQUAL(data8x8[4], 0x0a_u8);
    BOOST_REQUIRE_EQUAL(data8x8[5], 0xbc_u8);
    BOOST_REQUIRE_EQUAL(data8x8[6], 0xde_u8);
    BOOST_REQUIRE_EQUAL(data8x8[7], 0xf0_u8);
}

// unsafe_array_cast (array of arrays of integrals)
// ----------------------------------------------------------------------------

// 3 lls to 3 arrays of 4 shorts (std::array<std::array<uint16_t, 4>, 3>)
constexpr auto longs = 3_size;
constexpr uint64_t value64x3[longs]{ 0x0102030405060708_u64, 0x1122334455667788_u64, 0xabcdef1234567890_u64 };
using inner = std::array<uint16_t, 4>;
using outer = decltype(unsafe_array_cast<inner, 3>(value64x3));
static_assert(std::is_same_v<outer, const std::array<inner, 3>&>);

BOOST_AUTO_TEST_CASE(array_cast__unsafe_array_cast__const_array__expected)
{
    constexpr auto longs = 3_size;
    constexpr std::array<uint64_t, longs> value64x3
    {
        native_to_big_end(0x0102030405060708_u64),
        native_to_big_end(0x1122334455667788_u64),
        native_to_big_end(0xabcdef1234567890_u64)
    };

    auto& value16x4x3 = unsafe_array_cast<std::array<uint16_t, 4>, 3>(value64x3.data());
    BOOST_REQUIRE_EQUAL(value16x4x3[0][0], native_to_big_end(0x0102_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[0][1], native_to_big_end(0x0304_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[0][2], native_to_big_end(0x0506_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[0][3], native_to_big_end(0x0708_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[1][0], native_to_big_end(0x1122_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[1][1], native_to_big_end(0x3344_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[1][2], native_to_big_end(0x5566_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[1][3], native_to_big_end(0x7788_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[2][0], native_to_big_end(0xabcd_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[2][1], native_to_big_end(0xef12_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[2][2], native_to_big_end(0x3456_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[2][3], native_to_big_end(0x7890_u16));
}

BOOST_AUTO_TEST_CASE(array_cast__unsafe_array_cast__non_const_array__expected)
{
    constexpr auto longs = 3_size;
    std::array<uint64_t, longs> value64x3
    {
        native_to_big_end(0x0102030405060708_u64),
        native_to_big_end(0x1122334455667788_u64),
        native_to_big_end(0xabcdef1234567890_u64)
    };

    auto& value16x4x3 = unsafe_array_cast<std::array<uint16_t, 4>, 3>(value64x3.data());
    BOOST_REQUIRE_EQUAL(value16x4x3[0][0], native_to_big_end(0x0102_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[0][1], native_to_big_end(0x0304_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[0][2], native_to_big_end(0x0506_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[0][3], native_to_big_end(0x0708_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[1][0], native_to_big_end(0x1122_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[1][1], native_to_big_end(0x3344_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[1][2], native_to_big_end(0x5566_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[1][3], native_to_big_end(0x7788_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[2][0], native_to_big_end(0xabcd_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[2][1], native_to_big_end(0xef12_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[2][2], native_to_big_end(0x3456_u16));
    BOOST_REQUIRE_EQUAL(value16x4x3[2][3], native_to_big_end(0x7890_u16));

    // Demonstrate that this is a cast, not a copy.
    value16x4x3[0] = std::array<uint16_t, 4>
    {
        native_to_big_end(1_u16),
        native_to_big_end(2_u16),
        native_to_big_end(3_u16),
        native_to_big_end(4_u16),
    };
    value16x4x3[1] = std::array<uint16_t, 4>
    {
        native_to_big_end(5_u16),
        native_to_big_end(6_u16),
        native_to_big_end(7_u16),
        native_to_big_end(8_u16),
    };
    value16x4x3[2] = std::array<uint16_t, 4>
    {
        native_to_big_end(9_u16),
        native_to_big_end(10_u16),
        native_to_big_end(11_u16),
        native_to_big_end(12_u16),
    };

    BOOST_REQUIRE_EQUAL(value64x3[0], native_to_big_end(0x0001000200030004_u64));
    BOOST_REQUIRE_EQUAL(value64x3[1], native_to_big_end(0x0005000600070008_u64));
    BOOST_REQUIRE_EQUAL(value64x3[2], native_to_big_end(0x0009000a000b000c_u64));
}

// unsafe_vector_cast (vector of array references cast from bytes)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(array_cast__unsafe_vector_cast__const_data__expected)
{
    using data_t  = data_array<sizeof(uint16_t) * 2 * 3>;
    using inner_t = std_array<uint16_t, 2>;
    using outer_t = std_vector<std::reference_wrapper<const inner_t>>;
    const data_t bytes
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c
    };

    // All inner const as expected.
    using function_t  = decltype(unsafe_vector_cast<inner_t>(bytes.data(), one));
    using reference_t = decltype(unsafe_vector_cast<inner_t>(bytes.data(), one)[0]);
    using array_t     = decltype(unsafe_vector_cast<inner_t>(bytes.data(), one)[0].get());
    using value_t     = decltype(unsafe_vector_cast<inner_t>(bytes.data(), one)[0].get()[0]);
    static_assert(std::is_same_v<function_t, outer_t>);
    static_assert(std::is_same_v<reference_t, std::reference_wrapper<const inner_t>&>);
    static_assert(std::is_same_v<array_t, const inner_t&>);
    static_assert(std::is_same_v<value_t, const uint16_t&>);

    // Cast 12 bytes into vector of 3 const 4 byte (2 x 2) array references.
    const auto values32x3 = unsafe_vector_cast<inner_t>(bytes.data(), 3);

    BOOST_CHECK_EQUAL(values32x3[0].get()[0], native_to_big_end(0x0102_u16));
    BOOST_CHECK_EQUAL(values32x3[0].get()[1], native_to_big_end(0x0304_u16));
    BOOST_CHECK_EQUAL(values32x3[1].get()[0], native_to_big_end(0x0506_u16));
    BOOST_CHECK_EQUAL(values32x3[1].get()[1], native_to_big_end(0x0708_u16));
    BOOST_CHECK_EQUAL(values32x3[2].get()[0], native_to_big_end(0x090a_u16));
    BOOST_CHECK_EQUAL(values32x3[2].get()[1], native_to_big_end(0x0b0c_u16));

    // Cast from reference_wrapper also works.
    BOOST_CHECK_EQUAL(((inner_t)values32x3[0])[0], native_to_big_end(0x0102_u16));
    BOOST_CHECK_EQUAL(((inner_t)values32x3[0])[1], native_to_big_end(0x0304_u16));
    BOOST_CHECK_EQUAL(((inner_t)values32x3[1])[0], native_to_big_end(0x0506_u16));
    BOOST_CHECK_EQUAL(((inner_t)values32x3[1])[1], native_to_big_end(0x0708_u16));
    BOOST_CHECK_EQUAL(((inner_t)values32x3[2])[0], native_to_big_end(0x090a_u16));
    BOOST_CHECK_EQUAL(((inner_t)values32x3[2])[1], native_to_big_end(0x0b0c_u16));
}

BOOST_AUTO_TEST_CASE(array_cast__unsafe_vector_cast__non_const_data__expected)
{
    using data_t  = data_array<sizeof(uint16_t) * 2 * 3>;
    using inner_t = std_array<uint16_t, 2>;
    using outer_t = std_vector<std::reference_wrapper<inner_t>>;
    data_t bytes
    {
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c
    };

    // All innner non-const as expected.
    using function_t  = decltype(unsafe_vector_cast<inner_t>(bytes.data(), one));
    using reference_t = decltype(unsafe_vector_cast<inner_t>(bytes.data(), one)[0]);
    using array_t     = decltype(unsafe_vector_cast<inner_t>(bytes.data(), one)[0].get());
    using value_t     = decltype(unsafe_vector_cast<inner_t>(bytes.data(), one)[0].get()[0]);
    static_assert(std::is_same_v<function_t, outer_t>);
    static_assert(std::is_same_v<reference_t, std::reference_wrapper<inner_t>&>);
    static_assert(std::is_same_v<array_t, inner_t&>);
    static_assert(std::is_same_v<value_t, uint16_t&>);

    // Cast 12 bytes into vector of 3 non-const 4 byte (2 x 2) array references.
    auto values32x3 = unsafe_vector_cast<std_array<uint16_t, 2>>(bytes.data(), 3);

    BOOST_CHECK_EQUAL(values32x3[0].get()[0], native_to_big_end(0x0102_u16));
    BOOST_CHECK_EQUAL(values32x3[0].get()[1], native_to_big_end(0x0304_u16));
    BOOST_CHECK_EQUAL(values32x3[1].get()[0], native_to_big_end(0x0506_u16));
    BOOST_CHECK_EQUAL(values32x3[1].get()[1], native_to_big_end(0x0708_u16));
    BOOST_CHECK_EQUAL(values32x3[2].get()[0], native_to_big_end(0x090a_u16));
    BOOST_CHECK_EQUAL(values32x3[2].get()[1], native_to_big_end(0x0b0c_u16));

    // Demonstrate that it is a set of references to a cast, by writing forward.
    bytes =
    {
        0xab, 0xcd, 0xef, 0x01, 0x23, 0x45, 0x67, 0x89, 0xa1, 0xb2, 0xc3, 0xd4
    };

    BOOST_CHECK_EQUAL(values32x3[0].get()[0], native_to_big_end(0xabcd_u16));
    BOOST_CHECK_EQUAL(values32x3[0].get()[1], native_to_big_end(0xef01_u16));
    BOOST_CHECK_EQUAL(values32x3[1].get()[0], native_to_big_end(0x2345_u16));
    BOOST_CHECK_EQUAL(values32x3[1].get()[1], native_to_big_end(0x6789_u16));
    BOOST_CHECK_EQUAL(values32x3[2].get()[0], native_to_big_end(0xa1b2_u16));
    BOOST_CHECK_EQUAL(values32x3[2].get()[1], native_to_big_end(0xc3d4_u16));

    // Demonstrate that it is a set of references to a cast, by writing backward.
    values32x3[0].get()[0] = native_to_big_end(0xa000_u16);
    values32x3[0].get()[1] = native_to_big_end(0xb001_u16);
    values32x3[1].get()[0] = native_to_big_end(0xc002_u16);
    values32x3[1].get()[1] = native_to_big_end(0xd003_u16);
    values32x3[2].get()[0] = native_to_big_end(0xe004_u16);
    values32x3[2].get()[1] = native_to_big_end(0xf004_u16);

    constexpr data_t expected
    {
        0xa0, 0x00, 0xb0, 0x01, 0xc0, 0x02, 0xd0, 0x03, 0xe0, 0x04, 0xf0, 0x04
    };

    BOOST_CHECK_EQUAL(bytes, expected);
}

BOOST_AUTO_TEST_SUITE_END()
