#include <bitcoin/transaction_indexer.hpp>

#include <bitcoin/transaction.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

namespace posix_time = boost::posix_time;
using posix_time::time_duration;
using posix_time::minutes;
using posix_time::second_clock;

const time_duration update_interval = minutes(20);

auto now = []() { return second_clock::universal_time(); };

transaction_indexer::transaction_indexer(threadpool& pool)
  : async_strand(pool)
{
    // The queue is empty so no point processing expired items right away.
    last_expiry_update_ = now();
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

template <typename Iterator>
Iterator find_unexpired_begin(const Iterator first, const Iterator last,
    const time_duration transaction_lifetime)
{
    auto current_time = now();
    auto unexpired_begin = last;
    for (auto it = first; it != last; ++it)
        // First transaction
        if (it->timestamp_ > current_time - transaction_lifetime)
        {
            unexpired_begin = it;
            break;
        }
    return unexpired_begin;
}
template <typename Multimap>
void delete_tx(const hash_digest& tx_hash, Multimap& map)
{
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        const auto& point = it->second.point;
        if (point.hash == tx_hash)
            map.erase(it);
    }
}
void transaction_indexer::periodic_update()
{
    if (last_expiry_update_ < now() + update_interval)
        return;
    auto unexpired_begin = find_unexpired_begin(
        expiry_queue_.begin(), expiry_queue_.end(), transaction_lifetime_);
    // No expired transactions exist.
    if (unexpired_begin == expiry_queue_.end())
    {
        BITCOIN_ASSERT(expiry_queue_.empty() ||
            expiry_queue_.front().timestamp_ > now() - transaction_lifetime_);
        return;
    }
    BITCOIN_ASSERT(!expiry_queue_.empty());
    BITCOIN_ASSERT(
        expiry_queue_.front().timestamp_ <= now() - transaction_lifetime_);
    for (auto it = expiry_queue_.begin(); it != unexpired_begin; ++it)
    {
        delete_tx(it->tx_hash, spends_map_);
        delete_tx(it->tx_hash, outputs_map_);
    }
    // Now actually erase the expired entries from the queue.
    expiry_queue_.erase_after(expiry_queue_.before_begin(), unexpired_begin);
}

} // namespace libbitcoin

