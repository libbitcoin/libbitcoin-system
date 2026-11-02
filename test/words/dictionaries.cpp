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
#include "dictionaries.hpp"

BC_AUTO_TEST_SUITE(dictionaries_tests)

using namespace test::mnemonics_dictionaries;
using namespace bc::system::words;

BC_AUTO_TEST_CASE(dictionaries__count__42__42)
{
    const auto count = dictionaries<42, 24>::count();
    BC_REQUIRE_EQUAL(count, 42u);
}

BC_AUTO_TEST_CASE(dictionaries__count__instance__expected)
{
    BC_REQUIRE_EQUAL(instance.count(), 5u);
}

BC_AUTO_TEST_CASE(dictionaries__size__42__42)
{
    const auto size = dictionaries<24, 42>::size();
    BC_REQUIRE_EQUAL(size, 42u);
}

BC_AUTO_TEST_CASE(dictionaries__size__instance__expected)
{
    BC_REQUIRE_EQUAL(instance.size(), 10u);
}

BC_AUTO_TEST_CASE(dictionaries__exists__valid_identifiers__true)
{
    BC_REQUIRE(instance.exists(language::en));
    BC_REQUIRE(instance.exists(language::es));
    BC_REQUIRE(instance.exists(language::ja));
    BC_REQUIRE(instance.exists(language::zh_Hans));
    BC_REQUIRE(instance.exists(language::zh_Hant));
}

BC_AUTO_TEST_CASE(dictionaries__exists__invalid_identifier__false)
{
    BC_REQUIRE(!instance.exists(language::ko));
}

BC_AUTO_TEST_CASE(dictionaries__to_identifier__valid_names__expected)
{
    BC_REQUIRE(instance.to_identifier("en") == language::en);
    BC_REQUIRE(instance.to_identifier("es") == language::es);
    BC_REQUIRE(instance.to_identifier("ja") == language::ja);
    BC_REQUIRE(instance.to_identifier("zh_Hans") == language::zh_Hans);
    BC_REQUIRE(instance.to_identifier("zh_Hant") == language::zh_Hant);
}

BC_AUTO_TEST_CASE(dictionaries__to_identifier__invalid_name__none)
{
    BC_REQUIRE(instance.to_identifier("ko") == language::none);
}

BC_AUTO_TEST_CASE(dictionaries__to_name__valid_identifiers__expected)
{
    BC_REQUIRE_EQUAL(instance.to_name(language::en), "en");
    BC_REQUIRE_EQUAL(instance.to_name(language::es), "es");
    BC_REQUIRE_EQUAL(instance.to_name(language::ja), "ja");
    BC_REQUIRE_EQUAL(instance.to_name(language::zh_Hans), "zh_Hans");
    BC_REQUIRE_EQUAL(instance.to_name(language::zh_Hant), "zh_Hant");
}

BC_AUTO_TEST_CASE(dictionaries__to_name__invalid_identifier__empty)
{
    BC_REQUIRE_EQUAL(instance.to_name(language::ko), "");
}

BC_AUTO_TEST_CASE(dictionaries__at1__valid_languages__expected)
{
    BC_REQUIRE_EQUAL(instance.at(0, language::en), test_words_en.word[0]);
    BC_REQUIRE_EQUAL(instance.at(1, language::es), test_words_es.word[1]);
    BC_REQUIRE_EQUAL(instance.at(2, language::ja), test_words_ja.word[2]);
    BC_REQUIRE_EQUAL(instance.at(3, language::zh_Hans), test_words_zh_Hans.word[3]);
    BC_REQUIRE_EQUAL(instance.at(4, language::zh_Hant), test_words_zh_Hant.word[4]);
}

BC_AUTO_TEST_CASE(dictionaries__at1__invalid_language__empty)
{
    BC_REQUIRE_EQUAL(instance.at(5, language::ko), "");
}

BC_AUTO_TEST_CASE(dictionaries__at1__invalid_index__empty)
{
    BC_REQUIRE_EQUAL(instance.at(10, language::ja), "");
}

BC_AUTO_TEST_CASE(dictionaries__at2__zh_Hans__expected)
{
    const auto words = instance.at({ 0, 2, 4, 6, 8, 10 }, language::zh_Hans);
    BC_REQUIRE_EQUAL(words[0], test_words_zh_Hans.word[0]);
    BC_REQUIRE_EQUAL(words[1], test_words_zh_Hans.word[2]);
    BC_REQUIRE_EQUAL(words[2], test_words_zh_Hans.word[4]);
    BC_REQUIRE_EQUAL(words[3], test_words_zh_Hans.word[6]);
    BC_REQUIRE_EQUAL(words[4], test_words_zh_Hans.word[8]);
    BC_REQUIRE_EQUAL(words[5], "");
}

BC_AUTO_TEST_CASE(dictionaries__at2__none__empty)
{
    const auto words = instance.at({ 0, 2, 4 }, language::none);
    BC_REQUIRE(words.empty());
}

BC_AUTO_TEST_CASE(dictionaries__index1__valid_words__expected)
{
    BC_REQUIRE_EQUAL(instance.index(test_words_en.word[0], language::en), 0);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[1], language::es), 1);
    BC_REQUIRE_EQUAL(instance.index(test_words_ja.word[2], language::ja), 2);
    BC_REQUIRE_EQUAL(instance.index(test_words_zh_Hans.word[3], language::zh_Hans), 3);
    BC_REQUIRE_EQUAL(instance.index(test_words_zh_Hant.word[4], language::zh_Hant), 4);
}

BC_AUTO_TEST_CASE(dictionaries__index1__incorrect_identifiers__not_found)
{
    BC_REQUIRE_EQUAL(instance.index(test_words_en.word[0], language::zh_Hant), -1);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[1], language::ja), -1);
    BC_REQUIRE_EQUAL(instance.index(test_words_ja.word[2], language::zh_Hans), -1);
    BC_REQUIRE_EQUAL(instance.index(test_words_zh_Hans.word[3], language::es), -1);
    BC_REQUIRE_EQUAL(instance.index(test_words_zh_Hant.word[4], language::en), -1);
}

BC_AUTO_TEST_CASE(dictionaries__index1__overlapping_wordlists__expected)
{
    // Order does not matter since these words overlap in the same position.
    BC_REQUIRE_EQUAL(instance.index(test_words_zh_Hans.word[3], language::zh_Hant), 3);
    BC_REQUIRE_EQUAL(instance.index(test_words_zh_Hant.word[3], language::zh_Hans), 3);
}

BC_AUTO_TEST_CASE(dictionaries__index1__invalid_identifier__not_found)
{
    BC_REQUIRE_EQUAL(instance.index(test_words_en.word[0], language::ko), -1);
}

BC_AUTO_TEST_CASE(dictionaries__index1__invalid_word__not_found)
{
    BC_REQUIRE_EQUAL(instance.index("", language::ja), -1);
}

BC_AUTO_TEST_CASE(dictionaries__index1__none_with_valid_word__not_found)
{
    BC_REQUIRE_EQUAL(instance.index(test_words_ja.word[0], language::none), -1);
}

BC_AUTO_TEST_CASE(dictionaries__index1__none_with_invalid_word__not_found)
{
    BC_REQUIRE_EQUAL(instance.index("foo", language::none), -1);
}

BC_AUTO_TEST_CASE(dictionaries__index2__words__expected)
{
    const auto indexes = instance.index(
    {
        test_words_es.word[0],
        test_words_es.word[2],
        test_words_es.word[4],
        test_words_es.word[6],
        test_words_es.word[8],
        test_words_zh_Hant.word[3]
    }, language::es);
    BC_REQUIRE_EQUAL(indexes[0], 0);
    BC_REQUIRE_EQUAL(indexes[1], 2);
    BC_REQUIRE_EQUAL(indexes[2], 4);
    BC_REQUIRE_EQUAL(indexes[3], 6);
    BC_REQUIRE_EQUAL(indexes[4], 8);
    BC_REQUIRE_EQUAL(indexes[5], -1);
}

BC_AUTO_TEST_CASE(dictionaries__index2__none__empty)
{
    const auto indexes = instance.index(
    {
        test_words_es.word[0],
        test_words_es.word[1],
        test_words_es.word[2]
    }, language::none);
    BC_REQUIRE(indexes.empty());
}

BC_AUTO_TEST_CASE(dictionaries__contains1__default_dictionary__expected)
{
    BC_REQUIRE(instance.contains(test_words_en.word[0]) == language::en);
    BC_REQUIRE(instance.contains(test_words_es.word[1]) == language::es);
    BC_REQUIRE(instance.contains(test_words_ja.word[2]) == language::ja);
    BC_REQUIRE(instance.contains("foo") == language::none);
    BC_REQUIRE(instance.contains("") == language::none);
}

BC_AUTO_TEST_CASE(dictionaries__contains1__explicit_dictionary__expected)
{
    BC_REQUIRE(instance.contains(test_words_ja.word[2], language::ja) == language::ja);
    BC_REQUIRE(instance.contains("foo", language::none) == language::none);
    BC_REQUIRE(instance.contains("", language::none) == language::none);
}

BC_AUTO_TEST_CASE(dictionaries__contains1__incorrect_explicit_dictionary__none)
{
    BC_REQUIRE(instance.contains(test_words_en.word[0], language::ja) == language::none);
}

BC_AUTO_TEST_CASE(dictionaries__contains1__invalid_explicit_dictionary__none)
{
    BC_REQUIRE(instance.contains(test_words_en.word[0], language::ko) == language::none);
}

BC_AUTO_TEST_CASE(dictionaries__contains1__redundant_wordlists__expected)
{
    // Search order is guaranteed, returns first match.
    const auto lingo = instance.contains(test_words_zh_Hans.word[3]);
    BC_REQUIRE(lingo == language::zh_Hans);
}

BC_AUTO_TEST_CASE(dictionaries__contains1__empty__none)
{
    BC_REQUIRE(instance.contains("") == language::none);
}

BC_AUTO_TEST_CASE(dictionaries__contains2__valid_words__expected)
{
    BC_REQUIRE(instance.contains(
    {
        test_words_es.word[0],
        test_words_es.word[2],
        test_words_es.word[4],
        test_words_es.word[6],
        test_words_es.word[8]
    }, language::es) == language::es);
}

BC_AUTO_TEST_CASE(dictionaries__contains2__valid_words_none__expected)
{
    BC_REQUIRE(instance.contains(
    {
        test_words_es.word[1],
        test_words_es.word[3],
        test_words_es.word[5],
        test_words_es.word[7],
        test_words_es.word[9]
    }, language::none) == language::es);
}

BC_AUTO_TEST_CASE(dictionaries__contains2__inconsistent_words_none__none)
{
    BC_REQUIRE(instance.contains(
    {
        test_words_es.word[1],
        test_words_es.word[3],
        test_words_zh_Hant.word[5],
        test_words_es.word[7],
        test_words_es.word[9]
    }, language::none) == language::none);
}

BC_AUTO_TEST_CASE(dictionaries__contains2__mismatched_identifier__none)
{
    BC_REQUIRE(instance.contains(
    {
        test_words_es.word[1],
        test_words_es.word[3],
        test_words_es.word[5],
        test_words_es.word[7],
        test_words_es.word[9]
    }, language::ja) == language::none);
}

BC_AUTO_TEST_CASE(dictionaries__contains2__empty__en)
{
    // This is based on the order of dictionary insertion (first).
    BC_REQUIRE(instance.contains(string_list{}) == language::en);
}

BC_AUTO_TEST_SUITE_END()
