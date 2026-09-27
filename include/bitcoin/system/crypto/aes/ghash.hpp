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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES_GHASH_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES_GHASH_HPP

#include <span>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// Based on:
// NIST SP 800-38D [Recommendation for Block Cipher Modes of Operation:
// Galois/Counter Mode (GCM) and GMAC].
// nvlpubs.nist.gov/nistpubs/Legacy/SP/nistspecialpublication800-38d.pdf

namespace libbitcoin {
namespace system {
namespace aes {

/// GHASH universal hash, native (PCLMULQDQ or PMULL) or constant time
/// portable carryless multiplication.
template <bool Native = true>
class ghash
{
public:
    static constexpr auto block_bytes = 16_size;
    using block_t = std_array<uint8_t, block_bytes>;

    /// Initialize with the hash subkey (the cipher of the zero block).
    constexpr ghash(const block_t& key) NOEXCEPT;

    /// Absorb data, with any final partial block zero padded.
    constexpr void write(const_byte_span data) NOEXCEPT;

    /// The hash of data written.
    constexpr block_t flush() const NOEXCEPT;

protected:
    /// Blocks per reduction (native).
    static constexpr auto aggregate = 4_size;
    static constexpr auto use_clmul = Native && bc::have_aes && bc::have_128;

    /// A block as its big-endian high and low order words.
    using words_t = std_array<uint64_t, 2>;

    static constexpr words_t to_words(const block_t& block) NOEXCEPT;
    static constexpr block_t from_words(const words_t& words) NOEXCEPT;
    static constexpr block_t next(const_byte_span data, size_t start) NOEXCEPT;

    /// Portable.
    static constexpr uint64_t reverse(uint64_t value) NOEXCEPT;
    static constexpr uint64_t multiply(uint64_t x, uint64_t y) NOEXCEPT;
    static constexpr void multiply(words_t& y, const words_t& h) NOEXCEPT;
    constexpr void write_sliced(const_byte_span data) NOEXCEPT;

    /// Native.
    INLINE static xint128_t load(const block_t& block) NOEXCEPT;
    INLINE static block_t store(xint128_t value) NOEXCEPT;
    INLINE static xint128_t multiply(xint128_t a, xint128_t b) NOEXCEPT;
    void write_native(const_byte_span data) NOEXCEPT;

private:
    words_t key_;
    words_t hash_{};
};

} // namespace aes
} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <bool Native>
#define CLASS ghash<Native>

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_USE_OF_SPAN)

#include <bitcoin/system/impl/crypto/aes/ghash.ipp>

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

#undef CLASS
#undef TEMPLATE

#endif
