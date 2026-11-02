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
#include "dictionary.hpp"

BC_AUTO_TEST_SUITE(dictionary_tests)

using namespace test::mnemonics_dictionary;
using namespace bc::system::words;

BC_AUTO_TEST_CASE(dictionary__size__42__42)
{
    BC_REQUIRE_EQUAL(dictionary<42>::size(), 42u);
}

BC_AUTO_TEST_CASE(dictionary__identifier__empty__expected)
{
    BC_REQUIRE(dictionary<0>(language::en, {}).identifier() == language::en);
    BC_REQUIRE(dictionary<0>(language::es, {}).identifier() == language::es);
    BC_REQUIRE(dictionary<0>(language::it, {}).identifier() == language::it);
    BC_REQUIRE(dictionary<0>(language::fr, {}).identifier() == language::fr);
    BC_REQUIRE(dictionary<0>(language::cs, {}).identifier() == language::cs);
    BC_REQUIRE(dictionary<0>(language::pt, {}).identifier() == language::pt);
    BC_REQUIRE(dictionary<0>(language::ja, {}).identifier() == language::ja);
    BC_REQUIRE(dictionary<0>(language::ko, {}).identifier() == language::ko);
    BC_REQUIRE(dictionary<0>(language::zh_Hans, {}).identifier() == language::zh_Hans);
    BC_REQUIRE(dictionary<0>(language::zh_Hant, {}).identifier() == language::zh_Hant);
    BC_REQUIRE(dictionary<0>(language::none, {}).identifier() == language::none);
}

BC_AUTO_TEST_CASE(dictionary__name__empty__expected)
{
    BC_REQUIRE_EQUAL(dictionary<0>(language::en, {}).name(), "en");
    BC_REQUIRE_EQUAL(dictionary<0>(language::es, {}).name(), "es");
    BC_REQUIRE_EQUAL(dictionary<0>(language::it, {}).name(), "it");
    BC_REQUIRE_EQUAL(dictionary<0>(language::fr, {}).name(), "fr");
    BC_REQUIRE_EQUAL(dictionary<0>(language::cs, {}).name(), "cs");
    BC_REQUIRE_EQUAL(dictionary<0>(language::pt, {}).name(), "pt");
    BC_REQUIRE_EQUAL(dictionary<0>(language::ja, {}).name(), "ja");
    BC_REQUIRE_EQUAL(dictionary<0>(language::ko, {}).name(), "ko");
    BC_REQUIRE_EQUAL(dictionary<0>(language::zh_Hans, {}).name(), "zh_Hans");
    BC_REQUIRE_EQUAL(dictionary<0>(language::zh_Hant, {}).name(), "zh_Hant");
    BC_REQUIRE_EQUAL(dictionary<0>(language::none, {}).name(), "");
}

BC_AUTO_TEST_CASE(dictionary__at1__indexes__expected)
{
    BC_REQUIRE_EQUAL(instance.at(0), test_words_es.word[0]);
    BC_REQUIRE_EQUAL(instance.at(1), test_words_es.word[1]);
    BC_REQUIRE_EQUAL(instance.at(2), test_words_es.word[2]);
    BC_REQUIRE_EQUAL(instance.at(3), test_words_es.word[3]);
    BC_REQUIRE_EQUAL(instance.at(4), test_words_es.word[4]);
    BC_REQUIRE_EQUAL(instance.at(5), test_words_es.word[5]);
    BC_REQUIRE_EQUAL(instance.at(6), test_words_es.word[6]);
    BC_REQUIRE_EQUAL(instance.at(7), test_words_es.word[7]);
    BC_REQUIRE_EQUAL(instance.at(8), test_words_es.word[8]);
    BC_REQUIRE_EQUAL(instance.at(9), test_words_es.word[9]);
    BC_REQUIRE_EQUAL(instance.at(10), "");
}

BC_AUTO_TEST_CASE(dictionary__at2__indexes__expected)
{
    const auto words = instance.at({ 0, 2, 4, 6, 8, 10 });
    BC_REQUIRE_EQUAL(words[0], test_words_es.word[0]);
    BC_REQUIRE_EQUAL(words[1], test_words_es.word[2]);
    BC_REQUIRE_EQUAL(words[2], test_words_es.word[4]);
    BC_REQUIRE_EQUAL(words[3], test_words_es.word[6]);
    BC_REQUIRE_EQUAL(words[4], test_words_es.word[8]);
    BC_REQUIRE_EQUAL(words[5], "");
}

BC_AUTO_TEST_CASE(dictionary__index1__words__expected)
{
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[0]), 0);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[1]), 1);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[2]), 2);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[3]), 3);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[4]), 4);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[5]), 5);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[6]), 6);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[7]), 7);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[8]), 8);
    BC_REQUIRE_EQUAL(instance.index(test_words_es.word[9]), 9);
    BC_REQUIRE_EQUAL(instance.index(""), -1);
}

BC_AUTO_TEST_CASE(dictionary__index2__words__expected)
{
    const auto indexes = instance.index(
    {
        test_words_es.word[0],
        test_words_es.word[2],
        test_words_es.word[4],
        test_words_es.word[6],
        test_words_es.word[8],
        ""
    });
    BC_REQUIRE_EQUAL(indexes[0], 0);
    BC_REQUIRE_EQUAL(indexes[1], 2);
    BC_REQUIRE_EQUAL(indexes[2], 4);
    BC_REQUIRE_EQUAL(indexes[3], 6);
    BC_REQUIRE_EQUAL(indexes[4], 8);
    BC_REQUIRE_EQUAL(indexes[5], -1);
}

BC_AUTO_TEST_CASE(dictionary__contains1__words__expected)
{
    BC_REQUIRE(instance.contains(test_words_es.word[0]));
    BC_REQUIRE(instance.contains(test_words_es.word[1]));
    BC_REQUIRE(instance.contains(test_words_es.word[2]));
    BC_REQUIRE(instance.contains(test_words_es.word[3]));
    BC_REQUIRE(instance.contains(test_words_es.word[4]));
    BC_REQUIRE(instance.contains(test_words_es.word[5]));
    BC_REQUIRE(instance.contains(test_words_es.word[6]));
    BC_REQUIRE(instance.contains(test_words_es.word[7]));
    BC_REQUIRE(instance.contains(test_words_es.word[8]));
    BC_REQUIRE(instance.contains(test_words_es.word[9]));
    BC_REQUIRE(!instance.contains("foo"));
    BC_REQUIRE(!instance.contains("bar"));
    BC_REQUIRE(!instance.contains(""));
}

BC_AUTO_TEST_CASE(dictionary__contains1__empty__false)
{
    BC_REQUIRE(!instance.contains(""));
}

BC_AUTO_TEST_CASE(dictionary__contains2__valid_words__true)
{
    BC_REQUIRE(instance.contains(
    {
        test_words_es.word[0],
        test_words_es.word[2],
        test_words_es.word[4],
        test_words_es.word[6],
        test_words_es.word[8]
    }));
}

BC_AUTO_TEST_CASE(dictionary__contains2__invalid_word__false)
{
    BC_REQUIRE(!instance.contains(
    {
        test_words_es.word[0],
        test_words_es.word[2],
        "foo",
        test_words_es.word[6],
        test_words_es.word[8]
    }));
}

BC_AUTO_TEST_CASE(dictionary__contains2__empty__true)
{
    BC_REQUIRE(instance.contains(string_list{}));
}

BC_AUTO_TEST_SUITE_END()
