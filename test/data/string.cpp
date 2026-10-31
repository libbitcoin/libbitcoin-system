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

BC_AUTO_TEST_SUITE(string_tests)

#define TODO_TESTS true
#define STRING_TO_STRING 1
#define STRING_JOIN 1
#define STRING_SPLIT 1
#define STRING_TRIM 1
#define STRING_REDUCE 1
#define STRING_REPLACE 1
#define STRING_STARTS_WITH 1
#define STRING_ENDS_WITH 1

#ifdef STRING_TO_STRING

// to_string<>

BC_AUTO_TEST_CASE(string__to_string_integer__todo__todo)
{
    BC_REQUIRE(TODO_TESTS);
}

// to_string

BC_AUTO_TEST_CASE(string__to_string_slice__to_array__inverse)
{
    std::string result{ "foobar" };
    BC_REQUIRE_EQUAL(to_string(to_array("foobar")), result);
}

BC_AUTO_TEST_CASE(string__to_string_slice__to_chunk__inverse)
{
    std::string result{ "foobar" };
    BC_REQUIRE_EQUAL(to_string(to_chunk(result)), result);
}

BC_AUTO_TEST_CASE(string__to_string_slice__empty__empty)
{
    const data_chunk value{};
    const std::string expected{ "" };
    BC_REQUIRE_EQUAL(to_string(value), expected);
}

BC_AUTO_TEST_CASE(string__to_string_slice__vector__expected)
{
    const data_chunk value{ 'a', 'b', 'c' };
    const std::string expected{ "abc" };
    BC_REQUIRE_EQUAL(to_string(value), expected);
}

BC_AUTO_TEST_CASE(string__to_string_slice__array__expected)
{
    const data_array<3> value{ { 'a', 'b', 'c' } };
    const std::string expected{ "abc" };
    BC_REQUIRE_EQUAL(system::to_string(value), expected);
}

#endif // STRING_TO_STRING

#ifdef STRING_JOIN

// join1

BC_AUTO_TEST_CASE(string__join1__empties__one_space)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ "", "" }), " ");
}

BC_AUTO_TEST_CASE(string__join1__spaces__three_spaces)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ " ", " " }, " "), "   ");
}

BC_AUTO_TEST_CASE(string__join1__left_value__right_space)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ "a", "" }), "a ");
}

BC_AUTO_TEST_CASE(string__join1__right_value__left_space)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ "", "b" }), " b");
}

BC_AUTO_TEST_CASE(string__join1__values__delimited)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ "abc", "xyz" }), "abc xyz");
}

// join2

BC_AUTO_TEST_CASE(string__join2__empties_empty_delimiter__empty)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ "", "" }, ""), "");
}

BC_AUTO_TEST_CASE(string__join2__values_empty_delimiter__undelimited)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ "abc", "xyz" }, ""), "abcxyz");
}

BC_AUTO_TEST_CASE(string__join2__values_comma_delimiter__delimited)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ "abc", "xyz" }, ","), "abc,xyz");
}

BC_AUTO_TEST_CASE(string__join2__values_ideographic_space_delimiter__delimited)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ "abc", "xyz" }, ideographic_space), "abc\xe3\x80\x80xyz");
}

BC_AUTO_TEST_CASE(string__join2__untrimmed_values_comma_delimiter__untrimmed_delimited)
{
    BC_REQUIRE_EQUAL(system::join(string_list{ " abc ", " xyz " }, ","), " abc , xyz ");
}

#endif // STRING_JOIN

#ifdef STRING_SPLIT

// split0

BC_AUTO_TEST_CASE(string__split0__true_compress__trimmed_compressed)
{
    const std::string value{ "\t\r\n abc " };
    const string_list expected{ "abc" };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

BC_AUTO_TEST_CASE(string__split0__false_compress__trimmed)
{
    const std::string value{ " abc \t\r\n" };
    const string_list expected{ "abc" };
    BC_REQUIRE_EQUAL(system::split(value, ascii_whitespace, ascii_whitespace, false), expected);
}

// split1

BC_AUTO_TEST_CASE(string__split1__empty__empty)
{
    const std::string value{};
    const string_list expected{ "" };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

BC_AUTO_TEST_CASE(string__split1__empty_token__empty_token)
{
    const std::string value{ "" };
    const string_list expected{ value };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

BC_AUTO_TEST_CASE(string__split1__no_delimiter__one_token)
{
    const std::string value{ "abc" };
    const string_list expected{ value };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

BC_AUTO_TEST_CASE(string__split1__one_delimiter__split)
{
    const std::string value{ "abc xyz" };
    const string_list expected{ "abc", "xyz" };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

BC_AUTO_TEST_CASE(string__split1__nine_delimiters__compressed)
{
    const std::string value{ "abc   xyz   123   456" };
    const string_list expected{ "abc", "xyz", "123", "456" };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

BC_AUTO_TEST_CASE(string__split1__one_delimiter__left_trimmed_and_compressed)
{
    const std::string value{ "\t\r\n abc" };
    const string_list expected{ "abc" };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

BC_AUTO_TEST_CASE(string__split1__one_delimiter__right_trimmed_and_compressed)
{
    const std::string value{ "abc \t\r\n" };
    const string_list expected{ "abc" };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

BC_AUTO_TEST_CASE(string__split1__twelve_delimiters__trimmed_and_compressed)
{
    const std::string value{ "  \t\r\n  abc  \t\r\n  xyz  \t\r\n  " };
    const string_list expected{ "abc", "xyz" };
    BC_REQUIRE_EQUAL(system::split(value), expected);
}

// split2

BC_AUTO_TEST_CASE(string__split2__one_ideographic_space_delimiter__split)
{
    const std::string value{ "www\xe3\x80\x80mmm" };
    const string_list expected{ "www", "mmm" };
    BC_REQUIRE_EQUAL(system::split(value, ideographic_space), expected);
}

BC_AUTO_TEST_CASE(string__split2__three_ideographic_space_delimiters__compressed)
{
    const std::string value{ "\xe3\x80\x80www\xe3\x80\x80mmm\xe3\x80\x80" };
    const string_list expected{ "www", "mmm" };
    BC_REQUIRE_EQUAL(system::split(value, ideographic_space), expected);
}

BC_AUTO_TEST_CASE(string__split2__four_ideographic_space_delimiters__trimmed_compressed)
{
    const std::string value{ " \xe3\x80\x80 www \xe3\x80\x80\xe3\x80\x80 mmm \xe3\x80\x80 " };
    const string_list expected{ "www", "mmm" };
    BC_REQUIRE_EQUAL(system::split(value, ideographic_space), expected);
}

// split3

BC_AUTO_TEST_CASE(string__split3__one_delimiter_left__untrimmed)
{
    const std::string value{ "\t\r\n abc" };
    const string_list expected{ "\t\r\n", "abc" };
    BC_REQUIRE_EQUAL(system::split(value, ascii_space, false), expected);
}

BC_AUTO_TEST_CASE(string__split3__one_delimiter_right__untrimmed)
{
    const std::string value{ "abc \t\r\n" };
    const string_list expected{ "abc", "\t\r\n" };
    BC_REQUIRE_EQUAL(system::split(value, ascii_space, false), expected);
}

BC_AUTO_TEST_CASE(string__split3__twelve_delimiters__untrimmed_compressed)
{
    const std::string value{ "  \t\r\n  abc  \t\r\n  xyz  \t\r\n  " };
    const string_list expected{ "\t\r\n", "abc", "\t\r\n", "xyz", "\t\r\n" };
    BC_REQUIRE_EQUAL(system::split(value, ascii_space, false), expected);
}

BC_AUTO_TEST_CASE(string__split3__four_ideographic_space_delimiters__untrimmed_compressed)
{
    const std::string value{ " \xe3\x80\x80 www \xe3\x80\x80\xe3\x80\x80 mmm \xe3\x80\x80 " };
    const string_list expected{ " ", " www ", " mmm ", " " };
    BC_REQUIRE_EQUAL(system::split(value, ideographic_space, false), expected);
}

// split4

BC_AUTO_TEST_CASE(string__split4__twelve_comma_delimiters__trimmed_uncompressed)
{
    const std::string value{ ",,\t\r\n,,abc,,\t\r\n,,xyz,,\t\r\n,," };
    const string_list expected{ "", "", "", "", "abc", "", "", "", "xyz", "", "", "", "" };
    BC_REQUIRE_EQUAL(system::split(value, ",", true, false), expected);
}

BC_AUTO_TEST_CASE(string__split4__four_ideographic_space_delimiters__untrimmed_uncompressed)
{
    const std::string value{ " \xe3\x80\x80 www \xe3\x80\x80\xe3\x80\x80 mmm \xe3\x80\x80 " };
    const string_list expected{ " ", " www ", "", " mmm ", " " };
    BC_REQUIRE_EQUAL(system::split(value, ideographic_space, false, false), expected);
}

BC_AUTO_TEST_CASE(string__split5__ascii_and_ideographic_space_delimiters__untrimmed_uncompressed)
{
    const std::string value{ "\t \xe3\x80\x80 www \xe3\x80\x80\xe3\x80\x80 mmm \xe3\x80\x80 \t" };
    const string_list expected{ "\t", "", "", "www", "", "", "", "mmm", "", "", "\t" };
    const string_list delimiters{ ideographic_space, ascii_space };
    BC_REQUIRE_EQUAL(system::split(value, delimiters, {}, false), expected);
}

// split5

BC_AUTO_TEST_CASE(string__split5__ascii_and_ideographic_space_delimiters__untrimmed_compressed)
{
    const std::string value{ "\t \xe3\x80\x80 www \xe3\x80\x80\xe3\x80\x80 mmm \xe3\x80\x80 \t" };
    const string_list expected{ "\t", "www", "mmm", "\t" };
    const string_list delimiters{ ideographic_space, ascii_space };
    BC_REQUIRE_EQUAL(system::split(value, delimiters, {}), expected);
}

BC_AUTO_TEST_CASE(string__split5__alpha_bravo_charlie__compressed)
{
    const std::string value{ "alpha bravo charlie" };
    const string_list expected{ "lph", "r", "vo", "h", "rlie" };
    const string_list delimiters{ "a", "b", "c", " " };
    BC_REQUIRE_EQUAL(system::split(value, delimiters, {}, true), expected);
}

BC_AUTO_TEST_CASE(string__split5__alpha_bravo_charlie__trimmed)
{
    const std::string value{ "\talpha \tbravo\t charlie\t" };
    const string_list expected{ "", "lph", "", "", "r", "vo", "", "h", "rlie" };
    const string_list delimiters{ "a", "b", "c", " " };
    BC_REQUIRE_EQUAL(system::split(value, delimiters, ascii_whitespace, false), expected);
}

#endif // STRING_SPLIT

#ifdef STRING_TRIM

// trim

BC_AUTO_TEST_CASE(string__trim__empty__empty)
{
    std::string value{ "" };
    system::trim(value);
    BC_REQUIRE_EQUAL(value, "");
}

BC_AUTO_TEST_CASE(string__trim__no_whitespace__unchanged)
{
    std::string value{ "abcdefghij" };
    const auto expected = value;
    system::trim(value);
    BC_REQUIRE_EQUAL(value, value);
}

BC_AUTO_TEST_CASE(string__trim__all_whitespace__empty)
{
    std::string value{ " \t\n\v\f\r " };
    system::trim(value);
    BC_REQUIRE_EQUAL(value, "");
}

BC_AUTO_TEST_CASE(string__trim__internal_whitespace__unchanged)
{
    std::string value{ "1 \t\n\v\f\r 1" };
    const auto expected = value;
    system::trim(value);
    BC_REQUIRE_EQUAL(value, expected);
}

BC_AUTO_TEST_CASE(string__trim__external_whitespace__trimmed)
{
    std::string value{ "\t\n\v\f\r 1 \r\f\v\n\t" };
    system::trim(value);
    BC_REQUIRE_EQUAL(value, "1");
}

// trim_left

BC_AUTO_TEST_CASE(string__trim_left__todo__todo)
{
    BC_REQUIRE(TODO_TESTS);
}

// trim_right

BC_AUTO_TEST_CASE(string__trim_right__todo__todo)
{
    BC_REQUIRE(TODO_TESTS);
}

// trim_copy

BC_AUTO_TEST_CASE(string__trim_copy__empty__empty)
{
    BC_REQUIRE_EQUAL(system::trim_copy(""), "");
}

BC_AUTO_TEST_CASE(string__trim_copy__all_whitespace__empty)
{
    BC_REQUIRE_EQUAL(system::trim_copy(" \t\n\v\f\r "), "");
}

BC_AUTO_TEST_CASE(string__trim_copy__internal_whitespace__unchanged)
{
    BC_REQUIRE_EQUAL(system::trim_copy("1 \t\n\v\f\r 1"), "1 \t\n\v\f\r 1");
}

BC_AUTO_TEST_CASE(string__trim_copy__external_whitespace__trimmed)
{
    BC_REQUIRE_EQUAL(system::trim_copy("\t\n\v\f\r 1 \r\f\v\n\t"), "1");
}

BC_AUTO_TEST_CASE(string__trim_copy__move_external_whitespace__trimmed)
{
    BC_REQUIRE_EQUAL(system::trim_copy(std::string{ "\t\n\v\f\r 1 \r\f\v\n\t" }), "1");
}

BC_AUTO_TEST_CASE(string__trim_copy__copy_external_whitespace__trimmed)
{
    const std::string value{ "\t\n\v\f\r 1 \r\f\v\n\t" };
    BC_REQUIRE_EQUAL(system::trim_copy(value), "1");
}

// trim_left_copy

BC_AUTO_TEST_CASE(string__trim_left_copy__copy__expected)
{
    const std::string value{ "\t\n\v\f\r 1 \r\f\v\n\t" };
    BC_REQUIRE_EQUAL(system::trim_left_copy(value), "1 \r\f\v\n\t");
}

BC_AUTO_TEST_CASE(string__trim_left_copy__move__expected)
{
    BC_REQUIRE_EQUAL(system::trim_left_copy(std::string{ "\t\n\v\f\r 1 \r\f\v\n\t" }), "1 \r\f\v\n\t");
}

// trim_right_copy

BC_AUTO_TEST_CASE(string__trim_right_copy__copy__expected)
{
    const std::string value{ "\t\n\v\f\r 1 \r\f\v\n\t" };
    BC_REQUIRE_EQUAL(system::trim_right_copy(value), "\t\n\v\f\r 1");
}

BC_AUTO_TEST_CASE(string__trim_right_copy__move__expected)
{
    const std::string value{ "\t\n\v\f\r 1 \r\f\v\n\t" };
    BC_REQUIRE_EQUAL(system::trim_right_copy(value), "\t\n\v\f\r 1");
}

#endif // STRING_TRIM

#ifdef STRING_REDUCE

// reduce

BC_AUTO_TEST_CASE(string__reduce__empty__empty)
{
    string_list values{};
    const string_list expected{};
    reduce(values);
    BC_REQUIRE_EQUAL(values, expected);
}

BC_AUTO_TEST_CASE(string__reduce__nothing_to_do__unchanged)
{
    string_list values{ "a", "b", "c" };
    const string_list expected{ "a", "b", "c" };
    reduce(values);
    BC_REQUIRE_EQUAL(values, expected);
}

BC_AUTO_TEST_CASE(string__reduce__empties__single_empty)
{
    string_list values{ "", "", "" };
    const string_list expected{ "" };
    reduce(values);
    BC_REQUIRE_EQUAL(values, expected);
}

BC_AUTO_TEST_CASE(string__reduce__whitespace__trimmed)
{
    string_list values{ " \ta\t ", "\r\tb\t\n", "\t\tc\t\t" };
    const string_list expected{ "a", "b", "c" };
    reduce(values);
    BC_REQUIRE_EQUAL(values, expected);
}

BC_AUTO_TEST_CASE(string__reduce__only_whitespace__trimmed_and_reduced)
{
    string_list values{ " \t\t ", "\r\t\t\n", "\t\tc\t\t" };
    const string_list expected{ "c" };
    reduce(values);
    BC_REQUIRE_EQUAL(values, expected);
}

BC_AUTO_TEST_CASE(string__reduce__empties_reduce_false__unchanged)
{
    string_list values{ "", "", "" };
    const string_list expected = values;
    reduce(values, {}, false);
    BC_REQUIRE_EQUAL(values, expected);
}

BC_AUTO_TEST_CASE(string__reduce__padded_trim_false__unchanged)
{
    string_list values{ " \ta\t ", "\r\tb\t\n", "\t\tc\t\t" };
    const string_list expected = values;
    reduce(values, {}, true);
    BC_REQUIRE_EQUAL(values, expected);
}

BC_AUTO_TEST_CASE(string__reduce__only_whitespace_both_false__unchanged)
{
    string_list values{ " \t\t ", "\r\t\t\n", "\t\tc\t\t" };
    const string_list expected = values;
    reduce(values, {}, false);
    BC_REQUIRE_EQUAL(values, expected);
}

// reduce_copy

BC_AUTO_TEST_CASE(string__reduce_copy__todo__todo)
{
    BC_REQUIRE(TODO_TESTS);
}

#endif // STRING_REDUCE

#ifdef STRING_REPLACE

// replace

BC_AUTO_TEST_CASE(string__replace__from_empty__unchanged)
{
    std::string value{ "def" };
    const std::string expected{ "def" };
    BC_REQUIRE_EQUAL(replace(value, "", "klm"), 0u);
    BC_REQUIRE_EQUAL(value, expected);
}

BC_AUTO_TEST_CASE(string__replace__empty__empty)
{
    std::string value{};
    const std::string expected{};
    BC_REQUIRE_EQUAL(replace(value, "def", "klm"), 0u);
    BC_REQUIRE_EQUAL(value, expected);
}

BC_AUTO_TEST_CASE(string__replace__none__unchanged)
{
    std::string value{ "abc xyz abc xyz" };
    BC_REQUIRE_EQUAL(replace(value, "def", "klm"), 0u);
    BC_REQUIRE_EQUAL(value, value);
}

BC_AUTO_TEST_CASE(string__replace__multiple__expected)
{
    std::string value{ "abc def abc def" };
    const std::string expected{ "abc klm abc klm" };
    BC_REQUIRE_EQUAL(replace(value, "def", "klm"), 2u);
    BC_REQUIRE_EQUAL(value, expected);
}

BC_AUTO_TEST_CASE(string__replace__multiple_adjacent__expected)
{
    std::string value{ "defdefabcdef" };
    const std::string expected{ "klmklmabcklm" };
    BC_REQUIRE_EQUAL(replace(value, "def", "klm"), 3u);
    BC_REQUIRE_EQUAL(value, expected);
}

BC_AUTO_TEST_CASE(string__replace__shorter__expected)
{
    std::string value{ "abc def abc def" };
    const std::string expected{ "abc kl abc kl" };
    BC_REQUIRE_EQUAL(replace(value, "def", "kl"), 2u);
    BC_REQUIRE_EQUAL(value, expected);
}

BC_AUTO_TEST_CASE(string__replace__longer__expected)
{
    std::string value{ "abc def abc def" };
    const std::string expected{ "abc klmn abc klmn" };
    BC_REQUIRE_EQUAL(replace(value, "def", "klmn"), 2u);
    BC_REQUIRE_EQUAL(value, expected);
}

BC_AUTO_TEST_CASE(string__replace__reflexive__expected)
{
    std::string value{ "abc def abc def" };
    const std::string expected{ "abc defdef abc defdef" };
    BC_REQUIRE_EQUAL(replace(value, "def", "defdef"), 2u);
    BC_REQUIRE_EQUAL(value, expected);
}

BC_AUTO_TEST_CASE(string__replace__with_spaces__expected)
{
    std::string value{ "abc def abc def" };
    const std::string expected1{ " bc def  bc def" };
    BC_REQUIRE_EQUAL(replace(value, "a", " "), 2u);
    BC_REQUIRE_EQUAL(value, expected1);
    const std::string expected2{ "  c def   c def" };
    BC_REQUIRE_EQUAL(replace(value, "b", " "), 2u);
    BC_REQUIRE_EQUAL(value, expected2);
    const std::string expected3{ "    def     def" };
    BC_REQUIRE_EQUAL(replace(value, "c", " "), 2u);
    BC_REQUIRE_EQUAL(value, expected3);
    const string_list expected4{ "def", "", "", "", "", "def" };
    BC_REQUIRE_EQUAL(split(value, ascii_whitespace, ascii_whitespace, false), expected4);
}

// replace_copy

BC_AUTO_TEST_CASE(string__replace_copy__empty__empty)
{
    BC_REQUIRE_EQUAL(replace_copy("", "abc", "def"), "");
}

BC_AUTO_TEST_CASE(string__replace_copy__not_empty__expected)
{
    BC_REQUIRE_EQUAL(replace_copy("abc def abc def", "def", "defdef"), "abc defdef abc defdef");
}

#endif // STRING_REPLACE

#ifdef STRING_STARTS_WITH

BC_AUTO_TEST_CASE(string__starts_with__empty_empty__true)
{
    BC_REQUIRE(system::starts_with("", ""));
}

BC_AUTO_TEST_CASE(string__starts_with__ending__false)
{
    BC_REQUIRE(!system::starts_with("abc", "bc"));
}

BC_AUTO_TEST_CASE(string__starts_with__overflow__false)
{
    BC_REQUIRE(!system::starts_with("abc", "abcd"));
}

BC_AUTO_TEST_CASE(string__starts_with__found__true)
{
    BC_REQUIRE(system::starts_with("abc", "ab"));
}

#endif // STRING_STARTS_WITH

#ifdef STRING_ENDS_WITH

BC_AUTO_TEST_CASE(string__ends_with__empty_empty__true)
{
    BC_REQUIRE(system::ends_with("", ""));
}

BC_AUTO_TEST_CASE(string__ends_with__begining__false)
{
    BC_REQUIRE(!system::ends_with("abc", "ab"));
}

BC_AUTO_TEST_CASE(string__ends_with__overflow__false)
{
    BC_REQUIRE(!system::ends_with("abc", "dabc"));
}

BC_AUTO_TEST_CASE(string__ends_with__found__true)
{
    BC_REQUIRE(system::ends_with("abc", "bc"));
}

#endif // STRING_ENDS_WITH

BC_AUTO_TEST_SUITE_END()
