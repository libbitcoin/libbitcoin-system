/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_HPP

#include <cstddef>
#include <cstdint>
#include <istream>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API transaction
{
public:
    typedef std::shared_ptr<const transaction> ptr;

    // Constructors.
    // ------------------------------------------------------------------------

    /// Default transaction is an invalid object.
    transaction() noexcept;

    transaction(transaction&& other) noexcept;
    transaction(const transaction& other) noexcept;

    transaction(uint32_t version, uint32_t locktime, chain::inputs&& inputs,
        chain::outputs&& outputs) noexcept;
    transaction(uint32_t version, uint32_t locktime,
        const chain::inputs& inputs, const chain::outputs& outputs) noexcept;
    transaction(uint32_t version, uint32_t locktime, const inputs_ptr& inputs,
        const outputs_ptr& outputs) noexcept;
    
    transaction(const data_slice& data, bool witness) noexcept;
    transaction(std::istream&& stream, bool witness) noexcept;
    transaction(std::istream& stream, bool witness) noexcept;
    transaction(reader&& source, bool witness) noexcept;
    transaction(reader& source, bool witness) noexcept;

    // Operators.
    // ------------------------------------------------------------------------

    transaction& operator=(transaction&& other) noexcept;
    transaction& operator=(const transaction& other) noexcept;

    bool operator==(const transaction& other) const noexcept;
    bool operator!=(const transaction& other) const noexcept;

    // Serialization.
    // ------------------------------------------------------------------------

    data_chunk to_data(bool witness) const noexcept;
    void to_data(std::ostream& stream, bool witness) const noexcept;
    void to_data(writer& sink, bool witness) const noexcept;

    // Properties.
    // ------------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const noexcept;
    uint32_t version() const noexcept;
    uint32_t locktime() const noexcept;
    const inputs_ptr inputs() const noexcept;
    const outputs_ptr outputs() const noexcept;

    /// Computed properties.
    size_t weight() const noexcept;
    uint64_t fee() const noexcept;
    uint64_t claim() const noexcept;
    uint64_t value() const noexcept;
    hash_digest hash(bool witness) const noexcept;
    bool is_coinbase() const noexcept;
    bool is_segregated() const noexcept;
    size_t serialized_size(bool witness) const noexcept;

    // Methods.
    // ------------------------------------------------------------------------

    bool is_dusty(uint64_t minimum_output_value) const noexcept;
    size_t signature_operations(bool bip16, bool bip141) const noexcept;
    chain::points points() const noexcept;
    hash_digest output_hash(uint32_t index) const noexcept;
    hash_digest outputs_hash() const noexcept;
    hash_digest points_hash() const noexcept;
    hash_digest sequences_hash() const noexcept;

    // signature_hash exposed for op_check_multisig caching.
    hash_digest signature_hash(uint32_t index,
        const script& subscript, uint64_t value, uint8_t flags,
        script_version version, bool bip143) const noexcept;

    bool check_signature(const ec_signature& signature,
        const data_slice& public_key, const script& subscript,
        uint32_t index, uint64_t value, uint8_t flags, script_version version,
        bool bip143) const noexcept;

    bool create_endorsement(endorsement& out, const ec_secret& secret,
        const script& prevout_script, uint32_t index, uint64_t value,
        uint8_t flags, script_version version, bool bip143) const noexcept;

    // Guards (for tx pool without compact blocks).
    // ------------------------------------------------------------------------

    code guard() const noexcept;
    code guard(const context& state) const noexcept;

    // Validation (consensus checks).
    // ------------------------------------------------------------------------

    code check() const noexcept;
    code accept(const context& state) const noexcept;
    code connect(const context& state) const noexcept;

protected:
    transaction(bool segregated, uint32_t version, uint32_t locktime,
        const inputs_ptr& inputs, const outputs_ptr& outputs,
        bool valid) noexcept;

    // Guard (context free).
    // ------------------------------------------------------------------------

    ////bool is_coinbase() const noexcept;
    bool is_internal_double_spend() const noexcept;
    bool is_oversized() const noexcept;

    // Guard (contextual).
    // ------------------------------------------------------------------------

    ////bool is_segregated() const noexcept;
    bool is_overweight() const noexcept;

    // prevouts required
    ////bool is_missing_prevouts() const noexcept;
    bool is_signature_operations_limit(bool bip16, bool bip141) const noexcept;

    // Check (context free).
    // ------------------------------------------------------------------------

    bool is_empty() const noexcept;
    bool is_null_non_coinbase() const noexcept;
    bool is_invalid_coinbase_size() const noexcept;

    // Accept (contextual).
    // ------------------------------------------------------------------------

    bool is_non_final(size_t height, uint32_t timestamp,
        uint32_t median_time_past, bool bip113) const noexcept;

    // prevouts required
    bool is_missing_prevouts() const noexcept;
    bool is_overspent() const noexcept;
    bool is_immature(size_t height) const noexcept;
    bool is_locked(size_t height, uint32_t median_time_past) const noexcept;

    // prevout confirmation state required
    bool is_unconfirmed_spend(size_t height) const noexcept;
    bool is_confirmed_double_spend(size_t height) const noexcept;

    // Connect (contextual).
    // ------------------------------------------------------------------------

    code connect(const context& state, uint32_t index) const noexcept;

private:
    static transaction from_data(reader& source, bool witness) noexcept;
    static bool segregated(const chain::inputs& inputs) noexcept;
    static bool segregated(const inputs_ptr& inputs) noexcept;
    ////static size_t maximum_size(bool coinbase noexcept;

    // signature hash
    void signature_hash_single(writer& sink, uint32_t index,
        const script& subscript, uint8_t flags) const noexcept;
    void signature_hash_none(writer& sink, uint32_t index,
        const script& subscript, uint8_t flags) const noexcept;
    void signature_hash_all(writer& sink, uint32_t index,
        const script& subscript, uint8_t flags) const noexcept;
    hash_digest unversioned_signature_hash(uint32_t index,
        const script& subscript, uint8_t flags) const noexcept;
    hash_digest version_0_signature_hash(uint32_t index,
        const script& subscript, uint64_t value, uint8_t flags,
        bool bip143) const noexcept;

    // Transaction should be stored as shared (adds 16 bytes).
    // copy: 5 * 64 + 2 = 41 bytes (vs. 16 when shared).
    uint32_t version_;
    uint32_t locktime_;
    inputs_ptr inputs_;
    outputs_ptr outputs_;
    bool segregated_;
    bool valid_;

    // Witness transaction signature caching.
    // ------------------------------------------------------------------------

    struct hash_cache
    {
        hash_digest outputs;
        hash_digest points;
        hash_digest sequences;
    };

    void initialize_hash_cache() const noexcept;

    mutable std::unique_ptr<hash_cache> cache_;
};

typedef std::vector<transaction> transactions;
typedef std::vector<transaction::ptr> transaction_ptrs;
typedef std::shared_ptr<transaction_ptrs> transactions_ptr;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
