/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include "mnemonic.hpp"

#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;
using namespace bc::wallet;

BOOST_AUTO_TEST_SUITE(mnemonic_tests)

BOOST_AUTO_TEST_CASE(mnemonic__decode_mnemonic__no_passphrase)
{
    for (const auto& vector: mnemonic_no_passphrase)
    {
        const auto words = split(vector.mnemonic, ",");
        BOOST_REQUIRE(validate_mnemonic(words, vector.language));
        const auto seed = decode_mnemonic(words);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
    }
}

#ifdef WITH_ICU

BOOST_AUTO_TEST_CASE(mnemonic__decode_mnemonic__trezor)
{
    for (const auto& vector: mnemonic_trezor_vectors)
    {
        const auto words = split(vector.mnemonic, ",");
        BOOST_REQUIRE(validate_mnemonic(words));
        const auto seed = decode_mnemonic(words, vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__decode_mnemonic__japanese)
{
    for (const auto& vector: mnemonic_japanese_vectors)
    {
        const auto words = split(vector.mnemonic, ",");
        BOOST_REQUIRE(!words.empty());
        BOOST_REQUIRE(validate_mnemonic(words, vector.language));
        const auto seed = decode_mnemonic(words, vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__decode_mnemonic)
{
    for (const auto& vector: mnemonic_to_seed_vectors)
    {
        const auto words = split(vector.mnemonic, ",");
        BOOST_REQUIRE(validate_mnemonic(words));
        const auto seed = decode_mnemonic(words, vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__trezor)
{
    for (const mnemonic_result& vector: mnemonic_trezor_vectors)
    {
        data_chunk entropy;
        decode_base16(entropy, vector.entropy);
        const auto mnemonic = create_mnemonic(entropy, vector.language);
        BOOST_REQUIRE(mnemonic.size() > 0);
        BOOST_REQUIRE_EQUAL(join(mnemonic, ","), vector.mnemonic);
        BOOST_REQUIRE(validate_mnemonic(mnemonic));
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__japanese)
{
    for (const mnemonic_result& vector: mnemonic_japanese_vectors)
    {
        BOOST_REQUIRE(vector.entropy.size() % 2 == 0);
        data_chunk entropy;
        decode_base16(entropy, vector.entropy);
        const auto mnemonic = create_mnemonic(entropy, vector.language);
        BOOST_REQUIRE(mnemonic.size() > 0);
        BOOST_REQUIRE_EQUAL(join(mnemonic, ","), vector.mnemonic);
        BOOST_REQUIRE(validate_mnemonic(mnemonic));
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic)
{
    for (const mnemonic_result& vector: mnemonic_create_vectors)
    {
        data_chunk entropy;
        decode_base16(entropy, vector.entropy);
        const auto mnemonic = create_mnemonic(entropy, vector.language);
        BOOST_REQUIRE(mnemonic.size() > 0);
        BOOST_REQUIRE_EQUAL(join(mnemonic, ","), vector.mnemonic);
        BOOST_REQUIRE(validate_mnemonic(mnemonic));
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__validate_mnemonic__invalid)
{
    for (const auto& mnemonic: invalid_mnemonic_tests)
    {
        const auto words = split(mnemonic, ",");
        BOOST_REQUIRE(!validate_mnemonic(words));
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__tiny)
{
    const data_chunk entropy(4, 0xa9);
    const auto mnemonic = create_mnemonic(entropy);
    BOOST_REQUIRE_EQUAL(mnemonic.size(), 3u);
    BOOST_REQUIRE(validate_mnemonic(mnemonic));
}

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__giant)
{
    const data_chunk entropy(1024, 0xa9);
    const auto mnemonic = create_mnemonic(entropy);
    BOOST_REQUIRE_EQUAL(mnemonic.size(), 768u);
    BOOST_REQUIRE(validate_mnemonic(mnemonic));
}

#endif

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__en_es__no_intersection)
{
    const auto& english = language::en;
    const auto& spanish = language::es;
    size_t intersection = 0;
    for (const auto es: spanish)
    {
        std::string test(es);
        const auto it = std::find(english.begin(), english.end(), test);
        if (it != std::end(english))
            intersection++;
    }

    BOOST_REQUIRE_EQUAL(intersection, 0u);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__en_it__no_intersection)
{
    const auto& english = language::en;
    const auto& italian = language::it;
    size_t intersection = 0;
    for (const auto it: italian)
    {
        std::string test(it);
        const auto iter = std::find(english.begin(), english.end(), test);
        if (iter != std::end(english))
            intersection++;
    }

    BOOST_REQUIRE_EQUAL(intersection, 0u);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__fr_es__no_intersection)
{
    const auto& french = language::fr;
    const auto& spanish = language::es;
    size_t intersection = 0;
    for (const auto es: spanish)
    {
        std::string test(es);
        const auto it = std::find(french.begin(), french.end(), test);
        if (it != std::end(french))
            intersection++;
    }

    BOOST_REQUIRE_EQUAL(intersection, 0u);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__it_es__no_intersection)
{
    const auto& italian = language::it;
    const auto& spanish = language::es;
    size_t intersection = 0;
    for (const auto es: spanish)
    {
        std::string test(es);
        const auto it = std::find(italian.begin(), italian.end(), test);
        if (it != std::end(italian))
            intersection++;
    }

    BOOST_REQUIRE_EQUAL(intersection, 0u);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__fr_it__no_intersection)
{
    const auto& french = language::fr;
    const auto& italian = language::it;
    size_t intersection = 0;
    for (const auto it: italian)
    {
        std::string test(it);
        const auto iter = std::find(french.begin(), french.end(), test);
        if (iter != std::end(french))
            intersection++;
    }

    BOOST_REQUIRE_EQUAL(intersection, 0u);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__cs_ru__no_intersection)
{
    const auto& czech = language::cs;
    const auto& russian = language::ru;
    size_t intersection = 0;
    for (const auto ru: russian)
    {
        std::string test(ru);
        const auto iter = std::find(czech.begin(), czech.end(), test);
        if (iter != std::end(czech))
            intersection++;
    }

    BOOST_REQUIRE_EQUAL(intersection, 0u);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__cs_uk__no_intersection)
{
    const auto& czech = language::cs;
    const auto& ukranian = language::uk;
    size_t intersection = 0;
    for (const auto uk: ukranian)
    {
        std::string test(uk);
        const auto iter = std::find(czech.begin(), czech.end(), test);
        if (iter != std::end(czech))
            intersection++;
    }

    BOOST_REQUIRE_EQUAL(intersection, 0u);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__zh_Hans_Hant__intersection)
{
    const auto& simplified = language::zh_Hans;
    const auto& traditional = language::zh_Hant;
    size_t intersection = 0;
    for (const auto hant: traditional)
    {
        std::string test(hant);
        const auto it = std::find(simplified.begin(), simplified.end(), test);
        if (it != std::end(simplified))
            intersection++;
    }

    BOOST_REQUIRE_EQUAL(intersection, 1275u);
}

BOOST_AUTO_TEST_SUITE_END()
