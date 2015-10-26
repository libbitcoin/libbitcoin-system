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
#include <future>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::network;

BOOST_AUTO_TEST_SUITE(network_tests)

BOOST_AUTO_TEST_CASE(p2p__start__defaults__okay)
{
    settings configuration = p2p::testnet;
    configuration.threads = 1;
    configuration.inbound_connection_limit = 0;
    configuration.seeds = { configuration.seeds[1] };

    p2p network(configuration);

    std::promise<const code&> started;
    const auto start_handler = [&started](const code& ec)
    {
        started.set_value(ec);
    };

    network.start(start_handler);

    // Wait until start is complete.
    const auto start_code = started.get_future().get().value();
    BOOST_REQUIRE_EQUAL(start_code, error::success);

    std::promise<const code&> stopped;
    const auto stop_handler = [&stopped](const code& ec)
    {
        stopped.set_value(ec);
    };

    network.stop(stop_handler);

    // Wait until stop is complete.
    const auto stop_code = stopped.get_future().get().value();
    BOOST_REQUIRE_EQUAL(stop_code, error::success);
}

BOOST_AUTO_TEST_CASE(p2p__stop__defaults__okay)
{
    settings configuration = p2p::testnet;
    configuration.threads = 1;
    configuration.inbound_connection_limit = 0;

    p2p network(configuration);

    std::promise<const code&> result;
    const auto handler = [&result](const code& ec)
    {
        result.set_value(ec);
    };

    network.stop(handler);

    // Wait until stop is complete and verify it was successful.
    BOOST_REQUIRE_EQUAL(result.get_future().get().value(), error::success);
}

BOOST_AUTO_TEST_SUITE_END()
