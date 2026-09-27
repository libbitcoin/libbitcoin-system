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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_AES128_GCM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_AES128_GCM_HPP

#include <span>
#include <bitcoin/system/crypto/aes/ghash.hpp>
#include <bitcoin/system/crypto/algorithms.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// AES-128-GCM authenticated encryption with associated data (sp800-38d),
/// 96-bit nonce and 128-bit tag. The nonce MUST not be repeated for a key.
class BC_API aes128_gcm final
{
public:
    /// Ciphertext expansion (the appended tag).
    static constexpr size_t expansion = aes128::block_bytes;

    static constexpr size_t nonce_size = 12;
    typedef aes128::key_t secret;
    typedef data_array<nonce_size> nonce;

    aes128_gcm(const secret& key) NOEXCEPT;

    /// Rekey.
    void set_key(const secret& key) NOEXCEPT;

    /// Encrypt plain with aad into cipher (cipher = plain size + expansion).
    void encrypt(const_byte_span plain, const_byte_span aad,
        const nonce& iv, byte_span cipher) NOEXCEPT;

    /// Encrypt plain1 || plain2 with aad into cipher, avoiding the caller
    /// concatenation copy (cipher = sum of plain sizes + expansion).
    void encrypt(const_byte_span plain1, const_byte_span plain2,
        const_byte_span aad, const nonce& iv, byte_span cipher) NOEXCEPT;

    /// Decrypt cipher with aad into plain (cipher = plain size + expansion).
    /// False if the tag does not authenticate, in which case plain is cleared.
    bool decrypt(byte_span plain, const_byte_span aad, const nonce& iv,
        const_byte_span cipher) NOEXCEPT;

private:
    using block = aes128::block_t;

    static block counter(const nonce& iv, uint32_t value) NOEXCEPT;
    void authenticate(block& out, const_byte_span aad, const_byte_span cipher,
        const nonce& iv) const NOEXCEPT;

    aes128::schedule_t schedule_{};
    block key_{};
};

} // namespace system
} // namespace libbitcoin

#endif
