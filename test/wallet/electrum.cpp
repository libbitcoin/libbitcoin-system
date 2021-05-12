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
#include "electrum.hpp"

BOOST_AUTO_TEST_SUITE(electrum_tests)

using namespace bc::system::wallet;

BOOST_AUTO_TEST_CASE(electrum__vector__english__expected)
{
    const auto vector = vectors[0];
    electrum instance(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.lingo() == vector.lingo);
    BOOST_REQUIRE(instance.prefix() == vector.prefix);
    BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    // Entropy constructor generates only two words.
    // Entropy property similarly invalid in other vectors against expected.
    // Entropy empty if lingo not specified with words constructor.
    electrum derived(instance.entropy(), electrum::seed_prefix::witness, instance.lingo());
    BOOST_REQUIRE(derived.lingo() == instance.lingo());
    BOOST_REQUIRE(derived.prefix() == instance.prefix());
    ////BOOST_REQUIRE_EQUAL(derived.words(), instance.words());
    ////BOOST_REQUIRE_EQUAL(derived.sentence(), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(derived.entropy(), instance.entropy());
}

BOOST_AUTO_TEST_CASE(electrum__vector__english_with_passphrase__expected)
{
    const auto vector = vectors[1];
    electrum instance(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.lingo() == vector.lingo);
    BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);
}

// Test vector japanese is ascii space delimited.
// TODO: determine if electrum normalization strips ideographic spaces.
// TODO: split japanese on both types of spaces, join with ideographic.
BOOST_AUTO_TEST_CASE(electrum__vector__japanese__expected)
{
    const auto vector = vectors[2];
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    electrum instance(vector.mnemonic, vector.lingo);
    ////BOOST_REQUIRE(instance);
    ////BOOST_REQUIRE(instance.lingo() == vector.lingo);
    ////BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    ////BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__japanese_with_passphrase__expected)
{
    const auto vector = vectors[3];
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    electrum instance(vector.mnemonic, vector.lingo);
    ////BOOST_REQUIRE(instance);
    ////BOOST_REQUIRE(instance.lingo() == vector.lingo);
    ////BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    ////BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__chinese__expected)
{
    const auto vector = vectors[4];
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    electrum instance(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.lingo() == vector.lingo);
    ////BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    ////BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__chinese_with_passphrase__expected)
{
    const auto vector = vectors[5];
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    electrum instance(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.lingo() == vector.lingo);
    ////BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    ////BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
}

// Words are not found, normalization strips accent characters.
// Normalization of the dictionary would preclude proper mnemonic generation.
// TODO: verify whether BIP39 dictionaries are nfkd normalized. If so then they
// will match and generate properly. In that case, electrum would have to:
// (1) two sets of dictionaries, one normalized (breaks abstraction).
// (2) normalize the dictionary on load (still need the other for generation).
// (3) normalize on each compare (slower, requires injection of normalizer).
// (4) don't normalize words input, require exact match after split/trim (best).
// As long as words match, and we normalize words for seed generation, then
// normalization on seeding is sufficient to guarantee seed correctness.
// Is dictionaries are nfkd normalized, then both electrum/mnemonic may nfkd
// normalize word entry. Special electrum normalization can be reserved for
// seeding generation.
BOOST_AUTO_TEST_CASE(electrum__vector__spanish__expected)
{
    const auto vector = vectors[6];
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    electrum instance(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.lingo() == vector.lingo);
    ////BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    ////BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish_with_passphrase__expected)
{
    const auto vector = vectors[7];
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    electrum instance(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.lingo() == vector.lingo);
    ////BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    ////BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish2__expected)
{
    const auto vector = vectors[8];
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    electrum instance(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.lingo() == vector.lingo);
    ////BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    ////BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish3__expected)
{
    const auto vector = vectors[9];
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.mnemonic_chunk), vector.mnemonic);
    ////BOOST_REQUIRE_EQUAL(encode_base16(vector.passphrase_chunk), vector.passphrase);

    electrum instance(vector.mnemonic, vector.lingo);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE(instance.lingo() == vector.lingo);
    ////BOOST_REQUIRE(instance.prefix() == vector.prefix);
    ////BOOST_REQUIRE_EQUAL(instance.entropy(), vector.entropy);
    ////BOOST_REQUIRE_EQUAL(instance.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_SUITE_END()
