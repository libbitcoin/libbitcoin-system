/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <cstddef>
#include <cstdint>

BOOST_AUTO_TEST_SUITE(limits_tests)

constexpr uint64_t min_uint64 = 0;

BOOST_AUTO_TEST_CASE(limits__limit1__all__expected)
{
    BOOST_REQUIRE_EQUAL((limit< uint8_t>(min_uint64)), 0u);
    BOOST_REQUIRE_EQUAL((limit< int16_t>(min_uint64)), 0);
    BOOST_REQUIRE_EQUAL((limit<uint16_t>(min_uint64)), 0u);
    BOOST_REQUIRE_EQUAL((limit< int32_t>(min_uint64)), 0);
    BOOST_REQUIRE_EQUAL((limit<uint32_t>(min_uint64)), 0u);
    BOOST_REQUIRE_EQUAL((limit< int64_t>(min_uint64)), 0);
    BOOST_REQUIRE_EQUAL((limit<uint64_t>(min_uint64)), 0u);

    BOOST_REQUIRE_EQUAL(limit< uint8_t>(min_int64), 0u);
    BOOST_REQUIRE_EQUAL(limit< int16_t>(min_int64), min_int16);
    BOOST_REQUIRE_EQUAL(limit<uint16_t>(min_int64), 0u);
    BOOST_REQUIRE_EQUAL(limit< int32_t>(min_int64), min_int32);
    BOOST_REQUIRE_EQUAL(limit<uint32_t>(min_int64), 0u);
    BOOST_REQUIRE_EQUAL(limit< int64_t>(min_int64), min_int64);
    BOOST_REQUIRE_EQUAL(limit<uint64_t>(min_int64), 0u);

    BOOST_REQUIRE_EQUAL(limit< uint8_t>(max_int64), max_uint8);
    BOOST_REQUIRE_EQUAL(limit< int16_t>(max_int64), max_int16);
    BOOST_REQUIRE_EQUAL(limit<uint16_t>(max_int64), max_uint16);
    BOOST_REQUIRE_EQUAL(limit< int32_t>(max_int64), max_int32);
    BOOST_REQUIRE_EQUAL(limit<uint32_t>(max_int64), max_uint32);
    BOOST_REQUIRE_EQUAL(limit< int64_t>(max_int64), max_int64);
    BOOST_REQUIRE_EQUAL(limit<uint64_t>(max_int64), max_int64);

    BOOST_REQUIRE_EQUAL(limit< uint8_t>(max_uint64), max_uint8);
    BOOST_REQUIRE_EQUAL(limit< int16_t>(max_uint64), max_int16);
    BOOST_REQUIRE_EQUAL(limit<uint16_t>(max_uint64), max_uint16);
    BOOST_REQUIRE_EQUAL(limit< int32_t>(max_uint64), max_int32);
    BOOST_REQUIRE_EQUAL(limit<uint32_t>(max_uint64), max_uint32);
    BOOST_REQUIRE_EQUAL(limit< int64_t>(max_uint64), max_int64);
    BOOST_REQUIRE_EQUAL(limit<uint64_t>(max_uint64), max_uint64);
}

BOOST_AUTO_TEST_CASE(limits__limit3__all__expected)
{
    BOOST_REQUIRE_EQUAL(limit(size_t(0),  size_t(0),  size_t(0)),  size_t(0));
    BOOST_REQUIRE_EQUAL(limit(size_t(25), size_t(50), size_t(60)), size_t(50));
    BOOST_REQUIRE_EQUAL(limit(size_t(50), size_t(50), size_t(60)), size_t(50));
    BOOST_REQUIRE_EQUAL(limit(size_t(55), size_t(50), size_t(60)), size_t(55));
    BOOST_REQUIRE_EQUAL(limit(size_t(60), size_t(50), size_t(60)), size_t(60));
    BOOST_REQUIRE_EQUAL(limit(size_t(75), size_t(50), size_t(60)), size_t(60));

    BOOST_REQUIRE_EQUAL(limit( 0, size_t(0),  size_t(0)),  size_t(0));
    BOOST_REQUIRE_EQUAL(limit(25, size_t(50), size_t(60)), size_t(50));
    BOOST_REQUIRE_EQUAL(limit(50, size_t(50), size_t(60)), size_t(50));
    BOOST_REQUIRE_EQUAL(limit(55, size_t(50), size_t(60)), size_t(55));
    BOOST_REQUIRE_EQUAL(limit(60, size_t(50), size_t(60)), size_t(60));
    BOOST_REQUIRE_EQUAL(limit(75, size_t(50), size_t(60)), size_t(60));

    BOOST_REQUIRE_EQUAL(limit(size_t(0),  0,   0),  0);
    BOOST_REQUIRE_EQUAL(limit(size_t(25), 50, 60), 50);
    BOOST_REQUIRE_EQUAL(limit(size_t(50), 50, 60), 50);
    BOOST_REQUIRE_EQUAL(limit(size_t(55), 50, 60), 55);
    BOOST_REQUIRE_EQUAL(limit(size_t(60), 50, 60), 60);
    BOOST_REQUIRE_EQUAL(limit(size_t(75), 50, 60), 60);

    BOOST_REQUIRE_EQUAL(limit( 0,  0,  0),  0);
    BOOST_REQUIRE_EQUAL(limit(25, 50, 60), 50);
    BOOST_REQUIRE_EQUAL(limit(50, 50, 60), 50);
    BOOST_REQUIRE_EQUAL(limit(55, 50, 60), 55);
    BOOST_REQUIRE_EQUAL(limit(60, 50, 60), 60);
    BOOST_REQUIRE_EQUAL(limit(75, 50, 60), 60);

    BOOST_REQUIRE_EQUAL(limit(-25, -60, -50), -50);
    BOOST_REQUIRE_EQUAL(limit(-50, -60, -50), -50);
    BOOST_REQUIRE_EQUAL(limit(-55, -60, -50), -55);
    BOOST_REQUIRE_EQUAL(limit(-60, -60, -50), -60);
    BOOST_REQUIRE_EQUAL(limit(-75, -60, -50), -60);

    BOOST_REQUIRE_EQUAL(limit(-25, -60, 50), -25);
    BOOST_REQUIRE_EQUAL(limit(-50, -60, 50), -50);
    BOOST_REQUIRE_EQUAL(limit(-55, -60, 50), -55);
    BOOST_REQUIRE_EQUAL(limit(-60, -60, 50), -60);
    BOOST_REQUIRE_EQUAL(limit(-75, -60, 50), -60);
}

BOOST_AUTO_TEST_SUITE_END()
