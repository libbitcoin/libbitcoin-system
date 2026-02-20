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

// power

static_assert(power(0,  0u) == 0u);
static_assert(power(0u, 0u) == 0u);
static_assert(power(0,  1u) == 0u);
static_assert(power(0u, 1u) == 0u);

static_assert(power(1,  0u) == 1u);
static_assert(power(1u, 0u) == 1u);
static_assert(power(1,  1u) == 1u);
static_assert(power(1u, 1u) == 1u);

static_assert(power(2,  16u) == to_signed(power2(16u)));
static_assert(power(2u, 16u) == 0b1'0000'0000'0000'0000_size);

static_assert(power(3,   0u) == 1u);
static_assert(power(3u,  0u) == 1u);
static_assert(power(3,  10u) == 0xe6a9_size);
static_assert(power(3u, 10u) == 0xe6a9_size);

static_assert(power(-1, 0u) == 1u);
static_assert(power(-1, 1u) == 1_nsize);

static_assert(power(-3,  0u) == 1u);
static_assert(power(-3,  1u) == 3_nsize);
static_assert(power(-3, 10u) == 0xe6a9_size);

// power2
static_assert(power2(0u) == 1u);
static_assert(power2(0u) == 1u);
static_assert(power2(1u) == 2u);
static_assert(power2(1u) == 2u);
static_assert(power2<uint16_t>(15u) == 0b1000'0000'0000'0000_u16);
static_assert(power2<uint16_t>(16u) == 0_u16); // overflow

// power<>
static_assert(power<0>(16u) == power(0u, 16u));
static_assert(power<2>(16u) == power(2u, 16u));
static_assert(power<2>(16u) == power2(16u));
static_assert(power<2, uint16_t>(15u) == 0b1000'0000'0000'0000_u16);
static_assert(power<3>(0u) == 1u);
static_assert(power<3, size_t>(0u) == 1u);

// uint256_t
static_assert(power<2u, uint256_t>(10u) == 1024u);


// floating point
static_assert(is_same_type<decltype(power(0.0f, 0u)), float>);
static_assert(is_same_type<decltype(power(0.0, 0u)), double>);

static_assert(power(0.0f, 0u) == 0.0f);
static_assert(power(0.0, 0u) == 0.0);
static_assert(power(1.0, 0u) == 1.0);
static_assert(power(1.0, 1u) == 1.0);
static_assert(power(2.0, 1u) == 2.0);
static_assert(power(2.0, 2u) == 4.0);
static_assert(power(3.0, 3u) == 27.0);

// Actually 311.1696 but this is the floating point representation.
static_assert(power(4.2, 4u) == 311.16960000000006);

BOOST_AUTO_TEST_SUITE(power_tests)

// uintx is not constexpr.
BOOST_AUTO_TEST_CASE(power_uintx_tests)
{
    BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_SUITE_END()
