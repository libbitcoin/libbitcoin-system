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

BOOST_AUTO_TEST_SUITE(endian_set_tests)

template <size_t Count, typename Integral = uint32_t>
using numbers = std::array<Integral, Count>;

template <size_t Size>
constexpr numbers<Size> normalize(const numbers<Size>& little,
    const numbers<Size>& big)
{
    if constexpr (is_big_endian)
        return big;
    else
        return little;
}

constexpr auto size = 17;
constexpr auto native = numbers<size>
{
    0x04030201_u32,
    0x08070605_u32,
    0x0c0b0a09_u32,
    0x000f0e0d_u32,
    0x04030201_u32,
    0x08070605_u32,
    0x0c0b0a09_u32,
    0x000f0e0d_u32,

    0x04030201_u32,
    0x08070605_u32,
    0x0c0b0a09_u32,
    0x000f0e0d_u32,
    0x04030201_u32,
    0x08070605_u32,
    0x0c0b0a09_u32,
    0x000f0e0d_u32,

    0x04030201_u32,
};
constexpr auto reversed = numbers<size>
{
    0x01020304_u32,
    0x05060708_u32,
    0x090a0b0c_u32,
    0x0d0e0f00_u32,
    0x01020304_u32,
    0x05060708_u32,
    0x090a0b0c_u32,
    0x0d0e0f00_u32,

    0x01020304_u32,
    0x05060708_u32,
    0x090a0b0c_u32,
    0x0d0e0f00_u32,
    0x01020304_u32,
    0x05060708_u32,
    0x090a0b0c_u32,
    0x0d0e0f00_u32,

    0x01020304_u32
};
// This is the perspective of a little-endian machine.
// The normalize() helper reverses the expecation on a big-endian machine.
static_assert(to_big_endians(native) == normalize(reversed, native));
static_assert(from_big_endians(reversed) == normalize(native, reversed));
static_assert(to_little_endians(native) == normalize(native, reversed));
static_assert(from_little_endians(native) == normalize(native, reversed));

// These test 17 elements, above the loop unroll threshold.

BOOST_AUTO_TEST_CASE(endian__to_big_endians__non_constexpr__expected)
{
    numbers<size> out{};
    to_big_endians(out, native);
    BOOST_REQUIRE_EQUAL(out, normalize(reversed, native));
}

BOOST_AUTO_TEST_CASE(endian__from_big_endians__non_constexpr__expected)
{
    numbers<size> out{};
    from_big_endians(out, reversed);
    BOOST_REQUIRE_EQUAL(out, normalize(native, reversed));
}

BOOST_AUTO_TEST_CASE(endian__to_little_endians__non_constexpr__expected)
{
    numbers<size> out{};
    to_little_endians(out, native);
    BOOST_REQUIRE_EQUAL(out, normalize(native, reversed));
}

BOOST_AUTO_TEST_CASE(endian__from_little_endians__non_constexpr__expected)
{
    numbers<size> out{};
    from_little_endians(out, native);
    BOOST_REQUIRE_EQUAL(out, normalize(native, reversed));
}

template <size_t Size>
inline const numbers<Size>& reduce(const numbers<size>& value)
{
    return array_cast<array_element<numbers<size>>, Size>(value);
}

// These test return value and each level of the loop unroll (non-constexpr).

BOOST_AUTO_TEST_CASE(endian__to_big_endians__loop_unroll__expected)
{
    BOOST_CHECK_EQUAL(to_big_endians(reduce<1>(native)), reduce<1>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<2>(native)), reduce<2>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<3>(native)), reduce<3>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<4>(native)), reduce<4>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<5>(native)), reduce<5>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<6>(native)), reduce<6>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<7>(native)), reduce<7>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<8>(native)), reduce<8>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<9>(native)), reduce<9>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<10>(native)), reduce<10>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<11>(native)), reduce<11>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<12>(native)), reduce<12>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<13>(native)), reduce<13>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<14>(native)), reduce<14>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<15>(native)), reduce<15>(normalize(reversed, native)));
    BOOST_CHECK_EQUAL(to_big_endians(reduce<16>(native)), reduce<16>(normalize(reversed, native)));
}

BOOST_AUTO_TEST_CASE(endian__to_little_endians__loop_unroll__expected)
{
    BOOST_CHECK_EQUAL(to_little_endians(reduce<1>(native)), reduce<1>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<2>(native)), reduce<2>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<3>(native)), reduce<3>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<4>(native)), reduce<4>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<5>(native)), reduce<5>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<6>(native)), reduce<6>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<7>(native)), reduce<7>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<8>(native)), reduce<8>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<9>(native)), reduce<9>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<10>(native)), reduce<10>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<11>(native)), reduce<11>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<12>(native)), reduce<12>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<13>(native)), reduce<13>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<14>(native)), reduce<14>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<15>(native)), reduce<15>(normalize(native, reversed)));
    BOOST_CHECK_EQUAL(to_little_endians(reduce<16>(native)), reduce<16>(normalize(native, reversed)));
}

BOOST_AUTO_TEST_SUITE_END()
