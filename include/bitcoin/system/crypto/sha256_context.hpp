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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SHA256_CONTEXT_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SHA256_CONTEXT_HPP

#include <array>
#include <bitcoin/system/crypto/sha256.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace sha256 {

/// Streaming hash context.
struct context
{
    static constexpr auto counter_size = sizeof(uint64_t);
    using counter = data_array<counter_size>;

    /// Creates initial context.
    constexpr context() NOEXCEPT;

    /// Defaults.
    context(context&&) = default;
    context(const context&) = default;
    context& operator=(context&&) = default;
    context& operator=(const context&) = default;
    ~context() = default;

    /// Write data to the context.
    inline bool write(size_t size, const uint8_t* in) NOEXCEPT;

    /// Flush context to out32 (must be at least 32 bytes), resets context.
    inline void flush(uint8_t* out32) NOEXCEPT;

private:
    // Clear buffer after transforming state.
    constexpr void clear() NOEXCEPT;

    // Reset to initial context after flushing.
    constexpr void reset() NOEXCEPT;

    // Bytes remaining until buffer is full.
    constexpr size_t gap() const NOEXCEPT;

    // Transform if buffer is full.
    constexpr bool is_full() NOEXCEPT;

    // SHA256 is limited to max_uint64 - to_bits(counter_size) hashed bits.
    constexpr bool is_buffer_overflow(size_t bytes) NOEXCEPT;

    // Reserves space for counter serialization.
    constexpr size_t pad_size() const NOEXCEPT;

    // Append up to block_size bytes to buffer (fills gap if possible).
    constexpr size_t add_data(size_t bytes, const uint8_t* data) NOEXCEPT;

    // Increment the counter for unbuffered transforms.
    constexpr void increment(size_t blocks) NOEXCEPT;

    // Serialize the hashed bit count for finalization
    inline counter serialize_counter() const NOEXCEPT;

    // Normalize state as the final hash value.
    inline void serialize_state(digest& out) const NOEXCEPT;

    block buffer_;
    size_t size_{};
    uint64_t bits_{};
    sha256::state state_{ sha256::initial };
};

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/crypto/sha256_context.ipp>

#endif
