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

BOOST_AUTO_TEST_SUITE(electrum_tests)

using namespace bc::system::wallet;

BOOST_AUTO_TEST_CASE(electrum__decoder__japanese__expected)
{
    const auto vector = vectors[0];
    const auto entropy = accessor::decoder(split(vector.mnemonic), language::en);
    BOOST_REQUIRE_EQUAL(entropy, vector.entropy);
}

BOOST_AUTO_TEST_CASE(electrum__encoder__japanese_with_passphrase__expected)
{
    const auto vector = vectors[0];
    const auto words = accessor::encoder(vector.entropy, vector.lingo);
    BOOST_REQUIRE_EQUAL(words, split(vector.mnemonic));
}

BOOST_AUTO_TEST_CASE(electrum__grinder__minimum_null_witness_english__expected)
{
    const auto vector = vectors[0];
    const auto words = split(vector.mnemonic);
    BOOST_REQUIRE_EQUAL(words.size(), 12u);

    const auto result = accessor::grinder(vector.entropy, vector.prefix, vector.lingo, 10000u);
    BOOST_REQUIRE_EQUAL(result.entropy, vector.entropy);
    BOOST_REQUIRE_EQUAL(result.words, words);
}

// electrum standard vectors

BOOST_AUTO_TEST_CASE(electrum__vector__english__expected)
{
    ELECTRUM_VERIFY(0);
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__english_with_passphrase__expected)
{
    ELECTRUM_VERIFY(1);
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__japanese__expected)
{
    ELECTRUM_VERIFY(2);
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
}

BOOST_AUTO_TEST_CASE(electrum__vector__japanese_with_passphrase__expected)
{
    ELECTRUM_VERIFY(3);
#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
#else
    BOOST_REQUIRE_EQUAL(!instance1.to_seed(vector.passphrase));
#endif
}

BOOST_AUTO_TEST_CASE(electrum__vector__chinese__expected)
{
    ELECTRUM_VERIFY(4);
}

BOOST_AUTO_TEST_CASE(electrum__vector__chinese_with_passphrase__expected)
{
    ELECTRUM_VERIFY(5);
#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
#else
    BOOST_REQUIRE_EQUAL(!instance1.to_seed(vector.passphrase));
#endif
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish__expected)
{
    ELECTRUM_VERIFY(6);
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish_with_passphrase__expected)
{
    ELECTRUM_VERIFY(7);
#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
#else
    BOOST_REQUIRE_EQUAL(!instance1.to_seed(vector.passphrase));
#endif
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish2__expected)
{
    ELECTRUM_VERIFY(8);
}

BOOST_AUTO_TEST_CASE(electrum__vector__spanish3__expected)
{
    ELECTRUM_VERIFY(9);
#ifdef WITH_ICU
    BOOST_REQUIRE_EQUAL(instance1.to_seed(vector.passphrase), vector.to_hd());
#else
    BOOST_REQUIRE_EQUAL(!instance1.to_seed(vector.passphrase));
#endif
}

// statics

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

BOOST_AUTO_TEST_SUITE_END()
