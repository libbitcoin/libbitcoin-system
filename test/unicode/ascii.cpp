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

BC_AUTO_TEST_SUITE(ascii_tests)

// is_ascii_character

BC_AUTO_TEST_CASE(ascii__is_ascii_character__bounds__true)
{
    BC_REQUIRE(is_ascii_character(0x00000000));
    BC_REQUIRE(is_ascii_character(0x0000007f));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_character__out_of_bounds__false)
{
    BC_REQUIRE(!is_ascii_character(0x00000080));
    BC_REQUIRE(!is_ascii_character(0xffffffff));
}

// is_ascii_number

BC_AUTO_TEST_CASE(ascii__is_ascii_number__ascii_numbers__true)
{
    // ASCII number characters.
    BC_REQUIRE(is_ascii_number('0'));
    BC_REQUIRE(is_ascii_number('1'));
    BC_REQUIRE(is_ascii_number('2'));
    BC_REQUIRE(is_ascii_number('3'));
    BC_REQUIRE(is_ascii_number('4'));
    BC_REQUIRE(is_ascii_number('5'));
    BC_REQUIRE(is_ascii_number('6'));
    BC_REQUIRE(is_ascii_number('7'));
    BC_REQUIRE(is_ascii_number('8'));
    BC_REQUIRE(is_ascii_number('9'));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_number__non_ascii_numbers__false)
{
    // These tests are invalid, as the conversion to uint32_t is just a cast.
    // These fail in clang, as it recognizes them as multiple byte characters.
    ////BC_REQUIRE(!is_ascii_number('Ⅰ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅱ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅲ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅳ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅴ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅵ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅶ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅷ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅸ'));
    ////BC_REQUIRE(!is_ascii_number('Ⅹ'));

    // These verify (on msvc/gcc) that the utf8 characters are cast to uint32.
    ////BC_REQUIRE(0x00e285a0 == 'Ⅰ');
    ////BC_REQUIRE(0x00e285a1 == 'Ⅱ');
    ////BC_REQUIRE(0x00e285a2 == 'Ⅲ');
    ////BC_REQUIRE(0x00e285a3 == 'Ⅳ');
    ////BC_REQUIRE(0x00e285a4 == 'Ⅴ');
    ////BC_REQUIRE(0x00e285a5 == 'Ⅵ');
    ////BC_REQUIRE(0x00e285a6 == 'Ⅶ');
    ////BC_REQUIRE(0x00e285a7 == 'Ⅷ');
    ////BC_REQUIRE(0x00e285a8 == 'Ⅸ');
    ////BC_REQUIRE(0x00e285a9 == 'Ⅹ');

    // Roman numeral characters (unicode).
    BC_REQUIRE(!is_ascii_number(0x00002160));
    BC_REQUIRE(!is_ascii_number(0x00002161));
    BC_REQUIRE(!is_ascii_number(0x00002162));
    BC_REQUIRE(!is_ascii_number(0x00002163));
    BC_REQUIRE(!is_ascii_number(0x00002164));
    BC_REQUIRE(!is_ascii_number(0x00002165));
    BC_REQUIRE(!is_ascii_number(0x00002166));
    BC_REQUIRE(!is_ascii_number(0x00002167));
    BC_REQUIRE(!is_ascii_number(0x00002168));
    BC_REQUIRE(!is_ascii_number(0x00002169));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_number__non_numbers__false)
{
    // ASCII non-number characters (from ascii hex).
    BC_REQUIRE(!is_ascii_number('a'));
    BC_REQUIRE(!is_ascii_number('b'));
    BC_REQUIRE(!is_ascii_number('c'));
    BC_REQUIRE(!is_ascii_number('d'));
    BC_REQUIRE(!is_ascii_number('e'));
    BC_REQUIRE(!is_ascii_number('f'));
    BC_REQUIRE(!is_ascii_number('x'));
    BC_REQUIRE(!is_ascii_number('A'));
    BC_REQUIRE(!is_ascii_number('B'));
    BC_REQUIRE(!is_ascii_number('C'));
    BC_REQUIRE(!is_ascii_number('D'));
    BC_REQUIRE(!is_ascii_number('E'));
    BC_REQUIRE(!is_ascii_number('F'));
    BC_REQUIRE(!is_ascii_number('X'));
}

// is_ascii_lower

BC_AUTO_TEST_CASE(ascii__is_ascii_lower__ascii_lowers__true)
{
    BC_REQUIRE(is_ascii_lower('a'));
    BC_REQUIRE(is_ascii_lower('b'));
    BC_REQUIRE(is_ascii_lower('c'));
    BC_REQUIRE(is_ascii_lower('x'));
    BC_REQUIRE(is_ascii_lower('y'));
    BC_REQUIRE(is_ascii_lower('z'));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_lower__ascii_uppers__false)
{
    BC_REQUIRE(!is_ascii_lower('A'));
    BC_REQUIRE(!is_ascii_lower('B'));
    BC_REQUIRE(!is_ascii_lower('C'));
    BC_REQUIRE(!is_ascii_lower('X'));
    BC_REQUIRE(!is_ascii_lower('Y'));
    BC_REQUIRE(!is_ascii_lower('Z'));
}

// is_ascii_upper

BC_AUTO_TEST_CASE(ascii__is_ascii_upper__ascii_uppers__true)
{
    BC_REQUIRE(is_ascii_upper('A'));
    BC_REQUIRE(is_ascii_upper('B'));
    BC_REQUIRE(is_ascii_upper('C'));
    BC_REQUIRE(is_ascii_upper('X'));
    BC_REQUIRE(is_ascii_upper('Y'));
    BC_REQUIRE(is_ascii_upper('Z'));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_upper__ascii_lowers__false)
{
    BC_REQUIRE(!is_ascii_upper('a'));
    BC_REQUIRE(!is_ascii_upper('b'));
    BC_REQUIRE(!is_ascii_upper('c'));
    BC_REQUIRE(!is_ascii_upper('x'));
    BC_REQUIRE(!is_ascii_upper('y'));
    BC_REQUIRE(!is_ascii_upper('z'));
}

// is_ascii_alpha

BC_AUTO_TEST_CASE(ascii__is_ascii_alpha__ascii_alphas__true)
{
    BC_REQUIRE(is_ascii_alpha('A'));
    BC_REQUIRE(is_ascii_alpha('B'));
    BC_REQUIRE(is_ascii_alpha('C'));
    BC_REQUIRE(is_ascii_alpha('X'));
    BC_REQUIRE(is_ascii_alpha('Y'));
    BC_REQUIRE(is_ascii_alpha('Z'));
    BC_REQUIRE(is_ascii_alpha('a'));
    BC_REQUIRE(is_ascii_alpha('b'));
    BC_REQUIRE(is_ascii_alpha('c'));
    BC_REQUIRE(is_ascii_alpha('x'));
    BC_REQUIRE(is_ascii_alpha('y'));
    BC_REQUIRE(is_ascii_alpha('z'));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_alpha__ascii_non_alphas__false)
{
    BC_REQUIRE(!is_ascii_alpha('0'));
    BC_REQUIRE(!is_ascii_alpha('1'));
    BC_REQUIRE(!is_ascii_alpha('9'));
    BC_REQUIRE(!is_ascii_alpha('*'));
    BC_REQUIRE(!is_ascii_alpha('!'));
    BC_REQUIRE(!is_ascii_alpha('#'));
}

// is_ascii_alphanumeric

BC_AUTO_TEST_CASE(ascii__is_ascii_alphanumeric__ascii_alphanumerics__true)
{
    BC_REQUIRE(is_ascii_alphanumeric('A'));
    BC_REQUIRE(is_ascii_alphanumeric('B'));
    BC_REQUIRE(is_ascii_alphanumeric('C'));
    BC_REQUIRE(is_ascii_alphanumeric('X'));
    BC_REQUIRE(is_ascii_alphanumeric('Y'));
    BC_REQUIRE(is_ascii_alphanumeric('Z'));
    BC_REQUIRE(is_ascii_alphanumeric('a'));
    BC_REQUIRE(is_ascii_alphanumeric('b'));
    BC_REQUIRE(is_ascii_alphanumeric('c'));
    BC_REQUIRE(is_ascii_alphanumeric('x'));
    BC_REQUIRE(is_ascii_alphanumeric('y'));
    BC_REQUIRE(is_ascii_alphanumeric('z'));
    BC_REQUIRE(is_ascii_alphanumeric('0'));
    BC_REQUIRE(is_ascii_alphanumeric('1'));
    BC_REQUIRE(is_ascii_alphanumeric('9'));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_alphanumeric__ascii_non_alphanumerics__false)
{
    BC_REQUIRE(!is_ascii_alphanumeric('*'));
    BC_REQUIRE(!is_ascii_alphanumeric('!'));
    BC_REQUIRE(!is_ascii_alphanumeric('#'));
}

// is_ascii_separator

BC_AUTO_TEST_CASE(ascii__is_ascii_separator__ascii_space__true)
{
    // ASCII separator characters.
    BC_REQUIRE(is_ascii_separator(0x00000020));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_separator__ideographic_space__false)
{
    BC_REQUIRE(!is_ascii_separator(0x00003000));
}

// is_ascii_whitespace

BC_AUTO_TEST_CASE(ascii__is_ascii_whitespace__all_ascii_whitespace__true)
{
    // ASCII whitespace characters (C whitespace).
    BC_REQUIRE(is_ascii_whitespace(0x00000009));
    BC_REQUIRE(is_ascii_whitespace(0x0000000a));
    BC_REQUIRE(is_ascii_whitespace(0x0000000b));
    BC_REQUIRE(is_ascii_whitespace(0x0000000c));
    BC_REQUIRE(is_ascii_whitespace(0x0000000d));
    BC_REQUIRE(is_ascii_whitespace(0x00000020));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_whitespace__zero__false)
{
    BC_REQUIRE(!is_ascii_whitespace(0x00000000));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_whitespace__ideographic_space__false)
{
    BC_REQUIRE(!is_ascii_whitespace(0x00003000));
}

// is_ascii

BC_AUTO_TEST_CASE(ascii__is_ascii__empty__true)
{
    BC_REQUIRE(is_ascii(""));
}

BC_AUTO_TEST_CASE(ascii__is_ascii__alphanumeric__true)
{
    BC_REQUIRE(is_ascii("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii__below_128__true)
{
    BC_REQUIRE(is_ascii(std::string{ '\x00' }));
    BC_REQUIRE(is_ascii(std::string{ '\x7f' }));
}

BC_AUTO_TEST_CASE(ascii__is_ascii__above_127__false)
{
    BC_REQUIRE(!is_ascii(std::string{ '\x80' }));
    BC_REQUIRE(!is_ascii(std::string{ '\xff' }));
}

BC_AUTO_TEST_CASE(ascii__is_ascii__ideographic_space__false)
{
    BC_REQUIRE(!is_ascii(ideographic_space));
}

// is_ascii_numeric

BC_AUTO_TEST_CASE(ascii__is_ascii_numeric__empty__true)
{
    BC_REQUIRE(is_ascii_numeric(""));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_numeric__alphanumeric__false)
{
    BC_REQUIRE(!is_ascii_numeric("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_numeric__numeric__true)
{
    BC_REQUIRE(is_ascii_numeric("0123456789"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_numeric__negative_numerics__true)
{
    BC_REQUIRE(is_ascii_numeric("-0"));
    BC_REQUIRE(is_ascii_numeric("-1"));
    BC_REQUIRE(is_ascii_numeric("-01234567890"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_numeric__misplaced_negative_numerics__false)
{
    BC_REQUIRE(!is_ascii_numeric("0-"));
    BC_REQUIRE(!is_ascii_numeric("-1-"));
    BC_REQUIRE(!is_ascii_numeric("01234-567890"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_numeric__double_negative_numerics__false)
{
    BC_REQUIRE(!is_ascii_numeric("--0"));
    BC_REQUIRE(!is_ascii_numeric("--1"));
    BC_REQUIRE(!is_ascii_numeric("--01234567890"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_numeric__padded_numeric__false)
{
    BC_REQUIRE(!is_ascii_numeric(" 0123456789"));
    BC_REQUIRE(!is_ascii_numeric("0123456789 "));
    BC_REQUIRE(!is_ascii_numeric("- 0123456789"));
    BC_REQUIRE(!is_ascii_numeric(" -123456789"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_numeric__decimal_numeric__false)
{
    BC_REQUIRE(!is_ascii_numeric(".0123456789"));
    BC_REQUIRE(!is_ascii_numeric("."));
    BC_REQUIRE(!is_ascii_numeric("-.42"));
}

// is_ascii_alphabet

BC_AUTO_TEST_CASE(ascii__is_ascii_alphabet__empty__true)
{
    BC_REQUIRE(is_ascii_alphabet(""));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_alphabet__numeric__false)
{
    BC_REQUIRE(!is_ascii_alphabet("0123456789"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_alphabet__alphabet_true)
{
    BC_REQUIRE(is_ascii_alphabet("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"));
}

BC_AUTO_TEST_CASE(ascii__is_ascii_alphabet__padded_alphabet_false)
{
    BC_REQUIRE(!is_ascii_alphabet(" abc"));
    BC_REQUIRE(!is_ascii_alphabet("abc "));
    BC_REQUIRE(!is_ascii_alphabet(" abc "));
}

// ascii_to_lower

BC_AUTO_TEST_CASE(ascii__ascii_to_lower__empty__empty)
{
    const std::string value{ "" };
    BC_REQUIRE_EQUAL(ascii_to_lower(value), value);
}

BC_AUTO_TEST_CASE(ascii__ascii_to_lower__lower__unchanged)
{
    const std::string value{ "abcdefghijklmnopqrstuvwxyz0123456789" };
    BC_REQUIRE_EQUAL(ascii_to_lower(value), value);
}

BC_AUTO_TEST_CASE(ascii__ascii_to_lower__upper__lowered)
{
    const std::string value{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz0123456789" };
    BC_REQUIRE_EQUAL(ascii_to_lower(value), expected);
}

BC_AUTO_TEST_CASE(ascii__ascii_to_lower__mixed__lowered)
{
    const std::string value{ "AbCdEfGhIjKlMnOpQrStUvWxYz0123456789" };
    const std::string expected{ "abcdefghijklmnopqrstuvwxyz0123456789" };
    BC_REQUIRE_EQUAL(ascii_to_lower(value), expected);
}

// ascii_to_upper

BC_AUTO_TEST_CASE(ascii__ascii_to_upper__empty__empty)
{
    const std::string value{ "" };
    BC_REQUIRE_EQUAL(ascii_to_upper(value), value);
}

BC_AUTO_TEST_CASE(ascii__ascii_to_upper__lower__raised)
{
    const std::string value{ "abcdefghijklmnopqrstuvwxyz0123456789" };
    const std::string expected{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    BC_REQUIRE_EQUAL(ascii_to_upper(value), expected);
}

BC_AUTO_TEST_CASE(ascii__ascii_to_upper__upper__unchanged)
{
    const std::string value{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    BC_REQUIRE_EQUAL(ascii_to_upper(value), value);
}

BC_AUTO_TEST_CASE(ascii__ascii_to_upper__mixed__raised)
{
    const std::string value{ "AbCdEfGhIjKlMnOpQrStUvWxYz0123456789" };
    const std::string expected{ "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" };
    BC_REQUIRE_EQUAL(ascii_to_upper(value), expected);
}

// has_ascii_whitespace

BC_AUTO_TEST_CASE(ascii__has_ascii_whitespace__empty__false)
{
    BC_REQUIRE(!has_ascii_whitespace(""));
}

BC_AUTO_TEST_CASE(ascii__has_ascii_whitespace__spaces__true)
{
    // ASCII whitespace characters (C whitespace).
    BC_REQUIRE(has_ascii_whitespace("\t"));
    BC_REQUIRE(has_ascii_whitespace("\n"));
    BC_REQUIRE(has_ascii_whitespace("\v"));
    BC_REQUIRE(has_ascii_whitespace("\f"));
    BC_REQUIRE(has_ascii_whitespace("\r"));
    BC_REQUIRE(has_ascii_whitespace("\x20"));
}

BC_AUTO_TEST_CASE(ascii__has_ascii_whitespace__foo_bar__true)
{
    BC_REQUIRE(has_ascii_whitespace("foo bar"));
}

BC_AUTO_TEST_CASE(ascii__has_ascii_whitespace__foobar__false)
{
    BC_REQUIRE(!has_ascii_whitespace("foobar"));
}

// has_mixed_ascii_case

BC_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__empty__false)
{
    BC_REQUIRE(!has_mixed_ascii_case(""));
}

BC_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__mixed_case_with_non_ascii__true)
{
    BC_REQUIRE(has_mixed_ascii_case("\x80xYz"));
}

BC_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__lower_ascii__false)
{
    BC_REQUIRE(!has_mixed_ascii_case("abcdefghijklmnopqrstuvwxyz0123456789"));
}

BC_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__upper_ascii__false)
{
    BC_REQUIRE(!has_mixed_ascii_case("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"));
}

BC_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__mixed_ascii__true)
{
    BC_REQUIRE(has_mixed_ascii_case("AbCdEfGhIjKlMnOpQrStUvWxYz0123456789"));
}

BC_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__lower_ascii_with_ideographic_space__false)
{
    const auto unicode = ideographic_space + "abcdefghijklmnopqrstuvwxyz";
    BC_REQUIRE(!has_mixed_ascii_case(unicode));
}

BC_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__upper_ascii_with_ideographic_space__false)
{
    const auto unicode = ideographic_space + "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    BC_REQUIRE(!has_mixed_ascii_case(unicode));
}

BC_AUTO_TEST_CASE(ascii__has_mixed_ascii_case__mixed_ascii_with_ideographic_space__true)
{
    const auto unicode = ideographic_space + "AbCdEfGhIjKlMnOpQrStUvWxYz";
    BC_REQUIRE(has_mixed_ascii_case(unicode));
}

BC_AUTO_TEST_SUITE_END()
