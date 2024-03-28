/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

#include <istream>
#include <memory>
#include <vector>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/input.hpp>
#include <bitcoin/system/chain/output.hpp>
#include <bitcoin/system/chain/point.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/error.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API transaction
{
public:
    typedef std::shared_ptr<const transaction> cptr;
    typedef input_cptrs::const_iterator input_iterator;

    static bool is_coinbase_mature(size_t coinbase_height,
        size_t height) NOEXCEPT;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default transaction is an invalid object.
    transaction() NOEXCEPT;
    virtual ~transaction() NOEXCEPT;

    /// Cache is defaulted on copy/assign.
    transaction(transaction&& other) NOEXCEPT;
    transaction(const transaction& other) NOEXCEPT;

    transaction(uint32_t version, chain::inputs&& inputs,
        chain::outputs&& outputs, uint32_t locktime) NOEXCEPT;
    transaction(uint32_t version, const chain::inputs& inputs,
        const chain::outputs& outputs, uint32_t locktime) NOEXCEPT;
    transaction(uint32_t version, const inputs_cptr& inputs,
        const outputs_cptr& outputs, uint32_t locktime) NOEXCEPT;
    
    transaction(const data_slice& data, bool witness) NOEXCEPT;
    ////transaction(stream::in::fast&& stream, bool witness) NOEXCEPT;
    transaction(stream::in::fast& stream, bool witness) NOEXCEPT;
    transaction(std::istream&& stream, bool witness) NOEXCEPT;
    transaction(std::istream& stream, bool witness) NOEXCEPT;
    transaction(reader&& source, bool witness) NOEXCEPT;
    transaction(reader& source, bool witness) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

    /// Cache is defaulted on copy/assign.
    transaction& operator=(transaction&& other) NOEXCEPT;
    transaction& operator=(const transaction& other) NOEXCEPT;

    bool operator==(const transaction& other) const NOEXCEPT;
    bool operator!=(const transaction& other) const NOEXCEPT;

    /// Serialization.
    /// -----------------------------------------------------------------------

    data_chunk to_data(bool witness) const NOEXCEPT;
    void to_data(std::ostream& stream, bool witness) const NOEXCEPT;
    void to_data(writer& sink, bool witness) const NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------

    /// Native properties.
    bool is_valid() const NOEXCEPT;
    uint32_t version() const NOEXCEPT;
    const inputs_cptr& inputs_ptr() const NOEXCEPT;
    const outputs_cptr& outputs_ptr() const NOEXCEPT;
    uint32_t locktime() const NOEXCEPT;

    /// Computed properties.
    size_t weight() const NOEXCEPT;
    uint64_t fee() const NOEXCEPT;
    uint64_t claim() const NOEXCEPT;
    uint64_t value() const NOEXCEPT;
    hash_digest hash(bool witness) const NOEXCEPT;
    bool is_coinbase() const NOEXCEPT;
    bool is_segregated() const NOEXCEPT;
    size_t serialized_size(bool witness) const NOEXCEPT;

    /// Cache (these override hash(bool) computation).
    void set_hash(hash_digest&& hash) const NOEXCEPT;
    void set_witness_hash(hash_digest&& hash) const NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    bool is_empty() const NOEXCEPT;
    bool is_dusty(uint64_t minimum_output_value) const NOEXCEPT;

    /// Assumes coinbase if prevout not populated (returns only legacy sigops).
    size_t signature_operations(bool bip16, bool bip141) const NOEXCEPT;
    chain::points points() const NOEXCEPT;
    hash_digest outputs_hash() const NOEXCEPT;
    hash_digest points_hash() const NOEXCEPT;
    hash_digest sequences_hash() const NOEXCEPT;

    // signature_hash exposed for op_check_multisig caching.
    hash_digest signature_hash(const input_iterator& input, const script& sub,
        uint64_t value, uint8_t sighash_flags, script_version version,
        bool bip143) const NOEXCEPT;

    bool check_signature(const ec_signature& signature,
        const data_slice& public_key, const script& sub, uint32_t index,
        uint64_t value, uint8_t sighash_flags, script_version version,
        bool bip143) const NOEXCEPT;

    bool create_endorsement(endorsement& out, const ec_secret& secret,
        const script& sub, uint32_t index, uint64_t value,
        uint8_t sighash_flags, script_version version,
        bool bip143) const NOEXCEPT;

    /// Guards (for tx pool without compact blocks).
    /// -----------------------------------------------------------------------

    code guard_check() const NOEXCEPT;
    code guard_check(const context& ctx) const NOEXCEPT;
    code guard_accept(const context& ctx) const NOEXCEPT;

    /// Validation (consensus checks).
    /// -----------------------------------------------------------------------

    code check() const NOEXCEPT;
    code check(const context& ctx) const NOEXCEPT;
    code accept(const context& ctx) const NOEXCEPT;
    code connect(const context& ctx) const NOEXCEPT;
    code confirm(const context& ctx) const NOEXCEPT;

protected:
    transaction(uint32_t version, const chain::inputs_cptr& inputs,
        const chain::outputs_cptr& outputs, uint32_t locktime, bool segregated,
        bool valid) NOEXCEPT;

    /// Guard (context free).
    /// -----------------------------------------------------------------------

    ////bool is_coinbase() const NOEXCEPT;
    bool is_internal_double_spend() const NOEXCEPT;
    bool is_oversized() const NOEXCEPT;

    /// Guard ((requires context).
    /// -----------------------------------------------------------------------

    ////bool is_segregated() const NOEXCEPT;
    bool is_overweight() const NOEXCEPT;

    /// Requires prevouts (value).
    ////bool is_missing_prevouts() const NOEXCEPT;

    /// Assumes coinbase if prevout not populated (returns only legacy sigops).
    bool is_signature_operations_limit(bool bip16, bool bip141) const NOEXCEPT;

    /// Check (context free).
    /// -----------------------------------------------------------------------

    ////bool is_empty() const NOEXCEPT;
    bool is_null_non_coinbase() const NOEXCEPT;
    bool is_invalid_coinbase_size() const NOEXCEPT;

    /// Check (requires context).
    /// -----------------------------------------------------------------------

    bool is_non_final(size_t height, uint32_t timestamp,
        uint32_t median_time_past, bool bip113) const NOEXCEPT;

    /// Accept (requires prevouts).
    /// -----------------------------------------------------------------------

    /// Requires prevouts.
    bool is_missing_prevouts() const NOEXCEPT;

    /// Requires prevouts (value).
    bool is_overspent() const NOEXCEPT;

    /// Confirm (requires confirmation order).
    /// -----------------------------------------------------------------------

    /// Requires input.metadata.height/median_time_past (prevout confirmation).
    bool is_locked(size_t height, uint32_t median_time_past) const NOEXCEPT;

    /// Requires input.metadata.height (prevout confirmation).
    bool is_immature(size_t height) const NOEXCEPT;

    /// Requires input.metadata.height (prevout confirmation).
    bool is_unconfirmed_spend(size_t height) const NOEXCEPT;

    /// Requires input.metadata.height/spent (prevout confirmation).
    bool is_confirmed_double_spend(size_t height) const NOEXCEPT;

private:
    static transaction from_data(reader& source, bool witness) NOEXCEPT;
    static bool segregated(const chain::inputs& inputs) NOEXCEPT;
    static bool segregated(const chain::input_cptrs& inputs) NOEXCEPT;
    ////static size_t maximum_size(bool coinbase) NOEXCEPT;

    // signature hash
    hash_digest output_hash(const input_iterator& input) const NOEXCEPT;
    input_iterator input_at(uint32_t index) const NOEXCEPT;
    uint32_t input_index(const input_iterator& input) const NOEXCEPT;
    void signature_hash_single(writer& sink, const input_iterator& input,
        const script& sub, uint8_t sighash_flags) const NOEXCEPT;
    void signature_hash_none(writer& sink, const input_iterator& input,
        const script& sub, uint8_t sighash_flags) const NOEXCEPT;
    void signature_hash_all(writer& sink, const input_iterator& input,
        const script& sub, uint8_t sighash_flags) const NOEXCEPT;
    hash_digest unversioned_signature_hash(const input_iterator& input,
        const script& sub, uint8_t sighash_flags) const NOEXCEPT;
    hash_digest version_0_signature_hash(const input_iterator& input,
        const script& sub, uint64_t value, uint8_t sighash_flags,
        bool bip143) const NOEXCEPT;

    // Transaction should be stored as shared (adds 16 bytes).
    // copy: 5 * 64 + 2 = 41 bytes (vs. 16 when shared).
    uint32_t version_;
    chain::inputs_cptr inputs_;
    chain::outputs_cptr outputs_;
    uint32_t locktime_;

    // TODO: pack these flags.
    bool segregated_;
    bool valid_;

private:
    typedef struct
    {
        hash_digest outputs;
        hash_digest points;
        hash_digest sequences;
    } hash_cache;

    void initialize_hash_cache() const NOEXCEPT;

    // Signature and identity hash cashing (witness hash if witnessed).
    mutable std::unique_ptr<hash_cache> cache_{};
    mutable std::unique_ptr<const hash_digest> hash_{};
    mutable std::unique_ptr<const hash_digest> witness_hash_{};
};

typedef std::vector<transaction> transactions;
typedef std::vector<transaction::cptr> transaction_cptrs;
typedef std::shared_ptr<transaction_cptrs> transactions_ptr;
typedef std::shared_ptr<const transaction_cptrs> transactions_cptr;

DECLARE_JSON_VALUE_CONVERTORS(transaction);
DECLARE_JSON_VALUE_CONVERTORS(transaction::cptr);

} // namespace chain
} // namespace system
} // namespace libbitcoin

namespace std
{
template<>
struct hash<bc::system::chain::transaction>
{
    size_t operator()(const bc::system::chain::transaction& value) const NOEXCEPT
    {
        // Witness coinbases will collide (null_hash).
        return std::hash<bc::system::hash_digest>{}(value.hash(true));
    }
};
} // namespace std

#endif
