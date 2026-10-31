/**
 * Copyright (c) 2011-2026 libbitcoin developers
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

BC_AUTO_TEST_SUITE(base_16_tests)

// is_base16

BC_AUTO_TEST_CASE(base16__is_base16__limits__false)
{
    BC_REQUIRE(!is_base16(possible_narrow_and_sign_cast<char>(0)));
    BC_REQUIRE(!is_base16(max_uint8));
}

BC_AUTO_TEST_CASE(base16__is_base16__numbers__true)
{
    BC_REQUIRE(is_base16('0'));
    BC_REQUIRE(is_base16('1'));
    BC_REQUIRE(is_base16('2'));
    BC_REQUIRE(is_base16('3'));
    BC_REQUIRE(is_base16('4'));
    BC_REQUIRE(is_base16('5'));
    BC_REQUIRE(is_base16('6'));
    BC_REQUIRE(is_base16('7'));
    BC_REQUIRE(is_base16('8'));
    BC_REQUIRE(is_base16('9'));
}

BC_AUTO_TEST_CASE(base16__is_base16__lower_case__true)
{
    BC_REQUIRE(is_base16('a'));
    BC_REQUIRE(is_base16('b'));
    BC_REQUIRE(is_base16('c'));
    BC_REQUIRE(is_base16('d'));
    BC_REQUIRE(is_base16('e'));
    BC_REQUIRE(is_base16('f'));
}

BC_AUTO_TEST_CASE(base16__is_base16__upper_case__true)
{
    BC_REQUIRE(is_base16('A'));
    BC_REQUIRE(is_base16('B'));
    BC_REQUIRE(is_base16('C'));
    BC_REQUIRE(is_base16('D'));
    BC_REQUIRE(is_base16('E'));
    BC_REQUIRE(is_base16('F'));
}

BC_AUTO_TEST_CASE(base16__is_base16__boundaries__false)
{
    BC_REQUIRE(!is_base16(possible_narrow_and_sign_cast<char>('0' - 1)));
    BC_REQUIRE(!is_base16(possible_narrow_and_sign_cast<char>('9' + 1)));
    BC_REQUIRE(!is_base16(possible_narrow_and_sign_cast<char>('a' - 1)));
    BC_REQUIRE(!is_base16(possible_narrow_and_sign_cast<char>('f' + 1)));
    BC_REQUIRE(!is_base16(possible_narrow_and_sign_cast<char>('A' - 1)));
    BC_REQUIRE(!is_base16(possible_narrow_and_sign_cast<char>('F' + 1)));
}

// encode_octet

BC_AUTO_TEST_CASE(base16__encode_octet__undefined_behavior__expected)
{
    BC_REQUIRE_EQUAL(encode_octet("x0"), 0x80);
    BC_REQUIRE_EQUAL(encode_octet("0x"), 0x48);
    BC_REQUIRE_EQUAL(encode_octet("xy"), 0xc9);
}

BC_AUTO_TEST_CASE(base16__encode_octet__defined_behavior__expected)
{
    BC_REQUIRE_EQUAL(encode_octet("00"), 0x00);
    BC_REQUIRE_EQUAL(encode_octet("01"), 0x01);
    BC_REQUIRE_EQUAL(encode_octet("42"), 0x42);
    BC_REQUIRE_EQUAL(encode_octet("e5"), 0xe5);
    BC_REQUIRE_EQUAL(encode_octet("ff"), 0xff);
}

// encode_base16 (data_chunk)

BC_AUTO_TEST_CASE(base16__encode_base16_chunk__empty__empty)
{
    const data_chunk value{};
    const auto expected = "";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_base16_chunk__small__expected)
{
    const data_chunk value{ 0xba, 0xad, 0xf0, 0x0d };
    const auto expected = "baadf00d";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_base16_chunk__all_characters__expected)
{
    const data_chunk value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto expected = "0123456789abcdeffedcba9876543210";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

// encode_base16 (data_array)

BC_AUTO_TEST_CASE(base16__encode_base16_array__empty_array__empty)
{
    const data_array<0> value{};
    const auto expected = "";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_base16_array__small_array__expected)
{
    const data_array<4> value{ 0xba, 0xad, 0xf0, 0x0d };
    const auto expected = "baadf00d";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_base16_array__all_characters__expected)
{
    const data_array<16> value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto expected = "0123456789abcdeffedcba9876543210";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_base16_array__null_hash__expected)
{
    const auto& value = null_hash;
    const auto expected = "0000000000000000000000000000000000000000000000000000000000000000";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

// encode_base16 (string)

BC_AUTO_TEST_CASE(base16__encode_base16_string__empty__empty)
{
    const std::string value{};
    const auto expected = "";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_base16_string__foobar__expected)
{
    const std::string value{ "foobar" };
    const auto expected = "666f6f626172";
    BC_REQUIRE_EQUAL(encode_base16(value), expected);
}

// encode_hash (data_array)

BC_AUTO_TEST_CASE(base16__encode_hash_array__empty__empty)
{
    const data_array<0> value{};
    const auto expected = "";
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_hash_array__two_byte__empty)
{
    const data_array<2> value{ 0x42, 0x24 };
    const auto expected = "2442";
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_hash_array__mini_hash__empty)
{
    const mini_hash value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab
    };
    const auto expected = "ab8967452301";
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_hash_array__short_hash__empty)
{
    const short_hash value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x42, 0x42, 0x42, 0x42
    };
    const auto expected = "424242421032547698badcfeefcdab8967452301";
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_hash_array__hash_digest__expected)
{
    const hash_digest value
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10,
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto expected = "1032547698badcfeefcdab89674523011032547698badcfeefcdab8967452301";
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_hash_array__null_hash__expected)
{
    const auto& value = null_hash;
    const auto expected = "0000000000000000000000000000000000000000000000000000000000000000";
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_hash_array__long_hash__expected)
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
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

// encode_hash (data_chunk)

BC_AUTO_TEST_CASE(base16__encode_hash_chunk__empty__empty)
{
    const data_chunk value{};
    const auto expected = "";
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

BC_AUTO_TEST_CASE(base16__encode_hash_chunk__two_byte__empty)
{
    const data_chunk value{ 0x42, 0x24 };
    const auto expected = "2442";
    BC_REQUIRE_EQUAL(encode_hash(value), expected);
}

// decode_base16 (data_chunk)

BC_AUTO_TEST_CASE(base16__decode_base16_chunk__odd_character_count__false)
{
    const auto value = "42abc";
    data_chunk out;
    BC_REQUIRE(!decode_base16(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_base16_chunk__invalid_character__false)
{
    const auto value = "42xabc";
    data_chunk out;
    BC_REQUIRE(!decode_base16(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_base16_chunk__empty__empty)
{
    const data_chunk expected{};
    const auto value = "";
    data_chunk out;
    BC_REQUIRE(decode_base16(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_base16_chunk__small__expected)
{
    const data_chunk expected{ 0xba, 0xad, 0xf0, 0x0d };
    const auto value = "baadf00d";
    data_chunk out;
    BC_REQUIRE(decode_base16(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_base16_chunk__all_characters__expected)
{
    const data_chunk expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto value = "0123456789abcdeffedcba9876543210";
    data_chunk out;
    BC_REQUIRE(decode_base16(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_base16_chunk__null_hash__expected)
{
    const auto& expected = to_chunk(null_hash);
    const auto value = "0000000000000000000000000000000000000000000000000000000000000000";
    data_chunk out;
    BC_REQUIRE(decode_base16(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

// decode_base16 (data_array)

BC_AUTO_TEST_CASE(base16__decode_base16_array__low_character_count__false)
{
    const auto value = "424242";
    data_array<4> out{};
    BC_REQUIRE(!decode_base16(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_base16_array__high_character_count__false)
{
    const auto value = "424242";
    data_array<2> out{};
    BC_REQUIRE(!decode_base16(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_base16_array__odd_character_count__false)
{
    const auto value = "42abc";
    data_array<2> out{};
    BC_REQUIRE(!decode_base16(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_base16_array__invalid_character__false)
{
    const auto value = "42xabc";
    data_array<3> out{};
    BC_REQUIRE(!decode_base16(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_base16_array__empty__empty)
{
    const data_array<0> expected{};
    const auto value = "";
    data_array<0> out{};
    BC_REQUIRE(decode_base16(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_base16_array__small__expected)
{
    const data_array<4> expected{ 0xba, 0xad, 0xf0, 0x0d };
    const auto value = "baadf00d";
    data_array<4> out{};
    BC_REQUIRE(decode_base16(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_base16_array__all_characters__expected)
{
    const data_array<16> expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto value = "0123456789abcdeffedcba9876543210";
    data_array<16> out{};
    BC_REQUIRE(decode_base16(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_base16_array__null_hash__expected)
{
    const auto& expected = null_hash;
    const auto value = "0000000000000000000000000000000000000000000000000000000000000000";
    data_array<32> out{};
    BC_REQUIRE(decode_base16(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

// decode_hash (data_array)

BC_AUTO_TEST_CASE(base16__decode_hash__low_character_count__false)
{
    const auto value = "424242";
    data_array<4> out{};
    BC_REQUIRE(!decode_hash(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_hash__high_character_count__false)
{
    const auto value = "424242";
    data_array<2> out{};
    BC_REQUIRE(!decode_hash(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_hash__odd_character_count__false)
{
    const auto value = "42abc";
    data_array<2> out{};
    BC_REQUIRE(!decode_hash(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_hash__invalid_character__false)
{
    const auto value = "42xabc";
    data_array<3> out{};
    BC_REQUIRE(!decode_hash(out, value));
}

BC_AUTO_TEST_CASE(base16__decode_hash__empty__empty)
{
    const data_array<0> expected{};
    const auto value = "";
    data_array<0> out{};
    BC_REQUIRE(decode_hash(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_hash__small__expected)
{
    const data_array<4> expected{ 0xba, 0xad, 0xf0, 0x0d };
    const auto value = "0df0adba";
    data_array<4> out{};
    BC_REQUIRE(decode_hash(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_hash__all_characters__expected)
{
    const data_array<16> expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    const auto value = "1032547698badcfeefcdab8967452301";
    data_array<16> out{};
    BC_REQUIRE(decode_hash(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(base16__decode_hash__one_hash__expected)
{
    const data_array<32> expected
    {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    const auto value = "0000000000000000000000000000000000000000000000000000000000000001";
    data_array<32> out{};
    BC_REQUIRE(decode_hash(out, value));
    BC_REQUIRE_EQUAL(out, expected);
}

// base16_string

BC_AUTO_TEST_CASE(base16__base16_string__empty__empty)
{
    BC_REQUIRE_EQUAL(base16_string(""), "");
}

BC_AUTO_TEST_CASE(base16__base16_string__foobar__expected)
{
    BC_REQUIRE_EQUAL(base16_string("666f6f626172"), "foobar");
}

BC_AUTO_TEST_CASE(base16__base16_string__invalid_character__empty)
{
    BC_REQUIRE(base16_string("42axcd").empty());
}

// base16_chunk

BC_AUTO_TEST_CASE(base16__base16_chunk__empty__empty)
{
    const data_chunk expected{};
    BC_REQUIRE_EQUAL(base16_chunk(""), expected);
}

BC_AUTO_TEST_CASE(base16__base16_chunk__invalid_character__empty)
{
    BC_REQUIRE(base16_chunk("42axcd").empty());
}

BC_AUTO_TEST_CASE(base16__base16_chunk__even_character_count__expected)
{
    const data_chunk expected{ 0x42, 0xab, 0xcd };
    BC_REQUIRE_EQUAL(base16_chunk("42abcd"), expected);
}

BC_AUTO_TEST_CASE(base16__base16_chunk__all_characters__expected)
{
    const data_chunk expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    BC_REQUIRE_EQUAL(base16_chunk("0123456789abcdeffedcba9876543210"), expected);
}

BC_AUTO_TEST_CASE(base16__base16_chunk__one_hash__expected)
{
    const data_chunk expected
    {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    BC_REQUIRE_EQUAL(base16_chunk("0100000000000000000000000000000000000000000000000000000000000000"), expected);
}

// base16_array

BC_AUTO_TEST_CASE(base16__base16_array__empty__empty)
{
    const data_array<0> expected{};
    BC_REQUIRE_EQUAL(base16_array(""), expected);
}

BC_AUTO_TEST_CASE(base16__base16_array__invalid_character__zeroized)
{
    const data_array<3> expected{ 0x00, 0x00, 0x00 };
    BC_REQUIRE_EQUAL(base16_array("42axcd"), expected);
}

BC_AUTO_TEST_CASE(base16__base16_array__even_character_count__expected)
{
    const data_array<3> expected{ 0x42, 0xab, 0xcd };
    BC_REQUIRE_EQUAL(base16_array("42abcd"), expected);
}

BC_AUTO_TEST_CASE(base16__base16_array__all_characters__expected)
{
    const data_array<16> expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    BC_REQUIRE_EQUAL(base16_array("0123456789abcdeffedcba9876543210"), expected);
}

BC_AUTO_TEST_CASE(base16__base16_array__one_hash__expected)
{
    const data_array<32> expected
    {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    BC_REQUIRE_EQUAL(base16_array("0100000000000000000000000000000000000000000000000000000000000000"), expected);
}

// base16_hash

BC_AUTO_TEST_CASE(base16__base16_hash__empty__empty)
{
    const data_array<0> expected{};
    BC_REQUIRE_EQUAL(base16_hash(""), expected);
}

BC_AUTO_TEST_CASE(base16__base16_hash__invalid_character__zeroized)
{
    const data_array<3> expected{ 0x00, 0x00, 0x00 };
    BC_REQUIRE_EQUAL(base16_hash("42axcd"), expected);
}

BC_AUTO_TEST_CASE(base16__base16_hash__even_character_count__expected)
{
    const data_array<3> expected{ 0x42, 0xab, 0xcd };
    BC_REQUIRE_EQUAL(base16_hash("cdab42"), expected);
}

BC_AUTO_TEST_CASE(base16__base16_hash__all_characters__expected)
{
    const data_array<16> expected
    {
        0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef,
        0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10
    };
    BC_REQUIRE_EQUAL(base16_hash("1032547698badcfeefcdab8967452301"), expected);
}

BC_AUTO_TEST_CASE(base16__base16_hash__one_hash__expected)
{
    const data_array<32> expected
    {
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    BC_REQUIRE_EQUAL(base16_hash("0000000000000000000000000000000000000000000000000000000000000001"), expected);
}

BC_AUTO_TEST_SUITE_END()
