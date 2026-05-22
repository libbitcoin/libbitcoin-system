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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_FAST_BLOCK_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_FAST_BLOCK_HPP

#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/fast/fast_transaction.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API fast_block final
{
public:
    DELETE_COPY_MOVE(fast_block);

    fast_block(data_chunk&& buffer, bool witness) NOEXCEPT;

    /// Methods.
    bool is_valid() const NOEXCEPT;
    bool is_malleated() const NOEXCEPT;
    bool is_segregated() const NOEXCEPT;
    bool is_invalid_merkle_root() const NOEXCEPT;
    bool is_invalid_witness_commitment() const NOEXCEPT;

    /// Validation.
    code identify() const NOEXCEPT;
    code identify(const context& ctx) const NOEXCEPT;

protected:
    using fast_transactions = std::vector<fast_transaction>;

    /// Methods.
    static bool is_malleable64(const fast_transactions& txs) NOEXCEPT;
    bool is_malleated32() const NOEXCEPT;
    bool is_malleated64() const NOEXCEPT;
    size_t malleated32_size() const NOEXCEPT;
    bool is_malleated32(size_t width) const NOEXCEPT;

    /// Properties.
    hash_digest header_merkle_root() const NOEXCEPT;
    hashes transaction_hashes(bool witness) const NOEXCEPT;
    hash_digest generate_merkle_root(bool witness) const NOEXCEPT;
    bool committed_hash(hash_cref& out) const NOEXCEPT;
    bool get_witness_commitment(hash_cref& commitment) const NOEXCEPT;
    bool reserved_hash(hash_cref& out) const NOEXCEPT;
    bool get_witness_reservation(hash_cref& reservation) const NOEXCEPT;

private:
    bool witness_;
    data_chunk buffer_;
    fast_transactions txs_{};
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
