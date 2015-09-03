/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(format_tests)

// to_{big,little}_endian, from_{big,little}_endian_unsafe

BOOST_AUTO_TEST_CASE(endian_test)
{
    auto le = to_little_endian<uint32_t>(123456789);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(le.begin()), 123456789u);

    auto be = to_big_endian<uint32_t>(123456789);
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint32_t>(be.begin()), 123456789u);

    std::reverse(le.begin(), le.end());
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint32_t>(le.begin()), 123456789u);

    auto bytes = data_chunk{ 0xff };
    BOOST_REQUIRE_EQUAL(from_big_endian_unsafe<uint8_t>(bytes.begin()), 255u);

    auto quad = to_little_endian<uint64_t>(0x1122334455667788);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint64_t>(quad.begin()), 0x1122334455667788u);
}

BOOST_AUTO_TEST_SUITE_END()
