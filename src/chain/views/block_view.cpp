/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

#include <algorithm>
#include <iterator>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/views/transaction_view.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

using namespace system;

// constructor
// ----------------------------------------------------------------------------

block_view::block_view(data_chunk&& block_buffer, bool witness) NOEXCEPT
  : witness_{ witness }, buffer_{ to_shared(std::move(block_buffer)) }
{
    stream::in::fast istream(*buffer_);
    read::bytes::fast in(istream);
    in.skip_bytes(header::serialized_size());

    const auto txs = in.read_size(max_count);
    txs_.reserve(txs);

    for (size_t tx{}; tx < txs; ++tx)
        txs_.emplace_back(in, *buffer_, witness);

    // How are we getting empty tx and not seeing error here.
    if (!in)
        txs_.clear();
}

// serialization
// ----------------------------------------------------------------------------

data_chunk block_view::to_data(bool witness) const NOEXCEPT
{
    data_chunk data(serialized_size(witness));
    stream::out::fast ostream(data);
    write::bytes::fast out(ostream);
    to_data(out, witness);
    return data;
}

void block_view::to_data(std::ostream& stream, bool witness) const NOEXCEPT
{
    write::bytes::ostream out(stream);
    to_data(out, witness);
}

void block_view::to_data(writer& , bool ) const NOEXCEPT
{
    ////if (witness)
    ////{
    ////    sink.write_bytes(*buffer_);
    ////    return;
    ////}

    // TODO: write header from first bytes in buffer.
    ////header_->to_data(sink);
    ////sink.write_variable(txs_->size());

    // TODO: add writers to tx, skip witness as applicable.
    ////for (const auto& tx: *txs_)
    ////    tx->to_data(sink, witness);
    BC_ASSERT_MSG(false, "not implemented");
}

// public
// ----------------------------------------------------------------------------

bool block_view::is_valid() const NOEXCEPT
{
    return !txs_.empty();
}

bool block_view::is_segregated() const NOEXCEPT
{
    return witness_ && std::any_of(txs_.begin(), txs_.end(),
        [](const auto& tx) NOEXCEPT
        {
            return tx.is_segregated();
        });
}

hash_digest block_view::hash() const NOEXCEPT
{
    return bitcoin_hash(header::serialized_size(), buffer_->data());
}

size_t block_view::transactions() const NOEXCEPT
{
    return txs_.size();
}

const transaction_views& block_view::views() const NOEXCEPT
{
    return txs_;
}

size_t block_view::serialized_size(bool witness) const NOEXCEPT
{
    if (witness)
        return buffer_->size();

    auto total = header::serialized_size() + variable_size(txs_.size());
    for (const auto& tx: txs_)
        total += tx.serialized_size(false);

    return total;
}

code block_view::identify() const NOEXCEPT
{
    if (txs_.empty())
        return error::empty_block;

    if (is_malleated() || is_invalid_merkle_root())
        return error::invalid_transaction_commitment;

    return error::block_success;
}

code block_view::identify(const context& ctx) const NOEXCEPT
{
    const auto bip141 = ctx.is_enabled(bip141_rule);

    if (bip141 && is_invalid_witness_commitment())
        return error::invalid_witness_commitment;

    return error::block_success;
}

// protected
// ----------------------------------------------------------------------------

bool block_view::is_malleated() const NOEXCEPT
{
    return is_malleated64() || is_malleated32();
}

bool block_view::is_invalid_merkle_root() const NOEXCEPT
{
    return generate_merkle_root(false) != header_merkle_root();
}

bool block_view::is_invalid_witness_commitment() const NOEXCEPT
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
bool block_view::is_malleable64(const transaction_views& txs) NOEXCEPT
{
    return !txs.empty() && std::all_of(txs.begin(), txs.end(),
        [](const auto& tx) NOEXCEPT
        {
            return tx.serialized_size(false) == two * hash_size;
        });
}

bool block_view::is_malleated32() const NOEXCEPT
{
    return !is_zero(malleated32_size());
}

bool block_view::is_malleated64() const NOEXCEPT
{
    // First tx check is not sufficient, null point must be checked.
    return !txs_.empty() && !txs_.front().is_coinbase() &&
        is_malleable64(txs_);
}

size_t block_view::malleated32_size() const NOEXCEPT
{
    const auto malleated = txs_.size();
    for (auto mally = one; mally <= to_half(malleated); mally *= two)
        if (block::is_malleable32(malleated - mally, mally) &&
            is_malleated32(mally))
            return mally;

    return zero;
}

bool block_view::is_malleated32(size_t width) const NOEXCEPT
{
    const auto malleated = txs_.size();
    if (is_zero(width) || width > to_half(malleated))
        return false;

    auto mally = txs_.rbegin();
    auto legit = std::next(mally, width);

    while (!is_zero(width--))
        if ((*mally++).hash(false) != (*legit++).hash(false))
            return false;

    return true;
}

// merkle roots
// ----------------------------------------------------------------------------
// private

hash_digest block_view::header_merkle_root() const NOEXCEPT
{
    if (txs_.empty() || buffer_->empty())
        return null_hash;

    constexpr auto offset = sizeof(uint32_t) + hash_size;
    const auto start = std::next(buffer_->data(), offset);
    return unsafe_array_cast<uint8_t, hash_size>(start);
}

hashes block_view::transaction_hashes(bool witness) const NOEXCEPT
{
    hashes hashes{};
    hashes.reserve(txs_.size());
    for (const auto& tx: txs_)
        hashes.emplace_back(tx.hash(witness));

    return hashes;
}

hash_digest block_view::generate_merkle_root(bool witness) const NOEXCEPT
{
    return sha256::merkle_root(transaction_hashes(witness));
}

// witness commitment
// ----------------------------------------------------------------------------
// private

bool block_view::get_witness_commitment(hash_cref& commitment) const NOEXCEPT
{
    return !txs_.empty() && txs_.front().get_witness_commitment(commitment);
}

bool block_view::get_witness_reservation(hash_cref& reservation) const NOEXCEPT
{
    return !txs_.empty() && txs_.front().get_witness_reservation(reservation);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
