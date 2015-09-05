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
#ifndef LIBBITCOIN_BIP38_HPP
#define LIBBITCOIN_BIP38_HPP

#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/crypto.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace bip38 {

/**
 * A seed for use in creating an intermediate passphrase string.
 */
static BC_CONSTEXPR size_t salt_size = 4;
typedef byte_array<salt_size> salt;

/**
 * A seed for use in creating a key pair.
 */
static BC_CONSTEXPR size_t seed_size = 24;
typedef byte_array<seed_size> seed;

/**
 * An intermediate passphrase string type.
 */
static BC_CONSTEXPR size_t intermediate_encoded_size = 72;
static BC_CONSTEXPR size_t intermediate_decoded_size = 53;
typedef byte_array<intermediate_decoded_size> intermediate;

/**
 * An encrypted private key type (checked but not base58 encoded).
 */
static BC_CONSTEXPR size_t private_key_encoded_size = 58;
static BC_CONSTEXPR size_t private_key_decoded_size = 43;
typedef byte_array<private_key_decoded_size> private_key;

/**
 * An encrypted public key type (checked but not base58 encoded).
 * This is refered to as a confirmation code in bip38.
 */
static BC_CONSTEXPR size_t public_key_encoded_size = 75;
static BC_CONSTEXPR size_t public_key_decoded_size = 55;
typedef byte_array<public_key_decoded_size> public_key;

/**
 * Maximum values for use with create_intermediate.
 */
static BC_CONSTEXPR uint32_t max_intermediate_lot = 1048575;
static BC_CONSTEXPR uint32_t max_intermediate_sequence = 4095;

/**
 * Create an intermediate passphrase string for key pair generation.
 * @param[in]  lot         A lot, max allowed value 1048575 (2^20-1).
 * @param[in]  sequence    A sequence, max allowed value 4095 (2^12-1).
 * @param[in]  passphrase  A passphrase for use in the encryption.
 * @param[in]  salt        A random value.
 * @param[out] out_code    The new intermediate passphrase string.
 * @return true if the intermediate is created.
 */
BC_API bool create_intermediate(uint32_t lot, uint32_t sequence,
    const std::string& passphrase, const salt& salt,
    intermediate& out_code);

/**
 * Create an encrypted key pair from an intermediate passphrase string.
 * @param[in]  code         An intermediate passphrase string.
 * @param[in]  seed         A random value.
 * @param[out] out_private  The new encrypted private key.
 * @param[out] out_public   The new encrypted public key.
 * @param[in]  version      The coin address version byte.
 * @param[in]  compressed   Set true to associate ec public key compression.
 * @return true if the keys are created.
 */
BC_API bool create_key_pair(const intermediate& code, const seed& seed,
    private_key& out_private, public_key& out_public, uint8_t version,
    bool compressed=true);

#ifdef WITH_ICU

/**
 * Encrypt the ec secret to an encrypted public key using the passphrase.
 * @param[in]  secret       An ec secret to encrypt.
 * @param[in]  passphrase   A passphrase for use in the encryption.
 * @param[out] out_private  The new encrypted private key.
 * @param[in]  version      The coin address version byte.
 * @param[in]  compressed   Set true to associate ec public key compression.
 * @return true if the encrypted private key is populated.
 */
BC_API bool encrypt(const ec_secret& secret, const std::string& passphrase,
    private_key& out_private, uint8_t version, bool compressed=true);

/**
 * Decrypt the ec secret associated with the encrypted private key.
 * @param[in]  key         An encrypted private key.
 * @param[in]  passphrase  A passphrase for use in the decryption.
 * @param[out] out_secret  The decrypted ec secret.
 * @param[in]  version     The coin address version byte.
 * @return true if the encrypted private key is populated.
 */
BC_API bool decrypt(const private_key& key, const std::string& passphrase,
    ec_secret& out_secret);

/**
 * Decrypt the ec point associated with the encrypted public key.
 * @param[in]  key         An encrypted public key.
 * @param[in]  passphrase  A passphrase for use in the decryption.
 * @param[out] out_point   The decrypted ec point.
 * @return true if the public key is populated.
 */
BC_API bool decrypt(const public_key& key, const std::string& passphrase,
    ec_point& out_point);

#endif // WITH_ICU

} // namespace bip38
} // namespace libbitcoin

#endif
