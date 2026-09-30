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
#ifndef LIBBITCOIN_SYSTEM_X509_PRIVATE_KEY_HPP
#define LIBBITCOIN_SYSTEM_X509_PRIVATE_KEY_HPP

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace x509 {

/// A P-256 private key.
using secret = secp256r1::secret_t;

/// Decode an ECPrivateKey (sec1, rfc5915).
BC_API bool decode_sec1(secret& out, const_byte_span der) NOEXCEPT;

/// Decode a PrivateKeyInfo (pkcs8, rfc5958).
BC_API bool decode_pkcs8(secret& out, const_byte_span der) NOEXCEPT;

/// Decode an EncryptedPrivateKeyInfo (pkcs8) of pbes2 with pbkdf2,
/// hmacWithSHA256 and aes-128-cbc or aes-256-cbc (rfc8018).
BC_API bool decode_encrypted_pkcs8(secret& out, const_byte_span der,
    const std::string& password) NOEXCEPT;

/// Decode the first "EC PRIVATE KEY", "PRIVATE KEY" or
/// "ENCRYPTED PRIVATE KEY" block of the text.
BC_API bool decode_private_key(secret& out, const std::string& text,
    const std::string& password={}) NOEXCEPT;

/// Encode as ECPrivateKey (with curve and public key), empty if invalid.
BC_API data_chunk encode_sec1(const secret& key) NOEXCEPT;

/// Encode as PrivateKeyInfo, empty if invalid.
BC_API data_chunk encode_pkcs8(const secret& key) NOEXCEPT;

/// Encode as a "PRIVATE KEY" block, empty if invalid.
BC_API std::string encode_private_key(const secret& key) NOEXCEPT;

} // namespace x509
} // namespace system
} // namespace libbitcoin

#endif
