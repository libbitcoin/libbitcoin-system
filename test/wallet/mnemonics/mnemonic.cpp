/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <utility>
#include <sstream>
#include "mnemonic.hpp"

BOOST_AUTO_TEST_SUITE(mnemonic_tests)

using namespace test::mnemonics_mnemonic;
using namespace bc::system::wallet;

#define TODO_TESTS true
#define PUBLIC_STATIC 1
#define PROTECTED_STATIC 1
#define CONSTRUCTORS 1
#define PUBLIC_METHODS 1
#define OPERATORS 1
#define VERIFIED_VECTORS 1

#ifdef PUBLIC_STATIC

// contained_by

BOOST_AUTO_TEST_CASE(mnemonic__contained_by__invalid__none)
{
    BOOST_CHECK(mnemonic::contained_by({ "bogus" }, language::none) == language::none);
}

BOOST_AUTO_TEST_CASE(mnemonic__contained_by__ambiguous__expected)
{
    // contained_by returns the first matching language.
    // The only ambiguous set is en-fr (100 words).
    // So an en match should be explicitly tested against fr.
    BOOST_CHECK(mnemonic::contained_by(ambiguous_en_fr, language::none) == language::en);
    BOOST_CHECK(mnemonic::contained_by(ambiguous_en_fr, language::en) == language::en);
    BOOST_CHECK(mnemonic::contained_by(ambiguous_en_fr, language::fr) == language::fr);
}

BOOST_AUTO_TEST_CASE(mnemonic__contained_by__redundant__expected)
{
    // contained_by returns the first matching language.
    // The only redundant set is zh_Hans-zh_Hant (1275 words).
    BOOST_CHECK(mnemonic::contained_by(redundant_hans_hant, language::none) == language::zh_Hans);
    BOOST_CHECK(mnemonic::contained_by(redundant_hans_hant, language::zh_Hans) == language::zh_Hans);
    BOOST_CHECK(mnemonic::contained_by(redundant_hans_hant, language::zh_Hant) == language::zh_Hant);
}

BOOST_AUTO_TEST_CASE(mnemonic__contained_by__contained__expected)
{
    const auto words = vectors_en[23].words();
    BOOST_CHECK(mnemonic::contained_by(words, language::en) == language::en);
    BOOST_CHECK(mnemonic::contained_by(words, language::none) == language::en);
}

// is_valid_dictionary

BOOST_AUTO_TEST_CASE(mnemonic__is_valid_dictionary__none__false)
{
    BOOST_CHECK(!mnemonic::is_valid_dictionary(language::none));
}

BOOST_AUTO_TEST_CASE(mnemonic__is_valid_dictionary__all__true)
{
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::en));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::es));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::it));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::fr));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::cs));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::pt));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::ja));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::ko));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::zh_Hans));
    BOOST_CHECK(mnemonic::is_valid_dictionary(language::zh_Hant));
}

// is_valid_entropy_size

BOOST_AUTO_TEST_CASE(mnemonic__is_valid_entropy_size__invalid__false)
{
    BOOST_CHECK(!mnemonic::is_valid_entropy_size(15));
    BOOST_CHECK(!mnemonic::is_valid_entropy_size(33));
}

BOOST_AUTO_TEST_CASE(mnemonic__is_valid_entropy_size__valid__true)
{
    BOOST_CHECK(mnemonic::is_valid_entropy_size(16));
    BOOST_CHECK(mnemonic::is_valid_entropy_size(20));
    BOOST_CHECK(mnemonic::is_valid_entropy_size(24));
    BOOST_CHECK(mnemonic::is_valid_entropy_size(28));
    BOOST_CHECK(mnemonic::is_valid_entropy_size(32));
}

// is_valid_word_count

BOOST_AUTO_TEST_CASE(mnemonic__is_valid_word_count__invalid__false)
{
    BOOST_CHECK(!mnemonic::is_valid_word_count(11));
    BOOST_CHECK(!mnemonic::is_valid_word_count(25));
}

BOOST_AUTO_TEST_CASE(mnemonic__is_valid_word_count__valid__true)
{
    BOOST_CHECK(mnemonic::is_valid_word_count(12));
    BOOST_CHECK(mnemonic::is_valid_word_count(15));
    BOOST_CHECK(mnemonic::is_valid_word_count(18));
    BOOST_CHECK(mnemonic::is_valid_word_count(21));
    BOOST_CHECK(mnemonic::is_valid_word_count(24));
}

#endif // PUBLIC_STATIC

#ifdef PROTECTED_STATIC

// coders
// ----------------------------------------------------------------------------

// encoder

BOOST_AUTO_TEST_CASE(mnemonic__encoder__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// decoder

BOOST_AUTO_TEST_CASE(mnemonic__decoder__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// seeder

BOOST_AUTO_TEST_CASE(mnemonic__seeder__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// checksum_byte

BOOST_AUTO_TEST_CASE(mnemonic__checksum_byte__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// sizers
// ----------------------------------------------------------------------------

// checksum_bits1

BOOST_AUTO_TEST_CASE(mnemonic__checksum_bits1__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// checksum_bits2

BOOST_AUTO_TEST_CASE(mnemonic__checksum_bits2__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// entropy_bits1

BOOST_AUTO_TEST_CASE(mnemonic__entropy_bits1__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// entropy_bits2

BOOST_AUTO_TEST_CASE(mnemonic__entropy_bits2__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// entropy_size

BOOST_AUTO_TEST_CASE(mnemonic__entropy_size__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// word_count

BOOST_AUTO_TEST_CASE(mnemonic__word_count__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// checkers
// ----------------------------------------------------------------------------

// is_ambiguous language

BOOST_AUTO_TEST_CASE(mnemonic__is_ambiguous_language__not_none_requested__false)
{
    BOOST_CHECK(TODO_TESTS);
}

BOOST_AUTO_TEST_CASE(mnemonic__is_ambiguous_language__not_en_derived__false)
{
    BOOST_CHECK(TODO_TESTS);
}

BOOST_AUTO_TEST_CASE(mnemonic__is_ambiguous_language__not_fr_contained__false)
{
    BOOST_CHECK(TODO_TESTS);
}

BOOST_AUTO_TEST_CASE(mnemonic__is_ambiguous_language__ambiguous__true)
{
    BOOST_CHECK(TODO_TESTS);
}

// factories
// ----------------------------------------------------------------------------

// from_entropy

BOOST_AUTO_TEST_CASE(mnemonic__from_entropy__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

// from_words

BOOST_AUTO_TEST_CASE(mnemonic__from_words__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

#endif // PROTECTED_STATIC

#ifdef CONSTRUCTORS

// construct default

BOOST_AUTO_TEST_CASE(mnemonic__construct_default__always__invalid)
{
    BOOST_CHECK(!mnemonic());
}

// construct copy

BOOST_AUTO_TEST_CASE(mnemonic__construct_copy__always__expected)
{
    const mnemonic instance1(words12);
    const mnemonic instance2(instance1);
    BOOST_CHECK(instance2);
    BOOST_CHECK_EQUAL(instance2.words(), words12);
    BOOST_CHECK_EQUAL(instance2.entropy(), instance1.entropy());
    BOOST_CHECK(instance2.lingo() == instance1.lingo());
}

// construct move (default)

BOOST_AUTO_TEST_CASE(mnemonic__construct_move__always__expected)
{
    mnemonic instance1(words12);
    const mnemonic instance2(std::move(instance1));
    BOOST_CHECK(instance2);
    BOOST_CHECK_EQUAL(instance2.words(), words12);
    BOOST_CHECK_EQUAL(instance2.entropy(), instance1.entropy());
    BOOST_CHECK(instance2.lingo() == instance1.lingo());
}

// construct mnemonic

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__empty__invalid)
{
    BOOST_CHECK(!mnemonic(""));
    BOOST_CHECK(!mnemonic(split("")));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__misspelled__invalid)
{
    BOOST_CHECK(!mnemonic(invalid_misspelled_words12));
    BOOST_CHECK(!mnemonic(join(invalid_misspelled_words12)));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__short__invalid)
{
    BOOST_CHECK(!mnemonic(words11));
    BOOST_CHECK(!mnemonic(join(words11)));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__bad_checksum__invalid)
{
    BOOST_CHECK(!mnemonic(invalid_checksum_words12));
    BOOST_CHECK(!mnemonic(join(invalid_checksum_words12)));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__incorrect_language__invalid)
{
    BOOST_CHECK(!mnemonic(words12, language::zh_Hant));
    BOOST_CHECK(!mnemonic(join(words12), language::zh_Hant));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__correct_language__valid)
{
    BOOST_CHECK(mnemonic(words12, language::en));
    BOOST_CHECK(mnemonic(join(words12), language::en));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__any_language__valid)
{
    BOOST_CHECK(mnemonic(words12, language::none));
    BOOST_CHECK(mnemonic(join(words12), language::none));
}

// construct entropy

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__empty__invalid)
{
    BOOST_CHECK(TODO_TESTS);
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__short__invalid)
{
    BOOST_CHECK(TODO_TESTS);
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__long__invalid)
{
    BOOST_CHECK(TODO_TESTS);
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__valid__valid)
{
    BOOST_CHECK(TODO_TESTS);
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__bad_checksum__invalid)
{
    BOOST_CHECK(TODO_TESTS);
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__language_none__invalid)
{
    BOOST_CHECK(TODO_TESTS);
}

// construct protected

BOOST_AUTO_TEST_CASE(mnemonic__construct_protected__empty__expected)
{
    const accessor instance({}, {}, language::ja);
    BOOST_CHECK(!instance);
    BOOST_CHECK(instance.entropy().empty());
    BOOST_CHECK(instance.words().empty());
    BOOST_CHECK(instance.lingo() == language::ja);
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_protected__not_empty__expected)
{
    const data_chunk entropy(5, 0x42);
    const string_list words(5, "word");
    const accessor instance(entropy, words, language::ja);
    BOOST_CHECK(instance);
    BOOST_CHECK_EQUAL(instance.entropy(), entropy);
    BOOST_CHECK_EQUAL(instance.words(), words);
    BOOST_CHECK(instance.lingo() == language::ja);
}

#endif // CONSTRUCTORS

#ifdef PUBLIC_METHODS

// to_key

BOOST_AUTO_TEST_CASE(mnemonic__to_key__invalid__invalid)
{
    const mnemonic instance;
    BOOST_CHECK(!instance);
    BOOST_CHECK(!instance.to_key());
}

BOOST_AUTO_TEST_CASE(mnemonic__to_key__valid__valid)
{
    const mnemonic instance(words12);
    BOOST_CHECK(instance);
    BOOST_CHECK(instance.to_key());
}

BOOST_AUTO_TEST_CASE(mnemonic__to_key__network__expected)
{
    const mnemonic instance(words12);
    BOOST_CHECK(instance);
    BOOST_CHECK(starts_with(instance.to_key("foo", btc_mainnet_p2kh).encoded(), "xprv"));
    BOOST_CHECK(starts_with(instance.to_key("bar", btc_testnet_p2kh).encoded(), "tprv"));
}

// to_seed

BOOST_AUTO_TEST_CASE(mnemonic__to_seed__todo__todo)
{
    BOOST_CHECK(TODO_TESTS);
}

#endif // PUBLIC_METHODS

#ifdef OPERATORS

// All operators are implemented in the languages base class.

// assign

BOOST_AUTO_TEST_CASE(mnemonic__assign_copy__always__expected)
{
    const mnemonic instance1(words12);
    mnemonic instance2;
    instance2 = instance1;
    BOOST_CHECK(instance2);
    BOOST_CHECK_EQUAL(instance2.words(), words12);
    BOOST_CHECK_EQUAL(instance2.entropy(), instance1.entropy());
    BOOST_CHECK(instance2.lingo() == instance1.lingo());
}

BOOST_AUTO_TEST_CASE(mnemonic__assign_move__always__expected)
{
    mnemonic instance1(words12);
    mnemonic instance2;
    instance2 = std::move(instance1);
    BOOST_CHECK(instance2);
    BOOST_CHECK_EQUAL(instance2.words(), words12);
    BOOST_CHECK_EQUAL(instance2.entropy(), instance1.entropy());
    BOOST_CHECK(instance2.lingo() == instance1.lingo());
}

// in/equality

BOOST_AUTO_TEST_CASE(mnemonic__equality__always__expected)
{
    const mnemonic instance1(words12);
    const mnemonic instance2(words12);
    BOOST_CHECK(instance1);
    BOOST_CHECK(instance1 == instance2);
}

BOOST_AUTO_TEST_CASE(mnemonic__inequality__always__expected)
{
    const mnemonic instance1(words12);
    const mnemonic instance2(words24);
    BOOST_CHECK(instance1);
    BOOST_CHECK(instance2);
    BOOST_CHECK(instance1 != instance2);
}

// operator>>

BOOST_AUTO_TEST_CASE(mnemonic__deserialize__valid_whitespace__expected_trimmed)
{
    std::istringstream in{ join(words12) };
    mnemonic instance;
    in >> instance;
    BOOST_CHECK(instance);
    BOOST_CHECK_EQUAL(instance.words(), words12);
}

BOOST_AUTO_TEST_CASE(emnemonic__deserialize__invalid__invalid)
{
    std::istringstream in{ "foobar" };
    mnemonic instance;
    in >> instance;
    BOOST_CHECK(!instance);
}

// operator<<

BOOST_AUTO_TEST_CASE(mnemonic__serialize__valid_whitespace__expected)
{
    std::ostringstream out;
    const mnemonic instance(words12);
    out << instance;
    BOOST_CHECK_EQUAL(out.str(), join(words12));
}

BOOST_AUTO_TEST_CASE(mnemonic__serialize__invalid__invalid)
{
    std::ostringstream out;
    const mnemonic instance;
    BOOST_CHECK(!instance);
    out << instance;
    BOOST_CHECK(out.str().empty());
}

#endif // OPERATORS

#ifdef VERIFIED_VECTORS

BOOST_AUTO_TEST_CASE(mnemonic__verify_vectors__sizes__expected)
{
    BOOST_CHECK_EQUAL(vectors_en.size(), 24u);
    BOOST_CHECK_EQUAL(vectors_ja.size(), 24u);
}

BOOST_AUTO_TEST_CASE(mnemonic__verify_vectors__denormalization__expected)
{
#ifdef HAVE_ICU
    const ptrdiff_t abnormals_en = 0;
    const ptrdiff_t abnormals_ja = 22;
#else
    const ptrdiff_t abnormals_en = 0;
    const ptrdiff_t abnormals_ja = 0;
#endif
    // When HAVE_ICU is undefined normalization cannot be verified.
    BOOST_CHECK_EQUAL(abnormals(vectors_en, ascii_space), abnormals_en);
    BOOST_CHECK_EQUAL(abnormals(vectors_ja, ideographic_space), abnormals_ja);
}

BOOST_AUTO_TEST_CASE(mnemonic__verify_vectors__en_whitespace__single_ascii)
{
    for (const auto& vector: vectors_en)
    {
        // Only delimited by ascii_space and no other unicode whitespace.
        const auto words = split(vector.mnemonic, { ascii_space }, unicode_whitespace);
        BOOST_CHECK_EQUAL(join(words, ascii_space), vector.mnemonic);
        BOOST_CHECK(words.size() >= 12u);
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__verify_vectors__ja_whitespace__single_ideographic)
{
    for (const auto& vector: vectors_ja)
    {
        // Only delimited by ideographic_space and no other unicode whitespace.
        const auto words = split(vector.mnemonic, { ideographic_space }, unicode_whitespace);
        BOOST_CHECK_EQUAL(join(words, ideographic_space), vector.mnemonic);
        BOOST_CHECK(words.size() >= 12u);
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__verify_vectors__keys__match_hd_keys)
{
    for (const auto& vector: vectors_ja)
    {
        // The seed is hex encoded input to BIP32 key generation.
        // The key is constructed from BIP32 encoding, e.g. "xprv..."
        BOOST_CHECK_EQUAL(hd_private(to_chunk(vector.seed())), vector.hd_key());
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__vectors_en__expected)
{
    for (const auto& vector: vectors_en)
    {
        const mnemonic instance(vector.entropy());
        BOOST_CHECK(instance);
        BOOST_CHECK(instance.lingo() == language::en);
        BOOST_CHECK_EQUAL(instance.entropy(), vector.entropy());
        BOOST_CHECK_EQUAL(instance.sentence(), vector.sentence());
        BOOST_CHECK_EQUAL(instance.words(), vector.words());
        BOOST_CHECK_EQUAL(instance.to_key(vector.passphrase), vector.hd_key());
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_sentence__vectors_en__expected)
{
    for (const auto& vector: vectors_en)
    {
        const mnemonic instance(vector.mnemonic);
        BOOST_CHECK(instance);
        BOOST_CHECK(instance.lingo() == language::en);
        BOOST_CHECK_EQUAL(instance.entropy(), vector.entropy());
        BOOST_CHECK_EQUAL(instance.sentence(), vector.sentence());
        BOOST_CHECK_EQUAL(instance.words(), vector.words());
        BOOST_CHECK_EQUAL(instance.to_key(vector.passphrase), vector.hd_key());
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__vectors_ja__expected)
{
    for (const auto& vector: vectors_ja)
    {
        const mnemonic instance(vector.entropy(), language::ja);
        BOOST_CHECK(instance);
        BOOST_CHECK(instance.lingo() == language::ja);
        BOOST_CHECK_EQUAL(instance.entropy(), vector.entropy());
#ifdef HAVE_ICU
        // Passphrases are all non-ascii, but 22 of 24 mnemonics are
        // denormalized, so this will fail when HAVE_ICU is undefined.
        BOOST_CHECK_EQUAL(instance.sentence(), vector.sentence(ideographic_space));
        BOOST_CHECK_EQUAL(instance.words(), vector.words());

        // This one check fails for all in vectorized x64, but not in x32.
        // to_key applies pbkd<sha512>
        // sha512 vectorization should be disabled in x32.
        BOOST_CHECK_EQUAL(instance.to_key(vector.passphrase), vector.hd_key());
#endif
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_sentence__vectors_ja__expected)
{
#ifdef HAVE_ICU
    // Passphrases are all ascii, but 22 of 24 mnemonics are denormalized, so this
    // will fail when HAVE_ICU is undefined.
    for (const auto& vector: vectors_ja)
    {
        const mnemonic instance(vector.mnemonic);
        BOOST_CHECK(instance);
        BOOST_CHECK(instance.lingo() == language::ja);
        BOOST_CHECK_EQUAL(instance.entropy(), vector.entropy());
        BOOST_CHECK_EQUAL(instance.sentence(), vector.sentence(ideographic_space));
        BOOST_CHECK_EQUAL(instance.words(), vector.words());

        // This one check fails for all in vectorized x64, but not in x32.
        // to_key applies pbkd<sha512>
        // sha512 vectorization should be disabled in x32.
        BOOST_CHECK_EQUAL(instance.to_key(vector.passphrase), vector.hd_key());
    }
#endif
}

#endif // VERIFIED_VECTORS

BOOST_AUTO_TEST_SUITE_END()