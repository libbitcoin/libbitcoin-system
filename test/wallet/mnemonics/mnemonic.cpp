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
#include "mnemonic.hpp"

using namespace test::mnemonics_mnemonic;
using namespace bc::system::wallet;

BOOST_AUTO_TEST_SUITE(mnemonic_tests)

using namespace test::mnemonics_mnemonic;
using namespace bc::system::wallet;

// construct (mnemonic) 

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__empty__false)
{
    BOOST_REQUIRE(!mnemonic(""));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__misspelled__false)
{
    BOOST_REQUIRE(!mnemonic(
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon aboot"));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__short__false)
{
    BOOST_REQUIRE(!mnemonic(
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon"));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__bad_checksum__false)
{
    BOOST_REQUIRE(!mnemonic(
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon one"));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__incorrect_language__false)
{
    BOOST_REQUIRE(!mnemonic(
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon agent", language::zh_Hant));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__correct_language__true)
{
    BOOST_REQUIRE(mnemonic(
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon agent", language::en));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_mnemonic__any_language__true)
{
    BOOST_REQUIRE(mnemonic(
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon abandon "
        "abandon abandon abandon abandon abandon agent", language::none));
}

// BIP39 vectors

BOOST_AUTO_TEST_CASE(mnemonic__verify_vector__sizes__expected)
{
    BOOST_REQUIRE_EQUAL(vectors_en.size(), 24u);
    BOOST_REQUIRE_EQUAL(vectors_ja.size(), 24u);
}

BOOST_AUTO_TEST_CASE(mnemonic__verify_vector__denormalization__expected)
{
    BOOST_REQUIRE_EQUAL(abnormals(vectors_en, ascii_space), 0u);

    // When WITH_ICU is undefined normalization cannot be verified.
#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(abnormals(vectors_ja, ideographic_space), 22u);
#else
    BOOST_REQUIRE_EQUAL(abnormals(vectors_ja, ideographic_space), 0u);
#endif
}

BOOST_AUTO_TEST_CASE(mnemonic__verify_vector__ja_delimiters__single_ideographic_space)
{
    for (const auto& vector: vectors_ja)
    {
        // Not ascii delimited.
        BOOST_REQUIRE_EQUAL(split(vector.mnemonic).size(), 1u);

        // Only ideographic_space delimited and no other unicode whitespace.
        const auto words = split(vector.mnemonic, { ideographic_space }, unicode_whitespace);
        BOOST_REQUIRE_EQUAL(join(words, ideographic_space), vector.mnemonic);
        BOOST_REQUIRE(words.size() >= 12u);
    }
}

// No need to verify "seed" (mnemonic internals) as long as they match the hd keys.
BOOST_AUTO_TEST_CASE(mnemonic__verify_vector__seeds__match_hd_keys)
{
    for (const auto& vector: vectors_ja)
    {
        // The seed is hex encoded input to BIP32 key generation.
        // The key is constructed from BIP32 encoding, e.g. "xprv..."
        BOOST_REQUIRE_EQUAL(hd_private(vector.seed()), vector.hd_key());
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__vectors_en__expected)
{
    for (const auto& vector: vectors_en)
    {
        mnemonic instance(vector.entropy());
        BOOST_REQUIRE(instance);
        BOOST_REQUIRE(instance.lingo() == language::en);
        BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy());
        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.sentence());
        BOOST_REQUIRE_EQUAL(instance.words(), vector.words());
        BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.hd_key());
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_sentence__vectors_en__expected)
{
    for (const auto& vector : vectors_en)
    {
        mnemonic instance(vector.mnemonic);
        BOOST_REQUIRE(instance);
        BOOST_REQUIRE(instance.lingo() == language::en);
        BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy());
        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.sentence());
        BOOST_REQUIRE_EQUAL(instance.words(), vector.words());
        BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.hd_key());
    }
}

#ifdef WITH_ICU

// Passphrases are all ascii, but 22 of 24 mnemonics are denormalized, so these
// will fail when WITH_ICU is undefined.

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__vectors_ja__expected)
{
    for (const auto& vector: vectors_ja)
    {
        mnemonic instance(vector.entropy(), language::ja);
        BOOST_REQUIRE(instance);
        BOOST_REQUIRE(instance.lingo() == language::ja);
        BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy());
        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.sentence(ideographic_space));
        BOOST_REQUIRE_EQUAL(instance.words(), vector.words());
        BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.hd_key());
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_sentence__vectors_ja__expected)
{
    for (const auto& vector: vectors_ja)
    {
        mnemonic instance(vector.mnemonic);
        BOOST_REQUIRE(instance);
        BOOST_REQUIRE(instance.lingo() == language::ja);
        BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy());
        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.sentence(ideographic_space));
        BOOST_REQUIRE_EQUAL(instance.words(), vector.words());
        BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.hd_key());
    }
}

#endif // WITH_ICU

BOOST_AUTO_TEST_SUITE_END()