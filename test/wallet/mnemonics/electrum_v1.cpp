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
#include "../../test.hpp"
#include <sstream>
#include "electrum_v1.hpp"

BOOST_AUTO_TEST_SUITE(electrum_v1_tests)

using namespace bc::system::wallet;

// statics

BOOST_AUTO_TEST_CASE(electrum_v1__contained_by__empty_any__true)
{
    // This is based on the order of dictionary insertion (first).
    BOOST_REQUIRE(electrum_v1::contained_by({}) == language::en);
}

BOOST_AUTO_TEST_CASE(electrum_v1__contained_by__invalid_any__none)
{
    BOOST_REQUIRE(electrum_v1::contained_by({ "foo" }) == language::none);
}

BOOST_AUTO_TEST_CASE(electrum_v1__contained_by__yellow_lotus__none)
{
    // en and pt words
    BOOST_REQUIRE(electrum_v1::contained_by({ "yellow", "lotus" }) == language::none);
}

BOOST_AUTO_TEST_CASE(electrum_v1__contained_by__blind_faith__en)
{
    BOOST_REQUIRE(electrum_v1::contained_by({ "blind", "faith" }) == language::en);
}

BOOST_AUTO_TEST_CASE(electrum_v1__is_valid_dictionary__contained__true)
{
    BOOST_REQUIRE(electrum_v1::is_valid_dictionary(language::en));
    BOOST_REQUIRE(electrum_v1::is_valid_dictionary(language::pt));
}

BOOST_AUTO_TEST_CASE(electrum_v1__is_valid_dictionary__uncontained__false)
{
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::es));
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::it));
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::fr));
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::cs));
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::ja));
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::ko));
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::zh_Hans));
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::zh_Hant));
    BOOST_REQUIRE(!electrum_v1::is_valid_dictionary(language::none));
}

BOOST_AUTO_TEST_CASE(electrum_v1__is_valid_entropy_size__valid__true)
{
    BOOST_REQUIRE(electrum_v1::is_valid_entropy_size(16));
    BOOST_REQUIRE(electrum_v1::is_valid_entropy_size(32));
}

BOOST_AUTO_TEST_CASE(electrum_v1__is_valid_entropy_size__invalid__false)
{
    BOOST_REQUIRE(!electrum_v1::is_valid_entropy_size(0));
    BOOST_REQUIRE(!electrum_v1::is_valid_entropy_size(8));
    BOOST_REQUIRE(!electrum_v1::is_valid_entropy_size(15));
    BOOST_REQUIRE(!electrum_v1::is_valid_entropy_size(17));
    BOOST_REQUIRE(!electrum_v1::is_valid_entropy_size(31));
    BOOST_REQUIRE(!electrum_v1::is_valid_entropy_size(33));
    BOOST_REQUIRE(!electrum_v1::is_valid_entropy_size(64));
}

BOOST_AUTO_TEST_CASE(electrum_v1__is_valid_word_count__valid__true)
{
    BOOST_REQUIRE(electrum_v1::is_valid_word_count(12));
    BOOST_REQUIRE(electrum_v1::is_valid_word_count(24));
}

BOOST_AUTO_TEST_CASE(electrum_v1__is_valid_word_count__invalid__false)
{
    BOOST_REQUIRE(!electrum_v1::is_valid_word_count(0));
    BOOST_REQUIRE(!electrum_v1::is_valid_word_count(3));
    BOOST_REQUIRE(!electrum_v1::is_valid_word_count(6));
    BOOST_REQUIRE(!electrum_v1::is_valid_word_count(9));
    BOOST_REQUIRE(!electrum_v1::is_valid_word_count(15));
    BOOST_REQUIRE(!electrum_v1::is_valid_word_count(18));
    BOOST_REQUIRE(!electrum_v1::is_valid_word_count(27));
}

// construct default

BOOST_AUTO_TEST_CASE(electrum_v1__construct_default__always__invalid)
{
    BOOST_REQUIRE(!electrum_v1());
}

// construct copy

BOOST_AUTO_TEST_CASE(electrum_v1__construct_copy__always__expected)
{
    const auto sentence1 =
        "blind faith blind faith blind faith blind faith blind faith blind faith";

    electrum_v1 instance1(sentence1);
    BOOST_REQUIRE(instance1);

    electrum_v1 instance2(instance1);
    BOOST_REQUIRE(instance2);

    BOOST_REQUIRE_EQUAL(instance2.sentence(), sentence1);
    BOOST_REQUIRE_EQUAL(instance2.entropy(), instance1.entropy());
    BOOST_REQUIRE(instance2.lingo() == instance1.lingo());
}

// construct move (default)

BOOST_AUTO_TEST_CASE(electrum_v1__construct_move__always__expected)
{
    const auto sentence1 =
        "blind faith blind faith blind faith blind faith blind faith blind faith";

    electrum_v1 instance1(sentence1);
    BOOST_REQUIRE(instance1);

    electrum_v1 instance2(std::move(instance1));
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), sentence1);
}

// construct sentence

BOOST_AUTO_TEST_CASE(electrum_v1__construct_sentence__empty_sentence__invalid)
{
    BOOST_REQUIRE(!electrum_v1(""));
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_sentence__2_word_sentence__invalid_empty)
{
    electrum_v1 instance("blind faith");
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE(instance.sentence().empty());
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_sentence__12_word_sentence__valid_expected)
{
    const auto sentence =
        "blind faith blind faith blind faith blind faith blind faith blind faith";
    electrum_v1 instance(sentence);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.sentence(), sentence);
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_sentence__24_word_sentence__valid_expected)
{
    const auto sentence =
        "blind faith blind faith blind faith blind faith blind faith blind faith "
        "blind faith blind faith blind faith blind faith blind faith blind faith";
    electrum_v1 instance(sentence);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.sentence(), sentence);
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_sentence__26_word_sentence__valid_expected)
{
    const auto sentence =
        "blind faith blind faith blind faith blind faith blind faith blind faith "
        "blind faith blind faith blind faith blind faith blind faith blind faith "
        "blind faith";
    electrum_v1 instance(sentence);
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE(instance.sentence().empty());
}

// construct words

BOOST_AUTO_TEST_CASE(electrum_v1__construct_words__empty__invalid)
{
    BOOST_REQUIRE(!electrum_v1(string_list{}));
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_words__2_words__invalid)
{
    electrum_v1 instance("blind faith");
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE(instance.words().empty());
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_words__12_words__valid_expected)
{
    const string_list words
    {
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith"
    };

    electrum_v1 instance(words);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.words(), words);
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_words__24_words__valid_expected)
{
    const string_list words
    {
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith"
    };

    electrum_v1 instance(words);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.words(), words);
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_words__26_words__valid_expected)
{
    const string_list words
    {
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith"
    };

    electrum_v1 instance(words);
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE(instance.words().empty());
}

// construct entropy

BOOST_AUTO_TEST_CASE(electrum_v1__construct_entropy___empty__invalid)
{
    BOOST_REQUIRE(!electrum_v1(data_chunk{}));
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_entropy___8_bytes__invalid)
{
    const data_chunk entropy(8, 0x42);
    electrum_v1 instance(entropy);
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE(instance.entropy().empty());
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_entropy___16_bytes__valid_expected)
{
    const data_chunk entropy(16, 0x42);
    electrum_v1 instance(entropy);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.entropy(), entropy);
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_entropy___32_bytes__valid_expected)
{
    const data_chunk entropy(32, 0x42);
    electrum_v1 instance(entropy);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.entropy(), entropy);
}

BOOST_AUTO_TEST_CASE(electrum_v1__construct_entropy___64_bytes__invalid)
{
    const data_chunk entropy(64, 0x42);
    electrum_v1 instance(entropy);
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE(instance.entropy().empty());
}

// construct minimum_entropy

BOOST_AUTO_TEST_CASE(electrum_v1__construct_minimum_entropy__always__valid_expected)
{
    const electrum_v1::minimum_entropy entropy{ 0x42 };
    electrum_v1 instance(entropy);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.entropy(), to_chunk(entropy));
}

// construct maximum_entropy

BOOST_AUTO_TEST_CASE(electrum_v1__construct_maximum_entropy___always__valid_expected)
{
    const electrum_v1::maximum_entropy entropy{ 0x42 };
    electrum_v1 instance(entropy);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.entropy(), to_chunk(entropy));
}

// assign (default)

BOOST_AUTO_TEST_CASE(electrum_v1__assign_copy__always__expected)
{
    const auto sentence1 =
        "blind faith blind faith blind faith blind faith blind faith blind faith";

    const electrum_v1 instance1(sentence1);
    electrum_v1 instance2;
    instance2 = instance1;
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), instance1.entropy());
    BOOST_REQUIRE(instance2.lingo() == instance1.lingo());
}

BOOST_AUTO_TEST_CASE(electrum_v1__assign_move__always__expected)
{
    const auto sentence1 =
        "blind faith blind faith blind faith blind faith blind faith blind faith";

    electrum_v1 instance1(sentence1);
    electrum_v1 instance2;
    instance2 = std::move(instance1);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), instance1.entropy());
    BOOST_REQUIRE(instance2.lingo() == instance1.lingo());
}

// in/equality (default)

BOOST_AUTO_TEST_CASE(electrum_v1__equality__always__expected)
{
    const auto sentence1 =
        "blind faith blind faith blind faith blind faith blind faith blind faith";

    const electrum_v1 instance1(sentence1);
    const electrum_v1 instance2(sentence1);
    BOOST_REQUIRE(instance1 == instance2);
}

BOOST_AUTO_TEST_CASE(electrum_v1__inequality_move__always__expected)
{
    const auto sentence =
        "blind faith blind faith blind faith blind faith blind faith blind faith";

    const string_list words
    {
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith"
    };

    const electrum_v1 instance1(sentence);
    const electrum_v1 instance2(words);
    BOOST_REQUIRE(instance1 != instance2);
}

// operator>>

BOOST_AUTO_TEST_CASE(electrum_v1__deserialize___valid_whitespace__expected_trimmed)
{
    const auto sentence = "\tblind \nfaith \vblind faith blind faith \x20 blind faith blind faith blind\f faith\r";
    const auto expected = "blind faith blind faith blind faith blind faith blind faith blind faith";
    std::istringstream in{ sentence };
    electrum_v1 instance;
    in >> instance;
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE_EQUAL(instance.sentence(), expected);
}

BOOST_AUTO_TEST_CASE(electrum_v1__deserialize__invalid__invalid)
{
    std::istringstream in{ "foobar" };
    electrum_v1 instance;
    in >> instance;
    BOOST_REQUIRE(!instance);
}

// operator<<

BOOST_AUTO_TEST_CASE(electrum_v1__serialize___valid_whitespace__expected)
{
    const auto sentence = "\tblind \nfaith \vblind faith blind faith \x20 blind faith blind faith blind\f faith\r";
    const auto expected = "blind faith blind faith blind faith blind faith blind faith blind faith";
    std::ostringstream out;
    electrum_v1 instance(sentence);
    out << instance;
    BOOST_REQUIRE_EQUAL(out.str(), expected);
}

BOOST_AUTO_TEST_CASE(electrum_v1__serialize__invalid__invalid)
{
    std::ostringstream out;
    electrum_v1 instance;
    BOOST_REQUIRE(!instance);
    out << instance;
    BOOST_REQUIRE(out.str().empty());
}

BOOST_AUTO_TEST_SUITE_END()
