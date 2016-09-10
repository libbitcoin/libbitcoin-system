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

#include <string>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

bool equal(const message::reject& left, const message::reject& right)
{
    return (left.code == right.code)
        && (left.message == right.message)
        && (left.reason == right.reason)
        && (left.data == right.data);
}

static const std::string reason_text = "My Reason...";

static const hash_digest data
{
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F
    }
};

BOOST_AUTO_TEST_SUITE(reject_tests)

BOOST_AUTO_TEST_CASE(from_data_insufficient_bytes_failure)
{
    static const data_chunk raw{ 0xab };
    message::reject instance{};

    BOOST_REQUIRE_EQUAL(false, instance.from_data(message::version::level::maximum, raw));
}

BOOST_AUTO_TEST_CASE(from_data_insufficient_version_failure)
{
    const message::reject expected
    {
        message::block_message::command,
        message::reject::reason_code::dust,
        reason_text,
        data
    };

    const data_chunk raw = expected.to_data(message::version::level::maximum);
    message::reject instance{};

    BOOST_REQUIRE_EQUAL(false, instance.from_data(
        message::reject::version_minimum - 1, raw));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_chunk)
{
    const message::reject expected
    {
        message::block_message::command,
        message::reject::reason_code::dust,
        reason_text,
        data
    };

    const auto data = expected.to_data(message::version::level::maximum);
    const auto result = message::reject::factory_from_data(
        message::version::level::maximum, data);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_stream)
{
    const message::reject expected
    {
        message::block_message::command,
        message::reject::reason_code::insufficient_fee,
        reason_text,
        data
    };

    const auto data = expected.to_data(message::version::level::maximum);
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    const auto result = message::reject::factory_from_data(
        message::version::level::maximum, istream);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_CASE(roundtrip_to_data_factory_from_data_reader)
{
    const message::reject expected
    {
        message::transaction_message::command,
        message::reject::reason_code::duplicate,
        reason_text,
        data
    };

    const auto data = expected.to_data(message::version::level::maximum);
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    istream_reader source(istream);
    const auto result = message::reject::factory_from_data(
        message::version::level::maximum, source);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(equal(expected, result));
    BOOST_REQUIRE_EQUAL(data.size(),
        result.serialized_size(message::version::level::maximum));
    BOOST_REQUIRE_EQUAL(expected.serialized_size(message::version::level::maximum),
        result.serialized_size(message::version::level::maximum));
}

BOOST_AUTO_TEST_SUITE_END()
