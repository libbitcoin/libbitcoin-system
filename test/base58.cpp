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
#include <bitcoin/format.hpp>
#include <bitcoin/utility/base58.hpp>

using namespace libbitcoin;

// For some reason boost test seg faults if I use a function.
#define encdec_test(hex, encoded) \
{ \
    data_chunk data = decode_hex(hex); \
    BOOST_REQUIRE(encode_base58(data) == encoded); \
    BOOST_REQUIRE(decode_base58(encoded) == data); \
}

BOOST_AUTO_TEST_CASE(base58_test)
{
    data_chunk pubkey = {
        0x00,
        0x5c, 0xc8, 0x7f, 0x4a, 0x3f, 0xdf, 0xe3, 0xa2, 0x34, 0x6b,
        0x69, 0x53, 0x26, 0x7c, 0xa8, 0x67, 0x28, 0x26, 0x30, 0xd3,
        0xf9, 0xb7, 0x8e, 0x64
    };
    std::string address = "19TbMSWwHvnxAKy12iNm3KdbGfzfaMFViT";
    BOOST_REQUIRE(encode_base58(pubkey) == address);
    BOOST_REQUIRE(decode_base58(address) == pubkey);
    encdec_test("", "");
    encdec_test("61", "2g");
    encdec_test("626262", "a3gV");
    encdec_test("636363", "aPEr");
    encdec_test("73696d706c792061206c6f6e6720737472696e67", "2cFupjhnEsSn59qHXstmK2ffpLv2");
    encdec_test("00eb15231dfceb60925886b67d065299925915aeb172c06647", "1NS17iag9jJgTHD1VXjvLCEnZuQ3rJDE9L");
    encdec_test("516b6fcd0f", "ABnLTmg");
    encdec_test("bf4f89001e670274dd", "3SEo3LWLoPntC");
    encdec_test("572e4794", "3EFU7m");
    encdec_test("ecac89cad93923c02321", "EJDM8drfXA6uyA");
    encdec_test("10c8511e", "Rt5zm");
    encdec_test("00000000000000000000", "1111111111");
}

BOOST_AUTO_TEST_CASE(is_b58)
{
    const std::string base58_chars =
        "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    for (char c: base58_chars)
    {
        BOOST_REQUIRE(is_base58(c));
    }
    const std::string non_base58_chars = "0OIl+- //#";
    for (char c: non_base58_chars)
    {
        BOOST_REQUIRE(!is_base58(c));
    }
    BOOST_REQUIRE(is_base58("abcdjkk11"));
    BOOST_REQUIRE(!is_base58("abcdjkk011"));
}

