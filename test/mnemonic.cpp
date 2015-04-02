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
using namespace bc::bip39;

BOOST_AUTO_TEST_SUITE(mnemonic_tests)

BOOST_AUTO_TEST_CASE(entropy_to_en_mnemonic)
{
    for (const mnemonic_result& result: entropy_to_mnemonic_en)
    {
        data_chunk entropy;
        decode_base16(entropy, result.input);
        const auto mnemonic = create_mnemonic(entropy, bip39::language::en);
        BOOST_REQUIRE(mnemonic.size() > 0);
        BOOST_REQUIRE_EQUAL(join(mnemonic), result.expectation);
    }
}

BOOST_AUTO_TEST_CASE(en_mnemonic_to_seed)
{
    const auto& passphrase = "TREZOR";
    for (const mnemonic_result& result: mnemonic_to_seed_en)
    {
        const auto words = split(result.input);
        const auto seed = decode_mnemonic(words, passphrase);
        BOOST_REQUIRE(!seed.empty());
        BOOST_REQUIRE_EQUAL(encode_base16(seed), result.expectation);
    }
}

BOOST_AUTO_TEST_CASE(ensure_en_es_disjointness)
{
    const auto& english = *dictionary.at(language::en);
    const auto& spanish = *dictionary.at(language::es);
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
    const auto& simplified = *dictionary.at(language::zh_Hans);
    const auto& traditional = *dictionary.at(language::zh_Hant);
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
