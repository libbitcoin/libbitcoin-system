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
#include "../overloads.hpp"

using namespace bc;
using namespace bc::system;

BOOST_AUTO_TEST_SUITE(base_16_tests)

// is_base16

BOOST_AUTO_TEST_CASE(base16__is_base16__limits__false)
{
    BOOST_REQUIRE(!is_base16(0));
    BOOST_REQUIRE(!is_base16(max_uint8));
}

BOOST_AUTO_TEST_CASE(base16__is_base16__numbers__true)
{
    BOOST_REQUIRE(is_base16('0'));
    BOOST_REQUIRE(is_base16('1'));
    BOOST_REQUIRE(is_base16('2'));
    BOOST_REQUIRE(is_base16('3'));
    BOOST_REQUIRE(is_base16('4'));
    BOOST_REQUIRE(is_base16('5'));
    BOOST_REQUIRE(is_base16('6'));
    BOOST_REQUIRE(is_base16('7'));
    BOOST_REQUIRE(is_base16('8'));
    BOOST_REQUIRE(is_base16('9'));
}

BOOST_AUTO_TEST_CASE(base16__is_base16__lower_case__true)
{
    BOOST_REQUIRE(is_base16('a'));
    BOOST_REQUIRE(is_base16('b'));
    BOOST_REQUIRE(is_base16('c'));
    BOOST_REQUIRE(is_base16('d'));
    BOOST_REQUIRE(is_base16('e'));
    BOOST_REQUIRE(is_base16('f'));
}

BOOST_AUTO_TEST_CASE(base16__is_base16__upper_case__true)
{
    BOOST_REQUIRE(is_base16('A'));
    BOOST_REQUIRE(is_base16('B'));
    BOOST_REQUIRE(is_base16('C'));
    BOOST_REQUIRE(is_base16('D'));
    BOOST_REQUIRE(is_base16('E'));
    BOOST_REQUIRE(is_base16('F'));
}

BOOST_AUTO_TEST_CASE(base16__is_base16__boundaries__false)
{
    BOOST_REQUIRE(!is_base16('0' - 1));
    BOOST_REQUIRE(!is_base16('9' + 1));
    BOOST_REQUIRE(!is_base16('a' - 1));
    BOOST_REQUIRE(!is_base16('f' + 1));
    BOOST_REQUIRE(!is_base16('A' - 1));
    BOOST_REQUIRE(!is_base16('F' + 1));
}

// encode_base16(data_chunk)

BOOST_AUTO_TEST_CASE(base16__encode_base16__empty_chunk__empty)
{
    const data_chunk value{};
    const auto expected = "";
    BOOST_REQUIRE_EQUAL(encode_base16(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_base16__small_chunk__expected)
{
    const data_chunk value{ 0xba, 0xad, 0xf0, 0x0d };
    const auto expected = "baadf00d";
    BOOST_REQUIRE_EQUAL(encode_base16(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_base16__all_characters_chunk__expected)
{
    const data_chunk value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto expected = "0123456789abcdeffedcba9876543210";
    BOOST_REQUIRE_EQUAL(encode_base16(value), expected);
}

// encode_base16(byte_array)

BOOST_AUTO_TEST_CASE(base16__encode_base16__empty_array__empty)
{
    const byte_array<0> value{};
    const auto expected = "";
    BOOST_REQUIRE_EQUAL(encode_base16(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_base16__small_array__expected)
{
    const byte_array<4> value{ 0xba, 0xad, 0xf0, 0x0d };
    const auto expected = "baadf00d";
    BOOST_REQUIRE_EQUAL(encode_base16(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_base16__all_characters_array__expected)
{
    const byte_array<16> value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto expected = "0123456789abcdeffedcba9876543210";
    BOOST_REQUIRE_EQUAL(encode_base16(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_base16__null_hash__expected)
{
    const auto& value = null_hash;
    const auto expected = "0000000000000000000000000000000000000000000000000000000000000000";
    BOOST_REQUIRE_EQUAL(encode_base16(value), expected);
}

// decode_base16

BOOST_AUTO_TEST_CASE(base16__decode_base16__odd_character_count__false)
{
    const auto value = "42abc";
    data_chunk out;
    BOOST_REQUIRE(!decode_base16(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_base16__invalid_character__false)
{
    const auto value = "42xabc";
    data_chunk out;
    BOOST_REQUIRE(!decode_base16(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_base16__empty_array__empty)
{
    const data_chunk expected{};
    const auto value = "";
    data_chunk out;
    BOOST_REQUIRE(decode_base16(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_base16__small_array__expected)
{
    const data_chunk expected{ 0xba, 0xad, 0xf0, 0x0d };
    const auto value = "baadf00d";
    data_chunk out;
    BOOST_REQUIRE(decode_base16(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_base16__all_characters_array__expected)
{
    const data_chunk expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto value = "0123456789abcdeffedcba9876543210";
    data_chunk out;
    BOOST_REQUIRE(decode_base16(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_base16__null_hash__expected)
{
    const auto& expected = to_chunk(null_hash);
    const auto value = "0000000000000000000000000000000000000000000000000000000000000000";
    data_chunk out;
    BOOST_REQUIRE(decode_base16(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

// decode_base16<Size>

BOOST_AUTO_TEST_CASE(base16__decode_base16_array__low_character_count__false)
{
    const auto value = "424242";
    byte_array<4> out;
    BOOST_REQUIRE(!decode_base16(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_base16_array__high_character_count__false)
{
    const auto value = "424242";
    byte_array<2> out;
    BOOST_REQUIRE(!decode_base16(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_base16_array__odd_character_count__false)
{
    const auto value = "42abc";
    byte_array<2> out;
    BOOST_REQUIRE(!decode_base16(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_base16_array__invalid_character__false)
{
    const auto value = "42xabc";
    byte_array<3> out;
    BOOST_REQUIRE(!decode_base16(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_base16_array__empty__empty)
{
    const byte_array<0> expected{};
    const auto value = "";
    byte_array<0> out;
    BOOST_REQUIRE(decode_base16(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_base16_array__small__expected)
{
    const byte_array<4> expected{ 0xba, 0xad, 0xf0, 0x0d };
    const auto value = "baadf00d";
    byte_array<4> out;
    BOOST_REQUIRE(decode_base16(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_base16_array__all_characters__expected)
{
    const byte_array<16> expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto value = "0123456789abcdeffedcba9876543210";
    byte_array<16> out;
    BOOST_REQUIRE(decode_base16(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_base16_array__null_hash__expected)
{
    const auto& expected = to_array<32>(null_hash);
    const auto value = "0000000000000000000000000000000000000000000000000000000000000000";
    byte_array<32> out;
    BOOST_REQUIRE(decode_base16(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

// encode_hash

BOOST_AUTO_TEST_CASE(base16__encode_hash__empty__empty)
{
    const byte_array<0> value{};
    const auto expected = "";
    BOOST_REQUIRE_EQUAL(encode_hash(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_hash__one_byte__empty)
{
    const byte_array<1> value{ 0x42 };
    const auto expected = "42";
    BOOST_REQUIRE_EQUAL(encode_hash(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_hash__mini_hash__empty)
{
    const mini_hash value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab
    };
    const auto expected = "ab8967452301";
    BOOST_REQUIRE_EQUAL(encode_hash(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_hash__short_hash__empty)
{
    const short_hash value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x42, 0x42, 0x42, 0x42
    };
    const auto expected = "424242421032547698badcfeefcdab8967452301";
    BOOST_REQUIRE_EQUAL(encode_hash(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_hash__hash_digest__expected)
{
    const hash_digest value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto expected = "1032547698badcfeefcdab89674523011032547698badcfeefcdab8967452301";
    BOOST_REQUIRE_EQUAL(encode_hash(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_hash__null_hash__expected)
{
    const auto& value = null_hash;
    const auto expected = "0000000000000000000000000000000000000000000000000000000000000000";
    BOOST_REQUIRE_EQUAL(encode_hash(value), expected);
}

BOOST_AUTO_TEST_CASE(base16__encode_hash__long_hash__expected)
{
    const long_hash value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto expected = "1032547698badcfeefcdab89674523011032547698badcfeefcdab89674523011032547698badcfeefcdab89674523011032547698badcfeefcdab8967452301";
    BOOST_REQUIRE_EQUAL(encode_hash(value), expected);
}

// decode_hash

BOOST_AUTO_TEST_CASE(base16__decode_hash__low_character_count__false)
{
    const auto value = "424242";
    byte_array<4> out;
    BOOST_REQUIRE(!decode_hash(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_hash__high_character_count__false)
{
    const auto value = "424242";
    byte_array<2> out;
    BOOST_REQUIRE(!decode_hash(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_hash__odd_character_count__false)
{
    const auto value = "42abc";
    byte_array<2> out;
    BOOST_REQUIRE(!decode_hash(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_hash__invalid_character__false)
{
    const auto value = "42xabc";
    byte_array<3> out;
    BOOST_REQUIRE(!decode_hash(out, value));
}

BOOST_AUTO_TEST_CASE(base16__decode_hash__empty__empty)
{
    const byte_array<0> expected{};
    const auto value = "";
    byte_array<0> out;
    BOOST_REQUIRE(decode_hash(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_hash__small__expected)
{
    const byte_array<4> expected{ 0xba, 0xad, 0xf0, 0x0d };
    const auto value = "0df0adba";
    byte_array<4> out;
    BOOST_REQUIRE(decode_hash(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_hash__all_characters__expected)
{
    const byte_array<16> expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto value = "1032547698badcfeefcdab8967452301";
    byte_array<16> out;
    BOOST_REQUIRE(decode_hash(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(base16__decode_hash__one_hash__expected)
{
    const byte_array<32> expected
    {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    const auto value = "0000000000000000000000000000000000000000000000000000000000000001";
    byte_array<32> out;
    BOOST_REQUIRE(decode_hash(out, value));
    BOOST_REQUIRE_EQUAL(out, expected);
}

// base16_literal

BOOST_AUTO_TEST_CASE(base16__base16_literal__empty__empty)
{
    const byte_array<0> expected{};
    BOOST_REQUIRE_EQUAL(base16_literal(""), expected);
}

BOOST_AUTO_TEST_CASE(base16__base16_literal__odd_character_count__zeroized)
{
    const byte_array<3> expected{ 0x00, 0x00, 0x00 };
    BOOST_REQUIRE_EQUAL(base16_literal("424242a"), expected);
}

BOOST_AUTO_TEST_CASE(base16__base16_literal__invalid_character__zeroized)
{
    const byte_array<3> expected{ 0x00, 0x00, 0x00 };
    BOOST_REQUIRE_EQUAL(base16_literal("42axcd"), expected);
}

BOOST_AUTO_TEST_CASE(base16__base16_literal__even_character_count__expected)
{
    const byte_array<3> expected{ 0x42, 0xab, 0xcd };
    BOOST_REQUIRE_EQUAL(base16_literal("42abcd"), expected);
}

BOOST_AUTO_TEST_CASE(base16__base16_literal__all_characters__expected)
{
    const byte_array<16> expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    BOOST_REQUIRE_EQUAL(base16_literal("0123456789abcdeffedcba9876543210"), expected);
}

BOOST_AUTO_TEST_CASE(base16__base16_literal__one_hash__expected)
{
    const byte_array<32> expected
    {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    BOOST_REQUIRE_EQUAL(base16_literal("0100000000000000000000000000000000000000000000000000000000000000"), expected);
}

// hash_literal

BOOST_AUTO_TEST_CASE(base16__hash_literal__empty__empty)
{
    const byte_array<0> expected{};
    BOOST_REQUIRE_EQUAL(hash_literal(""), expected);
}

BOOST_AUTO_TEST_CASE(base16__hash_literal__odd_character_count__zeroized)
{
    const byte_array<3> expected{ 0x00, 0x00, 0x00 };
    BOOST_REQUIRE_EQUAL(hash_literal("424242a"), expected);
}

BOOST_AUTO_TEST_CASE(base16__hash_literal__invalid_character__zeroized)
{
    const byte_array<3> expected{ 0x00, 0x00, 0x00 };
    BOOST_REQUIRE_EQUAL(hash_literal("42axcd"), expected);
}

BOOST_AUTO_TEST_CASE(base16__hash_literal__even_character_count__expected)
{
    const byte_array<3> expected{ 0x42, 0xab, 0xcd };
    BOOST_REQUIRE_EQUAL(hash_literal("cdab42"), expected);
}

BOOST_AUTO_TEST_CASE(base16__hash_literal__all_characters__expected)
{
    const byte_array<16> expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    BOOST_REQUIRE_EQUAL(hash_literal("1032547698badcfeefcdab8967452301"), expected);
}

BOOST_AUTO_TEST_CASE(base16__hash_literal__one_hash__expected)
{
    const byte_array<32> expected
    {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    BOOST_REQUIRE_EQUAL(hash_literal("0000000000000000000000000000000000000000000000000000000000000001"), expected);
}

BOOST_AUTO_TEST_SUITE_END()
