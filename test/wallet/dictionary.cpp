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
}

BOOST_AUTO_TEST_CASE(dictionary__at1__spanish__expected)
{
    dictionary<test_dictionary_size> instance(language::es, test_words_es);
    BOOST_REQUIRE_EQUAL(instance.at(0), "ábaco");
    BOOST_REQUIRE_EQUAL(instance.at(1), "abdomen");
    BOOST_REQUIRE_EQUAL(instance.at(2), "abeja");
    BOOST_REQUIRE_EQUAL(instance.at(3), "abierto");
    BOOST_REQUIRE_EQUAL(instance.at(4), "abogado");
    BOOST_REQUIRE_EQUAL(instance.at(5), "abono");
    BOOST_REQUIRE_EQUAL(instance.at(6), "aborto");
    BOOST_REQUIRE_EQUAL(instance.at(7), "abrazo");
    BOOST_REQUIRE_EQUAL(instance.at(8), "abrir");
    BOOST_REQUIRE_EQUAL(instance.at(9), "abuelo");
    BOOST_REQUIRE_EQUAL(instance.at(10), "");
}

BOOST_AUTO_TEST_CASE(dictionary__at2__spanish__expected)
{
    dictionary<test_dictionary_size> instance(language::es, test_words_es);
    const auto words = instance.at({ 0, 2, 4, 6, 8, 10 });
    BOOST_REQUIRE_EQUAL(words[0], "ábaco");
    BOOST_REQUIRE_EQUAL(words[1], "abeja");
    BOOST_REQUIRE_EQUAL(words[2], "abogado");
    BOOST_REQUIRE_EQUAL(words[3], "aborto");
    BOOST_REQUIRE_EQUAL(words[4], "abrir");
    BOOST_REQUIRE_EQUAL(words[5], "");
}

BOOST_AUTO_TEST_CASE(dictionary__index1__spanish__expected)
{
    dictionary<test_dictionary_size> instance(language::es, test_words_es);
    BOOST_REQUIRE_EQUAL(instance.index("ábaco"), 0);
    BOOST_REQUIRE_EQUAL(instance.index("abdomen"), 1);
    BOOST_REQUIRE_EQUAL(instance.index("abeja"), 2);
    BOOST_REQUIRE_EQUAL(instance.index("abierto"), 3);
    BOOST_REQUIRE_EQUAL(instance.index("abogado"), 4);
    BOOST_REQUIRE_EQUAL(instance.index("abono"), 5);
    BOOST_REQUIRE_EQUAL(instance.index("aborto"), 6);
    BOOST_REQUIRE_EQUAL(instance.index("abrazo"), 7);
    BOOST_REQUIRE_EQUAL(instance.index("abrir"), 8);
    BOOST_REQUIRE_EQUAL(instance.index("abuelo"), 9);
    BOOST_REQUIRE_EQUAL(instance.index(""), -1);
}

BOOST_AUTO_TEST_CASE(dictionary__index2__spanish__expected)
{
    dictionary<test_dictionary_size> instance(language::es, test_words_es);
    const auto indexes = instance.index({ "ábaco", "abeja", "abogado", "aborto", "abrir", "" });
    BOOST_REQUIRE_EQUAL(indexes[0], 0);
    BOOST_REQUIRE_EQUAL(indexes[1], 2);
    BOOST_REQUIRE_EQUAL(indexes[2], 4);
    BOOST_REQUIRE_EQUAL(indexes[3], 6);
    BOOST_REQUIRE_EQUAL(indexes[4], 8);
    BOOST_REQUIRE_EQUAL(indexes[5], -1);
}

BOOST_AUTO_TEST_CASE(dictionary__contains1__spanish__expected)
{
    dictionary<test_dictionary_size> instance(language::es, test_words_es);
    BOOST_REQUIRE(instance.contains("ábaco"));
    BOOST_REQUIRE(instance.contains("abdomen"));
    BOOST_REQUIRE(instance.contains("abeja"));
    BOOST_REQUIRE(instance.contains("abierto"));
    BOOST_REQUIRE(instance.contains("abogado"));
    BOOST_REQUIRE(instance.contains("abono"));
    BOOST_REQUIRE(instance.contains("aborto"));
    BOOST_REQUIRE(instance.contains("abrazo"));
    BOOST_REQUIRE(instance.contains("abrir"));
    BOOST_REQUIRE(instance.contains("abuelo"));
    BOOST_REQUIRE(!instance.contains("foo"));
    BOOST_REQUIRE(!instance.contains("bar"));
    BOOST_REQUIRE(!instance.contains(""));
}

BOOST_AUTO_TEST_CASE(dictionary__contains2__spanish__expected)
{
    dictionary<test_dictionary_size> instance(language::es, test_words_es);
    BOOST_REQUIRE(instance.contains({ "abeja", "abogado", "ábaco", "aborto", "abrir" }));
    BOOST_REQUIRE(!instance.contains({ "ábaco", "foo", "abogado", "", "abrir", "bar" }));
}

BOOST_AUTO_TEST_SUITE_END()
