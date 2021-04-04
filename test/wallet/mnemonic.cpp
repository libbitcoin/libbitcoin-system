/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <algorithm>
#include <bitcoin/system.hpp>

using namespace bc::system;
using namespace bc::system::wallet;

BOOST_AUTO_TEST_SUITE(mnemonic_tests)

// decode_mnemonic

BOOST_AUTO_TEST_CASE(mnemonic__decode_mnemonic__no_passphrase_vectors__success)
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

BOOST_AUTO_TEST_CASE(mnemonic__decode_mnemonic__trezor_vectors__success)
{
    for (const auto& vector: mnemonic_trezor_vectors)
    {
        const auto words = split(vector.mnemonic, ",");
        BOOST_REQUIRE(validate_mnemonic(words));
        const auto seed = decode_mnemonic(words, vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__decode_mnemonic__japanese_vectors__success)
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

BOOST_AUTO_TEST_CASE(mnemonic__decode_mnemonic__to_seed_vectors__success)
{
    for (const auto& vector: mnemonic_to_seed_vectors)
    {
        const auto words = split(vector.mnemonic, ",");
        BOOST_REQUIRE(validate_mnemonic(words));
        const auto seed = decode_mnemonic(words, vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
    }
}

// create_mnemonic

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__trezor_vectors__success)
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

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__japanese_vectors__success)
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

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__vectors__success)
{
    for (const mnemonic_result& vector: mnemonic_create_vectors)
    {
        data_chunk entropy;
        decode_base16(entropy, vector.entropy);
        const auto mnemonic = create_mnemonic(entropy, vector.language);
        BOOST_REQUIRE_GT(mnemonic.size(), 0u);
        BOOST_REQUIRE_EQUAL(join(mnemonic, ","), vector.mnemonic);
        BOOST_REQUIRE(validate_mnemonic(mnemonic));
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__4_byte_entropy__success)
{
    const data_chunk entropy(4, 0xa9);
    const auto mnemonic = create_mnemonic(entropy);
    BOOST_REQUIRE_EQUAL(mnemonic.size(), 3u);
    BOOST_REQUIRE(validate_mnemonic(mnemonic));
}

BOOST_AUTO_TEST_CASE(mnemonic__create_mnemonic__1024_byte_entropy__success)
{
    const data_chunk entropy(1024, 0xa9);
    const auto mnemonic = create_mnemonic(entropy);
    BOOST_REQUIRE_EQUAL(mnemonic.size(), 768u);
    BOOST_REQUIRE(validate_mnemonic(mnemonic));
}

// validate_mnemonic (invalids)

BOOST_AUTO_TEST_CASE(mnemonic__validate_mnemonic__misspelled__false)
{
    BOOST_REQUIRE(!validate_mnemonic(split("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon aboot")));
}

BOOST_AUTO_TEST_CASE(mnemonic__validate_mnemonic__length_one__false)
{
    BOOST_REQUIRE(!validate_mnemonic(split("one")));
}

BOOST_AUTO_TEST_CASE(mnemonic__validate_mnemonic__length_two__false)
{
    BOOST_REQUIRE(!validate_mnemonic(split("one two")));
}

BOOST_AUTO_TEST_CASE(mnemonic__validate_mnemonic__length_eleven__false)
{
    BOOST_REQUIRE(!validate_mnemonic(split("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon")));
}

BOOST_AUTO_TEST_CASE(mnemonic__validate_mnemonic__bad_checksum__false)
{
    BOOST_REQUIRE(!validate_mnemonic(split("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon one")));
}

#endif

// dictionary intersections

static ptrdiff_t intersection(const dictionary& left, const dictionary& right)
{
    return std::count_if(left.begin(), left.end(),
        [&](const char* test)
    {
        return std::find(right.begin(), right.end(), test) != right.end();
    });
}

static bool intersects(const dictionary& left, const dictionary& right)
{
    return std::any_of(left.begin(), left.end(), [&](const char* test)
    {
        return std::find(right.begin(), right.end(), test) != right.end();
    });
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__en_es__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::en, language::es));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__es_it__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::es, language::it));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__it_fr__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::it, language::fr));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__fr_cs__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::fr, language::cs));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__cs_pt__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::cs, language::pt));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__pt_ko__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::pt, language::ko));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__ko_zh_Hans__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::ko, language::zh_Hans));
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__zh_Hans_Hant__1275_intersections)
{
    BOOST_REQUIRE_EQUAL(intersection(language::zh_Hans, language::zh_Hant), 1275);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__zh_Hant_en__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::zh_Hant, language::en));
}

BOOST_AUTO_TEST_SUITE_END()
