/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(ascii_tests)

// is_ascii_character

BOOST_AUTO_TEST_CASE(ascii__is_ascii_character__bounds__true)
{
    BOOST_REQUIRE(is_ascii_character(0x00000000));
    BOOST_REQUIRE(is_ascii_character(0x0000007f));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_character__out_of_bounds__false)
{
    BOOST_REQUIRE(!is_ascii_character(0x00000080));
    BOOST_REQUIRE(!is_ascii_character(0xffffffff));
}

// is_ascii_number

BOOST_AUTO_TEST_CASE(ascii__is_ascii_number__ascii_numbers__true)
{
    // ASCII number characters.
    BOOST_REQUIRE(is_ascii_number('0'));
    BOOST_REQUIRE(is_ascii_number('1'));
    BOOST_REQUIRE(is_ascii_number('2'));
    BOOST_REQUIRE(is_ascii_number('3'));
    BOOST_REQUIRE(is_ascii_number('4'));
    BOOST_REQUIRE(is_ascii_number('5'));
    BOOST_REQUIRE(is_ascii_number('6'));
    BOOST_REQUIRE(is_ascii_number('7'));
    BOOST_REQUIRE(is_ascii_number('8'));
    BOOST_REQUIRE(is_ascii_number('9'));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_number__non_ascii_numbers__false)
{
    // These tests are invalid, as the conversion to uint32_t is just a cast.
    // These fail in clang, as it recognizes them as multiple byte characters.
    ////BOOST_REQUIRE(!is_ascii_number('Ⅰ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅱ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅲ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅳ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅴ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅵ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅶ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅷ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅸ'));
    ////BOOST_REQUIRE(!is_ascii_number('Ⅹ'));

    // These verify (on msvc/gcc) that the utf8 characters are cast to uint32.
    ////BOOST_REQUIRE(0x00e285a0 == 'Ⅰ');
    ////BOOST_REQUIRE(0x00e285a1 == 'Ⅱ');
    ////BOOST_REQUIRE(0x00e285a2 == 'Ⅲ');
    ////BOOST_REQUIRE(0x00e285a3 == 'Ⅳ');
    ////BOOST_REQUIRE(0x00e285a4 == 'Ⅴ');
    ////BOOST_REQUIRE(0x00e285a5 == 'Ⅵ');
    ////BOOST_REQUIRE(0x00e285a6 == 'Ⅶ');
    ////BOOST_REQUIRE(0x00e285a7 == 'Ⅷ');
    ////BOOST_REQUIRE(0x00e285a8 == 'Ⅸ');
    ////BOOST_REQUIRE(0x00e285a9 == 'Ⅹ');

    // Roman numeral characters (unicode).
    BOOST_REQUIRE(!is_ascii_number(0x00002160));
    BOOST_REQUIRE(!is_ascii_number(0x00002161));
    BOOST_REQUIRE(!is_ascii_number(0x00002162));
    BOOST_REQUIRE(!is_ascii_number(0x00002163));
    BOOST_REQUIRE(!is_ascii_number(0x00002164));
    BOOST_REQUIRE(!is_ascii_number(0x00002165));
    BOOST_REQUIRE(!is_ascii_number(0x00002166));
    BOOST_REQUIRE(!is_ascii_number(0x00002167));
    BOOST_REQUIRE(!is_ascii_number(0x00002168));
    BOOST_REQUIRE(!is_ascii_number(0x00002169));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_number__non_numbers__false)
{
    // ASCII non-number characters (from ascii hex).
    BOOST_REQUIRE(!is_ascii_number('a'));
    BOOST_REQUIRE(!is_ascii_number('b'));
    BOOST_REQUIRE(!is_ascii_number('c'));
    BOOST_REQUIRE(!is_ascii_number('d'));
    BOOST_REQUIRE(!is_ascii_number('e'));
    BOOST_REQUIRE(!is_ascii_number('f'));
    BOOST_REQUIRE(!is_ascii_number('x'));
    BOOST_REQUIRE(!is_ascii_number('A'));
    BOOST_REQUIRE(!is_ascii_number('B'));
    BOOST_REQUIRE(!is_ascii_number('C'));
    BOOST_REQUIRE(!is_ascii_number('D'));
    BOOST_REQUIRE(!is_ascii_number('E'));
    BOOST_REQUIRE(!is_ascii_number('F'));
    BOOST_REQUIRE(!is_ascii_number('X'));
}

// is_ascii_lower

BOOST_AUTO_TEST_CASE(ascii__is_ascii_lower__ascii_lowers__true)
{
    BOOST_REQUIRE(is_ascii_lower('a'));
    BOOST_REQUIRE(is_ascii_lower('b'));
    BOOST_REQUIRE(is_ascii_lower('c'));
    BOOST_REQUIRE(is_ascii_lower('x'));
    BOOST_REQUIRE(is_ascii_lower('y'));
    BOOST_REQUIRE(is_ascii_lower('z'));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_lower__ascii_uppers__false)
{
    BOOST_REQUIRE(!is_ascii_lower('A'));
    BOOST_REQUIRE(!is_ascii_lower('B'));
    BOOST_REQUIRE(!is_ascii_lower('C'));
    BOOST_REQUIRE(!is_ascii_lower('X'));
    BOOST_REQUIRE(!is_ascii_lower('Y'));
    BOOST_REQUIRE(!is_ascii_lower('Z'));
}

// is_ascii_upper

BOOST_AUTO_TEST_CASE(ascii__is_ascii_upper__ascii_uppers__true)
{
    BOOST_REQUIRE(is_ascii_upper('A'));
    BOOST_REQUIRE(is_ascii_upper('B'));
    BOOST_REQUIRE(is_ascii_upper('C'));
    BOOST_REQUIRE(is_ascii_upper('X'));
    BOOST_REQUIRE(is_ascii_upper('Y'));
    BOOST_REQUIRE(is_ascii_upper('Z'));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_upper__ascii_lowers__false)
{
    BOOST_REQUIRE(!is_ascii_upper('a'));
    BOOST_REQUIRE(!is_ascii_upper('b'));
    BOOST_REQUIRE(!is_ascii_upper('c'));
    BOOST_REQUIRE(!is_ascii_upper('x'));
    BOOST_REQUIRE(!is_ascii_upper('y'));
    BOOST_REQUIRE(!is_ascii_upper('z'));
}

// is_ascii_alpha

BOOST_AUTO_TEST_CASE(ascii__is_ascii_alpha__ascii_alphas__true)
{
    BOOST_REQUIRE(is_ascii_alpha('A'));
    BOOST_REQUIRE(is_ascii_alpha('B'));
    BOOST_REQUIRE(is_ascii_alpha('C'));
    BOOST_REQUIRE(is_ascii_alpha('X'));
    BOOST_REQUIRE(is_ascii_alpha('Y'));
    BOOST_REQUIRE(is_ascii_alpha('Z'));
    BOOST_REQUIRE(is_ascii_alpha('a'));
    BOOST_REQUIRE(is_ascii_alpha('b'));
    BOOST_REQUIRE(is_ascii_alpha('c'));
    BOOST_REQUIRE(is_ascii_alpha('x'));
    BOOST_REQUIRE(is_ascii_alpha('y'));
    BOOST_REQUIRE(is_ascii_alpha('z'));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_alpha__ascii_non_alphas__false)
{
    BOOST_REQUIRE(!is_ascii_alpha('0'));
    BOOST_REQUIRE(!is_ascii_alpha('1'));
    BOOST_REQUIRE(!is_ascii_alpha('9'));
    BOOST_REQUIRE(!is_ascii_alpha('*'));
    BOOST_REQUIRE(!is_ascii_alpha('!'));
    BOOST_REQUIRE(!is_ascii_alpha('#'));
}

// is_ascii_alphanumeric

BOOST_AUTO_TEST_CASE(ascii__is_ascii_alphanumeric__ascii_alphanumerics__true)
{
    BOOST_REQUIRE(is_ascii_alphanumeric('A'));
    BOOST_REQUIRE(is_ascii_alphanumeric('B'));
    BOOST_REQUIRE(is_ascii_alphanumeric('C'));
    BOOST_REQUIRE(is_ascii_alphanumeric('X'));
    BOOST_REQUIRE(is_ascii_alphanumeric('Y'));
    BOOST_REQUIRE(is_ascii_alphanumeric('Z'));
    BOOST_REQUIRE(is_ascii_alphanumeric('a'));
    BOOST_REQUIRE(is_ascii_alphanumeric('b'));
    BOOST_REQUIRE(is_ascii_alphanumeric('c'));
    BOOST_REQUIRE(is_ascii_alphanumeric('x'));
    BOOST_REQUIRE(is_ascii_alphanumeric('y'));
    BOOST_REQUIRE(is_ascii_alphanumeric('z'));
    BOOST_REQUIRE(is_ascii_alphanumeric('0'));
    BOOST_REQUIRE(is_ascii_alphanumeric('1'));
    BOOST_REQUIRE(is_ascii_alphanumeric('9'));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_alphanumeric__ascii_non_alphanumerics__false)
{
    BOOST_REQUIRE(!is_ascii_alphanumeric('*'));
    BOOST_REQUIRE(!is_ascii_alphanumeric('!'));
    BOOST_REQUIRE(!is_ascii_alphanumeric('#'));
}

// is_ascii_separator

BOOST_AUTO_TEST_CASE(ascii__is_ascii_separator__ascii_space__true)
{
    // ASCII separator characters.
    BOOST_REQUIRE(is_ascii_separator(0x00000020));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_separator__ideographic_space__false)
{
    BOOST_REQUIRE(!is_ascii_separator(0x00003000));
}

// is_ascii_whitespace

BOOST_AUTO_TEST_CASE(ascii__is_ascii_whitespace__all_ascii_whitespace__true)
{
    // ASCII whitespace characters (C whitespace).
    BOOST_REQUIRE(is_ascii_whitespace(0x00000009));
    BOOST_REQUIRE(is_ascii_whitespace(0x0000000a));
    BOOST_REQUIRE(is_ascii_whitespace(0x0000000b));
    BOOST_REQUIRE(is_ascii_whitespace(0x0000000c));
    BOOST_REQUIRE(is_ascii_whitespace(0x0000000d));
    BOOST_REQUIRE(is_ascii_whitespace(0x00000020));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_whitespace__zero__false)
{
    BOOST_REQUIRE(!is_ascii_whitespace(0x00000000));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_whitespace__ideographic_space__false)
{
    BOOST_REQUIRE(!is_ascii_whitespace(0x00003000));
}

// is_ascii

BOOST_AUTO_TEST_CASE(ascii__is_ascii__empty__true)
{
    BOOST_REQUIRE(is_ascii(""));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii__alphanumeric__true)
{
    BOOST_REQUIRE(is_ascii("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii__below_128__true)
{
    BOOST_REQUIRE(is_ascii(std::string{ '\x00' }));
    BOOST_REQUIRE(is_ascii(std::string{ '\x7f' }));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii__above_127__false)
{
    BOOST_REQUIRE(!is_ascii(std::string{ '\x80' }));
    BOOST_REQUIRE(!is_ascii(std::string{ '\xff' }));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii__ideographic_space__false)
{
    BOOST_REQUIRE(!is_ascii(ideographic_space));
}

// is_ascii_numeric

BOOST_AUTO_TEST_CASE(ascii__is_ascii_numeric__empty__true)
{
    BOOST_REQUIRE(is_ascii_numeric(""));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_numeric__alphanumeric__false)
{
    BOOST_REQUIRE(!is_ascii_numeric("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_numeric__numeric__true)
{
    BOOST_REQUIRE(is_ascii_numeric("0123456789"));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_numeric__negative_numerics__true)
{
    BOOST_REQUIRE(is_ascii_numeric("-0"));
    BOOST_REQUIRE(is_ascii_numeric("-1"));
    BOOST_REQUIRE(is_ascii_numeric("-01234567890"));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_numeric__misplaced_negative_numerics__false)
{
    BOOST_REQUIRE(!is_ascii_numeric("0-"));
    BOOST_REQUIRE(!is_ascii_numeric("-1-"));
    BOOST_REQUIRE(!is_ascii_numeric("01234-567890"));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_numeric__double_negative_numerics__false)
{
    BOOST_REQUIRE(!is_ascii_numeric("--0"));
    BOOST_REQUIRE(!is_ascii_numeric("--1"));
    BOOST_REQUIRE(!is_ascii_numeric("--01234567890"));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_numeric__padded_numeric__false)
{
    BOOST_REQUIRE(!is_ascii_numeric(" 0123456789"));
    BOOST_REQUIRE(!is_ascii_numeric("0123456789 "));
    BOOST_REQUIRE(!is_ascii_numeric("- 0123456789"));
    BOOST_REQUIRE(!is_ascii_numeric(" -123456789"));
}

BOOST_AUTO_TEST_CASE(ascii__is_ascii_numeric__decimal_numeric__false)
{
    BOOST_REQUIRE(!is_ascii_numeric(".0123456789"));
    BOOST_REQUIRE(!is_ascii_numeric("."));
    BOOST_REQUIRE(!is_ascii_numeric("-.42"));
}

// ascii_to_lower

BOOST_AUTO_TEST_CASE(ascii__ascii_to_lower__empty__empty)
{
    const std::string value{ "" };
    BOOST_REQUIRE_EQUAL(ascii_to_lower(value), value);
}

BOOST_AUTO_TEST_CASE(ascii__ascii_to_lower__lower__unchanged)
{
    const std::string value{ "abcdefghijklmnopqrstuvwxyz0123456789" };
    BOOST_REQUIRE_EQUAL(ascii_to_lower(value), value);
}

BOOST_AUTO_TEST_CASE(ascii__ascii_to_lower__upper__lowered)
{
    const std::string value{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz0123456789" };
    BOOST_REQUIRE_EQUAL(ascii_to_lower(value), expected);
}

BOOST_AUTO_TEST_CASE(ascii__ascii_to_lower__mixed__lowered)
{
    const std::string value{ "AbCdEfGhIjKlMnOpQrStUvWxYz0123456789" };
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz0123456789" };
    BOOST_REQUIRE_EQUAL(ascii_to_lower(value), expected);
}

// ascii_to_upper

BOOST_AUTO_TEST_CASE(ascii__ascii_to_upper__empty__empty)
{
    const std::string value{ "" };
    BOOST_REQUIRE_EQUAL(ascii_to_upper(value), value);
}

BOOST_AUTO_TEST_CASE(ascii__ascii_to_upper__lower__raised)
{
    const std::string value{ "abcdefghijklmnopqrstuvwxyz0123456789" };
    const std::string expected{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    BOOST_REQUIRE_EQUAL(ascii_to_upper(value), expected);
}

BOOST_AUTO_TEST_CASE(ascii__ascii_to_upper__upper__unchanged)
{
    const std::string value{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    BOOST_REQUIRE_EQUAL(ascii_to_upper(value), value);
}

BOOST_AUTO_TEST_CASE(ascii__ascii_to_upper__mixed__raised)
{
    const std::string value{ "AbCdEfGhIjKlMnOpQrStUvWxYz0123456789" };
    const std::string expected{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    BOOST_REQUIRE_EQUAL(ascii_to_upper(value), expected);
}

// has_ascii_whitespace

BOOST_AUTO_TEST_CASE(ascii__has_ascii_whitespace__empty__false)
{
    BOOST_REQUIRE(!has_ascii_whitespace(""));
}

BOOST_AUTO_TEST_CASE(ascii__has_ascii_whitespace__spaces__true)
{
    // ASCII whitespace characters (C whitespace).
    BOOST_REQUIRE(has_ascii_whitespace("\t"));
    BOOST_REQUIRE(has_ascii_whitespace("\n"));
    BOOST_REQUIRE(has_ascii_whitespace("\v"));
    BOOST_REQUIRE(has_ascii_whitespace("\f"));
    BOOST_REQUIRE(has_ascii_whitespace("\r"));
    BOOST_REQUIRE(has_ascii_whitespace("\x20"));
}

BOOST_AUTO_TEST_CASE(ascii__has_ascii_whitespace__foo_bar__true)
{
    BOOST_REQUIRE(has_ascii_whitespace("foo bar"));
}

BOOST_AUTO_TEST_CASE(ascii__has_ascii_whitespace__foobar__false)
{
    BOOST_REQUIRE(!has_ascii_whitespace("foobar"));
}

// has_mixed_ascii_case

BOOST_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__empty__false)
{
    BOOST_REQUIRE(!has_mixed_ascii_case(""));
}

BOOST_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__mixed_case_with_non_ascii__true)
{
    BOOST_REQUIRE(has_mixed_ascii_case("\x80xYz"));
}

BOOST_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__lower_ascii__false)
{
    BOOST_REQUIRE(!has_mixed_ascii_case("abcdefghijklmnopqrstuvwxyz0123456789"));
}

BOOST_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__upper_ascii__false)
{
    BOOST_REQUIRE(!has_mixed_ascii_case("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"));
}

BOOST_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__mixed_ascii__true)
{
    BOOST_REQUIRE(has_mixed_ascii_case("AbCdEfGhIjKlMnOpQrStUvWxYz0123456789"));
}

BOOST_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__lower_ascii_with_ideographic_space__false)
{
    const auto unicode = ideographic_space + "abcdefghijklmnopqrstuvwxyz";
    BOOST_REQUIRE(!has_mixed_ascii_case(unicode));
}

BOOST_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__upper_ascii_with_ideographic_space__false)
{
    const auto unicode = ideographic_space + "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    BOOST_REQUIRE(!has_mixed_ascii_case(unicode));
}

BOOST_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__mixed_ascii_with_ideographic_space__true)
{
    const auto unicode = ideographic_space + "AbCdEfGhIjKlMnOpQrStUvWxYz";
    BOOST_REQUIRE(has_mixed_ascii_case(unicode));
}

BOOST_AUTO_TEST_SUITE_END()
