/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(dictionary_tests)

using namespace bc::system::wallet;

const size_t test_dictionary_size = 10;
typedef std::array<const char*, test_dictionary_size> test_words;

// First 10 words of BIP39 es, first word not sorted for c++ < operator.
const test_words test_words_es
{
    {
        "ábaco",
        "abdomen",
        "abeja",
        "abierto",
        "abogado",
        "abono",
        "aborto",
        "abrazo",
        "abrir",
        "abuelo"
    }
};

// This is the instance under test.
dictionary<test_dictionary_size> instance(language::es, test_words_es);

BOOST_AUTO_TEST_CASE(dictionary__size__42__42)
{
    BOOST_REQUIRE_EQUAL(dictionary<42>::size(), 42u);
}

BOOST_AUTO_TEST_CASE(dictionary__identifier__empty__expected)
{
    BOOST_REQUIRE(dictionary<0>(language::en, {}).identifier() == language::en);
    BOOST_REQUIRE(dictionary<0>(language::es, {}).identifier() == language::es);
    BOOST_REQUIRE(dictionary<0>(language::it, {}).identifier() == language::it);
    BOOST_REQUIRE(dictionary<0>(language::fr, {}).identifier() == language::fr);
    BOOST_REQUIRE(dictionary<0>(language::cs, {}).identifier() == language::cs);
    BOOST_REQUIRE(dictionary<0>(language::pt, {}).identifier() == language::pt);
    BOOST_REQUIRE(dictionary<0>(language::ja, {}).identifier() == language::ja);
    BOOST_REQUIRE(dictionary<0>(language::ko, {}).identifier() == language::ko);
    BOOST_REQUIRE(dictionary<0>(language::zh_Hans, {}).identifier() == language::zh_Hans);
    BOOST_REQUIRE(dictionary<0>(language::zh_Hant, {}).identifier() == language::zh_Hant);
    BOOST_REQUIRE(dictionary<0>(language::none, {}).identifier() == language::none);
}

BOOST_AUTO_TEST_CASE(dictionary__name__empty__expected)
{
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::en, {}).name(), "en");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::es, {}).name(), "es");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::it, {}).name(), "it");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::fr, {}).name(), "fr");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::cs, {}).name(), "cs");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::pt, {}).name(), "pt");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::ja, {}).name(), "ja");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::ko, {}).name(), "ko");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::zh_Hans, {}).name(), "zh_Hans");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::zh_Hant, {}).name(), "zh_Hant");
    BOOST_REQUIRE_EQUAL(dictionary<0>(language::none, {}).name(), "none");
}

BOOST_AUTO_TEST_CASE(dictionary__at1__indexes__expected)
{
    BOOST_REQUIRE_EQUAL(instance.at(0), test_words_es[0]);
    BOOST_REQUIRE_EQUAL(instance.at(1), test_words_es[1]);
    BOOST_REQUIRE_EQUAL(instance.at(2), test_words_es[2]);
    BOOST_REQUIRE_EQUAL(instance.at(3), test_words_es[3]);
    BOOST_REQUIRE_EQUAL(instance.at(4), test_words_es[4]);
    BOOST_REQUIRE_EQUAL(instance.at(5), test_words_es[5]);
    BOOST_REQUIRE_EQUAL(instance.at(6), test_words_es[6]);
    BOOST_REQUIRE_EQUAL(instance.at(7), test_words_es[7]);
    BOOST_REQUIRE_EQUAL(instance.at(8), test_words_es[8]);
    BOOST_REQUIRE_EQUAL(instance.at(9), test_words_es[9]);
    BOOST_REQUIRE_EQUAL(instance.at(10), "");
}

BOOST_AUTO_TEST_CASE(dictionary__at2__indexes__expected)
{
    const auto words = instance.at({ 0, 2, 4, 6, 8, 10 });
    BOOST_REQUIRE_EQUAL(words[0], test_words_es[0]);
    BOOST_REQUIRE_EQUAL(words[1], test_words_es[2]);
    BOOST_REQUIRE_EQUAL(words[2], test_words_es[4]);
    BOOST_REQUIRE_EQUAL(words[3], test_words_es[6]);
    BOOST_REQUIRE_EQUAL(words[4], test_words_es[8]);
    BOOST_REQUIRE_EQUAL(words[5], "");
}

BOOST_AUTO_TEST_CASE(dictionary__index1__words__expected)
{
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[0]), 0);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[1]), 1);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[2]), 2);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[3]), 3);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[4]), 4);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[5]), 5);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[6]), 6);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[7]), 7);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[8]), 8);
    BOOST_REQUIRE_EQUAL(instance.index(test_words_es[9]), 9);
    BOOST_REQUIRE_EQUAL(instance.index(""), -1);
}

BOOST_AUTO_TEST_CASE(dictionary__index2__words__expected)
{
    const auto indexes = instance.index(
    {
        test_words_es[0],
        test_words_es[2],
        test_words_es[4],
        test_words_es[6],
        test_words_es[8],
        ""
    });
    BOOST_REQUIRE_EQUAL(indexes[0], 0);
    BOOST_REQUIRE_EQUAL(indexes[1], 2);
    BOOST_REQUIRE_EQUAL(indexes[2], 4);
    BOOST_REQUIRE_EQUAL(indexes[3], 6);
    BOOST_REQUIRE_EQUAL(indexes[4], 8);
    BOOST_REQUIRE_EQUAL(indexes[5], -1);
}

BOOST_AUTO_TEST_CASE(dictionary__contains1__words__expected)
{
    BOOST_REQUIRE(instance.contains(test_words_es[0]));
    BOOST_REQUIRE(instance.contains(test_words_es[1]));
    BOOST_REQUIRE(instance.contains(test_words_es[2]));
    BOOST_REQUIRE(instance.contains(test_words_es[3]));
    BOOST_REQUIRE(instance.contains(test_words_es[4]));
    BOOST_REQUIRE(instance.contains(test_words_es[5]));
    BOOST_REQUIRE(instance.contains(test_words_es[6]));
    BOOST_REQUIRE(instance.contains(test_words_es[7]));
    BOOST_REQUIRE(instance.contains(test_words_es[8]));
    BOOST_REQUIRE(instance.contains(test_words_es[9]));
    BOOST_REQUIRE(!instance.contains("foo"));
    BOOST_REQUIRE(!instance.contains("bar"));
    BOOST_REQUIRE(!instance.contains(""));
}

BOOST_AUTO_TEST_CASE(dictionary__contains2__valid_words__true)
{
    BOOST_REQUIRE(instance.contains(
    {
        test_words_es[0],
        test_words_es[2],
        test_words_es[4],
        test_words_es[6],
        test_words_es[8]
    }));
}

BOOST_AUTO_TEST_CASE(dictionary__contains2__invalid_word__false)
{
    BOOST_REQUIRE(!instance.contains(
    {
        test_words_es[0],
        test_words_es[2],
        "foo",
        test_words_es[6],
        test_words_es[8]
    }));
}

BOOST_AUTO_TEST_SUITE_END()
