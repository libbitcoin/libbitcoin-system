/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
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
#include <new>
#include <boost/iostreams/stream.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

bool is_failure( std::ios_base::failure const& ex )
{
    return true;
}

BOOST_AUTO_TEST_SUITE(endian_tests)

BOOST_AUTO_TEST_CASE(from_big_endian_stream_unsafe_eof_stream_partial_read)
{
    const uint8_t content = 0xFF;
    const uint32_t expected = static_cast<uint32_t>(content) << ((sizeof(uint32_t) - sizeof(uint8_t)) * 8);

    std::stringstream stream;

    stream.put(content);

    BOOST_REQUIRE(!stream.eof());

    uint32_t value = 0;

    value = from_big_endian_stream_unsafe<uint32_t>(stream);

    BOOST_REQUIRE(expected == value);
    BOOST_REQUIRE(stream.fail());
    BOOST_REQUIRE(stream.eof());
}

BOOST_AUTO_TEST_CASE(from_big_endian_stream_unsafe_insufficient_data_stream_indicates_failure)
{
    data_chunk data = { 0xFF };
    data_source stream(data);

    BOOST_REQUIRE(!stream.eof());

    uint64_t value = from_little_endian_stream_unsafe<uint64_t>(stream);

    BOOST_REQUIRE(!stream);
    BOOST_REQUIRE(stream.eof());
}

BOOST_AUTO_TEST_CASE(from_big_endian_stream_unsafe_valid)
{
    const uint64_t expected = 4234531234;

    std::stringstream stream;

    auto bytes = to_big_endian(expected);
    std::for_each(bytes.begin(), bytes.end(), [&stream](uint8_t& value) { stream.put(value); });

    uint64_t value = from_big_endian_stream_unsafe<uint64_t>(stream);

    BOOST_REQUIRE(stream);
    BOOST_REQUIRE(expected == value);
}

BOOST_AUTO_TEST_CASE(from_little_endian_stream_unsafe_eof_stream_partial_read)
{
    const uint8_t content = 0xFF;
    const uint32_t expected = static_cast<uint32_t>(content);

    std::stringstream stream;

    stream.put(content);

    BOOST_REQUIRE(!stream.eof());

    uint32_t value = 0;

    value = from_little_endian_stream_unsafe<uint32_t>(stream);

    BOOST_REQUIRE(expected == value);
    BOOST_REQUIRE(stream.fail());
    BOOST_REQUIRE(stream.eof());
}

BOOST_AUTO_TEST_CASE(from_little_endian_stream_unsafe_insufficient_data_stream_indicates_failure)
{
    data_chunk data = { 0xFF };
    data_source stream(data);

    BOOST_REQUIRE(!stream.eof());

    uint64_t value = from_little_endian_stream_unsafe<uint64_t>(stream);

    BOOST_REQUIRE(!stream);
    BOOST_REQUIRE(stream.eof());
}

BOOST_AUTO_TEST_CASE(from_little_endian_stream_unsafe_valid)
{
    const uint64_t expected = 4234531234;

    std::stringstream stream;

    auto bytes = to_little_endian(expected);
    std::for_each(bytes.begin(), bytes.end(), [&stream](uint8_t& value) { stream.put(value); });

    uint64_t value = from_little_endian_stream_unsafe<uint64_t>(stream);

    BOOST_REQUIRE(stream);
    BOOST_REQUIRE(expected == value);
}

BOOST_AUTO_TEST_CASE(endian_test_from_format_file)
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

