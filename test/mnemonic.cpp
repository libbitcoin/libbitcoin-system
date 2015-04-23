/*
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "mnemonic.hpp"

#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(mnemonic_tests)

BOOST_AUTO_TEST_CASE(entropy_to_trezor_mnemonic)
{
    for (const mnemonic_result& result: mnemonic_trezor_vectors)
    {
        data_chunk entropy;
        decode_base16(entropy, result.entropy);
        const auto mnemonic = create_mnemonic(entropy, result.language);
        BOOST_REQUIRE(mnemonic.size() > 0);
        BOOST_REQUIRE_EQUAL(join(mnemonic, ","), result.mnemonic);
        BOOST_REQUIRE(validate_mnemonic(mnemonic));
    }
}

BOOST_AUTO_TEST_CASE(entropy_to_bx_new_mnemonic)
{
    for (const mnemonic_result& result: mnemonic_bx_new_vectors)
    {
        data_chunk entropy;
        decode_base16(entropy, result.entropy);
        const auto mnemonic = create_mnemonic(entropy, result.language);
        BOOST_REQUIRE(mnemonic.size() > 0);
        BOOST_REQUIRE_EQUAL(join(mnemonic, ","), result.mnemonic);
        BOOST_REQUIRE(validate_mnemonic(mnemonic));
    }
}

BOOST_AUTO_TEST_CASE(trezor_mnemonic_to_seed)
{
    for (const auto& result: mnemonic_trezor_vectors)
    {
        const auto words = split(result.mnemonic, ",");
        BOOST_REQUIRE(validate_mnemonic(words));
        const auto seed = decode_mnemonic(words, result.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), result.seed);
    }
}

BOOST_AUTO_TEST_CASE(bx_mnemonic_to_seed)
{
    for (const auto& result: mnemonic_bx_to_seed_vectors)
    {
        const auto words = split(result.mnemonic, ",");
        BOOST_REQUIRE(validate_mnemonic(words));
        const auto seed = decode_mnemonic(words, result.passphrase);
        BOOST_REQUIRE_EQUAL(encode_base16(seed), result.seed);
    }
}

BOOST_AUTO_TEST_CASE(tiny_mnemonic)
{
    const data_chunk entropy(4, 0xa9);
    const auto mnemonic = create_mnemonic(entropy);
    BOOST_REQUIRE_EQUAL(mnemonic.size(), 3u);
    BOOST_REQUIRE(validate_mnemonic(mnemonic));
}

BOOST_AUTO_TEST_CASE(giant_mnemonic)
{
    const data_chunk entropy(1024, 0xa9);
    const auto mnemonic = create_mnemonic(entropy);
    BOOST_REQUIRE_EQUAL(mnemonic.size(), 768u);
    BOOST_REQUIRE(validate_mnemonic(mnemonic));
}

BOOST_AUTO_TEST_CASE(invalid_mnemonics)
{
    for (const auto& mnemonic: invalid_mnemonic_tests)
    {
        const auto words = split(mnemonic, ",");
        BOOST_REQUIRE(!validate_mnemonic(words));
    }
}

BOOST_AUTO_TEST_CASE(ensure_en_es_disjointness)
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

BOOST_AUTO_TEST_CASE(ensure_zh_Hans_Hant_intersection)
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
