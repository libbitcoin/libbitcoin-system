/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(checksum_tests)

// TODO: add append_checksum<> tests.
// TODO: add build_checked_array<> tests.

BOOST_AUTO_TEST_CASE(checksum__append_checksum__size__increased_by_checksum_size)
{
    data_chunk data = { 0, 0, 0, 0, 0 };
    const auto data_size = data.size();
    append_checksum(data);
    BOOST_REQUIRE_EQUAL(data.size(), data_size + checksum_size);
}

BOOST_AUTO_TEST_CASE(checksum__append_checksum__empty__valid)
{
    data_chunk data = {};
    auto checksum = data.size();
    append_checksum(data);
    BOOST_REQUIRE_EQUAL(data[checksum++], 0x5du);
    BOOST_REQUIRE_EQUAL(data[checksum++], 0xf6u);
    BOOST_REQUIRE_EQUAL(data[checksum++], 0xe0u);
    BOOST_REQUIRE_EQUAL(data[checksum++], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__append_checksum__not_empty__valid)
{
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto checksum = data.size();
    append_checksum(data);
    BOOST_REQUIRE_EQUAL(data[checksum++], 0x79u);
    BOOST_REQUIRE_EQUAL(data[checksum++], 0x01u);
    BOOST_REQUIRE_EQUAL(data[checksum++], 0xafu);
    BOOST_REQUIRE_EQUAL(data[checksum++], 0x93u);
}

BOOST_AUTO_TEST_CASE(checksum__bitcoin_checksum__always__valid)
{
    data_chunk data = { 0, 0, 0, 0, 0 };
    const auto result = bitcoin_checksum(data);
    BOOST_REQUIRE_EQUAL(result, 0x93af0179u);
}

BOOST_AUTO_TEST_CASE(checksum__build_checked_array__empty__valid)
{
    data_chunk data = {};
    auto checksum = data.size();
    byte_array<checksum_size> out;
    const auto result = build_checked_array(out,
    {
        data
    });
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0x5du);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0xf6u);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0xe0u);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__build_checked_array__not_empty__valid)
{
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto checksum = data.size();
    byte_array<checksum_size + 5> out;
    const auto result = build_checked_array(out,
    {
        data
    });
    BOOST_REQUIRE(result);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0x79u);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0x01u);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0xafu);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0x93u);
}

BOOST_AUTO_TEST_CASE(checksum__build_checked_array__overflow__false)
{
    data_chunk data = { 0, 0, 0, 0, 0 };
    auto checksum = data.size();
    byte_array<checksum_size> out;
    const auto result = build_checked_array(out,
    {
        data
    });
    BOOST_REQUIRE(!result);
}

BOOST_AUTO_TEST_CASE(checksum__insert_checksum__empty__valid)
{
    data_chunk data = {};
    auto checksum = data.size();
    byte_array<checksum_size> out;
    BOOST_REQUIRE(insert_checksum(out));
    BOOST_REQUIRE_EQUAL(out[checksum++], 0x5du);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0xf6u);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0xe0u);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__insert_checksum__not_empty__valid)
{
    byte_array<checksum_size + 5> out{ { 0, 0, 0, 0, 0 } };
    auto checksum = out.size() - checksum_size;
    BOOST_REQUIRE(insert_checksum(out));
    BOOST_REQUIRE_EQUAL(out[checksum++], 0x79u);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0x01u);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0xafu);
    BOOST_REQUIRE_EQUAL(out[checksum++], 0x93u);
}

BOOST_AUTO_TEST_CASE(checksum__insert_checksum__underflow__false)
{
    byte_array<checksum_size - 1> out;
    BOOST_REQUIRE(!insert_checksum(out));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum__underflow__false)
{
    const data_chunk data = { 0, 0, 0 };
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum__not_set__false)
{
    const data_chunk data = { 0, 0, 0, 0, 0 };
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum__added__true)
{
    data_chunk data = { 0, 0, 0, 0, 0 };
    append_checksum(data);
    BOOST_REQUIRE(verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum__invalidated__false)
{
    data_chunk data = { 0, 0, 0, 0, 0 };
    const auto data_size = data.size();
    append_checksum(data);
    data[data_size] = 42;
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_SUITE_END()
