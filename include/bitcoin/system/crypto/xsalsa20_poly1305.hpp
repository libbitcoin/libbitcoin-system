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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_XSALSA20_POLY1305_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_XSALSA20_POLY1305_HPP

#include <bitcoin/system/crypto/poly1305.hpp>
#include <bitcoin/system/crypto/salsa20.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// NaCl secretbox (crypto_secretbox_xsalsa20poly1305): XSalsa20 encryption
/// authenticated by Poly1305 under a one-time key drawn from the keystream.
class BC_API xsalsa20_poly1305
{
public:
    /// Ciphertext expansion (the prepended tag).
    static constexpr size_t expansion = poly1305::tag_size;
    typedef salsa20::extended_nonce nonce;

    xsalsa20_poly1305(const salsa20::secret& key) NOEXCEPT;

    /// Rekey.
    void set_key(const salsa20::secret& key) NOEXCEPT;

    /// Encrypt plain into cipher (cipher = expansion + plain size).
    void encrypt(const_byte_span plain, const nonce& nonce,
        byte_span cipher) NOEXCEPT;

    /// Decrypt cipher into plain (cipher = expansion + plain size).
    /// False if the tag does not authenticate, in which case plain is cleared.
    bool decrypt(byte_span plain, const nonce& nonce,
        const_byte_span cipher) NOEXCEPT;

private:
    salsa20::secret key_;
};

} // namespace system
} // namespace libbitcoin

#endif
