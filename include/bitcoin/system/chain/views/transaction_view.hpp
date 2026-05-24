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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_VIEW_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_TRANSACTION_VIEW_HPP

#include <bitcoin/system/chain/enums/magic_numbers.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API transaction_view final
{
public:
    DEFAULT_COPY_MOVE(transaction_view);

    /// Source must be set to a tx position within the block buffer.
    /// Source position zero must be at the first byte of the block buffer.
    transaction_view(reader& tx_source, const data_chunk& block_buffer,
        bool witness) NOEXCEPT;

    /// Properties.
    bool is_empty() const NOEXCEPT;
    bool is_coinbase() const NOEXCEPT;
    bool is_segregated() const NOEXCEPT;
    size_t inputs() const NOEXCEPT;
    size_t outputs() const NOEXCEPT;
    uint32_t version() const NOEXCEPT;
    uint32_t locktime() const NOEXCEPT;
    size_t serialized_size(bool witness) const NOEXCEPT;
    const hash_digest& hash(bool witness) const NOEXCEPT;

    /// Methods.
    bool get_witness_commitment(hash_cref& commitment) const NOEXCEPT;
    bool get_witness_reservation(hash_cref& reservation) const NOEXCEPT;

    /// Streamers.
    static void write_input_script(flipper& sink, reader& source) NOEXCEPT;
    static void write_input(flipper& sink, reader& source) NOEXCEPT;
    static void write_output(flipper& sink, reader& source) NOEXCEPT;
    static void write_witness(flipper& sink, reader& source) NOEXCEPT;

    /// istreams.
    stream::in::fast get_inputs_stream() const NOEXCEPT;
    stream::in::fast get_outputs_stream() const NOEXCEPT;
    stream::in::fast get_witnesses_stream() const NOEXCEPT;

private:
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
    size_t witness_size_{};

    // Transaction hash.
    hash_digest txid_{};

    // Null hash if not segregated or stripped.
    hash_digest wtxid_{};
};

using transaction_views = std::vector<transaction_view>;

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
