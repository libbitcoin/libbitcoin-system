/**
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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "genesis_block.hpp"

// TODO: move this into the tests by source file organization.
BOOST_AUTO_TEST_SUITE(satoshi_words)

BOOST_AUTO_TEST_CASE(satoshi_words_mainnet_or_testnet)
{
    // Create genesis block.
    auto block = genesis_block();

    // Genesis block contains a single coinbase transaction.
    BOOST_REQUIRE_EQUAL(block.transactions.size(), 1u);

    // Get first transaction in block (coinbase).
    const bc::chain::transaction& coinbase_tx = block.transactions[0];

    // Coinbase tx has a single input.
    BOOST_REQUIRE_EQUAL(coinbase_tx.inputs.size(), 1u);
    const bc::chain::transaction_input& coinbase_input = coinbase_tx.inputs[0];

    // Convert the input script to its raw format.
    const bc::data_chunk raw_message = coinbase_input.script;

    // Convert to a string after removing the 8 byte checksum.
    BOOST_REQUIRE_GT(raw_message.size(), 8u);
    std::string message;
    message.resize(raw_message.size() - 8);
    std::copy(raw_message.begin() + 8, raw_message.end(), message.begin());

    BOOST_REQUIRE_EQUAL(message, "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks");
}

BOOST_AUTO_TEST_SUITE_END()
