/**
/// Copyright (c) 2011-2026 libbitcoin developers
 *
/// This file is part of libbitcoin.
 *
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
 *
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
 *
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_WALLET_KEYS_ENCRYPTED_KEYS_HPP
#define LIBBITCOIN_SYSTEM_WALLET_KEYS_ENCRYPTED_KEYS_HPP

#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/addresses/payment_address.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// The maximum lot and sequence values for encrypted key token creation.
static constexpr uint32_t ek_max_lot = 1048575;
static constexpr uint32_t ek_max_sequence = 4095;

/// A seed for use in creating an intermediate passphrase (token).
static constexpr size_t ek_salt_size = 4;
typedef data_array<ek_salt_size> ek_salt;

/// A seed for use in creating an intermediate passphrase (token).
static constexpr size_t ek_entropy_size = 8;
typedef data_array<ek_entropy_size> ek_entropy;

/// A seed for use in creating a key pair.
static constexpr size_t ek_seed_size = 24;
typedef data_array<ek_seed_size> ek_seed;

/// An intermediate passphrase (token) type (checked but not base58 encoded).
static constexpr size_t encrypted_token_encoded_size = 72;
static constexpr size_t encrypted_token_decoded_size = 53;
typedef data_array<encrypted_token_decoded_size> encrypted_token;

/// An encrypted private key type (checked but not base58 encoded).
static constexpr size_t ek_private_encoded_size = 58;
static constexpr size_t ek_private_decoded_size = 43;
typedef data_array<ek_private_decoded_size> encrypted_private;

/// DEPRECATED
/// Encrypted public key type (checked but not base58 encoded).
/// Referred to as confirmation code in bip38.
static constexpr size_t encrypted_public_encoded_size = 75;
static constexpr size_t encrypted_public_decoded_size = 55;
typedef data_array<encrypted_public_decoded_size> encrypted_public;

/// BIP38
/// It is requested that the unused flag bytes NOT be used for denoting that
/// the key belongs to an alt-chain [This shoud read "flag bits"].
enum ek_flag : uint8_t
{
    lot_sequence_key = 1 << 2,
    ec_compressed_key = 1 << 5,
    ec_non_multiplied_low = 1 << 6,
    ec_non_multiplied_high = 1 << 7,

    /// Two bits are used to represent "not multiplied".
    ec_non_multiplied = (ec_non_multiplied_low | ec_non_multiplied_high)
};


/// Create encrypted private key from intermediate passphrase.
/// `out_point` is always compressed, decompress as necessary to match state of
/// `compressed` parameter.
/// @param[out] out_private  New encrypted private key.
/// @param[out] out_point    ec compressed public key of new key pair.
/// @param[in]  token        Intermediate passphrase string.
/// @param[in]  seed         Random value for use in encryption.
/// @param[in]  version      Coin address version byte.
/// @param[in]  compressed   Set true to associate ec public key compression.
/// @return false if token checksum is not valid.
BC_API bool create_key_pair(encrypted_private& out_private,
    ec_compressed& out_point, const encrypted_token& token,
    const ek_seed& seed, uint8_t version, bool compressed=true) NOEXCEPT;

/// DEPRECATED (scenario)
/// Create encrypted key pair from intermediate passphrase.
/// `out_point` is always compressed, decompress as necessary to match state of
/// `compressed` parameter.
/// @param[out] out_private  New encrypted private key.
/// @param[out] out_public   New encrypted public key.
/// @param[out] out_point    Compressed ec public key of new key pair.
/// @param[in]  token        Intermediate passphrase string.
/// @param[in]  seed         Random value for use in encryption.
/// @param[in]  version      Coin address version byte.
/// @param[in]  compressed   Set true to associate ec public key compression.
/// @return false if token checksum is not valid.
BC_API bool create_key_pair(encrypted_private& out_private,
    encrypted_public& out_public, ec_compressed& out_point,
    const encrypted_token& token, const ek_seed& seed, uint8_t version,
    bool compressed=true) NOEXCEPT;

/// Create intermediate passphrase for subsequent key pair generation.
/// @param[out] out_token   New intermediate passphrase.
/// @param[in]  passphrase  Passphrase for use in encryption.
/// @param[in]  entropy     Random value for use in encryption.
/// @return false if token could not be created from entropy.
BC_API bool create_token(encrypted_token& out_token,
    const std::string& passphrase, const ek_entropy& entropy) NOEXCEPT;

/// Create intermediate passphrase for subsequent key pair generation.
/// @param[out] out_token   New intermediate passphrase.
/// @param[in]  passphrase  Passphrase for use in encryption.
/// @param[in]  salt        Random value for use in encryption.
/// @param[in]  lot         Lot, max allowed value 1048575 (2^20-1).
/// @param[in]  sequence    Sequence, max allowed value 4095 (2^12-1).
/// @return false if lot and/or sequence are out of range or
/// token could not be created from entropy.
BC_API bool create_token(encrypted_token& out_token,
    const std::string& passphrase, const ek_salt& salt, uint32_t lot,
    uint32_t sequence) NOEXCEPT;

/// Encrypt ec secret to encrypted public key using passphrase.
/// @param[out] out_private  New encrypted private key.
/// @param[in]  secret       ec secret to encrypt.
/// @param[in]  passphrase   Passphrase for use in encryption.
/// @param[in]  version      Coin address version byte.
/// @param[in]  compressed   Set true to associate ec public key compression.
/// @return false if secret could not be converted to public key.
BC_API bool encrypt(encrypted_private& out_private, const ec_secret& secret,
    const std::string& passphrase, uint8_t version,
    bool compressed=true) NOEXCEPT;

/// Decrypt ec secret associated with encrypted private key.
/// @param[out] out_secret      Decrypted ec secret.
/// @param[out] out_version     Coin address version.
/// @param[out] out_compressed  Compression of associated ec public key.
/// @param[in]  key             Encrypted private key.
/// @param[in]  passphrase      Passphrase from encryption or token.
/// @return false if key checksum or passphrase is not valid.
BC_API bool decrypt(ec_secret& out_secret, uint8_t& out_version,
    bool& out_compressed, const encrypted_private& key,
    const std::string& passphrase) NOEXCEPT;

/// DEPRECATED (scenario)
/// Decrypt ec point associated with encrypted public key.
/// @param[out] out_point       Decrypted ec compressed point.
/// @param[out] out_version     Coin address version of public key.
/// @param[out] out_compressed  Public key specified compression state.
/// @param[in]  key             Encrypted public key.
/// @param[in]  passphrase      Passphrase of associated token.
/// @return false if key checksum or passphrase is not valid.
BC_API bool decrypt(ec_compressed& out_point, uint8_t& out_version,
    bool& out_compressed, const encrypted_public& key,
    const std::string& passphrase) NOEXCEPT;

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
