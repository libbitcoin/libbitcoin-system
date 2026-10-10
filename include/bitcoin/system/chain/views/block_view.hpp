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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BLOCK_VIEW_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BLOCK_VIEW_HPP

#include <bitcoin/system/chain/batch/batch.hpp>
#include <bitcoin/system/chain/context.hpp>
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

class BC_API block
{
public:
    DEFAULT_COPY_MOVE(block);

    /// Segregation is managed and suppressed when witness is false.
    block(data_chunk&& block_buffer, bool witness) NOEXCEPT;

    /// Serialization.
    data_chunk to_data(bool witness) const NOEXCEPT;
    void to_data(std::ostream& stream, bool witness) const NOEXCEPT;
    void to_data(writer& sink, bool witness) const NOEXCEPT;

    /// Properties (hash is dynamically computed).
    bool is_valid() const NOEXCEPT;
    bool is_segregated() const NOEXCEPT;
    hash_digest hash() const NOEXCEPT;
    size_t transactions() const NOEXCEPT;
    size_t spends() const NOEXCEPT;
    const view::transactions& views() const NOEXCEPT;
    size_t serialized_size(bool witness) const NOEXCEPT;

    /// Populated properties (population required).
    bool is_populated() const NOEXCEPT;
    const view::inputs& inputs() const NOEXCEPT;
    const view::outputs& prevouts() const NOEXCEPT;

    /// Identity.
    code identify() const NOEXCEPT;
    code identify(const context& ctx) const NOEXCEPT;

    /// Population.
    /// Prevouts are the wire-encoded prevouts of each spend in block order.
    /// Fails if any populated prevout is internally immature or locked.
    code populate(const context& ctx, data_chunk&& prevouts) NOEXCEPT;

    /// Prevouts are the wire-encoded prevouts of each spend of the selected
    /// txs in block order, others are null (internal checks are skipped).
    bool populate(data_chunk&& prevouts,
        const std::vector<bool>& selected) NOEXCEPT;

    /// Validation (population required).
    code check() const NOEXCEPT;
    code check(const context& ctx) const NOEXCEPT;
    code accept(const context& ctx, size_t subsidy_interval,
        uint64_t initial_subsidy) const NOEXCEPT;
    code connect(const context& ctx, const signatures& capture) const NOEXCEPT;

protected:
    /// Identity helpers.
    bool is_malleated() const NOEXCEPT;
    bool is_invalid_merkle_root() const NOEXCEPT;
    bool is_invalid_witness_commitment() const NOEXCEPT;

    /// Validation helpers.
    bool is_oversized() const NOEXCEPT;
    bool is_first_non_coinbase() const NOEXCEPT;
    bool is_extra_coinbases() const NOEXCEPT;
    bool is_forward_reference() const NOEXCEPT;
    bool is_internal_double_spend() const NOEXCEPT;
    bool is_overweight() const NOEXCEPT;
    bool is_invalid_coinbase_script(size_t height) const NOEXCEPT;
    bool is_hash_limit_exceeded() const NOEXCEPT;
    bool is_overspent(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT;
    bool is_signature_operations_limited(bool bip16,
        bool bip141) const NOEXCEPT;

    size_t weight() const NOEXCEPT;
    uint64_t fees() const NOEXCEPT;
    uint64_t claim() const NOEXCEPT;
    uint64_t reward(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT;
    size_t signature_operations(bool bip16, bool bip141) const NOEXCEPT;

    code check_transactions() const NOEXCEPT;
    code check_transactions(const context& ctx) const NOEXCEPT;
    code accept_transactions(const context& ctx) const NOEXCEPT;
    code connect_transactions(const context& ctx,
        const signatures& capture) const NOEXCEPT;

private:
    static bool is_coinbase(const transaction& tx) NOEXCEPT;
    code malleated_or(const code& ec) const NOEXCEPT;

    // Malleation.
    static bool is_malleable64(const view::transactions& txs) NOEXCEPT;
    bool is_malleated32() const NOEXCEPT;
    bool is_malleated64() const NOEXCEPT;
    size_t malleated32_size() const NOEXCEPT;
    bool is_malleated32(size_t width) const NOEXCEPT;

    // Merkle roots.
    hash_digest header_merkle_root() const NOEXCEPT;
    hashes transaction_hashes(bool witness) const NOEXCEPT;
    hash_digest generate_merkle_root(bool witness) const NOEXCEPT;

    // Witness commitment.
    bool get_witness_commitment(hash_cref& commitment) const NOEXCEPT;
    bool get_witness_reservation(hash_cref& reservation) const NOEXCEPT;

    // Population.
    void populate_inputs() NOEXCEPT;
    bool populate_prevouts(data_chunk&& prevouts,
        const std::vector<bool>& selected) NOEXCEPT;
    code populate_internal(const context& ctx) const NOEXCEPT;

    bool witness_;
    chunk_cptr buffer_;
    view::transactions txs_{};

    // Populated (shared by copies).
    chunk_cptr prevout_buffer_{};
    std::shared_ptr<view::inputs> inputs_{};
    std::shared_ptr<view::outputs> prevouts_{};
};

} // namespace view
} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
