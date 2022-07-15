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

    /// Reset to initial context (for another hash round).
    constexpr void reset() NOEXCEPT;

    /// Clear buffer after transforming state.
    constexpr void clear() NOEXCEPT;

    /// Increment the counter for unbuffered transforms.
    constexpr bool increment(size_t blocks) NOEXCEPT;

    /// Transform if buffer is full.
    constexpr bool is_full() NOEXCEPT;

    /// Reserves space for counter serialization.
    constexpr size_t pad_size() const NOEXCEPT;

    /// Append up to block_size bytes to buffer (fills gap if possible).
    constexpr size_t add_data(size_t bytes, const uint8_t* data) NOEXCEPT;

    /// State reference for transformation.
    constexpr state& state() NOEXCEPT;

    /// Buffer const reference for transformation.
    inline const block& buffer() const NOEXCEPT;

    /// Serialize the hashed bit count for finalization
    inline counter serialize_counter() const NOEXCEPT;

    /// Normalize state as the final hash value.
    inline void serialize_state(digest& out) const NOEXCEPT;

protected:
    /// Bytes remaining until buffer is full.
    constexpr size_t gap() const NOEXCEPT;

    /// SHA256 is limited to max_uint64 - to_bits(counter_size) hashed bits.
    constexpr bool is_buffer_overflow(size_t bytes) NOEXCEPT;

private:
    block buffer_;
    size_t size_{};
    uint64_t bits_{};
    sha256::state state_{ sha256::initial };
};

// TODO: move these functions into class as sha256_accumulator.

/// Construct a default context and use this to iterate over data.
inline bool update(context& context, size_t size, const uint8_t* in) NOEXCEPT;

/// Finalize after last call to update, out32 is hash result.
inline void finalize(context& context, uint8_t* out32) NOEXCEPT;

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/crypto/sha256_context.ipp>

#endif
