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
#include "mnemonic.hpp"

BC_AUTO_TEST_SUITE(mnemonic_tests)

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

BC_AUTO_TEST_CASE(mnemonic__contained_by__invalid__none)
{
    BC_CHECK(mnemonic::contained_by({ "bogus" }, language::none) == language::none);
}

BC_AUTO_TEST_CASE(mnemonic__contained_by__ambiguous__expected)
{
    // contained_by returns the first matching language.
    // The only ambiguous set is en-fr (100 words).
    // So an en match should be explicitly tested against fr.
    BC_CHECK(mnemonic::contained_by(ambiguous_en_fr, language::none) == language::en);
    BC_CHECK(mnemonic::contained_by(ambiguous_en_fr, language::en) == language::en);
    BC_CHECK(mnemonic::contained_by(ambiguous_en_fr, language::fr) == language::fr);
}

BC_AUTO_TEST_CASE(mnemonic__contained_by__redundant__expected)
{
    // contained_by returns the first matching language.
    // The only redundant set is zh_Hans-zh_Hant (1275 words).
    BC_CHECK(mnemonic::contained_by(redundant_hans_hant, language::none) == language::zh_Hans);
    BC_CHECK(mnemonic::contained_by(redundant_hans_hant, language::zh_Hans) == language::zh_Hans);
    BC_CHECK(mnemonic::contained_by(redundant_hans_hant, language::zh_Hant) == language::zh_Hant);
}

BC_AUTO_TEST_CASE(mnemonic__contained_by__contained__expected)
{
    const auto words = vectors_en[23].words();
    BC_CHECK(mnemonic::contained_by(words, language::en) == language::en);
    BC_CHECK(mnemonic::contained_by(words, language::none) == language::en);
}

// is_valid_dictionary

BC_AUTO_TEST_CASE(mnemonic__is_valid_dictionary__none__false)
{
    BC_CHECK(!mnemonic::is_valid_dictionary(language::none));
}

BC_AUTO_TEST_CASE(mnemonic__is_valid_dictionary__all__true)
{
    BC_CHECK(mnemonic::is_valid_dictionary(language::en));
    BC_CHECK(mnemonic::is_valid_dictionary(language::es));
    BC_CHECK(mnemonic::is_valid_dictionary(language::it));
    BC_CHECK(mnemonic::is_valid_dictionary(language::fr));
    BC_CHECK(mnemonic::is_valid_dictionary(language::cs));
    BC_CHECK(mnemonic::is_valid_dictionary(language::pt));
    BC_CHECK(mnemonic::is_valid_dictionary(language::ja));
    BC_CHECK(mnemonic::is_valid_dictionary(language::ko));
    BC_CHECK(mnemonic::is_valid_dictionary(language::zh_Hans));
    BC_CHECK(mnemonic::is_valid_dictionary(language::zh_Hant));
}

// is_valid_entropy_size

BC_AUTO_TEST_CASE(mnemonic__is_valid_entropy_size__invalid__false)
{
    BC_CHECK(!mnemonic::is_valid_entropy_size(15));
    BC_CHECK(!mnemonic::is_valid_entropy_size(33));
}

BC_AUTO_TEST_CASE(mnemonic__is_valid_entropy_size__valid__true)
{
    BC_CHECK(mnemonic::is_valid_entropy_size(16));
    BC_CHECK(mnemonic::is_valid_entropy_size(20));
    BC_CHECK(mnemonic::is_valid_entropy_size(24));
    BC_CHECK(mnemonic::is_valid_entropy_size(28));
    BC_CHECK(mnemonic::is_valid_entropy_size(32));
}

// is_valid_word_count

BC_AUTO_TEST_CASE(mnemonic__is_valid_word_count__invalid__false)
{
    BC_CHECK(!mnemonic::is_valid_word_count(11));
    BC_CHECK(!mnemonic::is_valid_word_count(25));
}

BC_AUTO_TEST_CASE(mnemonic__is_valid_word_count__valid__true)
{
    BC_CHECK(mnemonic::is_valid_word_count(12));
    BC_CHECK(mnemonic::is_valid_word_count(15));
    BC_CHECK(mnemonic::is_valid_word_count(18));
    BC_CHECK(mnemonic::is_valid_word_count(21));
    BC_CHECK(mnemonic::is_valid_word_count(24));
}

#endif // PUBLIC_STATIC

#ifdef PROTECTED_STATIC

// coders
// ----------------------------------------------------------------------------

// encoder

BC_AUTO_TEST_CASE(mnemonic__encoder__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// decoder

BC_AUTO_TEST_CASE(mnemonic__decoder__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// seeder

BC_AUTO_TEST_CASE(mnemonic__seeder__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// checksum_byte

BC_AUTO_TEST_CASE(mnemonic__checksum_byte__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// sizers
// ----------------------------------------------------------------------------

// checksum_bits1

BC_AUTO_TEST_CASE(mnemonic__checksum_bits1__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// checksum_bits2

BC_AUTO_TEST_CASE(mnemonic__checksum_bits2__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// entropy_bits1

BC_AUTO_TEST_CASE(mnemonic__entropy_bits1__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// entropy_bits2

BC_AUTO_TEST_CASE(mnemonic__entropy_bits2__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// entropy_size

BC_AUTO_TEST_CASE(mnemonic__entropy_size__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// word_count

BC_AUTO_TEST_CASE(mnemonic__word_count__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// checkers
// ----------------------------------------------------------------------------

// is_ambiguous language

BC_AUTO_TEST_CASE(mnemonic__is_ambiguous_language__not_none_requested__false)
{
    BC_CHECK(TODO_TESTS);
}

BC_AUTO_TEST_CASE(mnemonic__is_ambiguous_language__not_en_derived__false)
{
    BC_CHECK(TODO_TESTS);
}

BC_AUTO_TEST_CASE(mnemonic__is_ambiguous_language__not_fr_contained__false)
{
    BC_CHECK(TODO_TESTS);
}

BC_AUTO_TEST_CASE(mnemonic__is_ambiguous_language__ambiguous__true)
{
    BC_CHECK(TODO_TESTS);
}

// factories
// ----------------------------------------------------------------------------

// from_entropy

BC_AUTO_TEST_CASE(mnemonic__from_entropy__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

// from_words

BC_AUTO_TEST_CASE(mnemonic__from_words__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

#endif // PROTECTED_STATIC

#ifdef CONSTRUCTORS

// construct default

BC_AUTO_TEST_CASE(mnemonic__construct_default__always__invalid)
{
    BC_CHECK(!mnemonic());
}

// construct copy

BC_AUTO_TEST_CASE(mnemonic__construct_copy__always__expected)
{
    const mnemonic instance1(words12);
    const mnemonic instance2(instance1);
    BC_CHECK(instance2);
    BC_CHECK_EQUAL(instance2.words(), words12);
    BC_CHECK_EQUAL(instance2.entropy(), instance1.entropy());
    BC_CHECK(instance2.lingo() == instance1.lingo());
}

// construct move (default)

BC_AUTO_TEST_CASE(mnemonic__construct_move__always__expected)
{
    mnemonic instance1(words12);
    const mnemonic instance2(std::move(instance1));
    BC_CHECK(instance2);
    BC_CHECK_EQUAL(instance2.words(), words12);
    BC_CHECK_EQUAL(instance2.entropy(), instance1.entropy());
    BC_CHECK(instance2.lingo() == instance1.lingo());
}

// construct mnemonic

BC_AUTO_TEST_CASE(mnemonic__construct_mnemonic__empty__invalid)
{
    BC_CHECK(!mnemonic(""));
    BC_CHECK(!mnemonic(split("")));
}

BC_AUTO_TEST_CASE(mnemonic__construct_mnemonic__misspelled__invalid)
{
    BC_CHECK(!mnemonic(invalid_misspelled_words12));
    BC_CHECK(!mnemonic(join(invalid_misspelled_words12)));
}

BC_AUTO_TEST_CASE(mnemonic__construct_mnemonic__short__invalid)
{
    BC_CHECK(!mnemonic(words11));
    BC_CHECK(!mnemonic(join(words11)));
}

BC_AUTO_TEST_CASE(mnemonic__construct_mnemonic__bad_checksum__invalid)
{
    BC_CHECK(!mnemonic(invalid_checksum_words12));
    BC_CHECK(!mnemonic(join(invalid_checksum_words12)));
}

BC_AUTO_TEST_CASE(mnemonic__construct_mnemonic__incorrect_language__invalid)
{
    BC_CHECK(!mnemonic(words12, language::zh_Hant));
    BC_CHECK(!mnemonic(join(words12), language::zh_Hant));
}

BC_AUTO_TEST_CASE(mnemonic__construct_mnemonic__correct_language__valid)
{
    BC_CHECK(mnemonic(words12, language::en));
    BC_CHECK(mnemonic(join(words12), language::en));
}

BC_AUTO_TEST_CASE(mnemonic__construct_mnemonic__any_language__valid)
{
    BC_CHECK(mnemonic(words12, language::none));
    BC_CHECK(mnemonic(join(words12), language::none));
}

// construct entropy

BC_AUTO_TEST_CASE(mnemonic__construct_entropy__empty__invalid)
{
    BC_CHECK(TODO_TESTS);
}

BC_AUTO_TEST_CASE(mnemonic__construct_entropy__short__invalid)
{
    BC_CHECK(TODO_TESTS);
}

BC_AUTO_TEST_CASE(mnemonic__construct_entropy__long__invalid)
{
    BC_CHECK(TODO_TESTS);
}

BC_AUTO_TEST_CASE(mnemonic__construct_entropy__valid__valid)
{
    BC_CHECK(TODO_TESTS);
}

BC_AUTO_TEST_CASE(mnemonic__construct_entropy__bad_checksum__invalid)
{
    BC_CHECK(TODO_TESTS);
}

BC_AUTO_TEST_CASE(mnemonic__construct_entropy__language_none__invalid)
{
    BC_CHECK(TODO_TESTS);
}

// construct protected

BC_AUTO_TEST_CASE(mnemonic__construct_protected__empty__expected)
{
    const accessor instance({}, {}, language::ja);
    BC_CHECK(!instance);
    BC_CHECK(instance.entropy().empty());
    BC_CHECK(instance.words().empty());
    BC_CHECK(instance.lingo() == language::ja);
}

BC_AUTO_TEST_CASE(mnemonic__construct_protected__not_empty__expected)
{
    const data_chunk entropy(5, 0x42);
    const string_list words(5, "word");
    const accessor instance(entropy, words, language::ja);
    BC_CHECK(instance);
    BC_CHECK_EQUAL(instance.entropy(), entropy);
    BC_CHECK_EQUAL(instance.words(), words);
    BC_CHECK(instance.lingo() == language::ja);
}

#endif // CONSTRUCTORS

#ifdef PUBLIC_METHODS

// to_key

BC_AUTO_TEST_CASE(mnemonic__to_key__invalid__invalid)
{
    const mnemonic instance;
    BC_CHECK(!instance);
    BC_CHECK(!instance.to_key());
}

BC_AUTO_TEST_CASE(mnemonic__to_key__valid__valid)
{
    const mnemonic instance(words12);
    BC_CHECK(instance);
    BC_CHECK(instance.to_key());
}

BC_AUTO_TEST_CASE(mnemonic__to_key__network__expected)
{
    const mnemonic instance(words12);
    BC_CHECK(instance);
    BC_CHECK(starts_with(instance.to_key("foo", btc_mainnet).encoded(), "xprv"));
    BC_CHECK(starts_with(instance.to_key("bar", btc_testnet).encoded(), "tprv"));
}

// to_seed

BC_AUTO_TEST_CASE(mnemonic__to_seed__todo__todo)
{
    BC_CHECK(TODO_TESTS);
}

#endif // PUBLIC_METHODS

#ifdef OPERATORS

// All operators are implemented in the languages base class.

// assign

BC_AUTO_TEST_CASE(mnemonic__assign_copy__always__expected)
{
    const mnemonic instance1(words12);
    mnemonic instance2;
    instance2 = instance1;
    BC_CHECK(instance2);
    BC_CHECK_EQUAL(instance2.words(), words12);
    BC_CHECK_EQUAL(instance2.entropy(), instance1.entropy());
    BC_CHECK(instance2.lingo() == instance1.lingo());
}

BC_AUTO_TEST_CASE(mnemonic__assign_move__always__expected)
{
    mnemonic instance1(words12);
    mnemonic instance2;
    instance2 = std::move(instance1);
    BC_CHECK(instance2);
    BC_CHECK_EQUAL(instance2.words(), words12);
    BC_CHECK_EQUAL(instance2.entropy(), instance1.entropy());
    BC_CHECK(instance2.lingo() == instance1.lingo());
}

// in/equality

BC_AUTO_TEST_CASE(mnemonic__equality__always__expected)
{
    const mnemonic instance1(words12);
    const mnemonic instance2(words12);
    BC_CHECK(instance1);
    BC_CHECK(instance1 == instance2);
}

BC_AUTO_TEST_CASE(mnemonic__inequality__always__expected)
{
    const mnemonic instance1(words12);
    const mnemonic instance2(words24);
    BC_CHECK(instance1);
    BC_CHECK(instance2);
    BC_CHECK(instance1 != instance2);
}

// operator>>

BC_AUTO_TEST_CASE(mnemonic__deserialize__valid_whitespace__expected_trimmed)
{
    std::istringstream in{ join(words12) };
    mnemonic instance;
    in >> instance;
    BC_CHECK(instance);
    BC_CHECK_EQUAL(instance.words(), words12);
}

BC_AUTO_TEST_CASE(emnemonic__deserialize__invalid__invalid)
{
    std::istringstream in{ "foobar" };
    mnemonic instance;
    in >> instance;
    BC_CHECK(!instance);
}

// operator<<

BC_AUTO_TEST_CASE(mnemonic__serialize__valid_whitespace__expected)
{
    std::ostringstream out;
    const mnemonic instance(words12);
    out << instance;
    BC_CHECK_EQUAL(out.str(), join(words12));
}

BC_AUTO_TEST_CASE(mnemonic__serialize__invalid__invalid)
{
    std::ostringstream out;
    const mnemonic instance;
    BC_CHECK(!instance);
    out << instance;
    BC_CHECK(out.str().empty());
}

#endif // OPERATORS

#ifdef VERIFIED_VECTORS

BC_AUTO_TEST_CASE(mnemonic__verify_vectors__sizes__expected)
{
    BC_CHECK_EQUAL(vectors_en.size(), 24u);
    BC_CHECK_EQUAL(vectors_ja.size(), 24u);
}

BC_AUTO_TEST_CASE(mnemonic__verify_vectors__denormalization__expected)
{
    constexpr ptrdiff_t abnormals_en = 0;
    constexpr ptrdiff_t abnormals_ja = 22;
    BC_CHECK_EQUAL(abnormals(vectors_en, ascii_space), abnormals_en);
    BC_CHECK_EQUAL(abnormals(vectors_ja, ideographic_space), abnormals_ja);
}

BC_AUTO_TEST_CASE(mnemonic__verify_vectors__en_whitespace__single_ascii)
{
    for (const auto& vector: vectors_en)
    {
        // Only delimited by ascii_space and no other unicode whitespace.
        const auto words = split(vector.mnemonic, { ascii_space }, unicode_whitespace);
        BC_CHECK_EQUAL(join(words, ascii_space), vector.mnemonic);
        BC_CHECK(words.size() >= 12u);
    }
}

BC_AUTO_TEST_CASE(mnemonic__verify_vectors__ja_whitespace__single_ideographic)
{
    for (const auto& vector: vectors_ja)
    {
        // Only delimited by ideographic_space and no other unicode whitespace.
        const auto words = split(vector.mnemonic, { ideographic_space }, unicode_whitespace);
        BC_CHECK_EQUAL(join(words, ideographic_space), vector.mnemonic);
        BC_CHECK(words.size() >= 12u);
    }
}

BC_AUTO_TEST_CASE(mnemonic__verify_vectors__keys__match_hd_keys)
{
    for (const auto& vector: vectors_ja)
    {
        // The seed is hex encoded input to BIP32 key generation.
        // The key is constructed from BIP32 encoding, e.g. "xprv..."
        BC_CHECK_EQUAL(hd_private(to_chunk(vector.seed())), vector.hd_key());
    }
}

BC_AUTO_TEST_CASE(mnemonic__construct_entropy__vectors_en__expected)
{
    for (const auto& vector: vectors_en)
    {
        const mnemonic instance(vector.entropy());
        BC_CHECK(instance);
        BC_CHECK(instance.lingo() == language::en);
        BC_CHECK_EQUAL(instance.entropy(), vector.entropy());
        BC_CHECK_EQUAL(instance.sentence(), vector.sentence());
        BC_CHECK_EQUAL(instance.words(), vector.words());
        BC_CHECK_EQUAL(instance.to_key(vector.passphrase), vector.hd_key());
    }
}

BC_AUTO_TEST_CASE(mnemonic__construct_sentence__vectors_en__expected)
{
    for (const auto& vector: vectors_en)
    {
        const mnemonic instance(vector.mnemonic);
        BC_CHECK(instance);
        BC_CHECK(instance.lingo() == language::en);
        BC_CHECK_EQUAL(instance.entropy(), vector.entropy());
        BC_CHECK_EQUAL(instance.sentence(), vector.sentence());
        BC_CHECK_EQUAL(instance.words(), vector.words());
        BC_CHECK_EQUAL(instance.to_key(vector.passphrase), vector.hd_key());
    }
}

BC_AUTO_TEST_CASE(mnemonic__construct_entropy__vectors_ja__expected)
{
    for (const auto& vector: vectors_ja)
    {
        const mnemonic instance(vector.entropy(), language::ja);
        BC_CHECK(instance);
        BC_CHECK(instance.lingo() == language::ja);
        BC_CHECK_EQUAL(instance.entropy(), vector.entropy());
        BC_CHECK_EQUAL(instance.sentence(), vector.sentence(ideographic_space));
        BC_CHECK_EQUAL(instance.words(), vector.words());

        // This one check fails under vectorization (sha512, so only 64 bit).
        BC_CHECK_EQUAL(instance.to_key(vector.passphrase), vector.hd_key());
    }
}

BC_AUTO_TEST_CASE(mnemonic__construct_sentence__vectors_ja__expected)
{
    for (const auto& vector: vectors_ja)
    {
        const mnemonic instance(vector.mnemonic);
        BC_CHECK(instance);
        BC_CHECK(instance.lingo() == language::ja);
        BC_CHECK_EQUAL(instance.entropy(), vector.entropy());
        BC_CHECK_EQUAL(instance.sentence(), vector.sentence(ideographic_space));
        BC_CHECK_EQUAL(instance.words(), vector.words());

        // This one check fails under vectorization (sha512, so only 64 bit).
        BC_CHECK_EQUAL(instance.to_key(vector.passphrase), vector.hd_key());
    }
}

#endif // VERIFIED_VECTORS


// sizers

BC_AUTO_TEST_CASE(mnemonic__checksum_bits__entropy__expected)
{
    BC_REQUIRE_EQUAL(accessor::checksum_bits(data_chunk(16, 0x00)), 4u);
    BC_REQUIRE_EQUAL(accessor::checksum_bits(data_chunk(32, 0x00)), 8u);
}

BC_AUTO_TEST_CASE(mnemonic__checksum_bits__words__expected)
{
    BC_REQUIRE_EQUAL(accessor::checksum_bits(string_list(12, "word")), 4u);
    BC_REQUIRE_EQUAL(accessor::checksum_bits(string_list(24, "word")), 8u);
}

BC_AUTO_TEST_CASE(mnemonic__entropy_bits__entropy__expected)
{
    BC_REQUIRE_EQUAL(accessor::entropy_bits(data_chunk(16, 0x00)), 128u);
    BC_REQUIRE_EQUAL(accessor::entropy_bits(data_chunk(32, 0x00)), 256u);
}

BC_AUTO_TEST_CASE(mnemonic__entropy_bits__words__expected)
{
    BC_REQUIRE_EQUAL(accessor::entropy_bits(string_list(12, "word")), 128u);
    BC_REQUIRE_EQUAL(accessor::entropy_bits(string_list(24, "word")), 256u);
}

BC_AUTO_TEST_CASE(mnemonic__entropy_size__words__expected)
{
    BC_REQUIRE_EQUAL(accessor::entropy_size(string_list(12, "word")), 16u);
    BC_REQUIRE_EQUAL(accessor::entropy_size(string_list(24, "word")), 32u);
}

BC_AUTO_TEST_CASE(mnemonic__word_count__entropy__expected)
{
    BC_REQUIRE_EQUAL(accessor::word_count(data_chunk(16, 0x00)), 12u);
    BC_REQUIRE_EQUAL(accessor::word_count(data_chunk(32, 0x00)), 24u);
}

// guards

BC_AUTO_TEST_CASE(mnemonic__from_entropy__invalid_size__invalid)
{
    BC_REQUIRE(!accessor::from_entropy(data_chunk(15, 0x00), language::en));
}

BC_AUTO_TEST_CASE(mnemonic__from_entropy__undefined_language__invalid)
{
    BC_REQUIRE(!accessor::from_entropy(data_chunk(16, 0x00), language::none));
}

BC_AUTO_TEST_CASE(mnemonic__from_words__language_mismatch__invalid)
{
    const auto instance = accessor::from_entropy(data_chunk(16, 0x00), language::en);
    BC_REQUIRE(instance);
    BC_REQUIRE(!accessor::from_words(instance.words(), language::es));
}

BC_AUTO_TEST_CASE(mnemonic__decoder__not_in_dictionary__empty)
{
    BC_REQUIRE(accessor::decoder(string_list(12, "bogus"), language::en).empty());
}

BC_AUTO_TEST_CASE(mnemonic__to_seed__invalid__empty)
{
    BC_REQUIRE_EQUAL(mnemonic{}.to_seed(""), long_hash{});
}

BC_AUTO_TEST_SUITE_END()