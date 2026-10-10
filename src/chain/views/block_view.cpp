/**
 * Copyright (c) 2011-2026 libbitcoin developers
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/chain/views/block_view.hpp>

#include <numeric>
#include <unordered_set>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/views/input_view.hpp>
#include <bitcoin/system/chain/views/output_view.hpp>
#include <bitcoin/system/chain/views/transaction_view.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {
namespace view {

using namespace system;

// constructor
// ----------------------------------------------------------------------------

block::block(data_chunk&& block_buffer, bool witness) NOEXCEPT
  : witness_{ witness }, buffer_{ to_shared(std::move(block_buffer)) }
{
    stream::in::fast istream(*buffer_);
    read::bytes::fast in(istream);
    in.skip_bytes(header::serialized_size());

    const auto txs = in.read_size(max_count);
    if (is_zero(txs))
        return;

    txs_.reserve(txs);
    txs_.emplace_back(in, *buffer_, true, witness);

    for (auto tx = one; tx < txs; ++tx)
        txs_.emplace_back(in, *buffer_, false, witness);

    if (!in)
        txs_.clear();
}

// serialization
// ----------------------------------------------------------------------------

data_chunk block::to_data(bool witness) const NOEXCEPT
{
    data_chunk data(serialized_size(witness));
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out, witness);
    return data;
}

void block::to_data(std::ostream& stream, bool witness) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void block::to_data(writer& sink, bool witness) const NOEXCEPT
{
    // The witnessed form is the original buffer.
    if (witness)
    {
        sink.write_bytes(*buffer_);
        return;
    }

    // Strip witness: the header and transaction count are unaffected.
    sink.write_bytes(buffer_->data(), header::serialized_size());
    sink.write_variable(txs_.size());
    for (const auto& tx: txs_)
        tx.to_data(sink, false);
}

// public
// ----------------------------------------------------------------------------

bool block::is_valid() const NOEXCEPT
{
    return !txs_.empty();
}

bool block::is_segregated() const NOEXCEPT
{
    return witness_ && std::any_of(txs_.begin(), txs_.end(),
        [](const auto& tx) NOEXCEPT
        {
            return tx.is_segregated();
        });
}

hash_digest block::hash() const NOEXCEPT
{
    return bitcoin_hash(header::serialized_size(), buffer_->data());
}

size_t block::transactions() const NOEXCEPT
{
    return txs_.size();
}

size_t block::spends() const NOEXCEPT
{
    // Overflow returns max_size_t.
    const auto sum = [](size_t total, const auto& tx) NOEXCEPT
    {
        return ceilinged_add(total, tx.inputs());
    };

    return txs_.empty() ? zero :
        std::accumulate(std::next(txs_.begin()), txs_.end(), zero, sum);
}

bool block::is_populated() const NOEXCEPT
{
    return !is_null(inputs_);
}

const view::inputs& block::inputs() const NOEXCEPT
{
    static const view::inputs empty{};
    return is_populated() ? *inputs_ : empty;
}

const view::outputs& block::prevouts() const NOEXCEPT
{
    static const view::outputs empty{};
    return is_populated() ? *prevouts_ : empty;
}

const view::transactions& block::views() const NOEXCEPT
{
    return txs_;
}

size_t block::serialized_size(bool witness) const NOEXCEPT
{
    if (witness)
        return buffer_->size();

    auto total = header::serialized_size() + variable_size(txs_.size());
    for (const auto& tx: txs_)
        total += tx.serialized_size(false);

    return total;
}

code block::identify() const NOEXCEPT
{
    if (txs_.empty())
        return error::empty_block;

    if (is_malleated() || is_invalid_merkle_root())
        return error::invalid_transaction_commitment;

    return error::block_success;
}

code block::identify(const context& ctx) const NOEXCEPT
{
    const auto invalid = ctx.is_enabled(bip141_rule) ?
        is_invalid_witness_commitment() : is_segregated();

    return invalid ? error::invalid_witness_commitment : error::block_success;
}

// protected
// ----------------------------------------------------------------------------

bool block::is_malleated() const NOEXCEPT
{
    return is_malleated64() || is_malleated32();
}

bool block::is_invalid_merkle_root() const NOEXCEPT
{
    return generate_merkle_root(false) != header_merkle_root();
}

bool block::is_invalid_witness_commitment() const NOEXCEPT
{
    if (txs_.empty())
        return false;

    hash_cref commit{ null_hash };
    if (!get_witness_commitment(commit))
        return is_segregated();

    hash_cref reserve{ null_hash };
    if (!get_witness_reservation(reserve))
        return true;

    return commit != sha256::double_hash(generate_merkle_root(true), reserve);
}

// malleation
// ----------------------------------------------------------------------------
// private

// static
bool block::is_malleable64(const view::transactions& txs) NOEXCEPT
{
    return !txs.empty() && std::all_of(txs.begin(), txs.end(),
        [](const auto& tx) NOEXCEPT
        {
            return tx.serialized_size(false) == two * hash_size;
        });
}

bool block::is_malleated32() const NOEXCEPT
{
    return !is_zero(malleated32_size());
}

bool block::is_malleated64() const NOEXCEPT
{
    // First tx check is not sufficient, null point must be checked.
    return !txs_.empty() && !txs_.front().is_null_point() &&
        is_malleable64(txs_);
}

size_t block::malleated32_size() const NOEXCEPT
{
    const auto malleated = txs_.size();
    for (auto width = one; width <= to_half(malleated); width *= two)
        if (is_malleated32(width))
            return width;

    return zero;
}

// A set of tx hashes has the merkle root of a shorter set if and only if at
// some depth its node count is even and above two and its last two nodes are
// equal: the shorter set has an odd count at that depth and clones its last
// node, which the longer set holds. This is the test at width depth.
bool block::is_malleated32(size_t width) const NOEXCEPT
{
    // Caller bounds width.
    BC_ASSERT(is_power2(width) && width <= to_half(txs_.size()));

    const auto malleated = txs_.size();
    const auto count = ceilinged_divide(malleated, width);
    if (is_odd(count) || count <= two)
        return false;

    const auto last = sub1(count) * width;
    const auto prior = last - width;
    const auto leaves = malleated - last;
    for (size_t at{}; at < width; ++at)
        if (txs_[last + chain::block::merkle_index(at, leaves, width)].hash(false) !=
            txs_[prior + at].hash(false))
            return false;

    return true;
}

// merkle roots
// ----------------------------------------------------------------------------
// private

hash_digest block::header_merkle_root() const NOEXCEPT
{
    BC_ASSERT(!txs_.empty() && !buffer_->empty());

    constexpr auto offset = sizeof(uint32_t) + hash_size;
    const auto start = std::next(buffer_->data(), offset);
    return unsafe_array_cast<uint8_t, hash_size>(start);
}

hashes block::transaction_hashes(bool witness) const NOEXCEPT
{
    hashes hashes{};
    hashes.reserve(txs_.size());
    for (const auto& tx: txs_)
        hashes.emplace_back(tx.hash(witness));

    return hashes;
}

hash_digest block::generate_merkle_root(bool witness) const NOEXCEPT
{
    return sha256::merkle_root(transaction_hashes(witness));
}

// witness commitment
// ----------------------------------------------------------------------------
// private

bool block::get_witness_commitment(hash_cref& commitment) const NOEXCEPT
{
    return !txs_.empty() && txs_.front().get_witness_commitment(commitment);
}

bool block::get_witness_reservation(hash_cref& reservation) const NOEXCEPT
{
    return !txs_.empty() && txs_.front().get_witness_reservation(reservation);
}

// population
// ----------------------------------------------------------------------------

code block::populate(const context& ctx, data_chunk&& prevouts) NOEXCEPT
{
    if (txs_.empty())
        return error::empty_block;

    populate_inputs();
    const std::vector<bool> selected(txs_.size(), true);
    if (!populate_prevouts(std::move(prevouts), selected))
        return error::missing_previous_output;

    return populate_internal(ctx);
}

bool block::populate(data_chunk&& prevouts,
    const std::vector<bool>& selected) NOEXCEPT
{
    if (txs_.empty() || selected.size() != txs_.size())
        return false;

    populate_inputs();
    return populate_prevouts(std::move(prevouts), selected);
}

// private
void block::populate_inputs() NOEXCEPT
{
    constexpr auto point_size = chain::point::serialized_size();
    constexpr auto sequence_size = sizeof(uint32_t);

    // Overflow returns max_size_t.
    const auto sum = [](size_t total, const auto& tx) NOEXCEPT
    {
        return ceilinged_add(total, tx.inputs());
    };

    inputs_ = to_shared<view::inputs>();
    inputs_->reserve(std::accumulate(txs_.begin(), txs_.end(), zero, sum));

    for (const auto& tx: txs_)
    {
        auto istream = tx.get_inputs_stream();
        read::bytes::fast source{ istream };
        auto wstream = tx.get_witnesses_stream();
        read::bytes::fast wsource{ wstream };
        const auto* inputs = tx.at_inputs();
        const auto* witnesses = tx.at_witnesses();
        const auto segregated = tx.is_segregated();

        for (size_t in{}; in < tx.inputs(); ++in)
        {
            const auto* input = std::next(inputs, source.get_read_position());
            source.skip_bytes(point_size);
            source.skip_bytes(source.read_size() + sequence_size);

            const uint8_t* witness{};
            size_t witness_size{};
            if (segregated)
            {
                witness = std::next(witnesses, wsource.get_read_position());
                witness_size = transaction::read_witness_size(wsource);
            }

            inputs_->emplace_back(input, witness, witness_size);
        }
    }

    // Ranges are assigned after all insertions, which invalidate end.
    auto begin = inputs_->cbegin();
    for (auto& tx: txs_)
    {
        const auto end = std::next(begin, tx.inputs());
        tx.set_inputs(begin, end);
        begin = end;
    }
}

// private
bool block::populate_prevouts(data_chunk&& prevouts,
    const std::vector<bool>& selected) NOEXCEPT
{
    prevout_buffer_ = to_shared(std::move(prevouts));
    prevouts_ = to_shared<view::outputs>();
    prevouts_->reserve(spends());

    // The vector is not reallocated, so its element addresses are stable.
    const auto* position = prevout_buffer_->data();
    const auto* end = std::next(position, prevout_buffer_->size());
    auto select = std::next(selected.cbegin());
    for (auto tx = std::next(txs_.begin()); tx != txs_.end(); ++tx, ++select)
    {
        if (!*select)
            continue;

        for (auto in = tx->inputs_begin(); in != tx->inputs_end(); ++in)
        {
            if (position >= end)
                return false;

            const auto& prevout = prevouts_->emplace_back(position);
            position = std::next(position, prevout.serialized_size());
            in->prevout = &prevout;
        }
    }

    return position == end;
}

// private
code block::populate_internal(const context& ctx) const NOEXCEPT
{
    const auto& self = txs_.front().hash(false);
    const auto bip68 = ctx.is_enabled(chain::flags::bip68_rule);
    unordered_set_of_hash_cref hashes{};

    for (auto tx = std::next(txs_.begin()); tx != txs_.end(); ++tx)
    {
        const auto relative = bip68 &&
            tx->version() >= relative_locktime_min_version;

        for (auto in = tx->inputs_begin(); in != tx->inputs_end(); ++in)
        {
            if (in->point_hash() == self)
                return error::coinbase_maturity;

            if (!relative || !chain::input::is_relative_locktime_applied(in->sequence()))
                continue;

            if (hashes.empty())
                for (const auto& other: txs_)
                    hashes.emplace(other.hash(false));

            // An internal spend has zero age, so any relative lock applies.
            if (hashes.contains(in->point_hash()) &&
                chain::input::is_relative_locked(in->sequence(), zero, zero, zero, zero))
                return error::relative_time_locked;
        }
    }

    return error::block_success;
}

// validation
// ----------------------------------------------------------------------------

code block::check() const NOEXCEPT
{
    BC_ASSERT(is_populated());

    if (txs_.empty())
        return error::empty_block;
    if (is_oversized())
        return error::block_size_limit;
    if (is_first_non_coinbase())
        return malleated_or(error::first_not_coinbase);
    if (is_extra_coinbases())
        return error::extra_coinbases;
    if (is_forward_reference())
        return error::forward_reference;
    if (is_internal_double_spend())
        return malleated_or(error::block_internal_double_spend);

    return check_transactions();
}

code block::check(const context& ctx) const NOEXCEPT
{
    BC_ASSERT(is_populated());
    const auto bip141 = ctx.is_enabled(bip141_rule);
    const auto bip34 = ctx.is_enabled(bip34_rule);
    const auto bip50 = ctx.is_enabled(bip50_rule);

    if (bip141 && is_overweight())
        return error::block_weight_limit;
    if (bip34 && is_invalid_coinbase_script(ctx.height))
        return error::coinbase_height_mismatch;
    if (bip50 && is_hash_limit_exceeded())
        return error::temporary_hash_limit;

    return check_transactions(ctx);
}

code block::accept(const context& ctx, size_t subsidy_interval,
    uint64_t initial_subsidy) const NOEXCEPT
{
    BC_ASSERT(is_populated());
    const auto bip16 = ctx.is_enabled(bip16_rule);
    const auto bip42 = ctx.is_enabled(bip42_rule);
    const auto bip141 = ctx.is_enabled(bip141_rule);

    if (is_overspent(ctx.height, subsidy_interval, initial_subsidy, bip42))
        return error::coinbase_value_limit;
    if (is_signature_operations_limited(bip16, bip141))
        return error::block_sigop_limit;

    return accept_transactions(ctx);
}

code block::connect(const context& ctx,
    const signatures& capture) const NOEXCEPT
{
    BC_ASSERT(is_populated());
    return connect_transactions(ctx, capture);
}

// protected
// ----------------------------------------------------------------------------

bool block::is_oversized() const NOEXCEPT
{
    return serialized_size(false) > max_block_size;
}

bool block::is_first_non_coinbase() const NOEXCEPT
{
    return !txs_.empty() && !is_coinbase(txs_.front());
}

bool block::is_extra_coinbases() const NOEXCEPT
{
    if (txs_.empty())
        return false;

    return std::any_of(std::next(txs_.begin()), txs_.end(),
        [](const auto& tx) NOEXCEPT
        {
            return is_coinbase(tx);
        });
}

bool block::is_forward_reference() const NOEXCEPT
{
    if (txs_.empty())
        return false;

    unordered_set_of_hash_cref hashes(sub1(txs_.size()));
    const auto end = std::prev(txs_.rend());
    for (auto tx = txs_.rbegin(); tx != end; ++tx)
    {
        for (auto in = tx->inputs_begin(); in != tx->inputs_end(); ++in)
            if (hashes.contains(in->point_hash()))
                return true;

        hashes.emplace(tx->hash(false));
    }

    return false;
}

bool block::is_internal_double_spend() const NOEXCEPT
{
    if (txs_.empty())
        return false;

    std::unordered_set<cref_point> points(spends());
    for (auto tx = std::next(txs_.begin()); tx != txs_.end(); ++tx)
        for (auto in = tx->inputs_begin(); in != tx->inputs_end(); ++in)
            if (!points.emplace(in->point_hash(), in->point_index()).second)
                return true;

    return false;
}

bool block::is_overweight() const NOEXCEPT
{
    return weight() > max_block_weight;
}

bool block::is_invalid_coinbase_script(size_t height) const NOEXCEPT
{
    if (txs_.empty() || is_zero(txs_.front().inputs()))
        return false;

    const auto& script = txs_.front().inputs_begin()->script();
    return !script::is_coinbase_pattern(script.ops(), height);
}

bool block::is_hash_limit_exceeded() const NOEXCEPT
{
    if (txs_.empty())
        return false;

    // A set is used to collapse duplicates.
    unordered_set_of_hash_cref hashes(txs_.size());

    // Just the coinbase tx hash, skip its null input hashes.
    hashes.emplace(txs_.front().hash(false));

    for (auto tx = std::next(txs_.begin()); tx != txs_.end(); ++tx)
    {
        hashes.emplace(tx->hash(false));
        for (auto in = tx->inputs_begin(); in != tx->inputs_end(); ++in)
            hashes.emplace(in->point_hash());
    }

    return hashes.size() > hash_limit;
}

bool block::is_overspent(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT
{
    return claim() > reward(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42);
}

bool block::is_signature_operations_limited(bool bip16,
    bool bip141) const NOEXCEPT
{
    const auto limit = bip141 ? max_fast_sigops : max_block_sigops;
    return signature_operations(bip16, bip141) > limit;
}

size_t block::weight() const NOEXCEPT
{
    // Block weight is 3 * nominal size * + 1 * witness size [bip141].
    return ceilinged_add(
        ceilinged_multiply(base_size_contribution, serialized_size(false)),
        ceilinged_multiply(total_size_contribution, serialized_size(true)));
}

uint64_t block::fees() const NOEXCEPT
{
    // Overflow returns max_uint64.
    const auto value = [](uint64_t total, const auto& tx) NOEXCEPT
    {
        return ceilinged_add(total, tx.fee());
    };

    return std::accumulate(txs_.begin(), txs_.end(), 0_u64, value);
}

uint64_t block::claim() const NOEXCEPT
{
    return txs_.empty() ? zero : txs_.front().spend();
}

uint64_t block::reward(size_t height, uint64_t subsidy_interval,
    uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT
{
    // Overflow returns max_uint64.
    return ceilinged_add(fees(), chain::block::subsidy(height, subsidy_interval,
        initial_block_subsidy_satoshi, bip42));
}

size_t block::signature_operations(bool bip16, bool bip141) const NOEXCEPT
{
    // Overflow returns max_size_t.
    const auto value = [=](size_t total, const auto& tx) NOEXCEPT
    {
        return ceilinged_add(total, tx.signature_operations(bip16, bip141));
    };

    return std::accumulate(txs_.begin(), txs_.end(), zero, value);
}

code block::check_transactions() const NOEXCEPT
{
    for (const auto& tx: txs_)
        if (const auto ec = tx.check())
            return ec;

    return error::block_success;
}

code block::check_transactions(const context& ctx) const NOEXCEPT
{
    for (const auto& tx: txs_)
        if (const auto ec = tx.check(ctx))
            return ec;

    return error::block_success;
}

code block::accept_transactions(const context& ctx) const NOEXCEPT
{
    if (!txs_.empty())
        for (auto tx = std::next(txs_.begin()); tx != txs_.end(); ++tx)
            if (const auto ec = tx->accept(ctx))
                return ec;

    return error::block_success;
}

code block::connect_transactions(const context& ctx,
    const signatures& capture) const NOEXCEPT
{
    if (!txs_.empty())
        for (auto tx = std::next(txs_.begin()); tx != txs_.end(); ++tx)
            if (const auto ec = tx->connect(ctx, capture))
                return ec;

    return error::block_success;
}

// private
// ----------------------------------------------------------------------------

// static
bool block::is_coinbase(const transaction& tx) NOEXCEPT
{
    return is_one(tx.inputs()) && tx.is_null_point();
}

code block::malleated_or(const code& ec) const NOEXCEPT
{
    return is_malleated() ? error::invalid_transaction_commitment : ec;
}

} // namespace view
} // namespace chain
} // namespace system
} // namespace libbitcoin
