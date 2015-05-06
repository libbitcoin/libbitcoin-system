/*
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
#include <sstream>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

bool is_failure( std::ios_base::failure const& ex )
{
    return true;
}

BOOST_AUTO_TEST_SUITE(endian_tests)

//BOOST_AUTO_TEST_CASE(failbit_stickyness_test)
//{
//    const uint8_t content = 0xFF;
//    const uint32_t expected = static_cast<uint32_t>(content) << ((sizeof(uint32_t) - sizeof(uint8_t)) * 8);
//
//    std::stringstream stream;
//
//    stream.put(content);
//
//    BOOST_REQUIRE(!stream.eof());
//
//    uint32_t value = 0;
//
//    value = from_big_endian_stream_unsafe<uint32_t>(stream);
//
//    BOOST_REQUIRE(expected == value);
//    BOOST_REQUIRE(stream.fail());
//    BOOST_REQUIRE(stream.eof());
//    BOOST_REQUIRE(!stream.bad());
//
//    stream.put(content);
//    BOOST_REQUIRE(!stream.bad());
//
//    uint8_t result = 0xFA;
//    stream.readsome(reinterpret_cast<char*>(&result), sizeof result);
//
//    BOOST_REQUIRE(!stream.bad());
//    BOOST_REQUIRE(stream.fail());
//    BOOST_REQUIRE(stream.eof());
//
//    std::cout << "result: " << result << std::endl;
//    BOOST_REQUIRE(result == 0xFA);
//}

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

BOOST_AUTO_TEST_CASE(from_big_endian_stream_unsafe_eof_stream_throw_on_fail)
{
    std::stringstream stream;

    stream.put(0xFF);

    stream.exceptions(std::stringstream::failbit);

    BOOST_REQUIRE(!stream.eof());

    uint64_t value = 0;

    BOOST_REQUIRE_EXCEPTION(value = from_big_endian_stream_unsafe<uint64_t>(stream),
        std::ios_base::failure,
        is_failure);

    BOOST_REQUIRE(stream.eof());
}

BOOST_AUTO_TEST_CASE(from_big_endian_stream_unsafe_valid)
{
    const uint64_t expected = 4234531234;

    std::stringstream stream;

    auto bytes = to_big_endian(expected);
    std::for_each(bytes.begin(), bytes.end(), [&stream](uint8_t& value) { stream.put(value); });

    uint64_t value = from_big_endian_stream_unsafe<uint64_t>(stream);

    BOOST_REQUIRE(stream.good());
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

BOOST_AUTO_TEST_CASE(from_little_endian_stream_unsafe_eof_stream_throw_on_fail)
{
    std::stringstream stream;

    stream.put(0xFF);

    stream.exceptions(std::stringstream::failbit);

    BOOST_REQUIRE(!stream.eof());

    uint64_t value = 0;

    BOOST_REQUIRE_EXCEPTION(value = from_little_endian_stream_unsafe<uint64_t>(stream),
        std::ios_base::failure,
        is_failure);

    BOOST_REQUIRE(stream.eof());
}

BOOST_AUTO_TEST_CASE(from_little_endian_stream_unsafe_valid)
{
    const uint64_t expected = 4234531234;

    std::stringstream stream;

    auto bytes = to_little_endian(expected);
    std::for_each(bytes.begin(), bytes.end(), [&stream](uint8_t& value) { stream.put(value); });

    uint64_t value = from_little_endian_stream_unsafe<uint64_t>(stream);

    BOOST_REQUIRE(stream.good());
    BOOST_REQUIRE(expected == value);
}

BOOST_AUTO_TEST_SUITE_END()

