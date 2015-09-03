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
 * A bip38 seed for use with an intermediate.
 */
static BC_CONSTEXPR size_t seed_size = 24;
typedef byte_array<seed_size> seed;

/**
 * A bip38 intermediate passphrase string type.
 */
////static BC_CONSTEXPR size_t intermediate_encoded_size = 72;
static BC_CONSTEXPR size_t intermediate_decoded_size = 53;
typedef byte_array<intermediate_decoded_size> intermediate;

/**
 * A checked confirmation code (checked but not base58 encoded).
 */
////static BC_CONSTEXPR size_t confirmation_code_encoded_size = 75;
static BC_CONSTEXPR size_t confirmation_code_decoded_size = 55;
typedef byte_array<confirmation_code_decoded_size> confirmation_code;

/**
 * An encrypted private key type (checked but not base58 encoded).
 */
////static BC_CONSTEXPR size_t private_key_encoded_size = 58;
static BC_CONSTEXPR size_t private_key_decoded_size = 43;
typedef byte_array<private_key_decoded_size> private_key;

/**
 * Create an intermediate encrypted private key.
 */
BC_API data_chunk create_intermediate(const intermediate& intermediate,
    const seed& seed, confirmation_code& out_confirmation,
    bool use_compression=true);

#ifdef WITH_ICU

/**
 * Extract the payment address associated with passphrase and confirmation.
 * @return true if there is an assocaited address.
 */
BC_API bool extract_address(const confirmation_code& confirmation,
    const std::string& passphrase, wallet::payment_address& out_address);

/**
 * Encrypt the private key given the specified passphrase.
 * The compression option controls the public key representation.
 */
BC_API private_key lock_secret(const ec_secret& secret,
    const std::string& passphrase, bool use_compression=true);

/**
 * Decrypt the encrypted private key using the passphrase.
 */
BC_API ec_secret unlock_secret(const private_key& encrypted_secret,
    const std::string& passphrase);

#endif // WITH_ICU

} // namespace bip38
} // namespace libbitcoin

#endif
