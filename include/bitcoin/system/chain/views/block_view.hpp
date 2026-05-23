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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_BLOCK_VIEW_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_BLOCK_VIEW_HPP

#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/views/transaction_view.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API block_view final
{
public:
    DELETE_COPY_MOVE(block_view);

    /// Segregation is managed and suppressed when witness is false.
    block_view(const data_chunk& block_buffer, bool witness) NOEXCEPT;

    /// Deserialization.
    bool is_valid() const NOEXCEPT;
    bool is_segregated() const NOEXCEPT;

    /// Properties (get_hash/hash are both dynamically computed).
    hash_digest hash() const NOEXCEPT;
    hash_digest get_hash() const NOEXCEPT;
    size_t transactions() const NOEXCEPT;
    const transaction_views& views() const NOEXCEPT;
    size_t serialized_size(bool witness) const NOEXCEPT;

    /// Validation.
    code identify() const NOEXCEPT;
    code identify(const context& ctx) const NOEXCEPT;

protected:
    /// Validation helpers.
    bool is_malleated() const NOEXCEPT;
    bool is_invalid_merkle_root() const NOEXCEPT;
    bool is_invalid_witness_commitment() const NOEXCEPT;

private:

    // Malleation.
    static bool is_malleable64(const transaction_views& txs) NOEXCEPT;
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

    bool witness_;
    const data_chunk& buffer_;
    transaction_views txs_{};
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
