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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_VIEW_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_VIEW_HPP

#include <bitcoin/system/chain/batch/batch.hpp>
#include <bitcoin/system/chain/context.hpp>
#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/chain/enums/script_version.hpp>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/chain/views/input_view.hpp>
#include <bitcoin/system/chain/views/output_view.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {
namespace view {

class BC_API transaction final
{
public:
    DEFAULT_COPY_MOVE(transaction);

    using input_iterator = view::inputs::const_iterator;

    /// Source must be set to a tx position within the block buffer.
    /// Source position zero must be at the first byte of the block buffer.
    transaction(reader& source, const data_chunk& block_buffer,
        bool coinbase, bool witness) NOEXCEPT;

    /// Serialization.
    data_chunk to_data(bool witness) const NOEXCEPT;
    void to_data(std::ostream& stream, bool witness) const NOEXCEPT;
    void to_data(writer& sink, bool witness) const NOEXCEPT;

    /// Properties.
    bool is_valid() const NOEXCEPT;
    bool is_coinbase() const NOEXCEPT;
    bool is_null_point() const NOEXCEPT;
    bool is_segregated() const NOEXCEPT;
    size_t inputs() const NOEXCEPT;
    size_t outputs() const NOEXCEPT;
    uint32_t version() const NOEXCEPT;
    uint32_t locktime() const NOEXCEPT;
    data_slice witnesses() const NOEXCEPT;
    size_t serialized_size(bool witness) const NOEXCEPT;
    const hash_digest& hash(bool witness) const NOEXCEPT;

    /// Store helpers.
    size_t input_table_size(bool pruned) const NOEXCEPT;
    size_t output_table_size() const NOEXCEPT;

    /// Methods.
    bool get_witness_commitment(hash_cref& commitment) const NOEXCEPT;
    bool get_witness_reservation(hash_cref& reservation) const NOEXCEPT;

    /// Streamers.
    static void write_input_script(flipper& sink, reader& source) NOEXCEPT;
    static void write_witness(flipper& sink, reader& source) NOEXCEPT;
    static size_t read_witness_size(reader& source) NOEXCEPT;

    /// istreams.
    stream::in::fast get_inputs_stream() const NOEXCEPT;
    stream::in::fast get_outputs_stream() const NOEXCEPT;
    stream::in::fast get_witnesses_stream() const NOEXCEPT;

    /// Populated properties (block view population required).
    /// -----------------------------------------------------------------------

    bool is_populated() const NOEXCEPT;
    input_iterator inputs_begin() const NOEXCEPT;
    input_iterator inputs_end() const NOEXCEPT;
    input_iterator input_at(uint32_t index) const NOEXCEPT;
    output output_at(uint32_t index) const NOEXCEPT;

    /// Computed properties.
    uint64_t fee() const NOEXCEPT;
    uint64_t spend() const NOEXCEPT;
    uint64_t value() const NOEXCEPT;
    size_t signature_operations(bool bip16, bool bip141) const NOEXCEPT;

    /// Validation (block view population required).
    /// -----------------------------------------------------------------------

    code check() const NOEXCEPT;
    code check(const context& ctx) const NOEXCEPT;
    code accept(const context& ctx) const NOEXCEPT;
    code connect(const context& ctx, const signatures& capture) const NOEXCEPT;

    /// Signature hashing (block view population required).
    bool signature_hash(hash_digest& out, const input_iterator& input,
        const script& subscript, uint64_t value, const hash_cptr& tapleaf,
        script_version version, uint8_t sighash_flags,
        uint32_t flags) const NOEXCEPT;

protected:
    friend class block;

    /// Population (block view).
    void set_inputs(const input_iterator& begin,
        const input_iterator& end) NOEXCEPT;

    /// Validation helpers.
    bool is_invalid_coinbase_size() const NOEXCEPT;
    bool is_null_non_coinbase() const NOEXCEPT;
    bool is_absolute_locked(size_t height, uint32_t timestamp,
        uint32_t median_time_past, bool bip113) const NOEXCEPT;
    bool is_missing_prevouts() const NOEXCEPT;
    bool is_overspent() const NOEXCEPT;
    code connect_input(const context& ctx, const input_iterator& it,
        const signatures& capture) const NOEXCEPT;

private:
    typedef struct
    {
        hash_digest points;
        hash_digest sequences;
        hash_digest outputs;
    } base_cache;
    typedef struct
    {
        hash_digest amounts;
        hash_digest scripts;
    } only_cache;

    // witness commitment
    static constexpr size_t reserved_pattern_size = 2;
    static constexpr size_t commitment_pattern_size = 6;
    static bool is_reserved_pattern(const uint8_t* stack, size_t size) NOEXCEPT;
    static bool is_commitment_pattern(const uint8_t* script,
        size_t size) NOEXCEPT;

    // buffer offsets
    const uint8_t* at_inputs() const NOEXCEPT;
    const uint8_t* at_outputs() const NOEXCEPT;
    const uint8_t* at_witnesses() const NOEXCEPT;

    // computed sizes
    size_t inputs_size() const NOEXCEPT;
    size_t outputs_size() const NOEXCEPT;
    size_t witnesses_size() const NOEXCEPT;
    size_t unstripped_size() const NOEXCEPT;
    size_t stripped_size() const NOEXCEPT;

    // Signature hash caching.
    // ------------------------------------------------------------------------

    void set_x1_base_hash() const NOEXCEPT;
    void set_x2_base_hash() const NOEXCEPT;
    void set_v1_only_hash() const NOEXCEPT;

    hash_digest x1_base_hash_points() const NOEXCEPT;
    hash_digest x1_base_hash_sequences() const NOEXCEPT;
    hash_digest x1_base_hash_outputs() const NOEXCEPT;
    hash_digest v1_only_hash_amounts() const NOEXCEPT;
    hash_digest v1_only_hash_scripts() const NOEXCEPT;

    const hash_digest& single_hash_points() const NOEXCEPT;
    const hash_digest& single_hash_amounts() const NOEXCEPT;
    const hash_digest& single_hash_scripts() const NOEXCEPT;
    const hash_digest& single_hash_sequences() const NOEXCEPT;
    const hash_digest& single_hash_outputs() const NOEXCEPT;

    const hash_digest& double_hash_points() const NOEXCEPT;
    const hash_digest& double_hash_sequences() const NOEXCEPT;
    const hash_digest& double_hash_outputs() const NOEXCEPT;

    // Signature hashing.
    // ------------------------------------------------------------------------

    uint32_t input_index(const input_iterator& input) const NOEXCEPT;
    bool output_overflow(size_t input) const NOEXCEPT;
    hash_digest output_hash_v0(const input_iterator& input) const NOEXCEPT;
    void write_outputs(writer& sink) const NOEXCEPT;

    void signature_hash_single(writer& sink, const input_iterator& input,
        const script& subscript, uint8_t sighash_flags) const NOEXCEPT;
    void signature_hash_none(writer& sink, const input_iterator& input,
        const script& subscript, uint8_t sighash_flags) const NOEXCEPT;
    void signature_hash_all(writer& sink, const input_iterator& input,
        const script& subscript, uint8_t sighash_flags) const NOEXCEPT;

    void unversioned_sighash(hash_digest& out, const input_iterator& input,
        const script& subscript, uint8_t sighash_flags) const NOEXCEPT;
    void version0_sighash(hash_digest& out, const input_iterator& input,
        const script& subscript, uint64_t value,
        uint8_t sighash_flags) const NOEXCEPT;
    bool version1_sighash(hash_digest& out, const input_iterator& input,
        const script& script, uint64_t value, const hash_cptr& tapleaf,
        uint8_t sighash_flags) const NOEXCEPT;

    // ------------------------------------------------------------------------

    // Pointer to tx in buffer.
    const uint8_t* tx_ptr_{};

    // Offset of first tx input in buffer.
    size_t in_offset_{};
    size_t in_count_{};

    // Offset of first tx output in buffer.
    size_t out_offset_{};
    size_t out_count_{};

    // Size of full transaction buffer (with witnesses/marker/sentinel).
    size_t size_{};

    // Size of tx witnesses only (without marker/sentinel).
    size_t witnesses_size_{};

    // Store helpers.
    size_t input_table_size_{};
    size_t output_table_size_{};

    // Transaction hash.
    hash_digest txid_{};

    // Null hash if !segregated or stripped or coinbase.
    hash_digest wtxid_{};

    // The transaction is segregated and not stripped.
    bool segregated_{};

    // The transaction is the first in its block.
    bool coinbase_{};

    // Populated input views (owned by the block view).
    bool populated_{};
    input_iterator inputs_begin_{};
    input_iterator inputs_end_{};

    // Signature hash caching (witness and taproot).
    mutable std::shared_ptr<base_cache> x1_base_cache_{};
    mutable std::shared_ptr<base_cache> x2_base_cache_{};
    mutable std::shared_ptr<only_cache> v1_only_cache_{};
};

using transactions = std::vector<transaction>;

} // namespace view
} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
