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
#include <sstream>

BC_AUTO_TEST_SUITE(deserialize_tests)

// <string> is overloaded to avoid trimming (straight pass through).

BC_AUTO_TEST_CASE(deserialize__string__empty__unchanged)
{
    std::string out;
    const auto value = "";
    BC_REQUIRE(deserialize(out, value));
    BC_REQUIRE_EQUAL(out, value);
}

BC_AUTO_TEST_CASE(deserialize__string__text__unchanged)
{
    std::string out;
    const auto value = "01234 567890";
    BC_REQUIRE(deserialize(out, value));
    BC_REQUIRE_EQUAL(out, value);
}

BC_AUTO_TEST_CASE(deserialize__string__text_padded__untrimmed)
{
    std::string out;
    const auto value = " \r\t\n\v01234 567890\r\t\n\v ";
    BC_REQUIRE(deserialize(out, value));
    BC_REQUIRE_EQUAL(out, value);
}

BC_AUTO_TEST_CASE(deserialize__string__istream_padded__untrimmed)
{
    std::string out;
    const auto value = " \r\t\n\v01234 567890\r\t\n\v ";
    std::istringstream in{ value };
    BC_REQUIRE(deserialize(out, in));
    BC_REQUIRE_EQUAL(out, value);
}

// <uint8_t>

// The template specialization must be observed before the template
// or the specialization will be lost and this will return char value.
BC_AUTO_TEST_CASE(deserialize__uint8__empty__false)
{
    uint8_t out;
    BC_REQUIRE(!deserialize(out, ""));
}

BC_AUTO_TEST_CASE(deserialize__uint8__uint8__base10)
{
    uint8_t out;
    BC_REQUIRE(deserialize(out, "0"));
    BC_REQUIRE_EQUAL(out, 0u);
    BC_REQUIRE(deserialize(out, "255"));
    BC_REQUIRE_EQUAL(out, 0xff);
    BC_REQUIRE(!deserialize(out, "256"));
}

BC_AUTO_TEST_CASE(deserialize__uint8__uchar__base10)
{
    unsigned char out;
    BC_REQUIRE(deserialize(out, "0"));
    BC_REQUIRE_EQUAL(out, 0u);
    BC_REQUIRE(deserialize(out, "255"));
    BC_REQUIRE_EQUAL(out, 0xff);
}

BC_AUTO_TEST_CASE(deserialize__uint8__istream__base10)
{
    uint8_t out;
    std::istringstream in{ "255" };
    BC_REQUIRE(deserialize(out, in));
    BC_REQUIRE_EQUAL(out, 0xff);
}

// <data_array<Size>>

BC_AUTO_TEST_CASE(deserialize__data_array__empty__empty)
{
    std_array<uint8_t, 0> out;
    const std_array<uint8_t, 0> expected{};
    BC_REQUIRE(deserialize(out, ""));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(deserialize__data_array__bytes__base16)
{
    std_array<uint8_t, 3> out;
    const std_array<uint8_t, 3> expected{ { 7, 42, 11 } };
    BC_REQUIRE(deserialize(out, "072a0b"));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(deserialize__data_array__istream__base16)
{
    std_array<uint8_t, 3> out;
    const std_array<uint8_t, 3> expected{ { 7, 42, 11 } };
    std::istringstream in{ "072a0b" };
    BC_REQUIRE(deserialize(out, in));
    BC_REQUIRE_EQUAL(out, expected);
}

// <data_chunk>

BC_AUTO_TEST_CASE(deserialize__data_chunk__empty__empty)
{
    std_vector<uint8_t> out;
    const std_vector<uint8_t> expected{};
    BC_REQUIRE(deserialize(out, ""));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(deserialize__data_chunk__bytes__base16)
{
    std_vector<uint8_t> out;
    const std_vector<uint8_t> expected{ 7, 42, 11 };
    BC_REQUIRE(deserialize(out, "072a0b"));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(deserialize__data_chunk__istream__base16)
{
    std_vector<uint8_t> out;
    const std_vector<uint8_t> expected{ 7, 42, 11 };
    std::istringstream in{ "072a0b" };
    BC_REQUIRE(deserialize(out, in));
    BC_REQUIRE_EQUAL(out, expected);
}

// <vector<Value>>

BC_AUTO_TEST_CASE(serialize__vector__overflow_base10_delimited__false)
{
    std_vector<uint16_t> out;
    BC_REQUIRE(!deserialize(out, "42 9999999 42"));
}

BC_AUTO_TEST_CASE(serialize__vector__untrimmed_base10_delimited__true)
{
    std_vector<int> out;
    const std_vector<int> expected{ 7, -42, 11 };
    BC_REQUIRE(deserialize(out, " 7  -42  11 "));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(serialize__vector__int_base10_delimited__ints_true)
{
    std_vector<int> out;
    const std_vector<int> expected{ 7, 42, 11 };
    BC_REQUIRE(deserialize(out, "7 42 11"));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(serialize__vector__int_negative_base10_delimited__ints_true)
{
    std_vector<int> out;
    const std_vector<int> expected{ -7, 42, -11 };
    BC_REQUIRE(deserialize(out, "-7 42 -11"));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(serialize__vector__char_delimited__chars_true)
{
    std_vector<char> out;
    const std_vector<char> expected{ 'a', 'b', 'c' };
    BC_REQUIRE(deserialize(out, "a b c"));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(serialize__vector__string_delimited__tokens_true)
{
    std_vector<std::string> out;
    const std_vector<std::string> expected{ "abc", "def", "ghi" };
    BC_REQUIRE(deserialize(out, "abc def ghi"));
    BC_REQUIRE_EQUAL(out, expected);
}

// Splitting on whitespace trims string collections before string override.
BC_AUTO_TEST_CASE(serialize__vector__string_delimited_padded__tokens_true)
{
    std_vector<std::string> out;
    const std_vector<std::string> expected{ "abc", "def", "ghi" };
    BC_REQUIRE(deserialize(out, " abc \n def \n ghi "));
    BC_REQUIRE_EQUAL(out, expected);
}

BC_AUTO_TEST_CASE(serialize__vector__istream__tokens_true)
{
    std_vector<std::string> out;
    const std_vector<std::string> expected{ "abc", "def", "ghi" };
    std::istringstream in{ " abc \n def \n ghi " };
    BC_REQUIRE(deserialize(out, in));
    BC_REQUIRE_EQUAL(out, expected);
}

// <Value>

BC_AUTO_TEST_CASE(deserialize__integer__empty__false)
{
    uint16_t out;
    BC_REQUIRE(!deserialize(out, ""));
}

BC_AUTO_TEST_CASE(deserialize__char__empty__false)
{
    char out;
    BC_REQUIRE(!deserialize(out, ""));
}

BC_AUTO_TEST_CASE(deserialize__integer__overflow_base10__false)
{
    uint16_t out;
    BC_REQUIRE(!deserialize(out, "9999999"));
}

BC_AUTO_TEST_CASE(deserialize__integer__invalid_base10__false)
{
    uint16_t out;
    BC_REQUIRE(!deserialize(out, "????"));
}

BC_AUTO_TEST_CASE(deserialize__int__untrimmed_base10__true)
{
    int out;
    BC_REQUIRE(deserialize(out, " 42 "));
    BC_REQUIRE_EQUAL(out, 42);
}

BC_AUTO_TEST_CASE(deserialize__int__unsigned_base10__true)
{
    int out;
    BC_REQUIRE(deserialize(out, "42"));
    BC_REQUIRE_EQUAL(out, 42);
}

BC_AUTO_TEST_CASE(deserialize__int__negative_base10__true)
{
    int out;
    BC_REQUIRE(deserialize(out, "-42"));
    BC_REQUIRE_EQUAL(out, -42);
}

BC_AUTO_TEST_CASE(deserialize__float__base10__true)
{
    float out;
    BC_REQUIRE(deserialize(out, ".42"));
    BC_REQUIRE_EQUAL(out, 0.42f);
}

BC_AUTO_TEST_CASE(deserialize__double__zero_padded_base10__true)
{
    double out;
    BC_REQUIRE(deserialize(out, "00.4200"));
    BC_REQUIRE_EQUAL(out, 0.42);
}

BC_AUTO_TEST_CASE(deserialize__char__text__char)
{
    char out;
    BC_REQUIRE(deserialize(out, "*"));
    BC_REQUIRE_EQUAL(out, '*');
}

BC_AUTO_TEST_CASE(deserialize__char__numeric_chars__first_char)
{
    char out;
    BC_REQUIRE(deserialize(out, "1234567890"));
    BC_REQUIRE_EQUAL(out, '1');
}

BC_AUTO_TEST_CASE(deserialize__uint16__min_max__true)
{
    uint16_t out;
    BC_REQUIRE(deserialize(out, "0"));
    BC_REQUIRE_EQUAL(out, 0u);
    BC_REQUIRE(deserialize(out, "65535"));
    BC_REQUIRE_EQUAL(out, 0xffff);
}

BC_AUTO_TEST_CASE(deserialize__uint16__min_max_leading_zeros__true)
{
    uint16_t out;
    BC_REQUIRE(deserialize(out, "000"));
    BC_REQUIRE_EQUAL(out, 0u);
    BC_REQUIRE(deserialize(out, "0065535"));
    BC_REQUIRE_EQUAL(out, 0xffff);
}

BC_AUTO_TEST_CASE(deserialize__uint16__min_max_padded__true)
{
    uint16_t out;
    BC_REQUIRE(deserialize(out, " 0 "));
    BC_REQUIRE_EQUAL(out, 0u);
    BC_REQUIRE(deserialize(out, " 65535 "));
    BC_REQUIRE_EQUAL(out, 0xffff);
}

BC_AUTO_TEST_CASE(deserialize__uint32__min_max__true)
{
    uint32_t out;
    BC_REQUIRE(deserialize(out, "0"));
    BC_REQUIRE_EQUAL(out, 0u);
    BC_REQUIRE(deserialize(out, "4294967295"));
    BC_REQUIRE_EQUAL(out, 0xffffffff);
}

BC_AUTO_TEST_CASE(deserialize__uint64__min_max__true)
{
    uint64_t out;
    BC_REQUIRE(deserialize(out, "0"));
    BC_REQUIRE_EQUAL(out, 0u);
    BC_REQUIRE(deserialize(out, "18446744073709551615"));
    BC_REQUIRE_EQUAL(out, 0xffffffffffffffff);
}

BC_AUTO_TEST_CASE(deserialize__size_t__min_uint32_max__true)
{
    size_t out;
    BC_REQUIRE(deserialize(out, "0"));
    BC_REQUIRE_EQUAL(out, 0u);
    BC_REQUIRE(deserialize(out, "4294967295"));
    BC_REQUIRE_EQUAL(out, 0xffffffff);
}

BC_AUTO_TEST_CASE(deserialize__size_t__stringstream__true)
{
    size_t out;
    std::stringstream in{ "4294967295" };
    BC_REQUIRE(deserialize(out, in));
    BC_REQUIRE_EQUAL(out, 0xffffffff);
}

BC_AUTO_TEST_SUITE_END()
