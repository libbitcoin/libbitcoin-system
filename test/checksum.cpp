/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_CASE(checksum_test)
{
    data_chunk data = {'d', 'a', 't', 'a'};

    // No checksum:
    BOOST_REQUIRE(!verify_checksum(data));

    // Valid checksum:
    append_checksum(data);
    BOOST_REQUIRE(data.size() == 8);
    BOOST_REQUIRE(verify_checksum(data));

    // Data corruption:
    data[0] = 1;
    BOOST_REQUIRE(!verify_checksum(data));
}
