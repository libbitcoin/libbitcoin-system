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

// byte/array casting requires reinterpret_cast, so those are not constexpr.
// On a big-endian machine, no operations or copies are made by serializations.
// On a little-endian machine a set of integers (two or eight) is byte-swapped.
struct context
{
    using integral = uint32_t;
    static constexpr size_t counts = 2;
    static constexpr size_t counter_size = counts * sizeof(integral);
    using count = std::array<integral, counts>;

    /// Counters are hashed on finalize, identifying a unique data size.
    count counter;

    /// State maintains the current hash result.
    state state;

    /// The buffer is temporary storage for fractional blocks.
    /// It accumulates bytes until full and is then transformed.
    /// Full blocks (64 bytes) are never populated to the buffer.
    block buffer;

    /// Construction creates initial context.
    constexpr context() NOEXCEPT;

    /// Defaults.
    context(context&&) = default;
    context(const context&) = default;
    context& operator=(context&&) = default;
    context& operator=(const context&) = default;
    ~context() = default;

    /// Reset to initial context.
    constexpr void reset() NOEXCEPT;

    /// Bytes remaining in the buffer.
    constexpr size_t gap_size() const NOEXCEPT;

    /// Reserves space for counter serialization.
    constexpr size_t pad_size() const NOEXCEPT;

    /// Append up to block_size bytes to buffer and update counter.
    /// Counter resets to zero if size is sufficient to fill the buffer.
    /// Use gap_size prior to set_data to determine whether block will be full.
    constexpr void set_data(size_t size, const uint8_t* bytes) NOEXCEPT;

    /// Append up to block_size bytes to buffer, does not affect counter.
    constexpr void add_data(size_t size, const uint8_t* bytes) NOEXCEPT;

    /// Normalize counter for incorporation into the final hash.
    inline data_array<counter_size> serialize_counter() const NOEXCEPT;

    /// Normalize state as the final hash value.
    inline void serialize_state(hash& out) const NOEXCEPT;

protected:
    /// Update the context for the added data.
    constexpr void set_size(integral size) NOEXCEPT;

    /// Get the buffer byte size from context.
    constexpr size_t get_size() const NOEXCEPT;

private:
    static constexpr size_t over = 0;
    static constexpr size_t bits = 1;
    static constexpr size_t to_bits = floored_log2(byte_bits);
    static constexpr size_t to_over = subtract(bc::bits<integral>, to_bits);
};

/// Defined in sha256.cpp (declaring here avoids context& circularity).
/// Published for support of streaming, use with context{} and context.reset().
BC_API void update(context& context, size_t size, const uint8_t* in) NOEXCEPT;
BC_API void finalize(context& context, uint8_t* out) NOEXCEPT;

} // namespace sha256
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/crypto/sha256_context.ipp>

#endif
