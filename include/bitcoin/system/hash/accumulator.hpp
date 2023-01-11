/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_HASH_ACCUMULATOR_HPP
#define LIBBITCOIN_SYSTEM_HASH_ACCUMULATOR_HPP

#include <string>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/algorithms.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Accumulator for SHA/RMD/MD# streaming hash algorithms.
/// Provides size/padding optimized finalized hashing utilities.
template <typename Algorithm, bool Checked = build_checked,
    if_base_of<algorithm_t, Algorithm> = true>
class accumulator
{
public:
    using byte_t = typename Algorithm::byte_t;
    using state_t = typename Algorithm::state_t;
    using digest_t = typename Algorithm::digest_t;

    /// Construct/reset.
    /// -----------------------------------------------------------------------

    /// Sets initial state to Hash initialization vector.
    constexpr accumulator() NOEXCEPT;

    /// Accepts an initial state and count of blocks accumulated by it.
    constexpr accumulator(const state_t& state, size_t blocks) NOEXCEPT;

    /// Reset accumulator to initial state (to reuse after flushing).
    constexpr void reset() NOEXCEPT;

    /// Write data to accumulator.
    /// -----------------------------------------------------------------------

    template <size_t Size>
    bool write(const data_array<Size>& data) NOEXCEPT;
    bool write(const data_chunk& data) NOEXCEPT;
    bool write(const std::string& data) NOEXCEPT;
    bool write(size_t size, const byte_t* data) NOEXCEPT;

    /// Flush accumulator state to digest (not idempotent, not destructive).
    /// Flush does not reset the accumulator (writes may continue as in pbkd).
    /// -----------------------------------------------------------------------

    digest_t flush() NOEXCEPT;
    void flush(digest_t& digest) NOEXCEPT;
    void flush(data_chunk& digest) NOEXCEPT;
    void flush(byte_t* digest) NOEXCEPT;

    digest_t double_flush() NOEXCEPT;
    void double_flush(digest_t& digest) NOEXCEPT;
    void double_flush(data_chunk& digest) NOEXCEPT;
    void double_flush(byte_t* digest) NOEXCEPT;

    /// Finalized hashes.
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static digest_t hash(const data_array<Size>& data) NOEXCEPT;
    static digest_t hash(const data_chunk& data) NOEXCEPT;
    static digest_t hash(const std::string& data) NOEXCEPT;
    static digest_t hash(size_t size, const byte_t* data) NOEXCEPT;

    template <size_t Size>
    static data_chunk hash_chunk(const data_array<Size>& data) NOEXCEPT;
    static data_chunk hash_chunk(const data_chunk& data) NOEXCEPT;
    static data_chunk hash_chunk(const std::string& data) NOEXCEPT;
    static data_chunk hash_chunk(size_t size, const byte_t* data) NOEXCEPT;

    /// Finalized double hashes (sha256/sha512 only).
    /// -----------------------------------------------------------------------

    template <size_t Size>
    static digest_t double_hash(const data_array<Size>& data) NOEXCEPT;
    static digest_t double_hash(const data_chunk& data) NOEXCEPT;
    static digest_t double_hash(const std::string& data) NOEXCEPT;
    static digest_t double_hash(size_t size, const byte_t* data) NOEXCEPT;

    template <size_t Size>
    static data_chunk double_hash_chunk(const data_array<Size>& data) NOEXCEPT;
    static data_chunk double_hash_chunk(const data_chunk& data) NOEXCEPT;
    static data_chunk double_hash_chunk(const std::string& data) NOEXCEPT;
    static data_chunk double_hash_chunk(size_t size, const byte_t* data) NOEXCEPT;

protected:
    using half_t = typename Algorithm::half_t;
    using block_t = typename Algorithm::block_t;
    using count_t = typename Algorithm::count_t;
    using counter = data_array<Algorithm::count_bytes>;
    using self = accumulator<Algorithm>;

    static constexpr auto count_size = array_count<counter>;
    static constexpr auto digest_size = array_count<digest_t>;
    static constexpr auto block_size = array_count<block_t>;
    static constexpr auto half_size = array_count<half_t>;

    /// Position of next write in the buffer.
    INLINE constexpr size_t next() const NOEXCEPT;

    /// Bytes remaining until buffer is full.
    INLINE constexpr size_t gap() const NOEXCEPT;

    /// True if buffer is full (same as empty).
    INLINE constexpr bool is_full() const NOEXCEPT;

    /// True if buffer is empty (same as full).
    INLINE constexpr bool is_empty() const NOEXCEPT;

    /// Accumulator is limited to [max_size_t/8 - 8|16] hashed bytes.
    INLINE constexpr bool is_buffer_overflow(size_t size) const NOEXCEPT;

    /// Append up to block_size bytes to buffer.
    /// False on overflow, returns accepted count.
    INLINE constexpr bool add_data(size_t& accepted, size_t size,
        const byte_t* data) NOEXCEPT;

    /// Add remainder of data to cleared buffer.
    /// Size determined acceptable by first add_data call.
    INLINE constexpr void add_data(size_t size, const byte_t* data) NOEXCEPT;

    /// Accumulate a set of blocks and buffer any remainder bytes.
    INLINE bool accumulate(size_t size, const byte_t* data) NOEXCEPT;

    /// Create and accumulate pad block.
    INLINE state_t pad() NOEXCEPT;

    /// Serialize the hashed byte count for finalization
    INLINE static constexpr counter serialize(size_t size) NOEXCEPT;

    /// Compute pad size, reserve space for counter serialization.
    INLINE constexpr size_t pad_size() const NOEXCEPT;

    /// Precomputed streaming pad buffer.
    static CONSTEVAL block_t stream_pad() NOEXCEPT;

private:
    size_t size_;
    state_t state_;
    block_t buffer_{};
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/accumulator.ipp>

#endif
