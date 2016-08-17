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

BOOST_AUTO_TEST_SUITE(fee_filter_tests)

BOOST_AUTO_TEST_CASE(fee_filter__from_data__insufficient_bytes_failure)
{
    data_chunk raw = { 0xab, 0x11 };
    message::fee_filter instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(message::version::level::maximum, raw));
}

BOOST_AUTO_TEST_CASE(fee_filter__from_data__insufficient_version_failure)
{
    const message::fee_filter expected{ 1 };
    const auto data = expected.to_data(message::fee_filter::version_maximum);
    message::fee_filter instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(
        message::filter_add::version_minimum - 1, data));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    const message::fee_filter expected{ 123 };
    const auto data = expected.to_data(message::fee_filter::version_maximum);
    const auto result = message::fee_filter::factory_from_data(
        message::fee_filter::version_maximum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    const message::fee_filter expected{ 325 };
    const auto data = expected.to_data(message::fee_filter::version_maximum);
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    const auto result = message::fee_filter::factory_from_data(
        message::fee_filter::version_maximum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const message::fee_filter expected{ 58246 };
    const auto data = expected.to_data(message::fee_filter::version_maximum);
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    istream_reader source(istream);
    const auto result = message::fee_filter::factory_from_data(
        message::fee_filter::version_maximum, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_SUITE_END()
