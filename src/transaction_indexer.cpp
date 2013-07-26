#include <bitcoin/transaction_indexer.hpp>

#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

transaction_indexer::transaction_indexer(threadpool& pool)
  : async_strand(pool)
{
}

void transaction_indexer::query(const payment_address& payaddr,
    query_handler handle_query)
{
    queue(
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
        get_info_list<spend_info_list>(payaddr, spends_map_),
        get_info_list<output_info_list>(payaddr, outputs_map_));
}

void transaction_indexer::index(const transaction_type& tx,
    completion_handler handle_index)
{
    queue(
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
        if (!extract_input_address(payaddr, input.input_script))
            continue;
        spends_map_.emplace(payaddr,
            spend_info_type{input_point{tx_hash, i}, input.previous_output});
    }
    for (uint32_t i = 0; i < tx.outputs.size(); ++i)
    {
        const transaction_output_type& output = tx.outputs[i];
        payment_address payaddr;
        // Nothing to see here folks. Move along.
        if (!extract(payaddr, output.output_script))
            continue;
        outputs_map_.emplace(payaddr,
            output_info_type{output_point{tx_hash, i}, output.value});
    }
}

template <typename Point, typename EntryMultimap>
auto find_entry(const payment_address& key, const Point& value_point,
    EntryMultimap& map) -> decltype(map.begin())
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

void transaction_indexer::deindex(const transaction_type& tx,
    completion_handler handle_deindex)
{
    queue(
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
        if (!extract_input_address(payaddr, input.input_script))
            continue;
        auto it = find_entry(payaddr,
            input_point{tx_hash, i}, spends_map_);
        BITCOIN_ASSERT_MSG(it != spends_map_.end(),
            "Can't deindex transaction twice");
        spends_map_.erase(it);
    }
    for (uint32_t i = 0; i < tx.outputs.size(); ++i)
    {
        const transaction_output_type& output = tx.outputs[i];
        payment_address payaddr;
        // Nothing to see here folks. Move along.
        if (!extract(payaddr, output.output_script))
            continue;
        auto it = find_entry(payaddr,
            output_point{tx_hash, i}, outputs_map_);
        BITCOIN_ASSERT_MSG(it != outputs_map_.end(),
            "Can't deindex transaction twice");
        outputs_map_.erase(it);
    }
    periodic_update();
}

void transaction_indexer::periodic_update()
{
    // map of tx hash -> payment_address
    // use that to find relevant spends/outputs to erase.
}

} // namespace libbitcoin

