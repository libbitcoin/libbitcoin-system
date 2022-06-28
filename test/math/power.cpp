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

// power2

static_assert(power2(0) == 1u);
static_assert(power2(0u) == 1u);

static_assert(power2(1) == 2u);
static_assert(power2(1u) == 2u);

static_assert(power2(-1) == 1u / 2u);
static_assert(power2(-2) == 1u / 4u);
static_assert(power2(-3) == 0u);
static_assert(power2<uint16_t>(15) == 0x8000u);
static_assert(power2<uint16_t>(16) == 0u);

static_assert(power(0, 0) == 0u);
static_assert(power(0u, 0u) == 0u);
static_assert(power(0, 1) == 0u);
static_assert(power(0u, 1u) == 0u);

static_assert(power(1, 0) == 1u);
static_assert(power(1u, 0u) == 1u);
static_assert(power(1, 1) == 1u);
static_assert(power(1u, 1u) == 1u);

static_assert(power(3, 0) == 1u);
static_assert(power(3u, 0u) == 1u);
static_assert(power(1, -1) == 1u / 1u);
static_assert(power(3, -1) == 1u / 3u);
static_assert(power(3, -2) == 1u / 9u);
static_assert(power(3, -3) == 1u / 81u);
static_assert(power(3, 10) == 0xe6a9u);
static_assert(power(3u, 10u) == 0xe6a9u);
static_assert(power<uint16_t>(3, 11) == 0xb3fbu);

static_assert(power(2, 16) == power2(16));
static_assert(power(2u, 16u) == power2(16u));

static_assert(power(-1, 0) == 1u);
static_assert(power(-1, 1) == static_cast<size_t>(-1));

static_assert(power(-3, 0) == 1u);
static_assert(power(-3, 1) == static_cast<size_t>(-3));
static_assert(power(-3, -1) == 1u / static_cast<size_t>(-3));
static_assert(power(-3, -2) ==  1u / 9u);
static_assert(power(-3, -3) == 1u / static_cast<size_t>(-81));
static_assert(power(-3, 10) == 0xe6a9u);
static_assert(power<uint16_t>(-3, 11) == 0x4c05u);

// power<base>
static_assert(power<0u>(16) == power(0, 16));
static_assert(power<2u>(16) == power(2, 16));
static_assert(power<2u>(16u) == power2(16u));
static_assert(power<2u, uint16_t>(15) == 0x8000u);
static_assert(power<3u>(0) == 1u);
static_assert(power<3u>(0u) == 1u);
static_assert(power<3u, size_t>(0u) == 1u);

// uint256_t
static_assert(power<2u, uint256_t>(10) == 1024u);

// uintx is not constexpr.

BOOST_AUTO_TEST_SUITE(power_tests)

BOOST_AUTO_TEST_CASE(power_uintx_tests)
{
    BOOST_REQUIRE(true);
}

BOOST_AUTO_TEST_SUITE_END()
