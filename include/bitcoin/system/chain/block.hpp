/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BLOCK_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BLOCK_HPP

#include <memory>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/header.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/transaction.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/math/math.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {

namespace chain {

class BC_API block
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(block);

    typedef std::shared_ptr<const block> cptr;

    static bool is_malleable64(const transaction_cptrs& txs) NOEXCEPT;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default block is an invalid object.
    block() NOEXCEPT;
    block(chain::header&& header, transactions&& txs) NOEXCEPT;
    block(const chain::header& header, const transactions& txs) NOEXCEPT;
    block(const chain::header::cptr& header,
        const transactions_cptr& txs) NOEXCEPT;

    block(stream::in::fast&& stream, bool witness) NOEXCEPT;
    block(stream::in::fast& stream, bool witness) NOEXCEPT;
    block(std::istream&& stream, bool witness) NOEXCEPT;
    block(std::istream& stream, bool witness) NOEXCEPT;
    block(reader&& source, bool witness) NOEXCEPT;
    block(reader& source, bool witness) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    bool operator==(const block& other) const NOEXCEPT;
    bool operator!=(const block& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data(bool witness) const NOEXCEPT;
    void to_data(std::ostream& stream, bool witness) const NOEXCEPT;
    void to_data(writer& sink, bool witness) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    size_t transactions() const NOEXCEPT;
    const chain::header& header() const NOEXCEPT;
    const chain::header::cptr header_ptr() const NOEXCEPT;
    const inputs_cptr inputs_ptr() const NOEXCEPT;
    const transactions_cptr& transactions_ptr() const NOEXCEPT;
    hashes transaction_hashes(bool witness) const NOEXCEPT;

    /// Computed properties.
    size_t outputs() const NOEXCEPT;
    size_t spends() const NOEXCEPT;
    size_t weight() const NOEXCEPT;
    uint64_t fees() const NOEXCEPT;
    uint64_t claim() const NOEXCEPT;
    hash_digest hash() const NOEXCEPT;
    bool is_segregated() const NOEXCEPT;
    size_t segregated() const NOEXCEPT;
    size_t serialized_size(bool witness) const NOEXCEPT;
    size_t signature_operations(bool bip16, bool bip141) const NOEXCEPT;

    /// Computed malleation properties.
    bool is_malleable() const NOEXCEPT;
    bool is_malleated() const NOEXCEPT;
    bool is_malleable32() const NOEXCEPT;
    bool is_malleated32() const NOEXCEPT;
    bool is_malleable64() const NOEXCEPT;
    bool is_malleated64() const NOEXCEPT;

    /// Cache setters/getters, not thread safe.
    /// -----------------------------------------------------------------------

    /// Cache (overrides hash() computation).
    void set_hashes(const data_chunk& data) NOEXCEPT;

    /// Reference used to avoid copy, sets cache if not set.
    const hash_digest& get_hash() const NOEXCEPT;

    /// Set/get memory allocation.
    void set_allocation(size_t allocation) const NOEXCEPT;
    size_t get_allocation() const NOEXCEPT;

    /// Identity.
    /// -----------------------------------------------------------------------

    code identify() const NOEXCEPT;
    code identify(const context& ctx) const NOEXCEPT;

    /// Validation.
    /// -----------------------------------------------------------------------

    /// Consensus checks (no DoS guards for block sync without headers first).
    code check() const NOEXCEPT;
    code check(const context& ctx) const NOEXCEPT;
    code accept(const context& ctx, size_t subsidy_interval,
        uint64_t initial_subsidy) const NOEXCEPT;
    code connect(const context& ctx) const NOEXCEPT;
    code confirm(const context& ctx) const NOEXCEPT;

    /// Populate previous outputs internal to the block.
    void populate() const NOEXCEPT;

    /// Populate previous outputs and metadata.locked internal to the block.
    /// Execution is shortcircuited for error with that metadata.locked set.
    /// False if any populated prevout is immature in the block context.
    code populate_with_metadata(const context& ctx) const NOEXCEPT;

protected:
    block(const chain::header::cptr& header,
        const chain::transactions_cptr& txs, bool valid) NOEXCEPT;

    code check_with_malleated() const NOEXCEPT;
    size_t malleated32_size() const NOEXCEPT;
    bool is_malleated32(size_t width) const NOEXCEPT;
    static constexpr bool is_malleable32(size_t set, size_t width) NOEXCEPT
    {
        // Malleable when set is odd at width depth and not before and not one.
        // This is the only case in which Merkle clones the last item in a set.
        for (auto depth = one; depth <= width; depth *= two, set = to_half(set))
            if (is_odd(set)) return depth == width && !is_one(set);

        return false;
    }

    /// Check (context free).
    /// -----------------------------------------------------------------------

    bool is_empty() const NOEXCEPT;
    bool is_oversized() const NOEXCEPT;
    bool is_first_non_coinbase() const NOEXCEPT;
    bool is_extra_coinbases() const NOEXCEPT;
    bool is_forward_reference() const NOEXCEPT;
    bool is_internal_double_spend() const NOEXCEPT;
    bool is_invalid_merkle_root() const NOEXCEPT;

    /// Accept (contextual).
    /// -----------------------------------------------------------------------

    bool is_overweight() const NOEXCEPT;
    bool is_invalid_coinbase_script(size_t height) const NOEXCEPT;
    bool is_hash_limit_exceeded() const NOEXCEPT;
    bool is_invalid_witness_commitment() const NOEXCEPT;

    /// Requires prevouts (value).
    bool is_overspent(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT;

    /// Assumes coinbase if prevout not populated (returns only legacy sigops).
    bool is_signature_operations_limited(bool bip16,
        bool bip141) const NOEXCEPT;

    /// Requires input.metadata.spent (prevout confirmation).
    bool is_unspent_coinbase_collision() const NOEXCEPT;

private:
    typedef struct { size_t nominal; size_t witnessed; } sizes;

    void assign_data(reader& source, bool witness) NOEXCEPT;
    static block from_data(reader& source, bool witness) NOEXCEPT;
    static sizes serialized_size(const transaction_cptrs& txs) NOEXCEPT;

    // context free
    hash_digest generate_merkle_root(bool witness) const NOEXCEPT;
    bool get_witness_commitment(hash_cref& commitment) const NOEXCEPT;
    bool get_witness_reservation(hash_cref& reservation) const NOEXCEPT;

    // contextual
    uint64_t reward(size_t height, uint64_t subsidy_interval,
        uint64_t initial_block_subsidy_satoshi, bool bip42) const NOEXCEPT;

    // delegated
    code check_transactions() const NOEXCEPT;
    code check_transactions(const context& ctx) const NOEXCEPT;
    code accept_transactions(const context& ctx) const NOEXCEPT;
    code connect_transactions(const context& ctx) const NOEXCEPT;
    code confirm_transactions(const context& ctx) const NOEXCEPT;

    // Block should be stored as shared (adds 16 bytes).
    // copy: 4 * 64 + 1 = 33 bytes (vs. 16 when shared).
    chain::header::cptr header_;
    chain::transactions_cptr txs_;

    // Cache.
    bool valid_;
    sizes size_;
    mutable size_t allocation_{};
};

typedef std_vector<block> blocks;

DECLARE_JSON_VALUE_CONVERTORS(block);
DECLARE_JSON_VALUE_CONVERTORS(block::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::block>
{
    size_t operator()(const bc::system::chain::block& value) const NOEXCEPT
    {
        return bc::system::unique_hash(value.hash());
    }
};
} // namespace std

#endif
