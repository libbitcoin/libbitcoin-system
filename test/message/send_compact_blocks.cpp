/**
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(send_compact_blocks_tests)

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    const message::send_compact_blocks expected{ true, 164 };
    const auto data = expected.to_data(peer_minimum_version);
    const auto result = message::send_compact_blocks::factory_from_data(
        peer_minimum_version, data);

    BOOST_REQUIRE_EQUAL(
        message::send_compact_blocks::satoshi_fixed_size(peer_minimum_version),
        data.size());
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(expected.high_bandwidth_mode, result.high_bandwidth_mode);
    BOOST_REQUIRE_EQUAL(expected.version, result.version);
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    const message::send_compact_blocks expected{ false, 5 };
    const auto data = expected.to_data(peer_minimum_version);
    data_source istream(data);
    const auto result = message::send_compact_blocks::factory_from_data(
        peer_minimum_version, istream);

    BOOST_REQUIRE_EQUAL(
        message::send_compact_blocks::satoshi_fixed_size(peer_minimum_version),
        data.size());
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(expected.high_bandwidth_mode, result.high_bandwidth_mode);
    BOOST_REQUIRE_EQUAL(expected.version, result.version);
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const message::send_compact_blocks expected{ true, 257 };
    const auto data = expected.to_data(peer_minimum_version);
    data_source istream(data);
    istream_reader source(istream);
    const auto result = message::send_compact_blocks::factory_from_data(
        peer_minimum_version, source);

    BOOST_REQUIRE_EQUAL(
        message::send_compact_blocks::satoshi_fixed_size(peer_minimum_version),
        data.size());
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE_EQUAL(expected.high_bandwidth_mode, result.high_bandwidth_mode);
    BOOST_REQUIRE_EQUAL(expected.version, result.version);
}

BOOST_AUTO_TEST_CASE(from_data_reader_invalid_mode_byte)
{
    data_chunk raw_data{ 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
    message::send_compact_blocks msg;
    bool result = msg.from_data(peer_minimum_version, raw_data);
    BOOST_REQUIRE(!result);
}

BOOST_AUTO_TEST_SUITE_END()
