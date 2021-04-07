/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system.hpp>

using namespace bc;
using namespace bc::system;

BOOST_AUTO_TEST_SUITE(checksum_tests)

// TODO: add append_checksum<> tests.
// TODO: add build_checked_array<> tests.

BOOST_AUTO_TEST_CASE(checksum__log2__0__0)
{
    BOOST_REQUIRE_EQUAL(system::log2(0), 0u);
}

BOOST_AUTO_TEST_CASE(checksum__log2__maximums__sizeof_maximum_minus_one)
{
    BOOST_REQUIRE_EQUAL(system::log2(max_uint8), sizeof(uint8_t) * byte_bits - 1u);
    BOOST_REQUIRE_EQUAL(system::log2(max_uint16), sizeof(uint16_t) * byte_bits - 1u);
    BOOST_REQUIRE_EQUAL(system::log2(max_uint32), sizeof(uint32_t) * byte_bits - 1u);
}

BOOST_AUTO_TEST_CASE(checksum__log2__powers_of_2__expected)
{
    BOOST_REQUIRE_EQUAL(system::log2(1u << 0), 0u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 1), 1u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 2), 2u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 3), 3u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 4), 4u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 5), 5u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 6), 6u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 7), 7u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 8), 8u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 9), 9u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 10), 10u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 11), 11u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 12), 12u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 13), 13u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 14), 14u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 15), 15u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 16), 16u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 17), 17u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 18), 18u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 19), 19u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 20), 20u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 21), 21u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 22), 22u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 23), 23u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 24), 24u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 25), 25u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 26), 26u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 27), 27u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 28), 28u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 29), 29u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 30), 30u);
    BOOST_REQUIRE_EQUAL(system::log2(1u << 31), 31u);
}

BOOST_AUTO_TEST_CASE(checksum__append_checksum__size__increased_by_checksum_size)
{
    data_chunk data{ 0, 0, 0, 0, 0 };
    append_checksum(data);
    BOOST_REQUIRE_EQUAL(data.size(), 5u + checksum_size);
}

BOOST_AUTO_TEST_CASE(checksum__append_checksum__empty__valid)
{
    data_chunk out;
    append_checksum(out);
    BOOST_REQUIRE_EQUAL(out[0], 0x5du);
    BOOST_REQUIRE_EQUAL(out[1], 0xf6u);
    BOOST_REQUIRE_EQUAL(out[2], 0xe0u);
    BOOST_REQUIRE_EQUAL(out[3], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__append_checksum__not_empty__valid)
{
    data_chunk out{ 0, 0, 0, 0, 0 };
    append_checksum(out);
    BOOST_REQUIRE_EQUAL(out[0], 0u);
    BOOST_REQUIRE_EQUAL(out[1], 0u);
    BOOST_REQUIRE_EQUAL(out[2], 0u);
    BOOST_REQUIRE_EQUAL(out[3], 0u);
    BOOST_REQUIRE_EQUAL(out[4], 0u);
    BOOST_REQUIRE_EQUAL(out[5], 0x79u);
    BOOST_REQUIRE_EQUAL(out[6], 0x01u);
    BOOST_REQUIRE_EQUAL(out[7], 0xafu);
    BOOST_REQUIRE_EQUAL(out[8], 0x93u);
}

BOOST_AUTO_TEST_CASE(checksum__bitcoin_checksum__always__valid)
{
    data_chunk data{ 0, 0, 0, 0, 0 };
    const auto result = bitcoin_checksum(data);
    BOOST_REQUIRE_EQUAL(result, 0x93af0179u);
}

BOOST_AUTO_TEST_CASE(checksum__build_checked_array__empty__valid)
{
    byte_array<0> data;
    const auto out = build_checked_array<checksum_size>(
    {
        data
    });
    BOOST_REQUIRE_EQUAL(out[0], 0x5du);
    BOOST_REQUIRE_EQUAL(out[1], 0xf6u);
    BOOST_REQUIRE_EQUAL(out[2], 0xe0u);
    BOOST_REQUIRE_EQUAL(out[3], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__build_checked_array__not_empty__valid)
{
    byte_array<5> data{ { 0, 0, 0, 0, 0 } };
    const auto out = build_checked_array<5 + checksum_size>(
    {
        data
    });
    BOOST_REQUIRE_EQUAL(out[0], 0u);
    BOOST_REQUIRE_EQUAL(out[1], 0u);
    BOOST_REQUIRE_EQUAL(out[2], 0u);
    BOOST_REQUIRE_EQUAL(out[3], 0u);
    BOOST_REQUIRE_EQUAL(out[4], 0u);
    BOOST_REQUIRE_EQUAL(out[5], 0x79u);
    BOOST_REQUIRE_EQUAL(out[6], 0x01u);
    BOOST_REQUIRE_EQUAL(out[7], 0xafu);
    BOOST_REQUIRE_EQUAL(out[8], 0x93u);
}

BOOST_AUTO_TEST_CASE(checksum__insert_checksum__empty__valid)
{
    byte_array<0> data;
    auto out = build_checked_array<checksum_size>(
    {
        data
    });
    insert_checksum(out);
    BOOST_REQUIRE_EQUAL(out[0], 0x5du);
    BOOST_REQUIRE_EQUAL(out[1], 0xf6u);
    BOOST_REQUIRE_EQUAL(out[2], 0xe0u);
    BOOST_REQUIRE_EQUAL(out[3], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__insert_checksum__not_empty__valid)
{
    byte_array<5> data{ { 0, 0, 0, 0, 0 } };
    auto out = build_checked_array<5 + checksum_size>(
    {
        data
    });
    insert_checksum(out);
    BOOST_REQUIRE_EQUAL(out[0], 0u);
    BOOST_REQUIRE_EQUAL(out[1], 0u);
    BOOST_REQUIRE_EQUAL(out[2], 0u);
    BOOST_REQUIRE_EQUAL(out[3], 0u);
    BOOST_REQUIRE_EQUAL(out[4], 0u);
    BOOST_REQUIRE_EQUAL(out[5], 0x79u);
    BOOST_REQUIRE_EQUAL(out[6], 0x01u);
    BOOST_REQUIRE_EQUAL(out[7], 0xafu);
    BOOST_REQUIRE_EQUAL(out[8], 0x93u);
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum__underflow__false)
{
    static const data_chunk data{ 0, 0, 0 };
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum__not_set__false)
{
    static const data_chunk data{ 0, 0, 0, 0, 0 };
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum__added__true)
{
    data_chunk data{ 0, 0, 0, 0, 0 };
    append_checksum(data);
    BOOST_REQUIRE(verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum__invalidated__false)
{
    data_chunk data{ 0, 0, 0, 0, 0 };
    const auto data_size = data.size();
    append_checksum(data);
    data[data_size] = 42;
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_SUITE_END()
