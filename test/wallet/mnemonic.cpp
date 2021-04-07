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

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__trezor_vectors__success)
{
    for (const auto& vector: mnemonic_trezor_en_vectors)
    {
        data_chunk entropy;
        BOOST_REQUIRE(decode_base16(entropy, vector.entropy));
        mnemonic instance(entropy);
        BOOST_REQUIRE(instance);
        BOOST_REQUIRE(instance.lexicon() == reference::en);
        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
        BOOST_REQUIRE_EQUAL(instance.words(), split(vector.mnemonic));
        BOOST_REQUIRE_EQUAL(encode_base16(instance.entropy()), vector.entropy);

#ifdef WITH_ICU
        const auto seed = instance.to_seed(vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
        BOOST_REQUIRE_EQUAL(hd_private(seed).encoded(), vector.hd_private_key);
#endif
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_entropy__japanese_vectors__success)
{
    for (const auto& vector: mnemonic_bip39jp_jp_vectors)
    {
        data_chunk entropy;
        BOOST_REQUIRE(decode_base16(entropy, vector.entropy));
        mnemonic instance(entropy, reference::ja);
        BOOST_REQUIRE(instance);
        BOOST_REQUIRE(instance.lexicon() == reference::ja);
        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
        BOOST_REQUIRE_EQUAL(instance.words(), split_regex(vector.mnemonic, mnemonic::ideographic_space));
        BOOST_REQUIRE_EQUAL(encode_base16(instance.entropy()), vector.entropy);

#ifdef WITH_ICU
        const auto seed = instance.to_seed(vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
        BOOST_REQUIRE_EQUAL(hd_private(seed).encoded(), vector.hd_private_key);
#endif
    }
}

// construct2

BOOST_AUTO_TEST_CASE(mnemonic__construct_sentence__trezor_vectors__success)
{
    for (const auto& vector: mnemonic_trezor_en_vectors)
    {
        mnemonic instance(vector.mnemonic);
        BOOST_REQUIRE(instance);
        BOOST_REQUIRE(instance.lexicon() == reference::en);
        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
        BOOST_REQUIRE_EQUAL(instance.words(), split(vector.mnemonic));
        BOOST_REQUIRE_EQUAL(encode_base16(instance.entropy()), vector.entropy);

#ifdef WITH_ICU
        const auto seed = instance.to_seed(vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
        BOOST_REQUIRE_EQUAL(hd_private(seed).encoded(), vector.hd_private_key);
#endif
    }
}

BOOST_AUTO_TEST_CASE(mnemonic__construct_sentence__japanese_vectors__success)
{
    for (const auto& vector: mnemonic_bip39jp_jp_vectors)
    {
        mnemonic instance(vector.mnemonic, reference::ja);
        BOOST_REQUIRE(instance);
        BOOST_REQUIRE(instance.lexicon() == reference::ja);
        BOOST_REQUIRE_EQUAL(instance.sentence(), vector.mnemonic);
        BOOST_REQUIRE_EQUAL(instance.words(), split_regex(vector.mnemonic, mnemonic::ideographic_space));
        BOOST_REQUIRE_EQUAL(encode_base16(instance.entropy()), vector.entropy);

#ifdef WITH_ICU
        const auto seed = instance.to_seed(vector.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), vector.seed);
        BOOST_REQUIRE_EQUAL(hd_private(seed).encoded(), vector.hd_private_key);
#endif
    }
}
// construct (pathological) 

BOOST_AUTO_TEST_CASE(mnemonic__construct1__misspelled_en__false)
{
    BOOST_REQUIRE(!mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon aboot", reference::en));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct1__length_one_en__false)
{
    BOOST_REQUIRE(!mnemonic("one", reference::en));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct1__length_two_en__false)
{
    BOOST_REQUIRE(!mnemonic("one two", reference::en));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct1__length_eleven_en__false)
{
    BOOST_REQUIRE(!mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon", reference::en));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct1__bad_checksum_en__false)
{
    ////BOOST_REQUIRE(!mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon one", reference::en));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct1__incorrect_langauge__false)
{
    BOOST_REQUIRE(!mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon agent", reference::zh_Hant));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct1__correct_langauge__true)
{
    ////BOOST_REQUIRE(mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon agent", reference::en));
}

BOOST_AUTO_TEST_CASE(mnemonic__construct1__any_langauge__true)
{
    ////BOOST_REQUIRE(mnemonic("abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon agent"));
}

// dictionary intersections

static ptrdiff_t intersection(const dictionary& left, const dictionary& right)
{
    return std::count_if(left.words.begin(), left.words.end(),
        [&](const char* test)
    {
        return std::find(right.words.begin(), right.words.end(), test) != right.words.end();
    });
}

static bool intersects(const dictionary& left, const dictionary& right)
{
    return std::any_of(left.words.begin(), left.words.end(), [&](const char* test)
    {
        return std::find(right.words.begin(), right.words.end(), test) != right.words.end();
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

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__zh_Hans_zh_Hant__1275_intersections)
{
    BOOST_REQUIRE_EQUAL(intersection(language::zh_Hans, language::zh_Hant), 1275);
}

BOOST_AUTO_TEST_CASE(mnemonic__dictionary__zh_Hant_en__no_intersection)
{
    BOOST_REQUIRE(!intersects(language::zh_Hant, language::en));
}

BOOST_AUTO_TEST_SUITE_END()
