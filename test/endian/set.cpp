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

// constexpr

template <size_t Count, typename Integral = uint32_t>
using numbers = std::array<Integral, Count>;

template <size_t Size>
constexpr numbers<Size> normalize(const numbers<Size>& little,
    const numbers<Size>& big)
{
    if constexpr (is_big_endian)
        return big;

    return little;
}

constexpr auto size = 8;
constexpr auto native = numbers<size>
{
    0x04030201_u32,
    0x08070605_u32,
    0x0c0b0a09_u32,
    0x000f0e0d_u32,
    0x04030201_u32,
    0x08070605_u32,
    0x0c0b0a09_u32,
    0x000f0e0d_u32
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
    0x0d0e0f00_u32
};

// This is the perspective of a little-endian machine.
// The normalize() helper reverses the expecation on a big-endian machine.
static_assert(to_big_endians(native) == normalize(reversed, native));
static_assert(from_big_endians(reversed) == normalize(native, reversed));
static_assert(to_little_endians(native) == normalize(native, reversed));
static_assert(from_little_endians(native) == normalize(native, reversed));

// inline

BOOST_AUTO_TEST_CASE(endian__to_big_endians__always__expected)
{
    numbers<size> out{};
    to_big_endians(out, native);
    BOOST_REQUIRE_EQUAL(out, normalize(reversed, native));
}

BOOST_AUTO_TEST_CASE(endian__from_big_endians__always__expected)
{
    numbers<size> out{};
    from_big_endians(out, reversed);
    BOOST_REQUIRE_EQUAL(out, normalize(native, reversed));
}

BOOST_AUTO_TEST_CASE(endian__to_little_endians__always__expected)
{
    numbers<size> out{};
    to_little_endians(out, native);
    BOOST_REQUIRE_EQUAL(out, normalize(native, reversed));
}

BOOST_AUTO_TEST_CASE(endian__from_little_endians__always__expected)
{
    numbers<size> out{};
    from_little_endians(out, native);
    BOOST_REQUIRE_EQUAL(out, normalize(native, reversed));
}

BOOST_AUTO_TEST_SUITE_END()
