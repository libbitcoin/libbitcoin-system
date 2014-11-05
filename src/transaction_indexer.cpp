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
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/transaction.hpp>
#include <bitcoin/bitcoin/transaction_indexer.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

transaction_indexer::transaction_indexer(threadpool& pool)
  : strand_(pool)
{
}

void transaction_indexer::query(const payment_address& payaddr,
    query_handler handle_query)
{
    strand_.queue(
        std::bind(&transaction_indexer::do_query,
            this, payaddr, handle_query));
}

template <typename InfoList, typename EntryMultimap>
InfoList get_info_list(const payment_address& payaddr,
    EntryMultimap& map)
{
    InfoList info;
    auto iter_pair = map.equal_range(payaddr);
    for (auto it = iter_pair.first; it != iter_pair.second; ++it)
        info.push_back(it->second);
    return info;
}
void transaction_indexer::do_query(const payment_address& payaddr,
    query_handler handle_query)
{
    handle_query(std::error_code(),
        get_info_list<output_info_list>(payaddr, outputs_map_),
        get_info_list<spend_info_list>(payaddr, spends_map_));
}

template <typename Point, typename EntryMultimap>
auto find_entry(const payment_address& key,
    const Point& value_point, EntryMultimap& map) -> decltype(map.begin())
{
    auto iter_pair = map.equal_range(key);
    for (auto it = iter_pair.first; it != iter_pair.second; ++it)
    {
        // This entry should only occur once in the multimap.
        if (it->second.point == value_point)
            return it;
    }
    return map.end();
}

template <typename Point, typename EntryMultimap>
bool index_does_not_exist(const payment_address& key,
    const Point& value_point, EntryMultimap& map)
{
    return find_entry(key, value_point, map) == map.end();
}

void transaction_indexer::index(const transaction_type& tx,
    completion_handler handle_index)
{
    strand_.queue(
        std::bind(&transaction_indexer::do_index,
            this, tx, handle_index));
}
void transaction_indexer::do_index(const transaction_type& tx,
    completion_handler handle_index)
{
    hash_digest tx_hash = hash_transaction(tx);
    for (uint32_t i = 0; i < tx.inputs.size(); ++i)
    {
        const transaction_input_type& input = tx.inputs[i];
        payment_address payaddr;
        // Nothing to see here folks. Move along.
        if (!extract(payaddr, input.script))
            continue;
        input_point point{tx_hash, i};
        BITCOIN_ASSERT_MSG(
            index_does_not_exist(payaddr, point, spends_map_),
            "Transaction is indexed duplicate times!");
        spends_map_.emplace(payaddr,
            spend_info_type{point, input.previous_output});
    }
    for (uint32_t i = 0; i < tx.outputs.size(); ++i)
    {
        const transaction_output_type& output = tx.outputs[i];
        payment_address payaddr;
        // Nothing to see here folks. Move along.
        if (!extract(payaddr, output.script))
            continue;
        output_point point{tx_hash, i};
        BITCOIN_ASSERT_MSG(
            index_does_not_exist(payaddr, point, outputs_map_),
            "Transaction is indexed duplicate times!");
        outputs_map_.emplace(payaddr,
            output_info_type{point, output.value});
    }
    handle_index(std::error_code());
}

void transaction_indexer::deindex(const transaction_type& tx,
    completion_handler handle_deindex)
{
    strand_.queue(
        std::bind(&transaction_indexer::do_deindex,
            this, tx, handle_deindex));
}
void transaction_indexer::do_deindex(const transaction_type& tx,
    completion_handler handle_deindex)
{
    hash_digest tx_hash = hash_transaction(tx);
    for (uint32_t i = 0; i < tx.inputs.size(); ++i)
    {
        const transaction_input_type& input = tx.inputs[i];
        payment_address payaddr;
        // Nothing to see here folks. Move along.
        if (!extract(payaddr, input.script))
            continue;
        input_point point{tx_hash, i};
        auto it = find_entry(payaddr, point, spends_map_);
        BITCOIN_ASSERT_MSG(it != spends_map_.end(),
            "Can't deindex transaction twice");
        spends_map_.erase(it);
        BITCOIN_ASSERT_MSG(
            index_does_not_exist(payaddr, point, spends_map_),
            "Transaction is indexed duplicate times!");
    }
    for (uint32_t i = 0; i < tx.outputs.size(); ++i)
    {
        const transaction_output_type& output = tx.outputs[i];
        payment_address payaddr;
        // Nothing to see here folks. Move along.
        if (!extract(payaddr, output.script))
            continue;
        output_point point{tx_hash, i};
        auto it = find_entry(payaddr, point, outputs_map_);
        BITCOIN_ASSERT_MSG(it != outputs_map_.end(),
            "Can't deindex transaction twice");
        outputs_map_.erase(it);
        BITCOIN_ASSERT_MSG(
            index_does_not_exist(payaddr, point, outputs_map_),
            "Transaction is indexed duplicate times!");
    }
    handle_deindex(std::error_code());
}

void blockchain_history_fetched(const std::error_code& ec,
    const history_list& history, const size_t stop_height,
    transaction_indexer& indexer, const payment_address& address,
    blockchain::fetch_handler_history handle_fetch);
void indexer_history_fetched(const std::error_code& ec,
    const output_info_list& outputs, const spend_info_list& spends,
    history_list history, const size_t stop_height,
    blockchain::fetch_handler_history handle_fetch);
// Fetch the history first from the blockchain and then from the indexer.
void fetch_history(blockchain& chain, transaction_indexer& indexer,
    const payment_address& address,
    blockchain::fetch_handler_history handle_fetch, size_t from_height)
{
    chain.fetch_history(address,
        std::bind(blockchain_history_fetched, _1, _2, _3,
            std::ref(indexer), address, handle_fetch), from_height);
}
void blockchain_history_fetched(const std::error_code& ec,
    const history_list& history, const size_t stop_height,
    transaction_indexer& indexer, const payment_address& address,
    blockchain::fetch_handler_history handle_fetch)
{
    if (ec)
        handle_fetch(ec, history_list(), 0);
    else
        indexer.query(address,
            std::bind(indexer_history_fetched, _1, _2, _3,
                history, stop_height, handle_fetch));
}
void indexer_history_fetched(const std::error_code& ec,
    const output_info_list& outputs, const spend_info_list& spends,
    history_list history, const size_t stop_height,
    blockchain::fetch_handler_history handle_fetch)
{
    if (ec)
    {
        handle_fetch(ec, history_list(), 0);
        return;
    }
    BITCOIN_ASSERT_MSG(false, "This code is untested.");
    // Just add in outputs.
    for (const output_info_type& output_info: outputs)
    {
        // There is always a chance of inconsistency.
        // So we resolve these and move on.
        // This can happen when new blocks arrive in, and indexer.query()
        // is called midway through a bunch of txpool.try_delete()
        // operations.
        // If do_query() is queued before the last do_doindex() and there's
        // a transaction in our query in that block, then we will have
        // a conflict.
        bool is_conflict = false;
        for (const history_row& row: history)
        {
            // Loop over outputs only.
            if (row.id != history_row_id::output)
                continue;
            // Usually the indexer and memory doesn't have any
            // transactions indexed that are already confirmed
            // and in the blockchain.
            // This is a rare corner case.
            if (row.point == output_info.point)
            {
                is_conflict = true;
                break;
            }
        }
        if (is_conflict)
            continue;
        // Everything OK. Insert outpoint.
        history.emplace_back(history_row{
            history_row_id::output,
            output_info.point,
            0,
            output_info.value
        });
    }
    // Now mark spends.
    for (const spend_info_type& spend_info: spends)
    {
        // Iterate history looking for the output we need.
        bool is_conflict = false;
        for (const history_row& row: history)
        {
            // Loop over inputs only.
            if (row.id != history_row_id::spend)
                continue;
            if (row.point == spend_info.point)
            {
                is_conflict = true;
                break;
            }
        }
        if (is_conflict)
            continue;
        // Everything OK. Insert spend.
        history.emplace_back(history_row{
            history_row_id::spend,
            spend_info.point,
            0,
            spend_checksum(spend_info.previous_output)
        });
        // This assert can be triggered if the pool fills and starts
        // dropping transactions.
        // In practice this should not happen often and isn't a problem.
        //BITCOIN_ASSERT_MSG(found, "Couldn't find output for adding spend");
    }
    handle_fetch(std::error_code(), history, stop_height);
}

} // namespace libbitcoin

