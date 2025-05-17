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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_HPP

#include <istream>
#include <memory>
#include <optional>
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
    /// Cache is copied/moved on copy/assign.
    DEFAULT_COPY_MOVE_DESTRUCT(transaction);

    typedef std::shared_ptr<const transaction> cptr;
    typedef input_cptrs::const_iterator input_iterator;

    /// Relative locktime also requires activation of bip68.
    static bool is_relative_locktime_applied(bool coinbase, uint32_t version,
        uint32_t sequence) NOEXCEPT;

    /// Not genesis and at least 100 blocks deep.
    static bool is_coinbase_mature(size_t coinbase_height,
        size_t height) NOEXCEPT;

    /// Optimized non-witness hash derivation using witness-serialized tx.
    static hash_digest desegregated_hash(size_t witnessed,
        size_t unwitnessed, const uint8_t* data) NOEXCEPT;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Default transaction is an invalid object.
    transaction() NOEXCEPT;

    transaction(uint32_t version, chain::inputs&& inputs,
        chain::outputs&& outputs, uint32_t locktime) NOEXCEPT;
    transaction(uint32_t version, const chain::inputs& inputs,
        const chain::outputs& outputs, uint32_t locktime) NOEXCEPT;
    transaction(uint32_t version, const inputs_cptr& inputs,
        const outputs_cptr& outputs, uint32_t locktime) NOEXCEPT;

    transaction(stream::in::fast&& stream, bool witness) NOEXCEPT;
    transaction(stream::in::fast& stream, bool witness) NOEXCEPT;
    transaction(std::istream&& stream, bool witness) NOEXCEPT;
    transaction(std::istream& stream, bool witness) NOEXCEPT;
    transaction(reader&& source, bool witness) NOEXCEPT;
    transaction(reader& source, bool witness) NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------

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
    size_t spends() const NOEXCEPT;
    size_t inputs() const NOEXCEPT;
    size_t outputs() const NOEXCEPT;
    uint32_t version() const NOEXCEPT;
    const inputs_cptr& inputs_ptr() const NOEXCEPT;
    const outputs_cptr& outputs_ptr() const NOEXCEPT;
    uint32_t locktime() const NOEXCEPT;

    /// Computed properties.
    size_t weight() const NOEXCEPT;
    uint64_t fee() const NOEXCEPT;
    uint64_t claim() const NOEXCEPT;
    uint64_t value() const NOEXCEPT;
    bool is_coinbase() const NOEXCEPT;
    bool is_segregated() const NOEXCEPT;
    hash_digest hash(bool witness) const NOEXCEPT;
    size_t serialized_size(bool witness) const NOEXCEPT;

    /// Cache setters/getters, not thread safe.
    /// -----------------------------------------------------------------------

    /// Initialize with externally-produced nominal hash value, as from store.
    void set_nominal_hash(const hash_digest& hash) const NOEXCEPT;

    /// Initialize with externally-produced witness hash value, as from store.
    /// This need not be set if the transaction is not segmented.
    void set_witness_hash(const hash_digest& hash) const NOEXCEPT;

    /// Reference used to avoid copy (not thread safe unless cached).
    const hash_digest& get_hash(bool witness) const NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    bool is_empty() const NOEXCEPT;
    bool is_dusty(uint64_t minimum_output_value) const NOEXCEPT;

    /// Requires no metadata, true if spend in own block would be locked.
    bool is_internally_locked(const input& in) const NOEXCEPT;

    /// Assumes coinbase if prevout not populated (returns only legacy sigops).
    size_t signature_operations(bool bip16, bool bip141) const NOEXCEPT;

    /// signature_hash exposed for op_check_multisig caching.
    bool signature_hash(hash_digest& out, const input_iterator& input,
        const script& sub, uint64_t value, uint8_t sighash_flags,
        script_version version, bool bip143, bool bip341) const NOEXCEPT;

    /// Not used internally.
    bool check_signature(const ec_signature& signature,
        const data_slice& public_key, const script& subscript, uint32_t index,
        uint64_t value, uint8_t sighash_flags, script_version version,
        uint32_t flags) const NOEXCEPT;

    /// Not used internally.
    bool create_endorsement(endorsement& out, const ec_secret& secret,
        const script& subscript, uint32_t index, uint64_t value,
        uint8_t sighash_flags, script_version version,
        uint32_t flags) const NOEXCEPT;

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
    transaction(uint32_t version, const inputs_cptr& inputs,
        const outputs_cptr& outputs, uint32_t locktime, bool segregated,
        bool valid) NOEXCEPT;

    /// Guard (context free).
    /// -----------------------------------------------------------------------

    ////bool is_coinbase() const NOEXCEPT;
    bool is_internal_double_spend() const NOEXCEPT;
    bool is_oversized() const NOEXCEPT;

    /// Guard (requires context).
    /// -----------------------------------------------------------------------

    ////bool is_segregated() const NOEXCEPT;
    bool is_overweight() const NOEXCEPT;

    /// Requires prevouts (value).
    ////bool is_missing_prevouts() const NOEXCEPT;

    /// Assumes coinbase if prevout not populated (returns only legacy sigops).
    bool is_signature_operations_limited(bool bip16,
        bool bip141) const NOEXCEPT;

    /// Check (context free).
    /// -----------------------------------------------------------------------

    ////bool is_empty() const NOEXCEPT;
    bool is_null_non_coinbase() const NOEXCEPT;
    bool is_invalid_coinbase_size() const NOEXCEPT;

    /// Check (requires context).
    /// -----------------------------------------------------------------------

    bool is_absolute_locked(size_t height, uint32_t timestamp,
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
    bool is_relative_locked(size_t height,
        uint32_t median_time_past) const NOEXCEPT;

    /// Requires input.metadata.height (prevout confirmation).
    bool is_immature(size_t height) const NOEXCEPT;

    /// Requires input.metadata.height (prevout confirmation).
    bool is_unconfirmed_spend(size_t height) const NOEXCEPT;

    /// Requires input.metadata.height/spent (prevout confirmation).
    bool is_confirmed_double_spend(size_t height) const NOEXCEPT;

private:
    typedef struct { size_t nominal; size_t witnessed; } sizes;

    static bool segregated(const chain::inputs& inputs) NOEXCEPT;
    static bool segregated(const input_cptrs& inputs) NOEXCEPT;
    static sizes serialized_size(const input_cptrs& inputs,
        const output_cptrs& outputs, bool segregated) NOEXCEPT;

    input_iterator input_at(uint32_t index) const NOEXCEPT;
    void assign_data(reader& source, bool witness) NOEXCEPT;
    chain::points points() const NOEXCEPT;

    // delegated
    code connect_input(const context& ctx,
        const input_iterator& it) const NOEXCEPT;

    // Caching.
    // ------------------------------------------------------------------------

    typedef struct
    {
        hash_digest points;
        hash_digest sequences;
        hash_digest outputs;
    } v0_cache;

    typedef struct
    {
        hash_digest amounts;
        hash_digest scripts;
        hash_digest points;
        hash_digest sequences;
        hash_digest outputs;
    } v1_cache;

    // Set sha256 cache if not set, so not thread safe unless cached.
    const hash_digest& single_hash_points() const NOEXCEPT;
    const hash_digest& single_hash_amounts() const NOEXCEPT;
    const hash_digest& single_hash_scripts() const NOEXCEPT;
    const hash_digest& single_hash_sequences() const NOEXCEPT;
    const hash_digest& single_hash_outputs() const NOEXCEPT;

    // Set sha256x2 cache if not set, so not thread safe unless cached.
    const hash_digest& double_hash_points() const NOEXCEPT;
    const hash_digest& double_hash_sequences() const NOEXCEPT;
    const hash_digest& double_hash_outputs() const NOEXCEPT;

    // Set v1 and/or v0 signature hash caches as applicable.
    void initialize_sighash_cache() const NOEXCEPT;
    void initialize_v0_cache() const NOEXCEPT;
    void initialize_v1_cache() const NOEXCEPT;

    hash_digest hash_points() const NOEXCEPT;
    hash_digest hash_sequences() const NOEXCEPT;
    hash_digest hash_outputs() const NOEXCEPT;

    // Signature hashing.
    // ------------------------------------------------------------------------

    static coverage mask_sighash(uint8_t sighash_flags) NOEXCEPT;
    static bool is_anyone_can_pay(uint8_t sighash_flags) NOEXCEPT;
    static uint32_t subscript_v1(const script& script) NOEXCEPT;
    uint8_t spend_type_v1(bool annex, bool tapscript) const NOEXCEPT;
    uint32_t input_index(const input_iterator& input) const NOEXCEPT;

    bool output_overflow(size_t input) const NOEXCEPT;
    hash_digest output_hash_v0(const input_iterator& input) const NOEXCEPT;

    void signature_hash_single(writer& sink, const input_iterator& input,
        const script& subscript, uint8_t sighash_flags) const NOEXCEPT;
    void signature_hash_none(writer& sink, const input_iterator& input,
        const script& subscript, uint8_t sighash_flags) const NOEXCEPT;
    void signature_hash_all(writer& sink, const input_iterator& input,
        const script& subscript, uint8_t sighash_flags) const NOEXCEPT;

    bool unversioned_sighash(hash_digest& out, const input_iterator& input,
        const script& subscript, uint8_t sighash_flags) const NOEXCEPT;
    bool version0_sighash(hash_digest& out, const input_iterator& input,
        const script& subscript, uint64_t value,
        uint8_t sighash_flags) const NOEXCEPT;
    bool version1_sighash(hash_digest& out, const input_iterator& input,
        const script& script, uint64_t value,
        uint8_t sighash_flags) const NOEXCEPT;

    // ------------------------------------------------------------------------

    // Transaction should be stored as shared (adds 16 bytes).
    // copy: 5 * 64 + 2 = 41 bytes (vs. 16 when shared).
    uint32_t version_;
    chain::inputs_cptr inputs_;
    chain::outputs_cptr outputs_;
    uint32_t locktime_;

    // Cache.
    bool segregated_;
    bool valid_;
    sizes size_;

    // Signature and identity hash caching (witness hash if witnessed).
    mutable std::optional<hash_digest> nominal_hash_{};
    mutable std::optional<hash_digest> witness_hash_{};

    mutable std::optional<v0_cache> sighash_cache_{};
    ////mutable std::shared_ptr<v0_cache> v0_cache_{};
    ////mutable std::shared_ptr<v1_cache> v1_cache_{};
};

typedef std_vector<transaction> transactions;
typedef std_vector<transaction::cptr> transaction_cptrs;
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
        return bc::system::unique_hash(value.hash(true));
    }
};
} // namespace std

#endif
