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
#include <bitcoin/system/chain/fast/fast_block.hpp>

#include <algorithm>
#include <iterator>
#include <bitcoin/system/chain/block.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/fast/fast_transaction.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

using namespace system;

// constructor
// ----------------------------------------------------------------------------

fast_block::fast_block(data_chunk&& buffer, bool witness) NOEXCEPT
  : witness_{ witness }, buffer_{ std::move(buffer) }
{
    stream::in::fast istream(buffer_);
    read::bytes::fast in(istream);
    in.skip_bytes(header::serialized_size());

    const auto txs = in.read_size(max_count);
    txs_.reserve(txs);

    for (size_t tx{}; tx < txs; ++tx)
        txs_.emplace_back(in, buffer_, witness);

    if (!in)
        txs_.clear();
}

// public
// ----------------------------------------------------------------------------

bool fast_block::is_valid() const NOEXCEPT
{
    return !txs_.empty();
}

code fast_block::identify() const NOEXCEPT
{
    if (txs_.empty())
        return error::empty_block;

    if (is_malleated() || is_invalid_merkle_root())
        return error::invalid_transaction_commitment;

    return error::block_success;
}

code fast_block::identify(const context& ctx) const NOEXCEPT
{
    const auto bip141 = ctx.is_enabled(bip141_rule);

    if (bip141 && is_invalid_witness_commitment())
        return error::invalid_witness_commitment;

    return error::block_success;
}

// protected
// ----------------------------------------------------------------------------

bool fast_block::is_malleated() const NOEXCEPT
{
    return is_malleated64() || is_malleated32();
}

bool fast_block::is_segregated() const NOEXCEPT
{
    return witness_ && std::any_of(txs_.begin(), txs_.end(),
        [](const auto& tx) NOEXCEPT
        {
            return tx.is_witnessed();
        });
}

bool fast_block::is_invalid_merkle_root() const NOEXCEPT
{
    return generate_merkle_root(false) != header_merkle_root();
}

bool fast_block::is_invalid_witness_commitment() const NOEXCEPT
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
bool fast_block::is_malleable64(const fast_transactions& txs) NOEXCEPT
{
    return !txs.empty() && std::all_of(txs.begin(), txs.end(),
        [](const auto& tx) NOEXCEPT
        {
            return tx.serialized_size(false) == two * hash_size;
        });
}

bool fast_block::is_malleated32() const NOEXCEPT
{
    return !is_zero(malleated32_size());
}

bool fast_block::is_malleated64() const NOEXCEPT
{
    // First tx check is not sufficient, null point must be checked.
    return !txs_.empty() && !txs_.front().is_coinbase() &&
        is_malleable64(txs_);
}

size_t fast_block::malleated32_size() const NOEXCEPT
{
    const auto malleated = txs_.size();
    for (auto mally = one; mally <= to_half(malleated); mally *= two)
        if (block::is_malleable32(malleated - mally, mally) &&
            is_malleated32(mally))
            return mally;

    return zero;
}

bool fast_block::is_malleated32(size_t width) const NOEXCEPT
{
    const auto malleated = txs_.size();
    if (is_zero(width) || width > to_half(malleated))
        return false;

    auto mally = txs_.rbegin();
    auto legit = std::next(mally, width);

    while (!is_zero(width--))
        if ((*mally++).id() != (*legit++).id())
            return false;

    return true;
}

// merkle roots
// ----------------------------------------------------------------------------
// private

hash_digest fast_block::header_merkle_root() const NOEXCEPT
{
    if (txs_.empty() || buffer_.empty())
        return null_hash;

    constexpr auto offset = sizeof(uint32_t) + hash_size;
    const auto start = std::next(buffer_.data(), offset);
    return unsafe_array_cast<uint8_t, hash_size>(start);
}

hashes fast_block::transaction_hashes(bool witness) const NOEXCEPT
{
    hashes hashes{};
    hashes.reserve(txs_.size());
    for (const auto& tx: txs_)
        hashes.emplace_back(witness ? tx.witness_id() : tx.id());

    return hashes;
}

hash_digest fast_block::generate_merkle_root(bool witness) const NOEXCEPT
{
    return sha256::merkle_root(transaction_hashes(witness));
}

// witness commitment
// ----------------------------------------------------------------------------
// private

bool fast_block::get_witness_commitment(hash_cref& commitment) const NOEXCEPT
{
    return !txs_.empty() && txs_.front().get_witness_commitment(commitment);
}

bool fast_block::get_witness_reservation(hash_cref& reservation) const NOEXCEPT
{
    return !txs_.empty() && txs_.front().get_witness_reservation(reservation);
}

} // namespace chain
} // namespace system
} // namespace libbitcoin
