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
#include "../../test.hpp"
#include <sstream>
#include "electrum.hpp"

BC_AUTO_TEST_SUITE(electrum_tests)

using namespace test::mnemonics_electrum;
using namespace bc::system::wallet;
using prefix = electrum::seed_prefix;

#define TODO_TESTS true
#define PUBLIC_STATIC 1
#define PROTECTED_STATIC 1
#define CONSTRUCTORS 1
#define PUBLIC_METHODS 1
#define OPERATORS 1
#define VERIFIED_VECTORS 1

#ifdef PUBLIC_STATIC

// contained_by

BC_AUTO_TEST_CASE(electrum__contained_by__invalid__none)
{
    BC_REQUIRE(electrum::contained_by({ "bogus" }, language::none) == language::none);
}

BC_AUTO_TEST_CASE(electrum__contained_by__ambiguous__expected)
{
    // contained_by returns the first matching language.
    // The only ambiguous set is en-fr (100 words).
    // So an en match should be explicitly tested against fr.
    BC_REQUIRE(electrum::contained_by(ambiguous_en_fr, language::none) == language::en);
    BC_REQUIRE(electrum::contained_by(ambiguous_en_fr, language::en) == language::en);
    BC_REQUIRE(electrum::contained_by(ambiguous_en_fr, language::fr) == language::fr);
}

BC_AUTO_TEST_CASE(electrum__contained_by__redundant__expected)
{
    // contained_by returns the first matching language.
    // The only redundant set is zh_Hans-zh_Hant (1275 words).
    BC_REQUIRE(electrum::contained_by(redundant_hans_hant, language::none) == language::zh_Hans);
    BC_REQUIRE(electrum::contained_by(redundant_hans_hant, language::zh_Hans) == language::zh_Hans);
    BC_REQUIRE(electrum::contained_by(redundant_hans_hant, language::zh_Hant) == language::zh_Hant);
}

BC_AUTO_TEST_CASE(electrum__contained_by__japanese__expected)
{
    const auto vector = vectors[electrum_vector::japanese];
    BC_REQUIRE(electrum::contained_by(split(vector.mnemonic), vector.lingo) == vector.lingo);
    BC_REQUIRE(electrum::contained_by(split(vector.mnemonic), language::none) == vector.lingo);
    BC_REQUIRE(electrum::contained_by(split(vector.mnemonic), language::ko) == language::none);
}

// is_valid_dictionary

BC_AUTO_TEST_CASE(electrum__is_valid_dictionary__none__false)
{
    BC_REQUIRE(!electrum::is_valid_dictionary(language::none));
}

BC_AUTO_TEST_CASE(electrum__is_valid_dictionary__all__true)
{
    BC_REQUIRE(electrum::is_valid_dictionary(language::en));
    BC_REQUIRE(electrum::is_valid_dictionary(language::es));
    BC_REQUIRE(electrum::is_valid_dictionary(language::it));
    BC_REQUIRE(electrum::is_valid_dictionary(language::fr));
    BC_REQUIRE(electrum::is_valid_dictionary(language::cs));
    BC_REQUIRE(electrum::is_valid_dictionary(language::pt));
    BC_REQUIRE(electrum::is_valid_dictionary(language::ja));
    BC_REQUIRE(electrum::is_valid_dictionary(language::ko));
    BC_REQUIRE(electrum::is_valid_dictionary(language::zh_Hans));
    BC_REQUIRE(electrum::is_valid_dictionary(language::zh_Hant));
}

// is_valid_entropy_size

BC_AUTO_TEST_CASE(electrum__is_valid_entropy_size__invalid__false)
{
    BC_REQUIRE(!electrum::is_valid_entropy_size(16));
    BC_REQUIRE(!electrum::is_valid_entropy_size(65));
}

BC_AUTO_TEST_CASE(electrum__is_valid_entropy_size__valid__true)
{
    BC_REQUIRE(electrum::is_valid_entropy_size(17));
    BC_REQUIRE(electrum::is_valid_entropy_size(64));
}

// is_valid_word_count

BC_AUTO_TEST_CASE(electrum__is_valid_word_count__invalid__false)
{
    BC_REQUIRE(!electrum::is_valid_word_count(11));
    BC_REQUIRE(!electrum::is_valid_word_count(47));
}

BC_AUTO_TEST_CASE(electrum__is_valid_word_count__valid__true)
{
    BC_REQUIRE(electrum::is_valid_word_count(12));
    BC_REQUIRE(electrum::is_valid_word_count(46));
}

// is_prefix

BC_AUTO_TEST_CASE(electrum__is_prefix_words__invalid_prefix__false)
{
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_standard), prefix::none));
    BC_REQUIRE(!electrum::is_prefix(split(mnemonic_standard), prefix::bip39));
    BC_REQUIRE(!electrum::is_prefix(split(mnemonic_standard), prefix::old));
}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__valid_prefix__true)
{
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_standard), prefix::standard));
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_witness), prefix::witness));
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_two_factor_authentication), prefix::two_factor_authentication));
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_two_factor_authentication_witness), prefix::two_factor_authentication_witness));

}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__two_factor_authentication_valid_size__true)
{
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_two_factor_authentication12), prefix::two_factor_authentication));
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_two_factor_authentication20), prefix::two_factor_authentication));
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_two_factor_authentication21), prefix::two_factor_authentication));
}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__two_factor_authentication_invalid_size__false)
{
    BC_REQUIRE(!electrum::is_prefix(split(mnemonic_two_factor_authentication11), prefix::two_factor_authentication));
    BC_REQUIRE(!electrum::is_prefix(split(mnemonic_two_factor_authentication13), prefix::two_factor_authentication));
    BC_REQUIRE(!electrum::is_prefix(split(mnemonic_two_factor_authentication19), prefix::two_factor_authentication));
}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__valid__true)
{
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_standard), prefix::standard));
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_witness), prefix::witness));
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_two_factor_authentication), prefix::two_factor_authentication));
    BC_REQUIRE(electrum::is_prefix(split(mnemonic_two_factor_authentication_witness), prefix::two_factor_authentication_witness));
}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__uppercased__true)
{
    const auto vector = vectors[electrum_vector::english];
    BC_REQUIRE(electrum::is_prefix(split(ascii_to_upper(vector.mnemonic)), vector.prefix));
}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__mixed__false)
{
    BC_REQUIRE(!electrum::is_prefix(mixed_words, prefix::witness));
}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__old__true)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__bip39__true)
{
    BC_REQUIRE(electrum::is_prefix(split("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about"), prefix::bip39));
}

BC_AUTO_TEST_CASE(electrum__is_prefix_words__ascii_upper_case__true)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__is_prefix_sentence__valid__true)
{
    BC_REQUIRE(electrum::is_prefix(mnemonic_standard, prefix::standard));
    BC_REQUIRE(electrum::is_prefix(mnemonic_witness, prefix::witness));
    BC_REQUIRE(electrum::is_prefix(mnemonic_two_factor_authentication, prefix::two_factor_authentication));
    BC_REQUIRE(electrum::is_prefix(mnemonic_two_factor_authentication_witness, prefix::two_factor_authentication_witness));
}

// to_prefix

// TODO: grind on mnemonics and electrum_v1 to find conflict test vectors.
// It's very possible that a BIP39-created mnemonic may conflict with electrum,
// but harder to create in the other direction.

BC_AUTO_TEST_CASE(electrum__to_prefix_words__electrum__expected)
{
    BC_REQUIRE(electrum::to_prefix(split(mnemonic_standard)) == prefix::standard);
    BC_REQUIRE(electrum::to_prefix(split(mnemonic_witness)) == prefix::witness);
    BC_REQUIRE(electrum::to_prefix(split(mnemonic_two_factor_authentication)) == prefix::two_factor_authentication);
    BC_REQUIRE(electrum::to_prefix(split(mnemonic_two_factor_authentication_witness)) == prefix::two_factor_authentication_witness);
}

BC_AUTO_TEST_CASE(electrum__to_prefix_words__uppercased__expected)
{
    const auto vector = vectors[electrum_vector::english];
    BC_REQUIRE(electrum::to_prefix(split(ascii_to_upper(vector.mnemonic))) == vector.prefix);
}

BC_AUTO_TEST_CASE(electrum__to_prefix_words__bip39__bip39)
{
    BC_REQUIRE(electrum::to_prefix(split("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about")) == prefix::bip39);
}

BC_AUTO_TEST_CASE(electrum__to_prefix_words__unprefixed__none)
{
    BC_REQUIRE(electrum::to_prefix(split("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon")) == prefix::none);
}

BC_AUTO_TEST_CASE(electrum__to_prefix_words__non_dictionary__none)
{
    BC_REQUIRE(electrum::to_prefix(split("foo bar baz")) == prefix::none);
}

BC_AUTO_TEST_CASE(electrum__to_prefix_sentence__standard__standard)
{
    BC_REQUIRE(electrum::to_prefix(mnemonic_standard) == prefix::standard);
}

// to_key

BC_AUTO_TEST_CASE(electrum__to_key__todo__todo)
{
    BC_REQUIRE(TODO_TESTS);
}

// to_seed

BC_AUTO_TEST_CASE(electrum__to_seed__todo1__todo)
{
    BC_REQUIRE(TODO_TESTS);
}

#endif // PUBLIC_STATIC

#ifdef PROTECTED_STATIC

// coders
// ----------------------------------------------------------------------------

// encoder

BC_AUTO_TEST_CASE(electrum__encoder__invalid__empty)
{
    BC_REQUIRE(accessor::encoder({ 42, 42 }, language::pt).empty());
}

BC_AUTO_TEST_CASE(electrum__encoder__japanese_with_passphrase__expected)
{
    const auto vector = vectors[0];
    const auto expected = split(vector.mnemonic);
    BC_REQUIRE_EQUAL(accessor::encoder(vector.entropy, vector.lingo), expected);
}

// decoder

BC_AUTO_TEST_CASE(electrum__decoder__invalid__empty)
{
    BC_REQUIRE(accessor::decoder({ "42", "42" }, language::es).empty());
}

BC_AUTO_TEST_CASE(electrum__decoder__japanese__expected)
{
    const auto vector = vectors[0];
    BC_REQUIRE_EQUAL(accessor::decoder(split(vector.mnemonic), language::en), vector.entropy);
}

// grinder

BC_AUTO_TEST_CASE(electrum__grinder__english__match_first_iteration)
{
    const auto vector = vectors[0];
    const auto words = split(vector.mnemonic);

    // This verifies that previously-derived entropy round trips.
    const auto result = accessor::grinder(vector.entropy, vector.prefix, vector.lingo, 42);
    BC_REQUIRE_EQUAL(result.entropy, vector.entropy);
    BC_REQUIRE_EQUAL(result.words, words);

    // The derivation is deterministic, always finds previously-found on one iteration.
    BC_REQUIRE_EQUAL(result.iterations, 0u);
}

BC_AUTO_TEST_CASE(electrum__grinder__null_entropy__match_first_iteration)
{
    const data_chunk entropy(17, 0x00);
    const auto find = prefix::two_factor_authentication;

    // This is an example of grinding to find the desired prefix.
    const auto result = accessor::grinder(entropy, find, language::zh_Hans, 1000);
    BC_REQUIRE_NE(result.entropy, entropy);
    BC_REQUIRE(electrum::is_prefix(result.words, find));

    // The derivation is deterministic, always finds this result at 274 iterations.
    BC_REQUIRE_EQUAL(result.iterations, 273u);
}

BC_AUTO_TEST_CASE(electrum__grinder__18_byte_spanish__match_first_iteration)
{
    const auto vector = vectors[6];
    const auto words = split(vector.mnemonic);

    // This is an example of an unused leading byte having no entropy contribution.
    const auto entropy = splice({ 0x00 }, vector.entropy);
    const auto result = accessor::grinder(vector.entropy, vector.prefix, vector.lingo, 42);
    BC_REQUIRE_EQUAL(result.entropy, vector.entropy);
    BC_REQUIRE_EQUAL(result.words, words);
    BC_REQUIRE_EQUAL(result.iterations, 0u);
}

BC_AUTO_TEST_CASE(electrum__grinder__19_byte_spanish__match_first_iteration)
{
    const auto vector = vectors[6];
    const auto words = split(vector.mnemonic);

    // This is an example of usable additional bytes having an entropy contribution.
    const auto entropy = splice({ 0x00, 0x00 }, vector.entropy);
    const auto result = accessor::grinder(entropy, vector.prefix, vector.lingo, 10000);
    BC_REQUIRE(!result.entropy.empty());
    BC_REQUIRE_NE(result.entropy, vector.entropy);
    BC_REQUIRE(electrum::is_prefix(result.words, vector.prefix));
    BC_REQUIRE_EQUAL(result.iterations, 41u);
}

BC_AUTO_TEST_CASE(electrum__grinder__not_found__iterations_expected)
{
    const auto limit = 40u;
    const auto vector = vectors[6];
    const auto entropy = splice({ 0x00, 0x00 }, vector.entropy);
    const auto result = accessor::grinder(entropy, vector.prefix, vector.lingo, limit);
    BC_REQUIRE(result.entropy.empty());
    BC_REQUIRE(result.words.empty());
    BC_REQUIRE_EQUAL(result.iterations, limit);
}

BC_AUTO_TEST_CASE(electrum__grinder__prefix_none__not_found)
{
    const auto limit = 40u;

    // This grinds to the limit but cannot match since "none" is not hexidecimal.
    // The same holds for 'bip39' and 'old'. These are guarded in from_entropy().
    const auto result = accessor::grinder(data_chunk(17, 0x00), prefix::none, language::en, limit);
    BC_REQUIRE_EQUAL(result.iterations, limit);
}

// seeder

BC_AUTO_TEST_CASE(electrum__seeder__non_ascii_passphrase__expected)
{
    BC_REQUIRE_NE(accessor::seeder(split(vectors[8].mnemonic), "なのか ひろい しなん"), null_long_hash);
}

// validator

// The validator cannot match 'old' or 'bip39' because their versions are not hexidecimal.

BC_AUTO_TEST_CASE(electrum__validator__invalid__false)
{
    BC_REQUIRE(!accessor::validator({ "bogus" }, prefix::standard));
}

BC_AUTO_TEST_CASE(electrum__validator__standard__true)
{
    BC_REQUIRE(accessor::validator(split(mnemonic_standard), prefix::standard));
}

BC_AUTO_TEST_CASE(electrum__validator__witness__true)
{
    BC_REQUIRE(accessor::validator(split(mnemonic_witness), prefix::witness));
}

BC_AUTO_TEST_CASE(electrum__validator__two_factor_authentication__true)
{
    BC_REQUIRE(accessor::validator(split(mnemonic_two_factor_authentication), prefix::two_factor_authentication));
}

BC_AUTO_TEST_CASE(electrum__validator__two_factor_authentication_witness__true)
{
    BC_REQUIRE(accessor::validator(split(mnemonic_two_factor_authentication_witness), prefix::two_factor_authentication_witness));
}

// sizers
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(electrum__entropy_bits1__boundaries__expected)
{
    // The number of bits for the given number of bytes.
    BC_REQUIRE_EQUAL(accessor::entropy_bits(data_chunk(17, 0)), 17u * 8u);
    BC_REQUIRE_EQUAL(accessor::entropy_bits(data_chunk(64, 0)), 64u * 8u);
}

BC_AUTO_TEST_CASE(electrum__entropy_bits2__boundaries__expected)
{
    // The number of bits for the given number of words.
    BC_REQUIRE_EQUAL(accessor::entropy_bits(string_list(12, "")), 12u * 11u);
    BC_REQUIRE_EQUAL(accessor::entropy_bits(string_list(46, "")), 46u * 11u);
}

BC_AUTO_TEST_CASE(electrum__entropy_size1__boundaries__expected)
{
    // This must round up to the next byte.
    // The required number of bytes to achieve the given bit strength.
    BC_REQUIRE_EQUAL(accessor::entropy_size(132), 132u / 8u + 1u);
    BC_REQUIRE_EQUAL(accessor::entropy_size(506), 506u / 8u + 1u);
}

BC_AUTO_TEST_CASE(electrum__entropy_size2__boundaries__expected)
{
    // These number of bits are wasted.
    BC_REQUIRE_EQUAL((12 * 11) % 8, 4);
    BC_REQUIRE_EQUAL((46 * 11) % 8, 2);

    // This must round up to the next byte.
    // The required number of bytes for the given number of words.
    BC_REQUIRE_EQUAL(accessor::entropy_size(string_list(12, "")), (12u * 11u) / 8u + 1u);
    BC_REQUIRE_EQUAL(accessor::entropy_size(string_list(46, "")), (46u * 11u) / 8u + 1u);
}

BC_AUTO_TEST_CASE(electrum__word_count1__boundaries__expected)
{
    // These number of bits are wasted.
    BC_REQUIRE_EQUAL((17 * 8) % 11, 4);
    BC_REQUIRE_EQUAL((64 * 8) % 11, 6);

    // The number of words that can be derived from the given entropy size.
    BC_REQUIRE_EQUAL(accessor::word_count(data_chunk(17, 0)), (17u * 8u) / 11u);
    BC_REQUIRE_EQUAL(accessor::word_count(data_chunk(64, 0)), (64u * 8u) / 11u);
}

BC_AUTO_TEST_CASE(electrum__word_count2__boundaries__expected)
{
    // The required number of words to achieve the given bit strength.
    BC_REQUIRE_EQUAL(accessor::word_count(132), 132u / 11u + 0u);
    BC_REQUIRE_EQUAL(accessor::word_count(506), 506u / 11u + 0u);

    // This must round up to the next word.
    BC_REQUIRE_EQUAL(accessor::word_count(131), 131u / 11u + 1u);
    BC_REQUIRE_EQUAL(accessor::word_count(505), 505u / 11u + 1u);
}

BC_AUTO_TEST_CASE(electrum__unused_bits__boundaries__expected)
{
    // The supported word ranges determine the number of wasted entropy bits.
    BC_REQUIRE_EQUAL((17 * 8) % 11, 4);
    BC_REQUIRE_EQUAL((64 * 8) % 11, 6);

    // The number of bits that must be wasted in conversion to words.
    BC_REQUIRE_EQUAL(accessor::unused_bits(data_chunk(17, 0)), (17u * 8u) % 11u);
    BC_REQUIRE_EQUAL(accessor::unused_bits(data_chunk(64, 0)), (64u * 8u) % 11u);
}

BC_AUTO_TEST_CASE(electrum__unused_bytes__boundaries__expected)
{
    // The entropy byte boundaries are chosen to not waste bytes.
    BC_REQUIRE_EQUAL(((17 * 8) % 11) / 8, 0);
    BC_REQUIRE_EQUAL(((64 * 8) % 11) / 8, 0);

    // The number of bytes that must be wasted in conversion to words.
    BC_REQUIRE_EQUAL(accessor::unused_bytes(data_chunk(17, 0)), ((17u * 8u) % 11u) / 8u);
    BC_REQUIRE_EQUAL(accessor::unused_bytes(data_chunk(64, 0)), ((64u * 8u) % 11u) / 8u);
}

BC_AUTO_TEST_CASE(electrum__usable_size__boundaries__expected)
{
    // The entropy byte boundaries are chosen to not waste bytes.
    BC_REQUIRE_EQUAL(17 - ((17 * 8) % 11) / 8, 17);
    BC_REQUIRE_EQUAL(64 - ((64 * 8) % 11) / 8, 64);

    // The number of bytes that can be used in conversion to words.
    BC_REQUIRE_EQUAL(accessor::usable_size(data_chunk(17, 0)), 17u - ((17u * 8u) % 11u) / 8u);
    BC_REQUIRE_EQUAL(accessor::usable_size(data_chunk(64, 0)), 64u - ((64u * 8u) % 11u) / 8u);
}

// checkers
// ----------------------------------------------------------------------------

// is_ambiguous language

BC_AUTO_TEST_CASE(electrum__is_ambiguous_language__not_none_requested__false)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__is_ambiguous_language__not_en_derived__false)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__is_ambiguous_language__not_fr_contained__false)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__is_ambiguous_language__ambiguous__true)
{
    BC_REQUIRE(TODO_TESTS);
}

// is_ambiguous two_factor_authentication

BC_AUTO_TEST_CASE(electrum__is_ambiguous_two_factor_authentication__unambiguous_prefix__false)
{
    BC_REQUIRE(!accessor::is_ambiguous(11, prefix::none));
    BC_REQUIRE(!accessor::is_ambiguous(12, prefix::old));
    BC_REQUIRE(!accessor::is_ambiguous(13, prefix::bip39));
    BC_REQUIRE(!accessor::is_ambiguous(19, prefix::standard));
    BC_REQUIRE(!accessor::is_ambiguous(20, prefix::witness));
    BC_REQUIRE(!accessor::is_ambiguous(21, prefix::two_factor_authentication_witness));
}

BC_AUTO_TEST_CASE(electrum__is_ambiguous_two_factor_authentication__unambiguous_size__false)
{
    BC_REQUIRE(!accessor::is_ambiguous(12, prefix::two_factor_authentication));
    BC_REQUIRE(!accessor::is_ambiguous(20, prefix::two_factor_authentication));
    BC_REQUIRE(!accessor::is_ambiguous(21, prefix::two_factor_authentication));
}

BC_AUTO_TEST_CASE(electrum__is_ambiguous_two_factor_authentication__ambiguous__true)
{
    BC_REQUIRE(accessor::is_ambiguous(11, prefix::two_factor_authentication));
    BC_REQUIRE(accessor::is_ambiguous(13, prefix::two_factor_authentication));
    BC_REQUIRE(accessor::is_ambiguous(19, prefix::two_factor_authentication));
}

// to_conflict
// is_conflict

BC_AUTO_TEST_CASE(electrum__to_conflict__electrum_v1_conflict__true)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__to_conflict__mnemonic_conflict__true)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__to_conflict__unconflicted__false)
{
    BC_REQUIRE(TODO_TESTS);
}

// normalized_to_prefix
// normalized_is_prefix

BC_AUTO_TEST_CASE(electrum__normalized_to_prefix__old_valid__true)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__normalized_to_prefix__old_invalid__false)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__normalized_to_prefix__bip39_invalid__false)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__normalized_to_prefix__bip39_valid__true)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__normalized_to_prefix__ambiguous__false)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__normalized_to_prefix__unambiguous_invalid__false)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__normalized_to_prefix__unambiguous_valid_not_none__false)
{
    BC_REQUIRE(TODO_TESTS);
}

BC_AUTO_TEST_CASE(electrum__normalized_to_prefix__unambiguous_valid_none__true)
{
    BC_REQUIRE(TODO_TESTS);
}

// is_seedable

BC_AUTO_TEST_CASE(electrum__is_seedable__not_seedable__false)
{
    BC_REQUIRE(!accessor::is_seedable(prefix::old));
    BC_REQUIRE(!accessor::is_seedable(prefix::bip39));
    BC_REQUIRE(!accessor::is_seedable(prefix::none));
}

BC_AUTO_TEST_CASE(electrum__is_seedable__seedable__true)
{
    BC_REQUIRE(accessor::is_seedable(prefix::standard));
    BC_REQUIRE(accessor::is_seedable(prefix::witness));
    BC_REQUIRE(accessor::is_seedable(prefix::two_factor_authentication));
    BC_REQUIRE(accessor::is_seedable(prefix::two_factor_authentication_witness));
}

// to_version

BC_AUTO_TEST_CASE(electrum__to_version__all__expected)
{
    BC_REQUIRE_EQUAL(accessor::to_version(prefix::old), "old");
    BC_REQUIRE_EQUAL(accessor::to_version(prefix::bip39), "bip39");
    BC_REQUIRE_EQUAL(accessor::to_version(prefix::standard), "01");
    BC_REQUIRE_EQUAL(accessor::to_version(prefix::witness), "100");
    BC_REQUIRE_EQUAL(accessor::to_version(prefix::two_factor_authentication), "101");
    BC_REQUIRE_EQUAL(accessor::to_version(prefix::two_factor_authentication_witness), "102");
    BC_REQUIRE_EQUAL(accessor::to_version(prefix::none), "none");
}

// factories
// ----------------------------------------------------------------------------

// from_words

BC_AUTO_TEST_CASE(electrum__from_words__empty_none__false)
{
    BC_REQUIRE(!accessor::from_words({}, language::none));
}

BC_AUTO_TEST_CASE(electrum__from_words__bogus_none__false)
{
    BC_REQUIRE(!accessor::from_words({ "bogus", "bogus" }, language::none));
}

BC_AUTO_TEST_CASE(electrum__from_words__low_word_count__false)
{
    BC_REQUIRE(!accessor::from_words(string_list(11, "眼"), language::zh_Hans));
}

BC_AUTO_TEST_CASE(electrum__from_words__minimum_word_count__true)
{
    BC_REQUIRE(accessor::from_words(string_list(12, "眼"), language::zh_Hans));
}

BC_AUTO_TEST_CASE(electrum__from_words__maximum_word_count__true)
{
    BC_REQUIRE(accessor::from_words(string_list(46, "眼"), language::zh_Hans));
}

BC_AUTO_TEST_CASE(electrum__from_words__high_word_count__false)
{
    BC_REQUIRE(!accessor::from_words(string_list(47, "眼"), language::zh_Hans));
}

BC_AUTO_TEST_CASE(electrum__from_words__unknown_prefix__true)
{
    // Add a 13th valid word.
    const auto extended = join({ mnemonic_standard, "ristoro" });

    // The instance is valid in relation to the discovered prefix.
    const auto instance = accessor::from_words(split(extended), language::it);

    // Electrum does not incorporate a checksum.
    // Words construction does not validate a prefix, it computes the prefix.
    BC_REQUIRE(instance);

    // The modified mnemonic is valid but does not have a known prefix.
    BC_REQUIRE(instance.prefix() == prefix::none);
}

BC_AUTO_TEST_CASE(electrum__from_words__mismatched_language__false)
{
    // Specify the incorrect dictionary to preclude mnemonic dictionary matching.
    BC_REQUIRE(!accessor::from_words(split(mnemonic_standard), language::cs));
}

BC_AUTO_TEST_CASE(electrum__from_words__uppercase_standard_italian__true)
{
    const auto instance = accessor::from_words(split(ascii_to_upper(mnemonic_standard)), language::it);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::standard);
}

BC_AUTO_TEST_CASE(electrum__from_words__standard_none__true)
{
    // With 'none' specified the mnemonic is valid if contained in exactly one dictionary
    // (only en-fr) or is the same in multiple dictionaries (only chinese).
    const auto instance = accessor::from_words(split(mnemonic_standard), language::none);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::standard);
}

BC_AUTO_TEST_CASE(electrum__from_words__standard_italian__true)
{
    const auto instance = accessor::from_words(split(mnemonic_standard), language::it);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::standard);
}

BC_AUTO_TEST_CASE(electrum__from_words__witness_italian__true)
{
    const auto instance = accessor::from_words(split(mnemonic_witness), language::it);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::witness);
}

BC_AUTO_TEST_CASE(electrum__from_words__two_factor_authentication_italian__true)
{
    const auto instance = accessor::from_words(split(mnemonic_two_factor_authentication), language::it);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication);
}

BC_AUTO_TEST_CASE(electrum__from_words__two_factor_authentication_witness_italian__true)
{
    const auto instance = accessor::from_words(split(mnemonic_two_factor_authentication_witness), language::it);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication_witness);
}

BC_AUTO_TEST_CASE(electrum__from_words__incorrect_explicit_language__false)
{
    BC_REQUIRE(!accessor::from_words(split(mnemonic_witness), language::ko));
}

BC_AUTO_TEST_CASE(electrum__from_words__conflicting_english_french__false)
{
    // There are 100 common words in en and fr dictionaries, but all of the
    // common words occupy a different index in their respective dictionaries.
    // No other dictionary pair exibits conflicting behavior.
    const auto english = accessor::from_words(ambiguous_en_fr, language::en);
    const auto french = accessor::from_words(ambiguous_en_fr, language::fr);

    // Explicit specification succeeds.
    BC_REQUIRE(english);
    BC_REQUIRE(french);

    // But note that entropy for the same mnemonics is different!
    BC_REQUIRE_NE(english.entropy(), french.entropy());

    // So the language must be explicitly expecified.
    BC_REQUIRE(!accessor::from_words(ambiguous_en_fr, language::none));

    // But this is only required when all words conflict (11 of 12 conflits).
    const auto english2 = accessor::from_words(distinct_en, language::none);
    BC_REQUIRE(english2);
    BC_REQUIRE_NE(english2.entropy(), english.entropy());

    // But this is only required when all words conflict (11 of 12 conflits).
    const auto french2 = accessor::from_words(distinct_fr, language::none);
    BC_REQUIRE(french2);
    BC_REQUIRE_NE(french2.entropy(), french.entropy());
}

BC_AUTO_TEST_CASE(electrum__from_words__conflicting_chinese__true)
{
    // There are 1275 common words in the zh_Hans and zh_Hant dictionaries, but
    // all common words occupy the same index in both dictionaries.
    // No other dictionary pair exibits this partial symmetry.
    const auto simplified = accessor::from_words(redundant_hans_hant, language::zh_Hans);
    const auto traditional = accessor::from_words(redundant_hans_hant, language::zh_Hant);

    // Explicit specification succeeds.
    BC_REQUIRE(simplified);
    BC_REQUIRE(traditional);

    // But note that entropy is the same for each.
    BC_REQUIRE_EQUAL(simplified.entropy(), traditional.entropy());

    // So the language does not ever have to be explicitly expecified.
    const auto none = accessor::from_words(redundant_hans_hant, language::none);
    BC_REQUIRE(none);
    BC_REQUIRE_EQUAL(none.entropy(), traditional.entropy());
    BC_REQUIRE_EQUAL(none.entropy(), simplified.entropy());
}

BC_AUTO_TEST_CASE(electrum__from_words__mixed_words__false)
{
    // All words must be from one dictionary.
    BC_REQUIRE(!accessor::from_words(mixed_words, language::none));
}

BC_AUTO_TEST_CASE(electrum__from_words__similar_words__false)
{
    // Normalization does not reduce á to a, false due to invalid prefix.
    BC_REQUIRE(!accessor::from_words(similar_words, language::none));
}

// from_entropy

BC_AUTO_TEST_CASE(electrum__from_entropy__invalid_prefixes__false)
{
    BC_REQUIRE(!accessor::from_entropy(data_chunk(17, 0x42), prefix::bip39, language::en, max_uint32));
    BC_REQUIRE(!accessor::from_entropy(data_chunk(17, 0x42), prefix::none, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__invalid_sizes__false)
{
    BC_REQUIRE(!accessor::from_entropy({}, prefix::standard, language::en, max_uint32));
    BC_REQUIRE(!accessor::from_entropy(data_chunk(16, 0x42), prefix::standard, language::en, max_uint32));
    BC_REQUIRE(!accessor::from_entropy(data_chunk(65, 0x42), prefix::standard, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__bound_sizes__true)
{
    // These grind, so tests may be slow, but is always the same.
    BC_REQUIRE(accessor::from_entropy(data_chunk(17, 0x42), prefix::standard, language::en, max_uint32));
    BC_REQUIRE(accessor::from_entropy(data_chunk(64, 0x42), prefix::standard, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__v1_prefix_invalid_sizes__false)
{
    BC_REQUIRE(!accessor::from_entropy(data_chunk(15, 0x42), prefix::old, language::en, max_uint32));
    BC_REQUIRE(!accessor::from_entropy(data_chunk(17, 0x42), prefix::old, language::en, max_uint32));
    BC_REQUIRE(!accessor::from_entropy(data_chunk(33, 0x42), prefix::old, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__v1_prefix_valid_sizes__true)
{
    BC_REQUIRE(accessor::from_entropy(data_chunk(16, 0x42), prefix::old, language::en, max_uint32));
    BC_REQUIRE(accessor::from_entropy(data_chunk(32, 0x42), prefix::old, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__language_none__false)
{
    // A mnemonic cannot be created without a specified dictionary.
    BC_REQUIRE(!accessor::from_entropy(vectors[6].entropy, prefix::standard, language::none, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__language_invalid__false)
{
    const auto vector = vectors[6];
    BC_REQUIRE(!accessor::from_entropy(vector.entropy, vector.prefix, language::none, 0));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__ambiguous_two_factor_authentication__false)
{
    const auto size13 = accessor::entropy_size(split(mnemonic_two_factor_authentication13));
    const auto size19 = accessor::entropy_size(split(mnemonic_two_factor_authentication19));
    BC_REQUIRE(!accessor::from_entropy(data_chunk(size13, 0x42), prefix::two_factor_authentication, language::en, max_uint32));
    BC_REQUIRE(!accessor::from_entropy(data_chunk(size19, 0x42), prefix::two_factor_authentication, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__unambiguous_two_factor_authentication__true)
{
    const auto size12 = accessor::entropy_size(split(mnemonic_two_factor_authentication12));
    const auto size20 = accessor::entropy_size(split(mnemonic_two_factor_authentication20));

    // These grind, so tests may be slow, but is always the same.
    BC_REQUIRE(accessor::from_entropy(data_chunk(size12, 0x42), prefix::two_factor_authentication, language::en, max_uint32));
    BC_REQUIRE(accessor::from_entropy(data_chunk(size20, 0x42), prefix::two_factor_authentication, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__valid_entropy_but_prefix_not_found__false)
{
    BC_REQUIRE(!accessor::from_entropy(data_chunk(17, 0x42), prefix::witness, language::en, 0));
}

BC_AUTO_TEST_CASE(electrum__from_entropy__previous_entropy_zero_grind__true)
{
    const auto vector = vectors[6];
    BC_REQUIRE(accessor::from_entropy(vector.entropy, vector.prefix, vector.lingo, 0));
}

#endif // PROTECTED_STATIC

#ifdef CONSTRUCTORS

// construct default

BC_AUTO_TEST_CASE(electrum__construct_default__always__invalid)
{
    BC_REQUIRE(!electrum());
}

// construct copy

BC_AUTO_TEST_CASE(electrum__construct_copy__always__expected)
{
    const auto vector1 = vectors[electrum_vector::chinese];
    const electrum instance1(vector1.mnemonic);
    const electrum instance2(instance1);
    BC_REQUIRE(instance2);
    BC_REQUIRE_EQUAL(instance2.sentence(), vector1.mnemonic);
    BC_REQUIRE_EQUAL(instance2.entropy(), vector1.entropy);
    BC_REQUIRE(instance2.prefix() == vector1.prefix);
    BC_REQUIRE(instance2.lingo() == vector1.lingo);
}

// construct move (default)

BC_AUTO_TEST_CASE(electrum__construct_move__always__expected)
{
    const auto vector1 = vectors[electrum_vector::spanish3];
    electrum instance1(vector1.mnemonic);
    const electrum instance2(std::move(instance1));
    BC_REQUIRE(instance2);
    BC_REQUIRE_EQUAL(instance2.sentence(), vector1.mnemonic);
    BC_REQUIRE_EQUAL(instance2.entropy(), vector1.entropy);
    BC_REQUIRE(instance2.prefix() == vector1.prefix);
    BC_REQUIRE(instance2.lingo() == vector1.lingo);
}

// construct v1 sentence

BC_AUTO_TEST_CASE(electrum__construct_v1_sentence__invalid__invalid_none)
{
    const electrum_v1 v1("foobar");
    const electrum instance(v1);
    BC_REQUIRE(!instance);
    BC_REQUIRE(instance.prefix() == prefix::none);
    BC_REQUIRE(instance.lingo() == language::none);
    BC_REQUIRE(instance.sentence().empty());
}

BC_AUTO_TEST_CASE(electrum__construct_v1_sentence__valid__valid_old_en)
{
    const electrum_v1 v1(distinct_v1);
    const electrum instance(v1);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::old);
    BC_REQUIRE(instance.lingo() == language::en);
    BC_REQUIRE_EQUAL(instance.sentence(), join(distinct_v1));
}

// construct ambiguities

BC_AUTO_TEST_CASE(electrum__construct_sentence__ambiguous_en_v1__valid_old_en)
{
    const electrum instance(ambiguous_en_v1);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::old);
    BC_REQUIRE(instance.lingo() == language::en);
    BC_REQUIRE_EQUAL(instance.sentence(), join(ambiguous_en_v1));
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__ambiguous_fr_pt_v1__valid_old_en)
{
    const electrum instance(ambiguous_fr_pt_v1);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::old);
    BC_REQUIRE(instance.lingo() == language::pt);
    BC_REQUIRE_EQUAL(instance.sentence(), join(ambiguous_fr_pt_v1));
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__ambiguous_en_fr_v1__valid_old_en)
{
    const electrum instance(ambiguous_en_fr_v1);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::old);
    BC_REQUIRE(instance.lingo() == language::en);
    BC_REQUIRE_EQUAL(instance.sentence(), join(ambiguous_en_fr_v1));
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__distinct_en__valid_prefix_none)
{
    const electrum instance(distinct_en);
    BC_REQUIRE(instance);

    // This vector is unprefixed, which is valid and reflected here.
    BC_REQUIRE(instance.prefix() == prefix::none);
    BC_REQUIRE(instance.lingo() == language::en);
    BC_REQUIRE_EQUAL(instance.sentence(), join(distinct_en));
}

// construct two_factor_authentication hack

BC_AUTO_TEST_CASE(electrum__construct_sentence__two_factor_authentication12__valid)
{
    const electrum instance(mnemonic_two_factor_authentication12);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication);
    BC_REQUIRE(instance.lingo() == language::pt);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication12);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__two_factor_authentication13__valid_none)
{
    const electrum instance(mnemonic_two_factor_authentication13);
    BC_REQUIRE(instance);

    // The 2fa prefix is not considered because of the 2fa hack.
    BC_REQUIRE(instance.prefix() == prefix::none);
    BC_REQUIRE(instance.lingo() == language::pt);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication13);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__two_factor_authentication19__valid_none)
{
    const electrum instance(mnemonic_two_factor_authentication19);
    BC_REQUIRE(instance);

    // The 2fa prefix is not considered because of the 2fa hack.
    BC_REQUIRE(instance.prefix() == prefix::none);
    BC_REQUIRE(instance.lingo() == language::pt);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication19);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__two_factor_authentication20__valid)
{
    const electrum instance(mnemonic_two_factor_authentication20);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication);
    BC_REQUIRE(instance.lingo() == language::pt);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication20);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__two_factor_authentication21__valid)
{
    const electrum instance(mnemonic_two_factor_authentication21);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication);
    BC_REQUIRE(instance.lingo() == language::pt);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication21);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__incorrect_language__invalid)
{
    const electrum instance(mnemonic_standard, language::ja);
    BC_REQUIRE(!instance);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__mnemonic_standard__valid)
{
    const electrum instance(mnemonic_standard, language::it);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::standard);
    BC_REQUIRE(instance.lingo() == language::it);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_standard);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__mnemonic_witness__valid)
{
    const electrum instance(mnemonic_witness, language::none);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::witness);
    BC_REQUIRE(instance.lingo() == language::it);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_witness);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__mnemonic_two_factor_authentication__valid)
{
    const electrum instance(mnemonic_two_factor_authentication);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication);
    BC_REQUIRE(instance.lingo() == language::it);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication);
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__mnemonic_two_factor_authentication_witness__valid)
{
    const electrum instance(mnemonic_two_factor_authentication_witness);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication_witness);
    BC_REQUIRE(instance.lingo() == language::it);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication_witness);
}

// construct words

BC_AUTO_TEST_CASE(electrum__construct_words__incorrect_language__invalid)
{
    const electrum instance(split(mnemonic_standard), language::ko);
    BC_REQUIRE(!instance);
}

BC_AUTO_TEST_CASE(electrum__construct_words__mnemonic_standard__valid)
{
    const electrum instance(split(mnemonic_standard), language::it);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::standard);
    BC_REQUIRE(instance.lingo() == language::it);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_standard);
}

BC_AUTO_TEST_CASE(electrum__construct_words__mnemonic_witness__valid)
{
    const electrum instance(split(mnemonic_witness), language::none);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::witness);
    BC_REQUIRE(instance.lingo() == language::it);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_witness);
}

BC_AUTO_TEST_CASE(electrum__construct_words__mnemonic_two_factor_authentication__valid)
{
    const electrum instance(split(mnemonic_two_factor_authentication));
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication);
    BC_REQUIRE(instance.lingo() == language::it);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication);
}

BC_AUTO_TEST_CASE(electrum__construct_words__mnemonic_two_factor_authentication_witness__valid)
{
    const electrum instance(split(mnemonic_two_factor_authentication_witness));
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication_witness);
    BC_REQUIRE(instance.lingo() == language::it);
    BC_REQUIRE_EQUAL(instance.sentence(), mnemonic_two_factor_authentication_witness);
}

// construct entropy

BC_AUTO_TEST_CASE(electrum__construct_entropy__empty__false)
{
    BC_REQUIRE(!electrum({}, prefix::standard, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__low_byte_count__false)
{
    BC_REQUIRE(!electrum(data_chunk(16, 0x42), prefix::standard, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__minimum_byte_count__true)
{
    // This grinds, so tests may be slow, but is always the same.
    BC_REQUIRE(electrum(data_chunk(17, 0x42), prefix::standard, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__maximum_byte_count__true)
{
    // This grinds, so tests may be slow, but is always the same.
    BC_REQUIRE(electrum(data_chunk(64, 0x42), prefix::standard, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__high_byte_count__false)
{
    BC_REQUIRE(!electrum(data_chunk(65, 0x42), prefix::standard, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__invalid_prefixes__false)
{
    BC_REQUIRE(!electrum(data_chunk(17, 0x42), prefix::bip39, language::en, max_uint32));
    BC_REQUIRE(!electrum(data_chunk(17, 0x42), prefix::none, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__prefix_old__expected)
{
    BC_REQUIRE(!electrum(data_chunk(15, 0x00), prefix::old, language::en, max_uint32));
    BC_REQUIRE(electrum(data_chunk(16, 0x00), prefix::old, language::en, max_uint32));
    BC_REQUIRE(!electrum(data_chunk(17, 0x00), prefix::old, language::en, max_uint32));
    BC_REQUIRE(electrum(data_chunk(32, 0x00), prefix::old, language::en, max_uint32));
    BC_REQUIRE(!electrum(data_chunk(33, 0x00), prefix::old, language::en, max_uint32));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__valid_entropy_but_prefix_not_found__false)
{
    BC_REQUIRE(!electrum(data_chunk(17, 0x42), prefix::standard, language::en, 0));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__previous_entropy_zero_grind__true)
{
    // Previous entropy round trips without grinding.
    const auto vector = vectors[6];
    BC_REQUIRE(electrum(vector.entropy, vector.prefix, vector.lingo, 0));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__invalid_language__false)
{
    const auto vector = vectors[6];
    BC_REQUIRE(!electrum(vector.entropy, vector.prefix, language::none, 0));
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__language_none__false)
{
    // A mnemonic cannot be created without a specified dictionary.
    BC_REQUIRE(!electrum(vectors[6].entropy, prefix::standard, language::none, max_uint32));
}

// construct protected

BC_AUTO_TEST_CASE(electrum__construct_protected__empty__empty)
{
    const accessor instance({}, {}, language::ja, prefix::bip39);
    BC_REQUIRE(!instance);
    BC_REQUIRE(instance.entropy().empty());
    BC_REQUIRE(instance.words().empty());
    BC_REQUIRE(instance.lingo() == language::ja);
    BC_REQUIRE(instance.prefix() == prefix::bip39);
}

BC_AUTO_TEST_CASE(electrum__construct_protected__not_empty__expected)
{
    const data_chunk entropy(5, 0x42);
    const string_list words(5, "word");
    const accessor instance(entropy, words, language::ja, prefix::bip39);
    BC_REQUIRE(instance);
    BC_REQUIRE_EQUAL(instance.entropy(), entropy);
    BC_REQUIRE_EQUAL(instance.words(), words);
    BC_REQUIRE(instance.lingo() == language::ja);
    BC_REQUIRE(instance.prefix() == prefix::bip39);
}

#endif // CONSTRUCTORS

#ifdef PUBLIC_METHODS

// prefix

BC_AUTO_TEST_CASE(electrum__prefix__valid__expected)
{
    const electrum instance(mnemonic_two_factor_authentication_witness);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::two_factor_authentication_witness);
}

// to_key (TODO: bip39, old, none)

BC_AUTO_TEST_CASE(electrum__to_key__invalid__invalid)
{
    const electrum instance;
    BC_REQUIRE(!instance);
    BC_REQUIRE(!instance.to_key());
}

BC_AUTO_TEST_CASE(electrum__to_key__prefix_none__invalid)
{
    const electrum instance(mnemonic_two_factor_authentication19);
    BC_REQUIRE(instance);

    // The 2fa prefix is 'none' (considered unprefixed) because of the 2fa hack.
    BC_REQUIRE(instance.prefix() == prefix::none);
    BC_REQUIRE(!instance.to_key());
}

BC_AUTO_TEST_CASE(electrum__to_key__prefix_bip39__invalid)
{
    // Valid (non-electrum/electrum_v1) BIP39 entropy from Trezor test vectors.
    const electrum instance("legal winner thank year wave sausage worth useful legal winner thank yellow");
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == prefix::bip39);
    BC_REQUIRE(!instance.to_key());
}

BC_AUTO_TEST_CASE(electrum__to_key__ascii__expected)
{
    const auto vector = vectors[electrum_vector::english];
    const electrum instance(vector.mnemonic);
    BC_REQUIRE(instance);

    const auto seed = instance.to_key(vector.passphrase, btc_mainnet);
    BC_REQUIRE(seed);
    BC_REQUIRE_EQUAL(seed, vector.to_hd());
}

BC_AUTO_TEST_CASE(electrum__to_key__ascii_uppercased__expected)
{
    const auto vector = vectors[electrum_vector::english];
    const electrum instance(vector.mnemonic);
    BC_REQUIRE(instance);

    const auto seed = instance.to_key(ascii_to_upper(vector.passphrase), btc_mainnet);
    BC_REQUIRE(seed);
    BC_REQUIRE_EQUAL(seed, vector.to_hd());
}

BC_AUTO_TEST_CASE(electrum__to_key__ascii_passphrase_mainnet__expected_hd_key)
{
    const electrum instance(split(vectors[7].mnemonic));
    const auto result = instance.to_key("Did you ever hear the tragedy of Darth Plagueis the Wise?", btc_mainnet);
    BC_REQUIRE_EQUAL(result.encoded(), "xprv9s21ZrQH143K3J7uaJtH3fPYadCubChqdXxqpWVDU9PasYoWhbCXLLQCP1DtCtuixztKC6eYXXqrk1an8sztbZ8L53MDLsvKLHZ3GaxY1d8");
}

BC_AUTO_TEST_CASE(electrum__to_key__ascii_passphrase_testnet__expected_hd_key)
{
    const electrum instance(split(vectors[7].mnemonic));
    const auto result = instance.to_key("Did you ever hear the tragedy of Darth Plagueis the Wise?", btc_testnet);
    BC_REQUIRE_EQUAL(result.encoded(), "tprv8ZgxMBicQKsPe7MSEsjnDK1Xtkd7pijqy5sxgvufx7t4f9YbgxYGr5meJBPYDGJ3LSR6CCGJgtRfCs8XG6LqQcPvbgZX1EeNFPJTiGTAqkt");
}

// to_seed

BC_AUTO_TEST_CASE(electrum__to_seed__todo2__todo)
{
    BC_REQUIRE(TODO_TESTS);
}

#endif // PUBLIC_METHODS

#ifdef OPERATORS

// All operators are implemented in the languages base class.

// assign

BC_AUTO_TEST_CASE(electrum__assign_copy__always__expected)
{
    const auto vector1 = vectors[electrum_vector::chinese];
    const electrum instance1(vector1.mnemonic);
    electrum instance2;
    instance2 = instance1;
    BC_REQUIRE(instance2);
    BC_REQUIRE_EQUAL(instance2.sentence(), vector1.mnemonic);
    BC_REQUIRE_EQUAL(instance2.entropy(), vector1.entropy);
    BC_REQUIRE(instance2.lingo() == instance1.lingo());
    BC_REQUIRE(instance2.prefix() == vector1.prefix);
}

BC_AUTO_TEST_CASE(electrum__assign_move__always__expected)
{
    const auto vector1 = vectors[electrum_vector::spanish3];
    electrum instance1(vector1.mnemonic);
    electrum instance2;
    instance2 = std::move(instance1);
    BC_REQUIRE(instance2);
    BC_REQUIRE_EQUAL(instance2.sentence(), vector1.mnemonic);
    BC_REQUIRE_EQUAL(instance2.entropy(), vector1.entropy);
    BC_REQUIRE(instance2.prefix() == vector1.prefix);
    BC_REQUIRE(instance2.lingo() == instance1.lingo());
}

// in/equality

BC_AUTO_TEST_CASE(electrum__equality__always__expected)
{
    const auto vector1 = vectors[electrum_vector::chinese];
    const electrum instance1(vector1.mnemonic);
    const electrum instance2(vector1.mnemonic);
    BC_REQUIRE(instance1);
    BC_REQUIRE(instance1 == instance2);
}

BC_AUTO_TEST_CASE(electrum__inequality__always__expected)
{
    const auto vector1 = vectors[electrum_vector::chinese];
    const auto vector2 = vectors[electrum_vector::spanish];
    const electrum instance1(vector1.mnemonic);
    const electrum instance2(vector2.mnemonic);
    BC_REQUIRE(instance1);
    BC_REQUIRE(instance2);
    BC_REQUIRE(instance1 != instance2);
}

// operator>>

BC_AUTO_TEST_CASE(electrum__deserialize__valid__expected)
{
    const auto vector = vectors[electrum_vector::japanese];
    std::istringstream in{ vector.mnemonic };
    electrum instance;
    in >> instance;
    BC_REQUIRE(instance);

    // The sentence is serialized according to documentation (ideographic_space),
    // but the Electrum test vector japanese mnemonics are joined with ascii spaces.
    BC_REQUIRE_EQUAL(instance.sentence(), join(split(vector.mnemonic), ideographic_space));
}

BC_AUTO_TEST_CASE(electrum__deserialize__invalid__invalid)
{
    std::istringstream in{ "foobar" };
    electrum instance;
    in >> instance;
    BC_REQUIRE(!instance);
}

// operator<<

BC_AUTO_TEST_CASE(electrum__serialize__valid__expected)
{
    const auto vector = vectors[electrum_vector::chinese];
    std::ostringstream out;
    const electrum instance(vector.mnemonic);
    BC_REQUIRE(instance);
    out << instance;
    BC_REQUIRE_EQUAL(out.str(), vector.mnemonic);
}

BC_AUTO_TEST_CASE(electrum__serialize__invalid__invalid)
{
    std::ostringstream out;
    const electrum instance;
    BC_REQUIRE(!instance);
    out << instance;
    BC_REQUIRE(out.str().empty());
}

#endif // OPERATORS

#ifdef VERIFIED_VECTORS

BC_AUTO_TEST_CASE(electrum__verify_vectors__size__expected)
{
    BC_REQUIRE_EQUAL(vectors.size(), 10u);
}

BC_AUTO_TEST_CASE(electrum__verify_vectors__denormalization__expected)
{
    constexpr ptrdiff_t abnormal_vectors{};
    BC_REQUIRE_EQUAL(abnormals(vectors, ascii_space), abnormal_vectors);
}

BC_AUTO_TEST_CASE(electrum__verify_vectors__all_whitespace__single_ascii)
{
    for (const auto& vector: vectors)
    {
        // Only delimited by ascii_space and no other unicode whitespace.
        const auto words = split(vector.mnemonic, { ascii_space }, unicode_whitespace);
        BC_REQUIRE_EQUAL(join(words, ascii_space), vector.mnemonic);
        BC_REQUIRE_EQUAL(words.size(), 12u);
    }
}

BC_AUTO_TEST_CASE(electrum__verify_vectors__strings__match_chunks)
{
    for (const auto& vector: vectors)
    {
        BC_REQUIRE_EQUAL(vector.mnemonic, to_string(vector.mnemonic_chunk));
        BC_REQUIRE_EQUAL(vector.passphrase, to_string(vector.passphrase_chunk));
    }
}

// Full round trip Electrum repo tests, constructed from mnemonic and entropy.
// Electrum test vector mnemonics just happen to be prenormalized, even though
// the Electrum repo dictionaries are not all normalized. But our dictionaries
// are fully-normalized, so construction from mnemonics succeeds with all
// non-ascii passphrases now supported via embedded Unicode tables.

BC_AUTO_TEST_CASE(electrum__construct_entropy__to_key__expected)
{
    const auto& vector_names = all_vector_names;

    for (const auto& vector_name: vector_names)
    {
        const auto vector = vectors[vector_name];
        const electrum instance(vector.mnemonic, vector.lingo);
        BC_REQUIRE_EQUAL(instance.to_key(vector.passphrase), vector.to_hd());
    }
}

BC_AUTO_TEST_CASE(electrum__construct_entropy__vectors__expected)
{
    for (const auto& vector: vectors)
    {
        const electrum instance(vector.entropy, vector.prefix, vector.lingo);
        BC_REQUIRE(instance);
        BC_REQUIRE_EQUAL(instance.sentence(), vector.sentence(vector.lingo));
        BC_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
        BC_REQUIRE(instance.prefix() == vector.prefix);
        BC_REQUIRE(instance.lingo() == vector.lingo);
    }
}

BC_AUTO_TEST_CASE(electrum__construct_sentence__vectors__expected)
{
    for (const auto& vector: vectors)
    {
        const electrum instance(vector.mnemonic, vector.lingo);
        BC_REQUIRE(instance);
        BC_REQUIRE_EQUAL(instance.sentence(), vector.sentence(vector.lingo));
        BC_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
        BC_REQUIRE(instance.prefix() == vector.prefix);
        BC_REQUIRE(instance.lingo() == vector.lingo);
    }
}

#endif // VERIFIED_VECTORS


// to_prefix

BC_AUTO_TEST_CASE(electrum__to_prefix__electrum_v1_words__old)
{
    const string_list v1_words
    {
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith"
    };

    BC_REQUIRE(electrum::to_prefix(v1_words) == electrum::seed_prefix::old);
}

BC_AUTO_TEST_CASE(electrum__to_prefix__not_in_any_dictionary__none)
{
    BC_REQUIRE(electrum::to_prefix(string_list(12, "bogus")) == electrum::seed_prefix::none);
}

BC_AUTO_TEST_CASE(electrum__to_prefix__sentence__matches_words)
{
    const std::string sentence{ "blind faith blind faith blind faith blind faith blind faith blind faith" };
    BC_REQUIRE(electrum::to_prefix(sentence) == electrum::seed_prefix::old);
}

BC_AUTO_TEST_CASE(electrum__is_prefix__sentence__expected)
{
    const std::string sentence{ "blind faith blind faith blind faith blind faith blind faith blind faith" };
    BC_REQUIRE(electrum::is_prefix(sentence, electrum::seed_prefix::old));
}

// to_seed

BC_AUTO_TEST_CASE(electrum__to_seed__invalid__empty)
{
    BC_REQUIRE_EQUAL(electrum{}.to_seed(""), long_hash{});
}

// keys

BC_AUTO_TEST_CASE(electrum__to_key__seed__round_trips)
{
    long_hash seed{};
    seed.fill(0x42);
    const auto key = electrum::to_key(seed, btc_mainnet);
    BC_REQUIRE(key);
    BC_REQUIRE_EQUAL(electrum::to_seed(key), seed);
}

// The first half of the seed is the secret, which must ec verify.
BC_AUTO_TEST_CASE(electrum__to_key__null_seed__invalid)
{
    BC_REQUIRE(!electrum::to_key(long_hash{}, btc_mainnet));
}


// Only native entropy prefixes are seedable.
BC_AUTO_TEST_CASE(electrum__to_seed__electrum_v1_prefix__empty)
{
    const string_list v1_words
    {
        "blind", "faith", "blind", "faith", "blind", "faith",
        "blind", "faith", "blind", "faith", "blind", "faith"
    };

    const electrum instance(v1_words, language::en);
    BC_REQUIRE(instance);
    BC_REQUIRE(instance.prefix() == electrum::seed_prefix::old);
    BC_REQUIRE_EQUAL(instance.to_seed(""), long_hash{});
}

BC_AUTO_TEST_SUITE_END()
