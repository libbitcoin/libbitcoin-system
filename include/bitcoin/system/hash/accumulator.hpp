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

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Accumulator for SHA/RMD/MD# streaming hash algorithms.
/// flush() is non-clearing (writes may continue).
template <typename Algorithm, bool Checked = checked_build>
struct accumulator
{
    using byte_t = typename Algorithm::byte_t;
    using state_t = typename Algorithm::state_t;
    using digest_t = typename Algorithm::digest_t;

    /// Sets initial state to Hash initialization vector.
    constexpr accumulator() NOEXCEPT;
    DEFAULT5(accumulator);

    /// Accepts an initial state and count of blocks it has accumulated.
    constexpr accumulator(size_t blocks, const state_t& state) NOEXCEPT;

    /// Write data to accumulator.
    inline bool write(const data_slice& data) NOEXCEPT;
    inline bool write(size_t size, const byte_t* data) NOEXCEPT;

    /// Flush accumulator state to digest.
    constexpr digest_t flush() NOEXCEPT;
    inline void flush(byte_t* digest) NOEXCEPT;

    /// Reset accumulator to initial state (to reuse after flushing).
    constexpr void reset() NOEXCEPT;

protected:
    using block_t = typename Algorithm::block_t;
    using counter = data_array<Algorithm::count_bytes>;

    /// Position of next write in the buffer.
    constexpr size_t next() const NOEXCEPT;

    /// Bytes remaining until buffer is full.
    constexpr size_t gap() const NOEXCEPT;

    /// Accumulator is limited to [max_size_t/8 - 8|16] hashed bytes.
    constexpr bool is_buffer_overflow(size_t bytes) const NOEXCEPT;

    /// Append up to block_size bytes to buffer.
    constexpr size_t add_data(size_t bytes, const byte_t* data) NOEXCEPT;

    /// Increment the counter for unbuffered transforms.
    constexpr void increment(size_t blocks) NOEXCEPT;

    /// Compute pad size, reserves space for counter serialization.
    constexpr size_t pad_size() const NOEXCEPT;

    /// Serialize the hashed byte count for finalization
    static RCONSTEXPR counter serialize(size_t bytes) NOEXCEPT;

    /// Precomputed streaming pad buffer.
    static CONSTEVAL block_t stream_pad() NOEXCEPT;

    /// Number of bytes in a block and in counter.
    static constexpr auto block_size = array_count<block_t>;
    static constexpr auto count_size = array_count<counter>;

private:
    size_t size_;
    state_t state_;
    block_t buffer_;
};

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/hash/accumulator.ipp>

#endif
