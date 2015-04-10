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
#include "genesis_block.hpp"

using namespace bc;

chain::block genesis_block()
{
    chain::block_header header;
    header.version = 1;
    header.previous_block_hash = null_hash;
    header.merkle = hash_literal(
        "4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    header.bits = 0x1d00ffff;

#ifdef ENABLE_TESTNET
    header.timestamp = 1296688602;
    header.nonce = 414098458;
#else
    header.timestamp = 1231006505;
    header.nonce = 2083236893;
#endif

    chain::transaction coinbase_tx;
    coinbase_tx.version = 1;
    coinbase_tx.locktime = 0;

    chain::transaction_input coinbase_input;
    coinbase_input.previous_output.hash = null_hash;
    coinbase_input.previous_output.index = bc::max_uint32;

    // The Times 03/Jan/2009 Chancellor on brink of second bailout for banks
    coinbase_input.script.push_operations(
        chain::operation{ chain::opcode::raw_data,
        data_chunk{ 0x04, 0xff, 0xff, 0x00, 0x1d, 0x01, 0x04, 0x45,
                   0x54, 0x68, 0x65, 0x20, 0x54, 0x69, 0x6d, 0x65,
                   0x73, 0x20, 0x30, 0x33, 0x2f, 0x4a, 0x61, 0x6e,
                   0x2f, 0x32, 0x30, 0x30, 0x39, 0x20, 0x43, 0x68,
                   0x61, 0x6e, 0x63, 0x65, 0x6c, 0x6c, 0x6f, 0x72,
                   0x20, 0x6f, 0x6e, 0x20, 0x62, 0x72, 0x69, 0x6e,
                   0x6b, 0x20, 0x6f, 0x66, 0x20, 0x73, 0x65, 0x63,
                   0x6f, 0x6e, 0x64, 0x20, 0x62, 0x61, 0x69, 0x6c,
                   0x6f, 0x75, 0x74, 0x20, 0x66, 0x6f, 0x72, 0x20,
                   0x62, 0x61, 0x6e, 0x6b, 0x73 } });

    coinbase_input.sequence = bc::max_input_sequence;

    coinbase_tx.inputs.push_back(coinbase_input);

    chain::transaction_output coinbase_output;
    coinbase_output.value = coin_price(50);

    coinbase_output.script.push_operations(
        chain::operation{ chain::opcode::special,
            data_chunk{0x04, 0x67, 0x8a, 0xfd, 0xb0, 0xfe, 0x55, 0x48,
                       0x27, 0x19, 0x67, 0xf1, 0xa6, 0x71, 0x30, 0xb7,
                       0x10, 0x5c, 0xd6, 0xa8, 0x28, 0xe0, 0x39, 0x09,
                       0xa6, 0x79, 0x62, 0xe0, 0xea, 0x1f, 0x61, 0xde,
                       0xb6, 0x49, 0xf6, 0xbc, 0x3f, 0x4c, 0xef, 0x38,
                       0xc4, 0xf3, 0x55, 0x04, 0xe5, 0x1e, 0xc1, 0x12,
                       0xde, 0x5c, 0x38, 0x4d, 0xf7, 0xba, 0x0b, 0x8d,
                       0x57, 0x8a, 0x4c, 0x70, 0x2b, 0x6b, 0xf1, 0x1d,
                       0x5f}});

    coinbase_output.script.push_operations(
        chain::operation{ chain::opcode::checksig, data_chunk() });

    coinbase_tx.outputs.push_back(coinbase_output);

    chain::block genesis;
    genesis.header = header;
    genesis.transactions.push_back(coinbase_tx);

    BITCOIN_ASSERT(genesis.transactions.size() == 1);

    BITCOIN_ASSERT(
        chain::block::generate_merkle_root(genesis.transactions) == genesis.header.merkle);

    return genesis;
}
