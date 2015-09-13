/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_ENCRYPTED_KEYS_HPP
#define LIBBITCOIN_ENCRYPTED_KEYS_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/crypto.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace wallet {

/**
 * A seed for use in creating an intermediate passphrase (token).
 */
static BC_CONSTEXPR size_t salt_size = 4;
typedef byte_array<salt_size> salt;

/**
 * A seed for use in creating an intermediate passphrase (token).
 */
static BC_CONSTEXPR size_t entropy_size = 8;
typedef byte_array<entropy_size> entropy;

/**
 * A seed for use in creating a key pair.
 */
static BC_CONSTEXPR size_t seed_size = 24;
typedef byte_array<seed_size> seed;

/**
 * An intermediate passphrase (token) type (checked but not base58 encoded).
 */
static BC_CONSTEXPR size_t token_encoded_size = 72;
static BC_CONSTEXPR size_t token_decoded_size = 53;
typedef byte_array<token_decoded_size> token;

/**
 * An encrypted private key type (checked but not base58 encoded).
 */
static BC_CONSTEXPR size_t private_key_encoded_size = 58;
static BC_CONSTEXPR size_t private_key_decoded_size = 43;
typedef byte_array<private_key_decoded_size> private_key;

/**
 * DEPRECATED
 * An encrypted public key type (checked but not base58 encoded).
 * This is refered to as a confirmation code in bip38.
 */
static BC_CONSTEXPR size_t public_key_encoded_size = 75;
static BC_CONSTEXPR size_t public_key_decoded_size = 55;
typedef byte_array<public_key_decoded_size> public_key;

/**
 * Maximum values for use with create_token.
 */
static BC_CONSTEXPR uint32_t max_token_lot = 1048575;
static BC_CONSTEXPR uint32_t max_token_sequence = 4095;

/**
 * Create an intermediate passphrase for subsequent key pair generation.
 * @param[out] out_token   The new intermediate passphrase.
 * @param[in]  passphrase  A passphrase for use in the encryption.
 * @param[in]  entropy     A random value for use in the encryption.
 */
BC_API void create_token(token& out_token, const std::string& passphrase,
    const entropy& entropy);

/**
 * Create an intermediate passphrase for subsequent key pair generation.
 * @param[out] out_token   The new intermediate passphrase.
 * @param[in]  passphrase  A passphrase for use in the encryption.
 * @param[in]  salt        A random value for use in the encryption.
 * @param[in]  lot         A lot, max allowed value 1048575 (2^20-1).
 * @param[in]  sequence    A sequence, max allowed value 4095 (2^12-1).
 * @return false if the lot and/or sequence are out of range.
 */
BC_API bool create_token(token& out_token, const std::string& passphrase,
    const salt& salt, uint32_t lot, uint32_t sequence);

/**
 * Create an encrypted private key from an intermediate passphrase.
 * @param[out] out_private  The new encrypted private key.
 * @param[out] out_point    The ec public key of the new key pair.
 * @param[in]  token        An intermediate passphrase string.
 * @param[in]  seed         A random value for use in the encryption.
 * @param[in]  version      The coin address version byte.
 * @param[in]  compressed   Set true to associate ec public key compression.
 * @return false if the token checksum is not valid.
 */
BC_API bool create_key_pair(private_key& out_private, ec_point& out_point,
    const token& token, const seed& seed, uint8_t version,
    bool compressed=true);

/**
 * DEPRECATED
 * Create an encrypted key pair from an intermediate passphrase.
 * @param[out] out_private  The new encrypted private key.
 * @param[out] out_public   The new encrypted public key.
 * @param[out] out_point    The ec public key of the new key pair.
 * @param[in]  token        An intermediate passphrase string.
 * @param[in]  seed         A random value for use in the encryption.
 * @param[in]  version      The coin address version byte.
 * @param[in]  compressed   Set true to associate ec public key compression.
 * @return false if the token checksum is not valid.
 */
BC_API bool create_key_pair(private_key& out_private, public_key& out_public,
    ec_point& out_point, const token& token, const seed& seed, uint8_t version,
    bool compressed=true);

#ifdef WITH_ICU

/**
 * Encrypt the ec secret to an encrypted public key using the passphrase.
 * @param[out] out_private  The new encrypted private key.
 * @param[in]  secret       An ec secret to encrypt.
 * @param[in]  passphrase   A passphrase for use in the encryption.
 * @param[in]  version      The coin address version byte.
 * @param[in]  compressed   Set true to associate ec public key compression.
 */
BC_API void encrypt(private_key& out_private, const ec_secret& secret,
    const std::string& passphrase, uint8_t version, bool compressed=true);

/**
 * Decrypt the ec secret associated with the encrypted private key.
 * @param[out] out_secret      The decrypted ec secret.
 * @param[out] out_version     The coin address version.
 * @param[out] out_compressed  The compression of the associated ec public key.
 * @param[in]  key             An encrypted private key.
 * @param[in]  passphrase      The passphrase from the encryption or token.
 * @return false if the key checksum or passphrase is not valid.
 */
BC_API bool decrypt(ec_secret& out_secret, uint8_t& out_version,
    bool& out_compressed, const private_key& key,
    const std::string& passphrase);

/**
 * DEPRECATED
 * Decrypt the ec point associated with the encrypted public key.
 * @param[out] out_point    The decrypted ec point.
 * @param[out] out_version  The coin address version of the public key.
 * @param[in]  key          An encrypted public key.
 * @param[in]  passphrase   The passphrase of the associated token.
 * @return false if the key checksum or passphrase is not valid.
 */
BC_API bool decrypt(ec_point& out_point, uint8_t& out_version,
    const public_key& key, const std::string& passphrase);

#endif // WITH_ICU

} // namespace wallet
} // namespace libbitcoin

#endif
