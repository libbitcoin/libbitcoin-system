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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_SALSA20_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_SALSA20_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Salsa20 stream cipher (Bernstein), 64-bit nonce and 64-bit block counter.
/// Keystream is continuous across crypt/stream calls. XSalsa20 extends the
/// nonce to 192 bits by deriving a subkey with HSalsa20 (NaCl convention).
class BC_API salsa20
{
public:
    /// Salsa20 secret is always 256 bits.
    static constexpr size_t secret_size = 32;
    typedef data_array<secret_size> secret;

    /// Salsa20 block is always 512 bits.
    static constexpr size_t block_size = 64;
    typedef data_array<block_size> block;

    /// XSalsa20 nonce is 192 bits, the first 128 of which are HSalsa20 input.
    static constexpr size_t input_size = 16;
    static constexpr size_t extended_nonce_size = 24;
    typedef data_array<input_size> input;
    typedef data_array<extended_nonce_size> extended_nonce;

    /// Derive a subkey from key and input (HSalsa20 core).
    static void hsalsa20(secret& out, const secret& key,
        const input& in) NOEXCEPT;

    /// Initialize at nonce zero, block counter zero.
    salsa20(const secret& key) NOEXCEPT;

    /// Initialize from an extended nonce (XSalsa20), block counter zero.
    salsa20(const secret& key, const extended_nonce& nonce) NOEXCEPT;

    /// Rekey, invalidating buffered keystream (seek to reposition).
    void set_key(const secret& key) NOEXCEPT;

    /// Set nonce and block counter, invalidating buffered keystream.
    void seek(uint64_t nonce, uint64_t counter) NOEXCEPT;

    /// XOR in with keystream into out (in/out may be the same).
    void crypt(const_byte_span in, byte_span out) NOEXCEPT;

    /// Write keystream into out.
    void stream(byte_span out) NOEXCEPT;

protected:
    static constexpr size_t state_words = 16;
    typedef std_array<uint32_t, state_words> state_t;
    typedef std_array<uint32_t, 8> keys_t;
    typedef std_array<uint32_t, 4> words_t;

    template <size_t A, size_t B, size_t C, size_t D>
    static constexpr void quarter(state_t& x) NOEXCEPT;
    static constexpr void double_round(state_t& x) NOEXCEPT;
    static constexpr void rounds(state_t& x) NOEXCEPT;

    /// The state with the four words that vary by application at 6-9.
    static constexpr state_t initial(const keys_t& key,
        const words_t& words) NOEXCEPT;

    void next(block& out) NOEXCEPT;

private:
    keys_t key_{};
    words_t nonce_{};
    uint64_t counter_{};
    block buffer_{};
    size_t offset_{};
};

} // namespace system
} // namespace libbitcoin

#endif
