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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_CHACHA20_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_CHACHA20_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>

namespace libbitcoin {
namespace system {

/// ChaCha20 stream cipher (rfc8439), 96-bit nonce and 32-bit block counter.
/// The nonce is expressed as a 32-bit and a 64-bit little-endian segment
/// (bip324 convention). Keystream is continuous across crypt/stream calls.
class BC_API chacha20
{
public:
    /// ChaCha20 secret is always 256 bits.
    static constexpr size_t secret_size = 32;
    typedef data_array<secret_size> secret;

    /// ChaCha20 block is always 512 bits.
    static constexpr size_t block_size = 64;
    typedef data_array<block_size> block;

    /// Initialize at nonce {0, 0}, block counter zero.
    chacha20(const secret& key) NOEXCEPT;

    /// Rekey, invalidating buffered keystream (seek to reposition).
    void set_key(const secret& key) NOEXCEPT;

    /// Set nonce and block counter, invalidating buffered keystream.
    void seek(uint32_t nonce32, uint64_t nonce64, uint32_t counter) NOEXCEPT;

    /// XOR in with keystream into out (in/out may be the same).
    void crypt(const_byte_span in, byte_span out) NOEXCEPT;

    /// Write keystream into out.
    void stream(byte_span out) NOEXCEPT;

protected:
    /// The state is sixteen words, over integral or extended integral words,
    /// the latter computing one block per 32 bit lane.
    static constexpr size_t state_words = 16;
    typedef std_array<uint32_t, 8> keys_t;
    typedef std_array<uint32_t, 3> nonce_t;

    template <typename Word>
    using state_t = std_array<Word, state_words>;

    /// The number of concurrent blocks computed by an extended word.
    template <typename xWord>
    static constexpr auto lanes = capacity<xWord, uint32_t>;

    /// The number of state vectors that comprise one transposed block.
    template <typename xWord>
    static constexpr auto parts = state_words / lanes<xWord>;

    /// The keystream bytes produced by one pass over an extended word.
    template <typename xWord>
    static constexpr auto xblock_size = block_size * lanes<xWord>;

    /// rounds
    /// -----------------------------------------------------------------------

    template <size_t A, size_t B, size_t C, size_t D, typename Word>
    static constexpr void quarter(state_t<Word>& x) NOEXCEPT;

    template <typename Word>
    static constexpr void double_round(state_t<Word>& x) NOEXCEPT;

    template <size_t Index, typename Word>
    static constexpr void summate(state_t<Word>& x,
        const state_t<Word>& start) NOEXCEPT;

    template <typename Word>
    static constexpr void rounds(state_t<Word>& x) NOEXCEPT;

    /// vectorization
    /// -----------------------------------------------------------------------

    template <typename xWord>
    static xWord counters(uint32_t counter) NOEXCEPT;

    template <typename xWord>
    using rows_t = std_array<xWord, lanes<xWord>>;

    template <size_t Index, typename xWord>
    static void interleave32(rows_t<xWord>& to,
        const rows_t<xWord>& from) NOEXCEPT;

    template <size_t Index, typename xWord>
    static void interleave64(rows_t<xWord>& to,
        const rows_t<xWord>& from) NOEXCEPT;

    template <size_t Index, size_t Span, typename xWord>
    static void exchange(rows_t<xWord>& to, const rows_t<xWord>& from) NOEXCEPT;

    template <size_t Lane, typename xWord>
    static void sequence(state_t<xWord>& keystream,
        const state_t<xWord>& x) NOEXCEPT;

    template <typename xWord>
    static void transpose(rows_t<xWord>& row) NOEXCEPT;

    template <typename xWord>
    static void transpose_groups(state_t<xWord>& x) NOEXCEPT;

    template <typename xWord>
    static void xnext(state_t<xWord>& keystream, const keys_t& key,
        const nonce_t& nonce, uint32_t counter) NOEXCEPT;

    template <typename xWord>
    static size_t xcrypt(const_byte_span in, byte_span out, size_t start,
        const keys_t& key, const nonce_t& nonce, uint32_t& counter) NOEXCEPT;

    void next(block& out) NOEXCEPT;

private:
    keys_t key_{};
    nonce_t nonce_{};
    uint32_t counter_{};
    block buffer_{};
    size_t offset_{ block_size };
};

} // namespace system
} // namespace libbitcoin

#endif
