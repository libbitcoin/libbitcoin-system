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

namespace libbitcoin {
namespace system {

/// ChaCha20 stream cipher (rfc8439), 96-bit nonce and 32-bit block counter.
/// The nonce is expressed as a 32-bit and a 64-bit little-endian segment
/// (bip324 convention). Keystream is continuous across crypt/stream calls.
class BC_API chacha20 final
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

private:
    void next(block& out) NOEXCEPT;

    std_array<uint32_t, 8> key_{};
    std_array<uint32_t, 3> nonce_{};
    uint32_t counter_{};
    block buffer_{};
    size_t offset_{ block_size };
};

} // namespace system
} // namespace libbitcoin

#endif
