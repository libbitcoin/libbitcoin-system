/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include "../test.hpp"
#include <cstdint>

BOOST_AUTO_TEST_SUITE(checksum_tests)

// insert_checksum

BOOST_AUTO_TEST_CASE(checksum__insert_checksum__empty__expected)
{
    byte_array<checksum_default_size> data;
    insert_checksum(data);
    BOOST_REQUIRE_EQUAL(data[0], 0x5du);
    BOOST_REQUIRE_EQUAL(data[1], 0xf6u);
    BOOST_REQUIRE_EQUAL(data[2], 0xe0u);
    BOOST_REQUIRE_EQUAL(data[3], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__insert_checksum__not_empty__expected)
{
    byte_array<5 + checksum_default_size> data{ 0, 0, 0, 0, 0 };
    insert_checksum(data);
    BOOST_REQUIRE_EQUAL(data[0], 0u);
    BOOST_REQUIRE_EQUAL(data[1], 0u);
    BOOST_REQUIRE_EQUAL(data[2], 0u);
    BOOST_REQUIRE_EQUAL(data[3], 0u);
    BOOST_REQUIRE_EQUAL(data[4], 0u);
    BOOST_REQUIRE_EQUAL(data[5], 0x79u);
    BOOST_REQUIRE_EQUAL(data[6], 0x01u);
    BOOST_REQUIRE_EQUAL(data[7], 0xafu);
    BOOST_REQUIRE_EQUAL(data[8], 0x93u);
}

BOOST_AUTO_TEST_CASE(checksum__insert_checksum_loaf__empty__expected)
{
    byte_array<0> data;
    const auto out = insert_checksum<checksum_default_size>({ { data } });
    BOOST_REQUIRE_EQUAL(out[0], 0x5du);
    BOOST_REQUIRE_EQUAL(out[1], 0xf6u);
    BOOST_REQUIRE_EQUAL(out[2], 0xe0u);
    BOOST_REQUIRE_EQUAL(out[3], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__insert_checksum_loaf__not_empty__expected)
{
    byte_array<5 + checksum_default_size> data{ { 0, 0, 0, 0, 0 } };
    const auto out = insert_checksum<5 + checksum_default_size>({ { data } });
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

BOOST_AUTO_TEST_CASE(checksum__insert_checksum_loaf__maximum_checksum__expected)
{
    constexpr size_t checksum_maximum = 32;
    byte_array<5 + checksum_maximum> data{ { 0, 0, 0, 0, 0 } };
    const auto out = insert_checksum<5 + checksum_maximum, checksum_maximum>({ { data } });
    BOOST_REQUIRE_EQUAL(out[0], 0u);
    BOOST_REQUIRE_EQUAL(out[1], 0u);
    BOOST_REQUIRE_EQUAL(out[2], 0u);
    BOOST_REQUIRE_EQUAL(out[3], 0u);
    BOOST_REQUIRE_EQUAL(out[4], 0u);
    BOOST_REQUIRE_EQUAL(out[5], 0x79u);
    BOOST_REQUIRE_EQUAL(out[36], 0xf3);
}

// append_checksum

BOOST_AUTO_TEST_CASE(checksum__append_checksum__empty__expected_size)
{
    data_chunk data{};
    append_checksum(data);
    BOOST_REQUIRE_EQUAL(data.size(), sizeof(uint32_t));
}

BOOST_AUTO_TEST_CASE(checksum__append_checksum__not_empty__expected_size)
{
    data_chunk data{ 0, 0, 0, 0, 0 };
    const auto size = data.size();
    append_checksum(data);
    BOOST_REQUIRE_EQUAL(data.size(), size + sizeof(uint32_t));
}

BOOST_AUTO_TEST_CASE(checksum__append_checksum__empty__expected)
{
    data_chunk out;
    append_checksum(out);
    BOOST_REQUIRE_EQUAL(out[0], 0x5du);
    BOOST_REQUIRE_EQUAL(out[1], 0xf6u);
    BOOST_REQUIRE_EQUAL(out[2], 0xe0u);
    BOOST_REQUIRE_EQUAL(out[3], 0xe2u);
}

BOOST_AUTO_TEST_CASE(checksum__append_checksum__zeros__expected)
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

BOOST_AUTO_TEST_CASE(checksum__append_checksum_loaf__zeros__expected)
{
    data_chunk data1{ 0, 0 };
    data_chunk data2{ 0, 0, 0 };
    const auto value = append_checksum({ data1 , data2 });
    BOOST_REQUIRE_EQUAL(value[0], 0u);
    BOOST_REQUIRE_EQUAL(value[1], 0u);
    BOOST_REQUIRE_EQUAL(value[2], 0u);
    BOOST_REQUIRE_EQUAL(value[3], 0u);
    BOOST_REQUIRE_EQUAL(value[4], 0u);
    BOOST_REQUIRE_EQUAL(value[5], 0x79u);
    BOOST_REQUIRE_EQUAL(value[6], 0x01u);
    BOOST_REQUIRE_EQUAL(value[7], 0xafu);
    BOOST_REQUIRE_EQUAL(value[8], 0x93u);
}

// verify_checksum<>

BOOST_AUTO_TEST_CASE(checksum__verify_checksum_array__round_trip__true)
{
    byte_array<3 + checksum_default_size> data{ 0, 0, 0 };
    insert_checksum(data);
    BOOST_REQUIRE(verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum_array__invalidated__false)
{
    byte_array<5 + checksum_default_size> data{ 0, 0, 0, 0, 0 };
    insert_checksum(data);
    data[0] = 42;
    BOOST_REQUIRE(!verify_checksum(data));
}

// verify_checksum

BOOST_AUTO_TEST_CASE(checksum__verify_checksum_slice__underflow__false)
{
    static const data_chunk data{ 0, 0, 0 };
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum_slice__not_set__false)
{
    static const data_chunk data{ 0, 0, 0, 0, 0 };
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum_slice__round_trip__true)
{
    data_chunk data{ 0, 0, 0, 0, 0 };
    append_checksum(data);
    BOOST_REQUIRE(verify_checksum(data));
}

BOOST_AUTO_TEST_CASE(checksum__verify_checksum_slice__invalidated__false)
{
    data_chunk data{ 0, 0, 0, 0, 0 };
    append_checksum(data);
    data[0] = 42;
    BOOST_REQUIRE(!verify_checksum(data));
}

BOOST_AUTO_TEST_SUITE_END()
