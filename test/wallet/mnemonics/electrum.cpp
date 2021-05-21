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
#include "electrum.hpp"
#include <utility>

BOOST_AUTO_TEST_SUITE(electrum_tests)

using namespace bc::system::wallet;
using prefix = electrum::seed_prefix;

// Protected methods.
// ----------------------------------------------------------------------------

// sizers

BOOST_AUTO_TEST_CASE(electrum__entropy_bits1__boundaries__expected)
{
    // The number of bits for the given number of bytes.
    BOOST_REQUIRE_EQUAL(accessor::entropy_bits(data_chunk(17, 0)), 17u * 8u);
    BOOST_REQUIRE_EQUAL(accessor::entropy_bits(data_chunk(64, 0)), 64u * 8u);
}

BOOST_AUTO_TEST_CASE(electrum__entropy_bits2__boundaries__expected)
{
    // The number of bits for the given number of words.
    BOOST_REQUIRE_EQUAL(accessor::entropy_bits(string_list(12, "")), 12u * 11u);
    BOOST_REQUIRE_EQUAL(accessor::entropy_bits(string_list(46, "")), 46u * 11u);
}

BOOST_AUTO_TEST_CASE(electrum__entropy_size1__boundaries__expected)
{
    // This must round up to the next byte.
    // The required number of bytes to achieve the given bit strength.
    BOOST_REQUIRE_EQUAL(accessor::entropy_size(132), 132u / 8u + 1u);
    BOOST_REQUIRE_EQUAL(accessor::entropy_size(506), 506u / 8u + 1u);
}

BOOST_AUTO_TEST_CASE(electrum__entropy_size2__boundaries__expected)
{
    // These number of bits are wasted.
    BOOST_REQUIRE_EQUAL((12 * 11) % 8, 4);
    BOOST_REQUIRE_EQUAL((46 * 11) % 8, 2);

    // This must round up to the next byte.
    // The required number of bytes for the given number of words.
    BOOST_REQUIRE_EQUAL(accessor::entropy_size(string_list(12, "")), (12u * 11u) / 8u + 1u);
    BOOST_REQUIRE_EQUAL(accessor::entropy_size(string_list(46, "")), (46u * 11u) / 8u + 1u);
}

BOOST_AUTO_TEST_CASE(electrum__word_count1__boundaries__expected)
{
    // These number of bits are wasted.
    BOOST_REQUIRE_EQUAL((17 * 8) % 11, 4);
    BOOST_REQUIRE_EQUAL((64 * 8) % 11, 6);

    // The number of words that can be derived from the given entropy size.
    BOOST_REQUIRE_EQUAL(accessor::word_count(data_chunk(17, 0)), (17u * 8u) / 11u);
    BOOST_REQUIRE_EQUAL(accessor::word_count(data_chunk(64, 0)), (64u * 8u) / 11u);
}

BOOST_AUTO_TEST_CASE(electrum__word_count2__boundaries__expected)
{
    // The required number of words to achieve the given bit strength.
    BOOST_REQUIRE_EQUAL(accessor::word_count(132), 132u / 11u + 0u);
    BOOST_REQUIRE_EQUAL(accessor::word_count(506), 506u / 11u + 0u);

    // This must round up to the next word.
    BOOST_REQUIRE_EQUAL(accessor::word_count(131), 131u / 11u + 1u);
    BOOST_REQUIRE_EQUAL(accessor::word_count(505), 505u / 11u + 1u);
}

BOOST_AUTO_TEST_CASE(electrum__unused_bits__boundaries__expected)
{
    // The supported word ranges determine the number of wasted entropy bits.
    BOOST_REQUIRE_EQUAL((17 * 8) % 11, 4);
    BOOST_REQUIRE_EQUAL((64 * 8) % 11, 6);

    // The number of bits that must be wasted in conversion to words.
    BOOST_REQUIRE_EQUAL(accessor::unused_bits(data_chunk(17, 0)), (17u * 8u) % 11u);
    BOOST_REQUIRE_EQUAL(accessor::unused_bits(data_chunk(64, 0)), (64u * 8u) % 11u);
}

BOOST_AUTO_TEST_CASE(electrum__unused_bytes__boundaries__expected)
{
    // The entropy byte boundaries are chosen to not waste bytes.
    BOOST_REQUIRE_EQUAL(((17 * 8) % 11) / 8, 0);
    BOOST_REQUIRE_EQUAL(((64 * 8) % 11) / 8, 0);

    // The number of bytes that must be wasted in conversion to words.
    BOOST_REQUIRE_EQUAL(accessor::unused_bytes(data_chunk(17, 0)), ((17u * 8u) % 11u) / 8u);
    BOOST_REQUIRE_EQUAL(accessor::unused_bytes(data_chunk(64, 0)), ((64u * 8u) % 11u) / 8u);
}

BOOST_AUTO_TEST_CASE(electrum__usable_size__boundaries__expected)
{
    // The entropy byte boundaries are chosen to not waste bytes.
    BOOST_REQUIRE_EQUAL(17 - ((17 * 8) % 11) / 8, 17);
    BOOST_REQUIRE_EQUAL(64 - ((64 * 8) % 11) / 8, 64);

    // The number of bytes that can be used in conversion to words.
    BOOST_REQUIRE_EQUAL(accessor::usable_size(data_chunk(17, 0)), 17u - ((17u * 8u) % 11u) / 8u);
    BOOST_REQUIRE_EQUAL(accessor::usable_size(data_chunk(64, 0)), 64u - ((64u * 8u) % 11u) / 8u);
}

// encoder

BOOST_AUTO_TEST_CASE(electrum__encoder__invalid__empty)
{
    BOOST_REQUIRE(accessor::encoder({ 42, 42 }, language::pt).empty());
}

BOOST_AUTO_TEST_CASE(electrum__encoder__japanese_with_passphrase__expected)
{
    const auto vector = vectors[0];
    const auto expected = split(vector.mnemonic);
    BOOST_REQUIRE_EQUAL(accessor::encoder(vector.entropy, vector.lingo), expected);
}

// decoder

BOOST_AUTO_TEST_CASE(electrum__decoder__invalid__empty)
{
    BOOST_REQUIRE(accessor::decoder({ "42", "42" }, language::es).empty());
}

BOOST_AUTO_TEST_CASE(electrum__decoder__japanese__expected)
{
    const auto vector = vectors[0];
    BOOST_REQUIRE_EQUAL(accessor::decoder(split(vector.mnemonic), language::en), vector.entropy);
}

// grinder

BOOST_AUTO_TEST_CASE(electrum__grinder__english__match_first_iteration)
{
    const auto vector = vectors[0];
    const auto words = split(vector.mnemonic);

    // This verifies that previously-derived entropy round trips.
    const auto result = accessor::grinder(vector.entropy, vector.prefix, vector.lingo, 42);
    BOOST_REQUIRE_EQUAL(result.entropy, vector.entropy);
    BOOST_REQUIRE_EQUAL(result.words, words);

    // The derivation is deterministic, always finds previously-found on one iteration.
    BOOST_REQUIRE_EQUAL(result.iterations, 0u);
}

BOOST_AUTO_TEST_CASE(electrum__grinder__null_entropy__match_first_iteration)
{
    const data_chunk entropy(17, 0x00);
    const auto find = prefix::two_factor_authentication;

    // This is an example of grinding to find the desired prefix.
    const auto result = accessor::grinder(entropy, find, language::zh_Hans, 1000);
    BOOST_REQUIRE_NE(result.entropy, entropy);
    BOOST_REQUIRE(electrum::is_version(result.words, find));

    // The derivation is deterministic, always finds this result at 274 iterations.
    BOOST_REQUIRE_EQUAL(result.iterations, 273u);
}

BOOST_AUTO_TEST_CASE(electrum__grinder__18_byte_spanish__match_first_iteration)
{
    const auto vector = vectors[6];
    const auto words = split(vector.mnemonic);

    // This is an example of an unused leading byte having no entropy contribution.
    const auto entropy = build_chunk({data_chunk{ 0x00 }, vector.entropy });
    const auto result = accessor::grinder(vector.entropy, vector.prefix, vector.lingo, 42);
    BOOST_REQUIRE_EQUAL(result.entropy, vector.entropy);
    BOOST_REQUIRE_EQUAL(result.words, words);
    BOOST_REQUIRE_EQUAL(result.iterations, 0u);
}

BOOST_AUTO_TEST_CASE(electrum__grinder__19_byte_spanish__match_first_iteration)
{
    const auto vector = vectors[6];
    const auto words = split(vector.mnemonic);

    // This is an example of usable additional bytes having an entropy contribution.
    const auto entropy = build_chunk({ data_chunk{ 0x00, 0x00 }, vector.entropy });
    const auto result = accessor::grinder(entropy, vector.prefix, vector.lingo, 10000);
    BOOST_REQUIRE(!result.entropy.empty());
    BOOST_REQUIRE_NE(result.entropy, vector.entropy);
    BOOST_REQUIRE(electrum::is_version(result.words, vector.prefix));
    BOOST_REQUIRE_EQUAL(result.iterations, 41u);
}

BOOST_AUTO_TEST_CASE(electrum__grinder__not_found__empty_iterations_zero)
{
    const auto limit = 40u;
    const auto vector = vectors[6];
    const auto entropy = build_chunk({ data_chunk{ 0x00, 0x00 }, vector.entropy });
    const auto result = accessor::grinder(entropy, vector.prefix, vector.lingo, limit);
    BOOST_REQUIRE(result.entropy.empty());
    BOOST_REQUIRE(result.words.empty());
    BOOST_REQUIRE_EQUAL(result.iterations, limit);
}

// seeder

#ifdef WITH_ICU
BOOST_AUTO_TEST_CASE(electrum__seeder__non_ascii_passphrase_with_icu__valid)
{
    BOOST_REQUIRE(accessor::seeder(split(vectors[8].mnemonic), "なのか ひろい しなん", hd_private::testnet));
}
#else
BOOST_AUTO_TEST_CASE(electrum__seeder__non_ascii_passphrase_without_icu__invalid)
{
    // This is the only seeder failure condition.
    BOOST_REQUIRE(!accessor::seeder(split(vectors[8].mnemonic), "なのか ひろい しなん", hd_private::testnet));
}
#endif

BOOST_AUTO_TEST_CASE(electrum__seeder__ascii_passphrase_mainnet__expected_hd_key)
{
    const auto result = accessor::seeder(split(vectors[7].mnemonic), "Did you ever hear the tragedy of Darth Plagueis the Wise?", hd_private::mainnet);
    BOOST_REQUIRE_EQUAL(result.encoded(), "xprv9s21ZrQH143K3J7uaJtH3fPYadCubChqdXxqpWVDU9PasYoWhbCXLLQCP1DtCtuixztKC6eYXXqrk1an8sztbZ8L53MDLsvKLHZ3GaxY1d8");
}

BOOST_AUTO_TEST_CASE(electrum__seeder__ascii_passphrase_testnet__expected_hd_key)
{
    const auto result = accessor::seeder(split(vectors[7].mnemonic), "Did you ever hear the tragedy of Darth Plagueis the Wise?", hd_private::testnet);
    BOOST_REQUIRE_EQUAL(result.encoded(), "tprv8ZgxMBicQKsPe7MSEsjnDK1Xtkd7pijqy5sxgvufx7t4f9YbgxYGr5meJBPYDGJ3LSR6CCGJgtRfCs8XG6LqQcPvbgZX1EeNFPJTiGTAqkt");
}

// prefixer

BOOST_AUTO_TEST_CASE(electrum__prefixer__none__expected)
{
    // All invalid wordlists or failures to match will produce 'none' (other enums unreachable).
    BOOST_REQUIRE(accessor::prefixer({ "bogus" }) == prefix::none);
}

BOOST_AUTO_TEST_CASE(electrum__prefixer__standard__expected)
{
    BOOST_REQUIRE(accessor::prefixer(split(mnemonic_standard)) == prefix::standard);
}

BOOST_AUTO_TEST_CASE(electrum__prefixer__witness__expected)
{
    BOOST_REQUIRE(accessor::prefixer(split(mnemonic_witness)) == prefix::witness);
}

BOOST_AUTO_TEST_CASE(electrum__prefixer__two_factor_authentication__expected)
{
    BOOST_REQUIRE(accessor::prefixer(split(mnemonic_two_factor_authentication)) == prefix::two_factor_authentication);
}

BOOST_AUTO_TEST_CASE(electrum__prefixer__two_factor_authentication_witness__expected)
{
    BOOST_REQUIRE(accessor::prefixer(split(mnemonic_two_factor_authentication_witness)) == prefix::two_factor_authentication_witness);
}

// validator

BOOST_AUTO_TEST_CASE(electrum__validator__invalid__false)
{
    BOOST_REQUIRE(!accessor::validator({ "bogus" }, prefix::standard));
}

BOOST_AUTO_TEST_CASE(electrum__validator__standard__true)
{
    BOOST_REQUIRE(accessor::validator(split(mnemonic_standard), prefix::standard));
}

BOOST_AUTO_TEST_CASE(electrum__validator__witness__true)
{
    BOOST_REQUIRE(accessor::validator(split(mnemonic_witness), prefix::witness));
}

BOOST_AUTO_TEST_CASE(electrum__validator__two_factor_authentication__true)
{
    BOOST_REQUIRE(accessor::validator(split(mnemonic_two_factor_authentication), prefix::two_factor_authentication));
}

BOOST_AUTO_TEST_CASE(electrum__validator__two_factor_authentication_witness__true)
{
    BOOST_REQUIRE(accessor::validator(split(mnemonic_two_factor_authentication_witness), prefix::two_factor_authentication_witness));
}

// from_words

BOOST_AUTO_TEST_CASE(electrum__from_words__empty_none__false)
{
    BOOST_REQUIRE(!accessor::from_words({}, language::none));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__bogus_none__false)
{
    BOOST_REQUIRE(!accessor::from_words({ "bogus", "bogus" }, language::none));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__low_word_count__false)
{
    BOOST_REQUIRE(!accessor::from_words(string_list(11, "眼"), language::zh_Hans));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__minimum_word_count__true)
{
    BOOST_REQUIRE(accessor::from_words(string_list(12, "眼"), language::zh_Hans));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__maximum_word_count__true)
{
    BOOST_REQUIRE(accessor::from_words(string_list(46, "眼"), language::zh_Hans));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__high_word_count__false)
{
    BOOST_REQUIRE(!accessor::from_words(string_list(47, "眼"), language::zh_Hans));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__unknown_prefix__true)
{
    // Add a 13th valid word.
    const auto extended = join({ mnemonic_standard, "ristoro" });

    // The instance is valid in relation to the discovered prefix.
    const auto instance = accessor::from_words(split(extended), language::it);

    // Electrum does not incorporate a checksum.
    // Words construction does not validate a prefix, it computes the prefix.
    BOOST_REQUIRE(instance);

    // The modified mnemonic is valid but does not have a known prefix.
    BOOST_REQUIRE(instance.prefix() == prefix::none);
}

BOOST_AUTO_TEST_CASE(electrum__from_words__mismatched_language__false)
{
    // Specify the incorrect dictionary to preclude mnemonic dictionary matching.
    BOOST_REQUIRE(!accessor::from_words(split(mnemonic_standard), language::cs));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__uppercase_standard_italian__true)
{
    // WITH_ICU not required for ascii case normalization.
    const auto instance = accessor::from_words(split(ascii_to_upper(mnemonic_standard)), language::it);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.prefix() == prefix::standard);
}

BOOST_AUTO_TEST_CASE(electrum__from_words__standard_none__true)
{
    // With 'none' specified the mnemonic is valid if contained in exactly one dictionary
    // (only en-fr) or is the same in multiple dictionaries (only chinese).
    const auto instance = accessor::from_words(split(mnemonic_standard), language::none);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.prefix() == prefix::standard);
}

BOOST_AUTO_TEST_CASE(electrum__from_words__standard_italian__true)
{
    const auto instance = accessor::from_words(split(mnemonic_standard), language::it);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.prefix() == prefix::standard);
}

BOOST_AUTO_TEST_CASE(electrum__from_words__witness_italian__true)
{
    const auto instance = accessor::from_words(split(mnemonic_witness), language::it);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.prefix() == prefix::witness);
}

BOOST_AUTO_TEST_CASE(electrum__from_words__two_factor_authentication_italian__true)
{
    const auto instance = accessor::from_words(split(mnemonic_two_factor_authentication), language::it);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.prefix() == prefix::two_factor_authentication);
}

BOOST_AUTO_TEST_CASE(electrum__from_words__two_factor_authentication_witness_italian__true)
{
    const auto instance = accessor::from_words(split(mnemonic_two_factor_authentication_witness), language::it);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.prefix() == prefix::two_factor_authentication_witness);
}

BOOST_AUTO_TEST_CASE(electrum__from_words__incorrect_explicit_language__false)
{
    BOOST_REQUIRE(!accessor::from_words(split(mnemonic_witness), language::ko));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__conflicting_english_french__false)
{
    // There are 100 common words in en and fr dictionaries, but all of the
    // common words occupy a different index in their respective dictionaries.
    // No other dictionary pair exibits conflicting behavior.
    const string_list ambiguous
    {
        "fragile", "fragile", "fragile", "fragile", "fragile", "fragile",
        "fragile", "fragile", "fragile", "fragile", "fragile", "fragile"
    };

    // The words are in different positions in two dictionaries.
    const auto english = accessor::from_words(ambiguous, language::en);
    const auto french = accessor::from_words(ambiguous, language::fr);

    // Explicit specification succeeds.
    BOOST_REQUIRE(english);
    BOOST_REQUIRE(french);

    // But note that entropy for the same mnemonics is different!
    BOOST_REQUIRE_NE(english.entropy(), french.entropy());

    // So the language must be explicitly expecified.
    BOOST_REQUIRE(!accessor::from_words(ambiguous, language::none));

    // But this is only required when all words conflict.

    const string_list distinct_english
    {
        "differ", "fragile", "fragile", "fragile", "fragile", "fragile",
        "fragile", "fragile", "fragile", "fragile", "fragile", "fragile"
    };

    const auto english2 = accessor::from_words(distinct_english, language::none);
    BOOST_REQUIRE(english2);
    BOOST_REQUIRE_NE(english2.entropy(), english.entropy());

    const string_list distinct_french
    {
        "différer", "fragile", "fragile", "fragile", "fragile", "fragile",
        "fragile", "fragile", "fragile", "fragile", "fragile", "fragile"
    };

    const auto french2 = accessor::from_words(distinct_french, language::none);
    BOOST_REQUIRE(french2);
    BOOST_REQUIRE_NE(french2.entropy(), french.entropy());
}

BOOST_AUTO_TEST_CASE(electrum__from_words__conflicting_chinese__true)
{
    // There are 1275 common words in the zh_Hans and zh_Hant dictionaries, but
    // all common words occupy the same index in both dictionaries.
    // No other dictionary pair exibits this partial symmetry.
    const string_list redundant
    {
        "的", "一", "是", "在", "不", "的", "一", "是", "在", "不", "的", "一", "是"
    };

    // These words are in the same positions in both dictionaries.
    const auto simplified = accessor::from_words(redundant, language::zh_Hans);
    const auto traditional = accessor::from_words(redundant, language::zh_Hant);

    // Explicit specification succeeds.
    BOOST_REQUIRE(simplified);
    BOOST_REQUIRE(traditional);

    // But note that entropy is the same for each.
    BOOST_REQUIRE_EQUAL(simplified.entropy(), traditional.entropy());

    // So the language does not ever have to be explicitly expecified.
    const auto none = accessor::from_words(redundant, language::none);
    BOOST_REQUIRE(none);
    BOOST_REQUIRE_EQUAL(none.entropy(), traditional.entropy());
    BOOST_REQUIRE_EQUAL(none.entropy(), simplified.entropy());
}

BOOST_AUTO_TEST_CASE(electrum__from_words__mixed_languages__false)
{
    const string_list mixed
    {
        "below", "가격", "あいこくしん", "abaisser", "abaco", "ábaco",
        "abdikace", "abandon", "abacate", "的", "歇", "above"
    };

    // All words must be from one dictionary.
    BOOST_REQUIRE(!accessor::from_words(mixed, language::none));
}

BOOST_AUTO_TEST_CASE(electrum__from_words__similar_words__false)
{
    const string_list similar
    {
        "ábaco", "abaco", "ábaco", "abaco", "ábaco", "abaco",
        "ábaco", "abaco", "ábaco", "abaco", "ábaco", "abaco",
    };

    // Normalization does not reduce á to a, false due to invalid prefix.
    BOOST_REQUIRE(!accessor::from_words(similar, language::none));
}

// from_entropy

// This grinds, so tests may be slow, depending on entropy.

BOOST_AUTO_TEST_CASE(electrum__from_entropy__empty__false)
{
    BOOST_REQUIRE(!accessor::from_entropy({}, prefix::standard, language::en, max_uint16));
}

BOOST_AUTO_TEST_CASE(electrum__from_entropy__low_byte_count__false)
{
    BOOST_REQUIRE(!accessor::from_entropy(data_chunk(16, 0x42), prefix::standard, language::en, max_uint16));
}

BOOST_AUTO_TEST_CASE(electrum__from_entropy__minimum_byte_count__true)
{
    // This grinds, so tests may be slow, but is alway the same.
    BOOST_REQUIRE(accessor::from_entropy(data_chunk(17, 0x42), prefix::standard, language::en, max_uint16));
}

BOOST_AUTO_TEST_CASE(electrum__from_entropy__maximum_byte_count__true)
{
    // This grinds, so tests may be slow, but is alway the same.
    BOOST_REQUIRE(accessor::from_entropy(data_chunk(64, 0x42), prefix::standard, language::en, max_uint16));
}

BOOST_AUTO_TEST_CASE(electrum__from_entropy__high_byte_count__true)
{
    BOOST_REQUIRE(!accessor::from_entropy(data_chunk(65, 0x42), prefix::standard, language::en, max_uint16));
}

BOOST_AUTO_TEST_CASE(electrum__from_entropy__invalid_prefixes__false)
{
    BOOST_REQUIRE(!accessor::from_entropy(data_chunk(17, 0x42), prefix::old, language::en, max_uint16));
    BOOST_REQUIRE(!accessor::from_entropy(data_chunk(17, 0x42), prefix::bip39, language::en, max_uint16));
    BOOST_REQUIRE(!accessor::from_entropy(data_chunk(17, 0x42), prefix::none, language::en, max_uint16));
}

BOOST_AUTO_TEST_CASE(electrum__from_entropy__valid_entropy_but_prefix_not_found__false)
{
    BOOST_REQUIRE(!accessor::from_entropy(data_chunk(17, 0x42), prefix::standard, language::en, 0));
}

BOOST_AUTO_TEST_CASE(electrum__from_entropy__previous_entropy_zero_grind__true)
{
    const auto vector = vectors[6];

    // Previous entropy round trips without grinding.
    BOOST_REQUIRE(accessor::from_entropy(vector.entropy, vector.prefix, vector.lingo, 0));
}

BOOST_AUTO_TEST_CASE(electrum__from_entropy__language_none__false)
{
    // A mnemonic cannot be created without a specified dictionary.
    BOOST_REQUIRE(!accessor::from_entropy(vectors[6].entropy, prefix::standard, language::none, max_uint16));
}

// is_valid_seed_prefix

BOOST_AUTO_TEST_CASE(electrum__is_valid_seed_prefix__invalid__false)
{
    BOOST_REQUIRE(!accessor::is_valid_seed_prefix(prefix::none));
    BOOST_REQUIRE(!accessor::is_valid_seed_prefix(prefix::bip39));
    BOOST_REQUIRE(!accessor::is_valid_seed_prefix(prefix::old));
}

BOOST_AUTO_TEST_CASE(electrum__is_valid_seed_prefix__valid__true)
{
    BOOST_REQUIRE(accessor::is_valid_seed_prefix(prefix::standard));
    BOOST_REQUIRE(accessor::is_valid_seed_prefix(prefix::witness));
    BOOST_REQUIRE(accessor::is_valid_seed_prefix(prefix::two_factor_authentication));
    BOOST_REQUIRE(accessor::is_valid_seed_prefix(prefix::two_factor_authentication_witness));
}

// is_valid_two_factor_authentication_size

BOOST_AUTO_TEST_CASE(electrum__is_valid_two_factor_authentication_size__invalid__false)
{
    // count == word_count(strength_minimum) ||...
    BOOST_REQUIRE(!accessor::is_valid_two_factor_authentication_size(11));
    BOOST_REQUIRE(!accessor::is_valid_two_factor_authentication_size(13));

    // ...count >= minimum_two_factor_authentication_words
    BOOST_REQUIRE(!accessor::is_valid_two_factor_authentication_size(19));
}

BOOST_AUTO_TEST_CASE(electrum__is_valid_two_factor_authentication_size__valid__true)
{
    // count == word_count(strength_minimum) ||...
    BOOST_REQUIRE(accessor::is_valid_two_factor_authentication_size(12));

    // ...count >= minimum_two_factor_authentication_words
    BOOST_REQUIRE(accessor::is_valid_two_factor_authentication_size(20));
    BOOST_REQUIRE(accessor::is_valid_two_factor_authentication_size(21));
}

// Public static.
// ----------------------------------------------------------------------------

// contained_by

BOOST_AUTO_TEST_CASE(electrum__contained_by__invalid__none)
{
    BOOST_REQUIRE(electrum::contained_by({ "bogus" }, language::none) == language::none);
}

BOOST_AUTO_TEST_CASE(electrum__contained_by__ambiguous__expected)
{
    const string_list ambiguous
    {
        "fragile", "fragile", "fragile", "fragile", "fragile", "fragile",
        "fragile", "fragile", "fragile", "fragile", "fragile", "fragile"
    };

    // contained_by returns the first matching language.
    // The only ambiguous set is en-fr (100 words).
    // So an en match should be explicitly tested against fr.
    BOOST_REQUIRE(electrum::contained_by(ambiguous, language::none) == language::en);
    BOOST_REQUIRE(electrum::contained_by(ambiguous, language::en) == language::en);
    BOOST_REQUIRE(electrum::contained_by(ambiguous, language::fr) == language::fr);
}

BOOST_AUTO_TEST_CASE(electrum__contained_by__redundant__expected)
{
    const string_list redundant
    {
        "的", "一", "是", "在", "不", "的", "一", "是", "在", "不", "的", "一", "是"
    };

    // contained_by returns the first matching language.
    // The only redundant set is zh_Hans-zh_Hant (1275 words).
    BOOST_REQUIRE(electrum::contained_by(redundant, language::none) == language::zh_Hans);
    BOOST_REQUIRE(electrum::contained_by(redundant, language::zh_Hans) == language::zh_Hans);
    BOOST_REQUIRE(electrum::contained_by(redundant, language::zh_Hant) == language::zh_Hant);
}

BOOST_AUTO_TEST_CASE(electrum__contained_by__japanese__expected)
{
    const auto vector = vectors[electrum_vector::japanese];
    BOOST_REQUIRE(electrum::contained_by(split(vector.mnemonic), vector.lingo) == vector.lingo);
    BOOST_REQUIRE(electrum::contained_by(split(vector.mnemonic), language::none) == vector.lingo);
    BOOST_REQUIRE(electrum::contained_by(split(vector.mnemonic), language::ko) == language::none);
}

// is_valid_dictionary

BOOST_AUTO_TEST_CASE(electrum__is_valid_dictionary__none__false)
{
    BOOST_REQUIRE(!electrum::is_valid_dictionary(language::none));
}

BOOST_AUTO_TEST_CASE(electrum__is_valid_dictionary__all__true)
{
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::en));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::es));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::it));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::fr));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::cs));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::pt));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::ja));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::ko));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::zh_Hans));
    BOOST_REQUIRE(electrum::is_valid_dictionary(language::zh_Hant));
}

// is_valid_entropy_size

BOOST_AUTO_TEST_CASE(electrum__is_valid_entropy_size__invalid__false)
{
    BOOST_REQUIRE(!electrum::is_valid_entropy_size(16));
    BOOST_REQUIRE(!electrum::is_valid_entropy_size(65));
}

BOOST_AUTO_TEST_CASE(electrum__is_valid_entropy_size__valid__true)
{
    BOOST_REQUIRE(electrum::is_valid_entropy_size(17));
    BOOST_REQUIRE(electrum::is_valid_entropy_size(64));
}

// is_valid_word_count

BOOST_AUTO_TEST_CASE(electrum__is_valid_word_count__invalid__false)
{
    BOOST_REQUIRE(!electrum::is_valid_word_count(11));
    BOOST_REQUIRE(!electrum::is_valid_word_count(47));
}

BOOST_AUTO_TEST_CASE(electrum__is_valid_word_count__valid__true)
{
    BOOST_REQUIRE(electrum::is_valid_word_count(12));
    BOOST_REQUIRE(electrum::is_valid_word_count(46));
}

// is_version

BOOST_AUTO_TEST_CASE(electrum__is_version__invalid_prefix__false)
{
    BOOST_REQUIRE(!electrum::is_version(split(mnemonic_standard), prefix::none));
    BOOST_REQUIRE(!electrum::is_version(split(mnemonic_standard), prefix::bip39));
    BOOST_REQUIRE(!electrum::is_version(split(mnemonic_standard), prefix::old));
}

BOOST_AUTO_TEST_CASE(electrum__is_version__valid_prefix__true)
{
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_standard), prefix::standard));
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_witness), prefix::witness));
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_two_factor_authentication), prefix::two_factor_authentication));
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_two_factor_authentication_witness), prefix::two_factor_authentication_witness));

}

BOOST_AUTO_TEST_CASE(electrum__is_version__two_factor_authentication_valid_size__true)
{
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_two_factor_authentication12), prefix::two_factor_authentication));
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_two_factor_authentication20), prefix::two_factor_authentication));
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_two_factor_authentication21), prefix::two_factor_authentication));
}

BOOST_AUTO_TEST_CASE(electrum__is_version__two_factor_authentication_invalid_size__false)
{
    BOOST_REQUIRE(!electrum::is_version(split(mnemonic_two_factor_authentication11), prefix::two_factor_authentication));
    BOOST_REQUIRE(!electrum::is_version(split(mnemonic_two_factor_authentication13), prefix::two_factor_authentication));
    BOOST_REQUIRE(!electrum::is_version(split(mnemonic_two_factor_authentication19), prefix::two_factor_authentication));
}

BOOST_AUTO_TEST_CASE(electrum__is_version__valid__true)
{
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_standard), prefix::standard));
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_witness), prefix::witness));
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_two_factor_authentication), prefix::two_factor_authentication));
    BOOST_REQUIRE(electrum::is_version(split(mnemonic_two_factor_authentication_witness), prefix::two_factor_authentication_witness));
}

BOOST_AUTO_TEST_CASE(electrum__is_version__uppercased__true)
{
    const auto vector = vectors[electrum_vector::english];
    BOOST_REQUIRE(electrum::is_version(split(ascii_to_upper(vector.mnemonic)), vector.prefix));
}

BOOST_AUTO_TEST_CASE(electrum__is_version__mixed__false)
{
    const string_list mixed
    {
        "below", "가격", "あいこくしん", "abaisser", "abaco", "ábaco",
        "abdikace", "abandon", "abacate", "的", "歇", "above"
    };

    BOOST_REQUIRE(!electrum::is_version(mixed, prefix::witness));
}

// to_version

BOOST_AUTO_TEST_CASE(electrum__to_version__all__expected)
{
    BOOST_REQUIRE_EQUAL(electrum::to_version(prefix::old), "old");
    BOOST_REQUIRE_EQUAL(electrum::to_version(prefix::bip39), "bip39");
    BOOST_REQUIRE_EQUAL(electrum::to_version(prefix::standard), "01");
    BOOST_REQUIRE_EQUAL(electrum::to_version(prefix::witness), "100");
    BOOST_REQUIRE_EQUAL(electrum::to_version(prefix::two_factor_authentication), "101");
    BOOST_REQUIRE_EQUAL(electrum::to_version(prefix::two_factor_authentication_witness), "102");
    BOOST_REQUIRE_EQUAL(electrum::to_version(prefix::none), "none");
}

// Public non-static.
// ----------------------------------------------------------------------------

// construct default

BOOST_AUTO_TEST_CASE(electrum__construct_default__always__invalid)
{
    BOOST_REQUIRE(!electrum());
}

// construct copy

BOOST_AUTO_TEST_CASE(electrum__construct_copy__always__expected)
{
    const auto vector1 = vectors[electrum_vector::chinese];
    electrum instance1(vector1.mnemonic);
    electrum instance2(instance1);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), vector1.mnemonic);
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector1.entropy);
    BOOST_REQUIRE(instance2.prefix() == vector1.prefix);
}

// construct move (default)

BOOST_AUTO_TEST_CASE(electrum__construct_move__always__expected)
{
    const auto vector1 = vectors[electrum_vector::spanish3];
    electrum instance1(vector1.mnemonic);
    electrum instance2(std::move(instance1));
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), vector1.mnemonic);
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector1.entropy);
    BOOST_REQUIRE(instance2.prefix() == vector1.prefix);
}

// to_seed

BOOST_AUTO_TEST_CASE(electrum__to_seed__invalid__invalid)
{
    electrum instance;
    BOOST_REQUIRE(!instance);
    BOOST_REQUIRE(!instance.to_seed("passphrase", 42));
}

BOOST_AUTO_TEST_CASE(electrum__to_seed__ascii__expected)
{
    const auto vector = vectors[electrum_vector::english];
    electrum instance(vector.mnemonic);
    BOOST_REQUIRE(instance);

    const auto seed = instance.to_seed(vector.passphrase, hd_private::mainnet);
    BOOST_REQUIRE(seed);
    BOOST_REQUIRE_EQUAL(seed, vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__to_seed__ascii_uppercased__expected)
{
    const auto vector = vectors[electrum_vector::english];
    electrum instance(vector.mnemonic);
    BOOST_REQUIRE(instance);

    const auto seed = instance.to_seed(ascii_to_upper(vector.passphrase), hd_private::mainnet);
    BOOST_REQUIRE(seed);
    BOOST_REQUIRE_EQUAL(seed, vector.to_hd());
}

// assign (default)

BOOST_AUTO_TEST_CASE(electrum__assign_copy__always__expected)
{
    const auto vector1 = vectors[electrum_vector::chinese];
    const electrum instance1(vector1.mnemonic);
    electrum instance2;
    instance2 = instance1;
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), vector1.mnemonic);
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector1.entropy);
    BOOST_REQUIRE(instance2.prefix() == vector1.prefix);
}

BOOST_AUTO_TEST_CASE(electrum__assign_move__always__expected)
{
    const auto vector1 = vectors[electrum_vector::spanish3];
    electrum instance1(vector1.mnemonic);
    electrum instance2;
    instance2 = std::move(instance1);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), vector1.mnemonic);
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector1.entropy);
    BOOST_REQUIRE(instance2.prefix() == vector1.prefix);
}

// in/equality (default)

BOOST_AUTO_TEST_CASE(electrum__equality__always__expected)
{
    const auto vector1 = vectors[electrum_vector::chinese];
    const electrum instance1(vector1.mnemonic);
    const electrum instance2(vector1.mnemonic);
    BOOST_REQUIRE(instance1 == instance2);
}

BOOST_AUTO_TEST_CASE(electrum__inequality_move__always__expected)
{
    const auto vector1 = vectors[electrum_vector::chinese];
    const auto vector2 = vectors[electrum_vector::spanish];
    const electrum instance1(vector1.mnemonic);
    const electrum instance2(vector2.mnemonic);
    BOOST_REQUIRE(instance1 != instance2);
}

// operator>>

BOOST_AUTO_TEST_CASE(electrum__deserialize__valid__expected)
{
    const auto vector = vectors[electrum_vector::japanese];
    std::istringstream in{ vector.mnemonic };
    electrum instance;
    in >> instance;
    BOOST_REQUIRE(instance);

    // The sentence is serialized according to documentation (ideographic_space),
    // but the Electrum test vector japanese mnemonics are joined with ascii spaces.
    BOOST_REQUIRE_EQUAL(instance.sentence(), join(split(vector.mnemonic), ideographic_space));
}

BOOST_AUTO_TEST_CASE(electrum__deserialize__invalid__invalid)
{
    std::istringstream in{ "foobar" };
    electrum instance;
    in >> instance;
    BOOST_REQUIRE(!instance);
}

// operator<<

BOOST_AUTO_TEST_CASE(electrum__serialize__valid__expected)
{
    const auto vector = vectors[electrum_vector::chinese];
    std::ostringstream out;
    electrum instance(vector.mnemonic);
    BOOST_REQUIRE(instance);
    out << instance;
    BOOST_REQUIRE_EQUAL(out.str(), vector.mnemonic);
}

BOOST_AUTO_TEST_CASE(electrum__serialize__invalid__invalid)
{
    std::ostringstream out;
    electrum instance;
    BOOST_REQUIRE(!instance);
    out << instance;
    BOOST_REQUIRE(out.str().empty());
}

// grinder (need to retest once electrum_v1/mnemonic are enabled/tested)
// to_prefix (test once electrum_v1/mnemonic are enabled/tested)

// Standard test vectors.
// ----------------------------------------------------------------------------
// Tested within:
// to_seed(with/without ICU)
// construct (valid words/entropy(0))
// prefix()
// base.lingo()
// base.sentence()
// base.entropy()

// Full round trip Electrum repo tests, constructed from mnemonic and entropy.
// Electrum test vector mnemonics just happen to be prenormalized, even though
// the Electrum repo dictionaries are not all normalized. But our dictionaries
// are fully-normalized, so construction from mnemonics succeeds even without
// WITH_ICU defined. However non-ascii passhrases always require WITH_ICU, so
// those checks are conditionally excluded below.

BOOST_AUTO_TEST_CASE(electrum__vector__english__expected)
{
    const auto index = electrum_vector::english;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__english_with_passphrase__expected)
{
    const auto index = electrum_vector::english_with_passphrase;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__japanese__expected)
{
    const auto index = electrum_vector::japanese;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_NE(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(join(split(instance1.sentence(), ideographic_space)), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__japanese_with_passphrase__expected)
{
    const auto index = electrum_vector::japanese_with_passphrase;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_NE(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(join(split(instance1.sentence(), ideographic_space)), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
#endif
}

BOOST_AUTO_TEST_CASE(electrum__vector__chinese__expected)
{
    const auto index = electrum_vector::chinese;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__chinese_with_passphrase__expected)
{
    const auto index = electrum_vector::chinese_with_passphrase;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
#endif
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish__expected)
{
    const auto index = electrum_vector::spanish;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish_with_passphrase__expected)
{
    const auto index = electrum_vector::spanish_with_passphrase;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
#endif
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish2__expected)
{
    const auto index = electrum_vector::spanish2;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish3__expected)
{
    const auto index = electrum_vector::spanish3;

    const auto vector = vectors[index];
    BOOST_REQUIRE_EQUAL(vector.index, index);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.mnemonic), vector.mnemonic_chunk);
    BOOST_REQUIRE_EQUAL(to_chunk(vector.passphrase), vector.passphrase_chunk);

    electrum instance1(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance1);
    BOOST_REQUIRE(instance1.lingo() == vector.lingo);
    BOOST_REQUIRE(instance1.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance1.sentence(), vector.mnemonic);
    BOOST_REQUIRE_EQUAL(instance1.entropy(), vector.entropy);

    electrum instance2(vector.entropy, vector.prefix, vector.lingo);
    BOOST_REQUIRE(instance2);
    BOOST_REQUIRE(instance2.lingo() == vector.lingo);
    BOOST_REQUIRE(instance2.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance2.sentence(), instance1.sentence());
    BOOST_REQUIRE_EQUAL(instance2.entropy(), vector.entropy);

#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
    BOOST_REQUIRE_EQUAL(instance2.to_seed(vector.passphrase), vector.to_hd());
#endif
}

BOOST_AUTO_TEST_SUITE_END()
