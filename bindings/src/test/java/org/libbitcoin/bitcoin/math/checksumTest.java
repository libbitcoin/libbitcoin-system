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
package org.libbitcoin.math;

//BOOST_AUTO_TEST_SUITE(checksum_tests)

// TODO: add append_checksum<> tests.
// TODO: add build_checked_array<> tests.
import static org.libbitcoin.math.math.append_checksum;

class cheksumTest {
	
	public static void checksum__append_checksum__size__increased_by_checksum_size()
	{
		ucharVector data = new ucharVector(5);
		for (int i=0; i<data.size(); i++) {
			data.set(i,0);
		}
		int data_size = data.size();
		append_checksum(data);
		assertEquals(data.size(), data_size + getChecksum_size());
	}
	
	public static void checksum__append_checksum__empty__valid()
	{
//		data_chunk data = {};
//		auto checksum = data.size();
//		append_checksum(data);
//		assertEquals(data[checksum++], 0x5du);
//		assertEquals(data[checksum++], 0xf6u);
//		assertEquals(data[checksum++], 0xe0u);
//		assertEquals(data[checksum++], 0xe2u);
	}
	
	public static void checksum__append_checksum__not_empty__valid()
	{
//		data_chunk data = { 0, 0, 0, 0, 0 };
//		auto checksum = data.size();
//		append_checksum(data);
//		assertEquals(data[checksum++], 0x79u);
//		assertEquals(data[checksum++], 0x01u);
//		assertEquals(data[checksum++], 0xafu);
//		assertEquals(data[checksum++], 0x93u);
	}
	
	public static void checksum__bitcoin_checksum__always__valid()
	{
//		data_chunk data = { 0, 0, 0, 0, 0 };
//		const auto result = bitcoin_checksum(data);
//		assertEquals(result, 0x93af0179u);
	}
	
	public static void checksum__build_checked_array__empty__valid()
	{
//		data_chunk data = {};
//		auto checksum = data.size();
//		byte_array<checksum_size> out;
//		const auto result = build_checked_array(out,
//		{
//			data
//		});
//		BOOST_REQUIRE(result);
//		assertEquals(out[checksum++], 0x5du);
//		assertEquals(out[checksum++], 0xf6u);
//		assertEquals(out[checksum++], 0xe0u);
//		assertEquals(out[checksum++], 0xe2u);
	}
	
	public static void checksum__build_checked_array__not_empty__valid()
	{
//		data_chunk data = { 0, 0, 0, 0, 0 };
//		auto checksum = data.size();
//		byte_array<checksum_size + 5> out;
//		const auto result = build_checked_array(out,
//		{
//			data
//		});
//		BOOST_REQUIRE(result);
//		assertEquals(out[checksum++], 0x79u);
//		assertEquals(out[checksum++], 0x01u);
//		assertEquals(out[checksum++], 0xafu);
//		assertEquals(out[checksum++], 0x93u);
	}
	
	public static void checksum__build_checked_array__overflow__false()
	{
//		data_chunk data = { 0, 0, 0, 0, 0 };
//		auto checksum = data.size();
//		byte_array<checksum_size> out;
//		const auto result = build_checked_array(out,
//		{
//			data
//		});
//		BOOST_REQUIRE(!result);
	}
	
	public static void checksum__insert_checksum__empty__valid()
	{
//		data_chunk data = {};
//		auto checksum = data.size();
//		byte_array<checksum_size> out;
//		BOOST_REQUIRE(insert_checksum(out));
//		assertEquals(out[checksum++], 0x5du);
//		assertEquals(out[checksum++], 0xf6u);
//		assertEquals(out[checksum++], 0xe0u);
//		assertEquals(out[checksum++], 0xe2u);
	}
	
	public static void checksum__insert_checksum__not_empty__valid()
	{
//		byte_array<checksum_size + 5> out{ { 0, 0, 0, 0, 0 } };
//		auto checksum = out.size() - checksum_size;
//		BOOST_REQUIRE(insert_checksum(out));
//		assertEquals(out[checksum++], 0x79u);
//		assertEquals(out[checksum++], 0x01u);
//		assertEquals(out[checksum++], 0xafu);
//		assertEquals(out[checksum++], 0x93u);
	}
	
	public static void checksum__insert_checksum__underflow__false()
	{
//		byte_array<checksum_size - 1> out;
//		BOOST_REQUIRE(!insert_checksum(out));
	}
	
	public static void checksum__verify_checksum__underflow__false()
	{
//		const data_chunk data = { 0, 0, 0 };
//		BOOST_REQUIRE(!verify_checksum(data));
	}
	
	public static void checksum__verify_checksum__not_set__false()
	{
//		const data_chunk data = { 0, 0, 0, 0, 0 };
//		BOOST_REQUIRE(!verify_checksum(data));
	}
	
	public static void checksum__verify_checksum__added__true()
	{
//		data_chunk data = { 0, 0, 0, 0, 0 };
//		append_checksum(data);
//		BOOST_REQUIRE(verify_checksum(data));
	}
	
	public static void checksum__verify_checksum__invalidated__false()
	{
//		data_chunk data = { 0, 0, 0, 0, 0 };
//		const auto data_size = data.size();
//		append_checksum(data);
//		data[data_size] = 42;
//		BOOST_REQUIRE(!verify_checksum(data));
	}
	
	public static void main(String[] args) {
	}
	
}