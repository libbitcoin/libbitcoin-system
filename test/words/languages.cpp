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
#include "languages.hpp"

BC_AUTO_TEST_SUITE(languages_tests)

using namespace test::mnemonics_languages;
using namespace bc::system::words;

// statics

BC_AUTO_TEST_CASE(languages__from_name__always__expected)
{
    BC_REQUIRE(languages::from_name("en") == language::en);
    BC_REQUIRE(languages::from_name("es") == language::es);
    BC_REQUIRE(languages::from_name("it") == language::it);
    BC_REQUIRE(languages::from_name("fr") == language::fr);
    BC_REQUIRE(languages::from_name("cs") == language::cs);
    BC_REQUIRE(languages::from_name("pt") == language::pt);
    BC_REQUIRE(languages::from_name("ja") == language::ja);
    BC_REQUIRE(languages::from_name("ko") == language::ko);
    BC_REQUIRE(languages::from_name("zh_Hans") == language::zh_Hans);
    BC_REQUIRE(languages::from_name("zh_Hant") == language::zh_Hant);
}

BC_AUTO_TEST_CASE(languages__from_name__invalid__none)
{
    BC_REQUIRE(languages::from_name("foo") == language::none);
    BC_REQUIRE(languages::from_name("") == language::none);
}

BC_AUTO_TEST_CASE(languages__to_name__all__expected)
{
    BC_REQUIRE_EQUAL(languages::to_name(language::en), "en");
    BC_REQUIRE_EQUAL(languages::to_name(language::es), "es");
    BC_REQUIRE_EQUAL(languages::to_name(language::it), "it");
    BC_REQUIRE_EQUAL(languages::to_name(language::fr), "fr");
    BC_REQUIRE_EQUAL(languages::to_name(language::cs), "cs");
    BC_REQUIRE_EQUAL(languages::to_name(language::pt), "pt");
    BC_REQUIRE_EQUAL(languages::to_name(language::ja), "ja");
    BC_REQUIRE_EQUAL(languages::to_name(language::ko), "ko");
    BC_REQUIRE_EQUAL(languages::to_name(language::zh_Hans), "zh_Hans");
    BC_REQUIRE_EQUAL(languages::to_name(language::zh_Hant), "zh_Hant");
    BC_REQUIRE_EQUAL(languages::to_name(language::none), "");
}

BC_AUTO_TEST_CASE(languages__to_delimiter__non_japanese__space)
{
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::en), ascii_space);
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::es), ascii_space);
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::it), ascii_space);
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::fr), ascii_space);
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::cs), ascii_space);
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::pt), ascii_space);
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::ko), ascii_space);
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::zh_Hans), ascii_space);
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::zh_Hant), ascii_space);
}

BC_AUTO_TEST_CASE(languages__to_delimiter__japanese__ideographic_space)
{
    BC_REQUIRE_EQUAL(languages::to_delimiter(language::ja), ideographic_space);
}

BC_AUTO_TEST_CASE(languages__join__korean__space_delimited)
{
    const auto expected =
        std::string(test_words_zh_Hant.word[0]) + ascii_space +
        test_words_es.word[1] + ascii_space +
        test_words_zh_Hant.word[2] + ascii_space +
        test_words_es.word[3];

    const string_list words
    {
        test_words_zh_Hant.word[0],
        test_words_es.word[1],
        test_words_zh_Hant.word[2],
        test_words_es.word[3]
    };

    BC_REQUIRE_EQUAL(languages::join(words, language::ko), expected);
}

BC_AUTO_TEST_CASE(languages__join__japanese__ideographic_space_delimited)
{
    const auto expected =
        std::string(test_words_zh_Hant.word[0]) + ideographic_space +
        test_words_es.word[1] + ideographic_space +
        test_words_zh_Hant.word[2] + ideographic_space +
        test_words_es.word[3];

    const string_list words
    {
        test_words_zh_Hant.word[0],
        test_words_es.word[1],
        test_words_zh_Hant.word[2],
        test_words_es.word[3]
    };

    BC_REQUIRE_EQUAL(languages::join(words, language::ja), expected);
}

BC_AUTO_TEST_CASE(languages__split__korean_ascii_space_delimited__split)
{
    const string_list expected
    {
        test_words_zh_Hans.word[0],
        test_words_es.word[1],
        test_words_zh_Hans.word[2],
        test_words_es.word[3]
    };

    const auto sentence =
        std::string(test_words_zh_Hans.word[0]) + ascii_space +
        test_words_es.word[1] + ascii_space +
        test_words_zh_Hans.word[2] + ascii_space +
        test_words_es.word[3];

    BC_REQUIRE_EQUAL(languages::split(sentence, language::ko), expected);
}

// The space character is the only ascii whitespace that is also a separator.
BC_AUTO_TEST_CASE(languages__split__korean_non_separator_whitespace_delimited__unsplit)
{
    const auto sentence =
        std::string(test_words_zh_Hans.word[0]) + '\t' +
        test_words_es.word[1] + '\f' +
        test_words_zh_Hans.word[2] + '\n' +
        test_words_es.word[3];

    const string_list expected{ sentence };

    BC_REQUIRE_EQUAL(languages::split(sentence, language::ko), expected);
}

// Language splitting applies all unicode separator characters.
BC_AUTO_TEST_CASE(languages__split__japanese_ascii_space_delimited__split)
{
    const string_list expected
    {
        test_words_zh_Hant.word[0],
        test_words_es.word[1],
        test_words_zh_Hant.word[2],
        test_words_es.word[3]
    };

    const auto sentence =
        std::string(test_words_zh_Hans.word[0]) + ascii_space +
        test_words_es.word[1] + ascii_space +
        test_words_zh_Hans.word[2] + ascii_space +
        test_words_es.word[3];

    BC_REQUIRE_EQUAL(languages::split(sentence, language::ja), expected);
}

BC_AUTO_TEST_CASE(languages__split__japanese_ideographic_space_space_delimited__split)
{
    const string_list expected
    {
        test_words_zh_Hans.word[0],
        test_words_es.word[1],
        test_words_zh_Hans.word[2],
        test_words_es.word[3]
    };

    const auto sentence =
        std::string(test_words_zh_Hans.word[0]) + ideographic_space +
        test_words_es.word[1] + ideographic_space +
        test_words_zh_Hans.word[2] + ideographic_space +
        test_words_es.word[3];

    BC_REQUIRE_EQUAL(languages::split(sentence, language::ja), expected);
}

BC_AUTO_TEST_CASE(languages__try_normalize__empty__empty)
{
    const string_list words{};
    const string_list expected{};
    BC_REQUIRE_EQUAL(accessor::try_normalize(words), expected);
}

BC_AUTO_TEST_CASE(languages__try_normalize__lower_ascii__unchanged)
{
    const string_list words{ "abc", "def", "xyz" };
    BC_REQUIRE_EQUAL(accessor::try_normalize(words), words);
}

BC_AUTO_TEST_CASE(languages__try_normalize__mixed_ascii__lowered)
{
    const string_list words{ "aBc", "DeF", "xYz" };
    const string_list expected{ "abc", "def", "xyz" };
    BC_REQUIRE_EQUAL(accessor::try_normalize(words), expected);
}

BC_AUTO_TEST_CASE(languages__try_normalize__padded_mixed_ascii__lowered_trimmed)
{
    const string_list words{ " aBc ", "  DeF  ", "\t\r\nxYz\f\v" };
    const string_list expected{ "abc", "def", "xyz" };
    BC_REQUIRE_EQUAL(accessor::try_normalize(words), expected);
}

BC_AUTO_TEST_CASE(languages__try_normalize__with_icu__lowered_normalized)
{
    string_list words{ base16_string("c3a16261636f"), "XyZ" };
    const string_list expected{ base16_string("61cc816261636f"), ascii_to_lower(words[1]) };
    const auto result = accessor::try_normalize(words);
    BC_REQUIRE_EQUAL(result, expected);
}

// construct/properties/bool

BC_AUTO_TEST_CASE(languages__default_construct__always__invalid_expected)
{
    accessor instance;
    BC_REQUIRE(!instance);
    BC_REQUIRE(instance.entropy().empty());
    BC_REQUIRE(instance.words().empty());
    BC_REQUIRE(instance.lingo() == language::none);
}

BC_AUTO_TEST_CASE(languages__copy_construct__always__copies_all_accessor_and_base_values)
{
    const auto value = "value";
    const accessor instance1({ 42, 42, 42 }, { "foo", "foo", "foo" }, language::zh_Hant, value);
    const accessor instance2(instance1);
    BC_REQUIRE(instance1 && instance2);
    BC_REQUIRE_EQUAL(instance1.entropy(), instance2.entropy());
    BC_REQUIRE_EQUAL(instance1.words(), instance2.words());
    BC_REQUIRE(instance1.lingo() == instance2.lingo());
    BC_REQUIRE_EQUAL(instance1.value(), instance2.value());
    BC_REQUIRE_EQUAL(instance1.value(), value);
}

BC_AUTO_TEST_CASE(languages__values_construct__empty_entropy__invalid_expected)
{
    const string_list words{ "foo" };
    accessor instance({}, words, language::en);
    BC_REQUIRE(!instance);

    // This is why languages is a protected class, there are no consistency guards.
    BC_REQUIRE(instance.entropy().empty());
    BC_REQUIRE_EQUAL(instance.words(), words);
    BC_REQUIRE(instance.lingo() == language::en);
}

BC_AUTO_TEST_CASE(languages__values_construct__empty_words__valid_expected)
{
    const data_chunk entropy{ 42, 42 };
    accessor instance(entropy, {}, language::none);
    BC_REQUIRE(instance);

    // This is why languages is a protected class, there are no consistency guards.
    BC_REQUIRE_EQUAL(instance.entropy(), entropy);
    BC_REQUIRE(instance.words().empty());
    BC_REQUIRE(instance.lingo() == language::none);
}

BC_AUTO_TEST_CASE(languages__values_construct__not_empty__valid_expected)
{
    const data_chunk entropy{ 42, 42 };
    const string_list words{ "foo", "bar" };
    accessor instance(entropy, words, language::none);
    BC_REQUIRE(instance);

    // This is why languages is a protected class, there are no consistency guards.
    BC_REQUIRE_EQUAL(instance.entropy(), entropy);
    BC_REQUIRE_EQUAL(instance.words(), words);
    BC_REQUIRE(instance.lingo() == language::none);
}

// sentence

BC_AUTO_TEST_CASE(languages__sentence__empty_words__empty)
{
    accessor instance({}, {}, language::none);
    BC_REQUIRE(instance.sentence().empty());
}

BC_AUTO_TEST_CASE(languages__sentence__words_not_japanese__ascii_delimited)
{
    const string_list words{ "foo", "bar" };
    accessor instance({}, words, language::es);
    BC_REQUIRE_EQUAL(instance.sentence(), "foo bar");
}

BC_AUTO_TEST_CASE(languages__sentence__words_japanese__ideographic_space_delimited)
{
    const auto expected = std::string("foo") + ideographic_space + "bar";
    const string_list words{ "foo", "bar" };
    accessor instance({}, words, language::ja);
    BC_REQUIRE_EQUAL(instance.sentence(), expected);
}

// operators

BC_AUTO_TEST_CASE(languages__assignment__always__copies_all_accessor_and_base_values)
{
    const auto value = "value";
    const accessor instance1({ 42, 42, 42 }, { "foo", "foo", "foo" }, language::zh_Hant, value);
    const accessor instance2 = instance1;
    BC_REQUIRE(instance1 && instance2);
    BC_REQUIRE_EQUAL(instance1.entropy(), instance2.entropy());
    BC_REQUIRE_EQUAL(instance1.words(), instance2.words());
    BC_REQUIRE(instance1.lingo() == instance2.lingo());
    BC_REQUIRE_EQUAL(instance1.value(), instance2.value());
    BC_REQUIRE_EQUAL(instance1.value(), value);
}

BC_AUTO_TEST_CASE(languages__less_than__same_same_words__expected)
{
    // Entropy has no effect on less than operator.
    const string_list words{ "a", "b", "c" };
    accessor instance1({ 42 }, words, language::none);
    accessor instance2({}, words, language::none);
    BC_REQUIRE(!(instance1 < instance2));
    BC_REQUIRE(!(instance2 < instance1));
    BC_REQUIRE(!(instance1 < instance1));
    BC_REQUIRE(!(instance2 < instance2));
}

BC_AUTO_TEST_CASE(languages__less_than__same_delimited_distinct_words__expected)
{
    const string_list words1{ "a", "b", "c" };
    const string_list words2{ "b", "c" };
    accessor instance1({}, words1, language::es);
    accessor instance2({}, words2, language::es);
    BC_REQUIRE(instance1 < instance2);
    BC_REQUIRE(!(instance2 < instance1));
    BC_REQUIRE(!(instance1 < instance1));
    BC_REQUIRE(!(instance2 < instance2));
}

BC_AUTO_TEST_CASE(languages__less_than__differently_delimited_same_words__expected)
{
    // ideographic_space > ascii_space
    const string_list words{ "a", "b", "c" };
    accessor instance1({}, words, language::es);
    accessor instance2({}, words, language::ja);
    BC_REQUIRE(instance1 < instance2);
    BC_REQUIRE(!(instance2 < instance1));
    BC_REQUIRE(!(instance1 < instance1));
    BC_REQUIRE(!(instance2 < instance2));
}

BC_AUTO_TEST_CASE(languages__equality__same__expected)
{
    const data_chunk entropy{ 0x01, 0x02, 0x03 };
    const string_list words{ "a", "b", "c" };
    accessor instance1(entropy, words, language::es);
    accessor instance2(entropy, words, language::es);
    BC_REQUIRE(instance1 == instance2);
    BC_REQUIRE(instance2 == instance1);
    BC_REQUIRE(instance1 == instance1);
    BC_REQUIRE(instance2 == instance2);
}

BC_AUTO_TEST_CASE(languages__equality__distinct_entropy__expected)
{
    const string_list words{ "a", "b", "c" };
    accessor instance1({}, words, language::es);
    accessor instance2({ 0x01, 0x02, 0x03 }, words, language::ja);
    BC_REQUIRE(!(instance1 == instance2));
    BC_REQUIRE(!(instance2 == instance1));
    BC_REQUIRE(instance1 == instance1);
    BC_REQUIRE(instance2 == instance2);
}

BC_AUTO_TEST_CASE(languages__equality__distinct_words__expected)
{
    const data_chunk entropy{ 0x01, 0x02, 0x03 };
    accessor instance1(entropy, {}, language::es);
    accessor instance2(entropy, { "a", "b", "c" }, language::ja);
    BC_REQUIRE(!(instance1 == instance2));
    BC_REQUIRE(!(instance2 == instance1));
    BC_REQUIRE(instance1 == instance1);
    BC_REQUIRE(instance2 == instance2);
}

BC_AUTO_TEST_CASE(languages__equality__distinct_language__expected)
{
    const data_chunk entropy{ 0x01, 0x02, 0x03 };
    const string_list words{ "a", "b", "c" };
    accessor instance1(entropy, words, language::es);
    accessor instance2(entropy, words, language::ja);
    BC_REQUIRE(!(instance1 == instance2));
    BC_REQUIRE(!(instance2 == instance1));
    BC_REQUIRE(instance1 == instance1);
    BC_REQUIRE(instance2 == instance2);
}

BC_AUTO_TEST_CASE(languages__inequality__distinct_entropy__expected)
{
    const string_list words{ "a", "b", "c" };
    accessor instance2({}, words, language::ja);
    accessor instance1({ 0x01, 0x02, 0x03 }, words, language::ja);
    BC_REQUIRE(instance1 != instance2);
    BC_REQUIRE(instance2 != instance1);
    BC_REQUIRE(!(instance1 != instance1));
    BC_REQUIRE(!(instance2 != instance2));
}

BC_AUTO_TEST_CASE(languages__inequality__distinct_words__expected)
{
    const data_chunk entropy{ 0x01, 0x02, 0x03 };
    accessor instance1(entropy, {}, language::ja);
    accessor instance2(entropy, { "a", "b", "c" }, language::ja);
    BC_REQUIRE(instance1 != instance2);
    BC_REQUIRE(instance2 != instance1);
    BC_REQUIRE(!(instance1 != instance1));
    BC_REQUIRE(!(instance2 != instance2));
}

BC_AUTO_TEST_CASE(languages__inequality__distinct_language__expected)
{
    const data_chunk entropy{ 0x01, 0x02, 0x03 };
    const string_list words{ "a", "b", "c" };
    accessor instance1(entropy, words, language::es);
    accessor instance2(entropy, words, language::ja);
    BC_REQUIRE(instance1 != instance2);
    BC_REQUIRE(instance2 != instance1);
    BC_REQUIRE(!(instance1 != instance1));
    BC_REQUIRE(!(instance2 != instance2));
}

BC_AUTO_TEST_SUITE_END()
