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
block_type step(const block_point& root, const block_point& head, size_t n)
{
    std::cout << "Stepping " << n << std::endl;
    transaction_type coinbase_tx = create_coinbase(public_key());
    const block_type* prev_blk = &head.blk;
    static hash_digest txh6 = null_hash;
    if (n == 4)
    {
        prev_blk = lookup(root, {0, 0, 0});
        output_point prevout{
            hash_transaction(root.prefix_chain[2].transactions[0]), 0};
        transaction_type tx = construct_transaction(prevout);
        txh6 = hash_transaction(tx);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 5)
    {
        prev_blk = lookup(root, {0, 0, 0, 0});
    }
    else if (n == 6)
    {
        output_point prevout{
            hash_transaction(root.prefix_chain[1].transactions[0]), 0};
        transaction_type tx = construct_transaction(prevout);
        txh6 = hash_transaction(tx);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 7)
    {
        BITCOIN_ASSERT(txh6 != null_hash);
        output_point prevout{txh6, 0};
        transaction_type tx = construct_transaction(prevout);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 8)
    {
        prev_blk = lookup(root, {0, 0, 0});
    }
    else if (n == 9)
    {
        BITCOIN_ASSERT(txh6 != null_hash);
        output_point prevout{txh6, 0};
        transaction_type tx = construct_transaction(prevout);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 11)
    {
        prev_blk = lookup(root, {0, 0, 0, 1});
    }
    else if (n == 15)
    {
        // Attempt double spend.
        output_point prevout{
            hash_transaction(root.prefix_chain[2].transactions[0]), 0};
        transaction_type tx = construct_transaction(prevout);
        txh6 = hash_transaction(tx);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    else if (n == 16)
    {
        prev_blk = lookup(root, {0, 0, 0, 2, 0});
    }
    else if (n == 20)
    {
        BITCOIN_ASSERT(txh6 != null_hash);
        output_point prevout{txh6, 0};
        transaction_type tx = construct_transaction(prevout);
        return mine_next(*prev_blk, {coinbase_tx, tx});
    }
    return mine_next(*prev_blk, {coinbase_tx});
}

