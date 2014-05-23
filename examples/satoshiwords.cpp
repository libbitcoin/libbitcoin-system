/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
/*
  Display the genesis block message by Satoshi.
*/
#ifdef _MSC_VER
#include <iostream>
#endif
#include <bitcoin/bitcoin.hpp>

using namespace bc;

int main()
{
    // Create genesis block.
    block_type genblk = genesis_block();
    // Genesis block contains a single coinbase transaction.
    assert(genblk.transactions.size() == 1);
    // Get first transaction in block (coinbase).
    const transaction_type& coinbase_tx = genblk.transactions[0];
    // Coinbase tx has a single input.
    assert(coinbase_tx.inputs.size() == 1);
    const transaction_input_type& coinbase_input = coinbase_tx.inputs[0];
    // Convert the input script to its raw format.
    const data_chunk& raw_block_message = save_script(coinbase_input.script);
    // Convert this to an std::string.
    std::string message;
    message.resize(raw_block_message.size());
    std::copy(raw_block_message.begin(), raw_block_message.end(),
        message.begin());
    // Display the genesis block message.
    std::cout << message << std::endl;
    return 0;
}

