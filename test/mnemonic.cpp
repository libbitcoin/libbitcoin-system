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

BOOST_AUTO_TEST_CASE(entropy_to_mnemonic)
{
    for (const mnemonic_result& result: entropy_to_mnemonic_tests)
    {
        data_chunk data;
        decode_base16(data, result.input);
        const auto words = create_mnemonic(data, bip39::language::en);
        BOOST_REQUIRE(words.size() > 0);
        BOOST_REQUIRE_EQUAL(join(words), result.result);
    }
}

BOOST_AUTO_TEST_CASE(mnemonic_to_seed)
{
    typedef std::vector<std::string> string_list;
    const auto passphrase = "TREZOR";
    for (const mnemonic_result& result: mnemonic_to_seed_tests)
    {
        const auto words = split(result.input);
        data_chunk data = decode_mnemonic(words, passphrase);
        BOOST_REQUIRE(!data.empty());
        BOOST_REQUIRE_EQUAL(encode_base16(data), result.result);
    }
}

BOOST_AUTO_TEST_SUITE_END()
