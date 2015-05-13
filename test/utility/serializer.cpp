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

BOOST_AUTO_TEST_SUITE(serializer_tests)

BOOST_AUTO_TEST_CASE(roundtrip_serialize_deserialize)
{
    data_chunk data(1 + 2 + 4 + 8 + 4 + 4 + 3 + 7);
    auto s = make_serializer(data.begin());
    s.write_byte(0x80);
    s.write_2_bytes(0x8040);
    s.write_4_bytes(0x80402010);
    s.write_8_bytes(0x8040201011223344);
    s.write_big_endian<uint32_t>(0x80402010);
    s.write_variable_uint(1234);
    s.write_data(to_data_chunk(to_little_endian<uint32_t>(0xbadf00d)));
    s.write_string("hello");
    auto ds = make_deserializer(data.begin(), s.iterator());
    BOOST_REQUIRE_EQUAL(ds.read_byte(), 0x80u);
    BOOST_REQUIRE_EQUAL(ds.read_2_bytes(), 0x8040u);
    BOOST_REQUIRE_EQUAL(ds.read_4_bytes(), 0x80402010u);
    BOOST_REQUIRE_EQUAL(ds.read_8_bytes(), 0x8040201011223344u);
    BOOST_REQUIRE_EQUAL(ds.read_big_endian<uint32_t>(),  0x80402010u);
    BOOST_REQUIRE_EQUAL(ds.read_variable_uint(), 1234u);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(
        ds.read_data(4).begin()), 0xbadf00du);
    BOOST_REQUIRE_EQUAL(ds.read_string(), "hello");
    BOOST_REQUIRE_THROW(ds.read_byte(), end_of_stream);
}

BOOST_AUTO_TEST_SUITE_END()
