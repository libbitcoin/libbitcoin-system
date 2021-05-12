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

#include <string>

BOOST_AUTO_TEST_SUITE(languages_tests)

using namespace bc::system::wallet;

constexpr auto ideographic_space = "\xe3\x80\x80";
constexpr auto ascii_space = "\x20";

const size_t test_dictionary_size = 10;
typedef std::array<const char*, test_dictionary_size> test_words;

const test_words test_words_en
{
    {
        "abandon",
        "ability",
        "able",
        "about",
        "above",
        "absent",
        "absorb",
        "abstract",
        "absurd",
        "abuse"
    }
};

const test_words test_words_es
{
    {
        "ábaco",
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

const test_words test_words_ja
{
    {
        "あいこくしん",
        "あいさつ",
        "あいだ",
        "あおぞら",
        "あかちゃん",
        "あきる",
        "あけがた",
        "あける",
        "あこがれる",
        "あさい"
    }
};

const test_words test_words_zh_Hans
{
    {
        "的",
        "一",
        "是",
        "在",
        "不",
        "了",
        "有",
        "和",
        "人",
        "这"
    }
};

const test_words test_words_zh_Hant
{
    {
        "的",
        "一",
        "是",
        "在",
        "不",
        "了",
        "有",
        "和",
        "人",
        "這"
    }
};


// languages is a protected class, provide test accesss.
class accessor
  : public languages
{
public:
    accessor()
      : languages(), value_()
    {
    }

    accessor(const languages& other)
      : languages(other), value_()
    {
    }

    accessor(const data_chunk& entropy, const string_list& words,
        language identifier, const std::string& value="")
      : languages(entropy, words, identifier), value_(value)
    {
    }

    // Expose protected method.
    static string_list normalize(const string_list& words)
    {
        return languages::normalize(words);
    }

    // Verify assignment and copy construction with derived member.
    const std::string& value() const
    {
        return value_;
    }

private:
    std::string value_;
};

// statics

BOOST_AUTO_TEST_CASE(languages__from_name__always__expected)
{
    BOOST_REQUIRE(languages::from_name("en") == language::en);
    BOOST_REQUIRE(languages::from_name("es") == language::es);
    BOOST_REQUIRE(languages::from_name("it") == language::it);
    BOOST_REQUIRE(languages::from_name("fr") == language::fr);
    BOOST_REQUIRE(languages::from_name("cs") == language::cs);
    BOOST_REQUIRE(languages::from_name("pt") == language::pt);
    BOOST_REQUIRE(languages::from_name("ja") == language::ja);
    BOOST_REQUIRE(languages::from_name("ko") == language::ko);
    BOOST_REQUIRE(languages::from_name("zh_Hans") == language::zh_Hans);
    BOOST_REQUIRE(languages::from_name("zh_Hant") == language::zh_Hant);
}

BOOST_AUTO_TEST_CASE(languages__from_name__invalid__none)
{
    BOOST_REQUIRE(languages::from_name("foo") == language::none);
    BOOST_REQUIRE(languages::from_name("") == language::none);
}

BOOST_AUTO_TEST_CASE(languages__to_name__all__expected)
{
    BOOST_REQUIRE_EQUAL(languages::to_name(language::en), "en");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::es), "es");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::it), "it");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::fr), "fr");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::cs), "cs");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::pt), "pt");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::ja), "ja");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::ko), "ko");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::zh_Hans), "zh_Hans");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::zh_Hant), "zh_Hant");
    BOOST_REQUIRE_EQUAL(languages::to_name(language::none), "");
}

BOOST_AUTO_TEST_CASE(languages__to_delimiter__non_japanese__space)
{
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::en), ascii_space);
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::es), ascii_space);
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::it), ascii_space);
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::fr), ascii_space);
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::cs), ascii_space);
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::pt), ascii_space);
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::ko), ascii_space);
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::zh_Hans), ascii_space);
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::zh_Hant), ascii_space);
}

BOOST_AUTO_TEST_CASE(languages__to_delimiter__japanese__ideographic_space)
{
    BOOST_REQUIRE_EQUAL(languages::to_delimiter(language::ja), ideographic_space);
}

BOOST_AUTO_TEST_CASE(languages__join__korean__space_delimited)
{
    const auto expected =
        std::string(test_words_zh_Hant[0]) + ascii_space +
        test_words_es[1] + ascii_space +
        test_words_zh_Hant[2] + ascii_space +
        test_words_es[3];

    const string_list words
    {
        test_words_zh_Hant[0],
        test_words_es[1],
        test_words_zh_Hant[2],
        test_words_es[3]
    };

    BOOST_REQUIRE_EQUAL(languages::join(words, language::ko), expected);
}

BOOST_AUTO_TEST_CASE(languages__join__japanese__ideographic_space_delimited)
{
    const auto expected =
        std::string(test_words_zh_Hant[0]) + ideographic_space +
        test_words_es[1] + ideographic_space +
        test_words_zh_Hant[2] + ideographic_space +
        test_words_es[3];

    const string_list words
    {
        test_words_zh_Hant[0],
        test_words_es[1],
        test_words_zh_Hant[2],
        test_words_es[3]
    };

    BOOST_REQUIRE_EQUAL(languages::join(words, language::ja), expected);
}

BOOST_AUTO_TEST_CASE(languages__split__korean_ascii_space_delimited__split)
{
    const string_list expected
    {
        test_words_zh_Hans[0],
        test_words_es[1],
        test_words_zh_Hans[2],
        test_words_es[3]
    };

    const auto sentence =
        std::string(test_words_zh_Hans[0]) + ascii_space +
        test_words_es[1] + ascii_space +
        test_words_zh_Hans[2] + ascii_space +
        test_words_es[3];

    BOOST_REQUIRE_EQUAL(languages::split(sentence, language::ko), expected);
}

BOOST_AUTO_TEST_CASE(languages__split__korean_ideographic_space_delimited__unsplit)
{
    const auto sentence =
        std::string(test_words_zh_Hans[0]) + ideographic_space +
        test_words_es[1] + ideographic_space +
        test_words_zh_Hans[2] + ideographic_space +
        test_words_es[3];

    const string_list expected{ sentence };

    BOOST_REQUIRE_EQUAL(languages::split(sentence, language::ko), expected);
}

BOOST_AUTO_TEST_CASE(languages__split__japanese_ascii_space_delimited__unsplit)
{
    const auto sentence =
        std::string(test_words_zh_Hans[0]) + ascii_space +
        test_words_es[1] + ascii_space +
        test_words_zh_Hans[2] + ascii_space +
        test_words_es[3];

    const string_list expected{ sentence };

    BOOST_REQUIRE_EQUAL(languages::split(sentence, language::ja), expected);
}

BOOST_AUTO_TEST_CASE(languages__split__japanese_ideographic_space_space_delimited__split)
{
    const string_list expected
    {
        test_words_zh_Hans[0],
        test_words_es[1],
        test_words_zh_Hans[2],
        test_words_es[3]
    };

    const auto sentence =
        std::string(test_words_zh_Hans[0]) + ideographic_space +
        test_words_es[1] + ideographic_space +
        test_words_zh_Hans[2] + ideographic_space +
        test_words_es[3];

    BOOST_REQUIRE_EQUAL(languages::split(sentence, language::ja), expected);
}

BOOST_AUTO_TEST_CASE(languages__normalize__lower_ascii__unchanged)
{
    const string_list words{ "abc", "def", "xyz" };
    BOOST_REQUIRE_EQUAL(accessor::normalize(words), words);
}

BOOST_AUTO_TEST_CASE(languages__normalize__mixed_ascii__lowered)
{
    const string_list words{ "aBc", "DeF", "xYz" };
    const string_list expected{ "abc", "def", "xyz" };
    BOOST_REQUIRE_EQUAL(accessor::normalize(words), expected);
}

BOOST_AUTO_TEST_CASE(languages__normalize__padded_mixed_ascii__lowered_trimmed)
{
    const string_list words{ " aBc ", "  DeF  ", "\t\r\nxYz\f\v" };
    const string_list expected{ "abc", "def", "xyz" };
    BOOST_REQUIRE_EQUAL(accessor::normalize(words), expected);
}

////#ifdef WITH_ICU
////BOOST_AUTO_TEST_CASE(languages__normalize__non_ascii_with_icu__nfkd_ascii_lowered_trimmed)
////{
////    const string_list words{ " aBc ", " ábaco ", "\t\r\nxYz\f\v" };
////    const string_list expected{ "abc", "ábaco", "xyz" };
////    BOOST_REQUIRE_EQUAL(accessor::normalize(words), expected);
////}
////#else
////BOOST_AUTO_TEST_CASE(languages__normalize__non_ascii_without_icu__ascii_lowered_trimmed)
////{
////    const string_list words{ " aBc ", " ábaco ", "\t\r\nxYz\f\v" };
////    const string_list expected{ "abc", "ábaco", "xyz" };
////    BOOST_REQUIRE_EQUAL(accessor::normalize(words), expected);
////}
////#endif

////BOOST_AUTO_TEST_CASE(languages__normalize__empty__empty)
////{
////    BOOST_REQUIRE_EQUAL(accessor::normalize({}), string_list{});
////}

// construct/properties/bool

BOOST_AUTO_TEST_CASE(languages__default_construct__always__invalid_expected)
{
    accessor instance;
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE(instance.entropy().empty());
    BOOST_REQUIRE(instance.words().empty());
    BOOST_REQUIRE(instance.lingo() == language::none);
}

BOOST_AUTO_TEST_CASE(languages__copy_construct__always__copies_all_accessor_and_base_values)
{
    const auto value = "value";
    const accessor instance1({ 42, 42, 42 }, { "foo", "foo", "foo" }, language::zh_Hant, value);
    const accessor instance2(instance1);
    BOOST_REQUIRE(instance1 && instance2);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), instance2.entropy());
    BOOST_REQUIRE_EQUAL(instance1.words(), instance2.words());
    BOOST_REQUIRE(instance1.lingo() == instance2.lingo());
    BOOST_REQUIRE_EQUAL(instance1.value(), instance2.value());
    BOOST_REQUIRE_EQUAL(instance1.value(), value);
}

BOOST_AUTO_TEST_CASE(languages__values_construct__empty_entropy__invalid_expected)
{
    const string_list words{ "foo" };
    accessor instance({}, words, language::en);
    BOOST_REQUIRE(!instance);

    // This is why languages is a protected class, there are no consistency guards.
    BOOST_REQUIRE(instance.entropy().empty());
    BOOST_REQUIRE_EQUAL(instance.words(), words);
    BOOST_REQUIRE(instance.lingo() == language::en);
}

BOOST_AUTO_TEST_CASE(languages__values_construct__empty_words__valid_expected)
{
    const data_chunk entropy{ 42, 42 };
    accessor instance(entropy, {}, language::none);
    BOOST_REQUIRE(instance);

    // This is why languages is a protected class, there are no consistency guards.
    BOOST_REQUIRE_EQUAL(instance.entropy(), entropy);
    BOOST_REQUIRE(instance.words().empty());
    BOOST_REQUIRE(instance.lingo() == language::none);
}

BOOST_AUTO_TEST_CASE(languages__values_construct__not_empty__valid_expected)
{
    const data_chunk entropy{ 42, 42 };
    const string_list words{ "foo", "bar" };
    accessor instance(entropy, words, language::none);
    BOOST_REQUIRE(instance);

    // This is why languages is a protected class, there are no consistency guards.
    BOOST_REQUIRE_EQUAL(instance.entropy(), entropy);
    BOOST_REQUIRE_EQUAL(instance.words(), words);
    BOOST_REQUIRE(instance.lingo() == language::none);
}

// sentence

BOOST_AUTO_TEST_CASE(languages__sentence__empty_words__empty)
{
    accessor instance({}, {}, language::none);
    BOOST_REQUIRE(instance.sentence().empty());
}

BOOST_AUTO_TEST_CASE(languages__sentence__words_not_japanese__ascii_delimited)
{
    const string_list words{ "foo", "bar" };
    accessor instance({}, words, language::es);
    BOOST_REQUIRE_EQUAL(instance.sentence(), "foo bar");
}

BOOST_AUTO_TEST_CASE(languages__sentence__words_japanese__ideographic_space_delimited)
{
    const auto expected = std::string("foo") + ideographic_space + "bar";
    const string_list words{ "foo", "bar" };
    accessor instance({}, words, language::ja);
    BOOST_REQUIRE_EQUAL(instance.sentence(), expected);
}

// operators

BOOST_AUTO_TEST_CASE(languages__assignment__always__copies_all_accessor_and_base_values)
{
    const auto value = "value";
    const accessor instance1({ 42, 42, 42 }, { "foo", "foo", "foo" }, language::zh_Hant, value);
    const accessor instance2 = instance1;
    BOOST_REQUIRE(instance1 && instance2);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), instance2.entropy());
    BOOST_REQUIRE_EQUAL(instance1.words(), instance2.words());
    BOOST_REQUIRE(instance1.lingo() == instance2.lingo());
    BOOST_REQUIRE_EQUAL(instance1.value(), instance2.value());
    BOOST_REQUIRE_EQUAL(instance1.value(), value);
}

BOOST_AUTO_TEST_CASE(languages__less_than__same_same_words__expected)
{
    // Entropy has no effect on less than operator.
    const string_list words{ "a", "b", "c" };
    accessor instance1({ 42 }, words, language::none);
    accessor instance2({}, words, language::none);
    BOOST_REQUIRE(!(instance1 < instance2));
    BOOST_REQUIRE(!(instance2 < instance1));
    BOOST_REQUIRE(!(instance1 < instance1));
    BOOST_REQUIRE(!(instance2 < instance2));
}

BOOST_AUTO_TEST_CASE(languages__less_than__same_delimited_distinct_words__expected)
{
    const string_list words1{ "a", "b", "c" };
    const string_list words2{ "b", "c" };
    accessor instance1({}, words1, language::es);
    accessor instance2({}, words2, language::es);
    BOOST_REQUIRE(instance1 < instance2);
    BOOST_REQUIRE(!(instance2 < instance1));
    BOOST_REQUIRE(!(instance1 < instance1));
    BOOST_REQUIRE(!(instance2 < instance2));
}

BOOST_AUTO_TEST_CASE(languages__less_than__differently_delimited_same_words__expected)
{
    // ideographic_space > ascii_space
    const string_list words{ "a", "b", "c" };
    accessor instance1({}, words, language::es);
    accessor instance2({}, words, language::ja);
    BOOST_REQUIRE(instance1 < instance2);
    BOOST_REQUIRE(!(instance2 < instance1));
    BOOST_REQUIRE(!(instance1 < instance1));
    BOOST_REQUIRE(!(instance2 < instance2));
}

BOOST_AUTO_TEST_CASE(languages__equality__same_entropy_and_language__expected)
{
    // Words have no effect on equality operators.
    const data_chunk entropy{ 0x01, 0x02, 0x03 };
    accessor instance1(entropy, {}, language::es);
    accessor instance2(entropy, { "foo" }, language::es);
    BOOST_REQUIRE(instance1 == instance2);
    BOOST_REQUIRE(instance2 == instance1);
    BOOST_REQUIRE(instance1 == instance1);
    BOOST_REQUIRE(instance2 == instance2);
}

BOOST_AUTO_TEST_CASE(languages__equality__same_entropy_distinct_language__expected)
{
    const data_chunk entropy{ 0x01, 0x02, 0x03 };
    accessor instance1(entropy, {}, language::es);
    accessor instance2(entropy, {}, language::ja);
    BOOST_REQUIRE(!(instance1 == instance2));
    BOOST_REQUIRE(!(instance2 == instance1));
    BOOST_REQUIRE(instance1 == instance1);
    BOOST_REQUIRE(instance2 == instance2);
}

BOOST_AUTO_TEST_CASE(languages__inequality__distinct_entropy_same_language__expected)
{
    const data_chunk entropy1{ 0x01, 0x02, 0x03 };
    const data_chunk entropy2{ 0x02, 0x03, 0x04 };
    accessor instance1(entropy1, {}, language::ja);
    accessor instance2(entropy2, {}, language::ja);
    BOOST_REQUIRE(instance1 != instance2);
    BOOST_REQUIRE(instance2 != instance1);
    BOOST_REQUIRE(!(instance1 != instance1));
    BOOST_REQUIRE(!(instance2 != instance2));
}

BOOST_AUTO_TEST_CASE(languages__inequality__same_entropy_distinct_language__expected)
{
    const data_chunk entropy{ 0x01, 0x02, 0x03 };
    accessor instance1(entropy, {}, language::es);
    accessor instance2(entropy, {}, language::ja);
    BOOST_REQUIRE(instance1 != instance2);
    BOOST_REQUIRE(instance2 != instance1);
    BOOST_REQUIRE(!(instance1 != instance1));
    BOOST_REQUIRE(!(instance2 != instance2));
}

BOOST_AUTO_TEST_SUITE_END()
