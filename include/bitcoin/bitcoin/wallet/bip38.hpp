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

#ifdef WITH_ICU

/**
 * A bip38 seed for use with an intermediate.
 */
static BC_CONSTEXPR size_t seed_size = 24;
typedef byte_array<seed_size> seed;

/**
 * A checked intermediate (checked but not base58 encoded).
 */
static BC_CONSTEXPR size_t intermediate_encoded_size = 72;
static BC_CONSTEXPR size_t intermediate_decoded_size = 53;
typedef byte_array<intermediate_decoded_size> intermediate;

/**
 * A checked confirmation code (checked but not base58 encoded).
 */
static BC_CONSTEXPR size_t confirmation_code_encoded_size = 75;
static BC_CONSTEXPR size_t confirmation_code_decoded_size = 55;
typedef byte_array<confirmation_code_decoded_size> confirmation_code;

/**
 * A checked encrypted private key type (checked but not base58 encoded).
 */
static BC_CONSTEXPR size_t encrypted_key_encoded_size = 58;
static BC_CONSTEXPR size_t encrypted_key_decoded_size = 43;
typedef byte_array<encrypted_key_decoded_size> encrypted_private_key;

/**
 * Performs bip38 encryption based on the given intermediate and random 24 byte
 * seed provided. The confirmation code is an output parameter.
 */
BC_API data_chunk lock_intermediate(const intermediate& intermediate,
    const seed& seed, confirmation_code& out_confirmation,
    bool use_compression=true);

/**
 * Performs bip38 validation on the specified confirmation code using the
 * passphrase.  If a Bitcoin address depends on the passphrase, the address is
 * returned in out_address.
 */
BC_API bool lock_verify(const confirmation_code& confirmation,
    const std::string& passphrase, wallet::payment_address& out_address);

/**
 * Performs bip38 encryption on the private key given the specified passphrase.
 */
BC_API data_chunk lock_secret(const ec_secret& secret,
    const std::string& passphrase, bool use_compression=true);

/**
 * Performs bip38 decryption on the encrypted key given the specified
 * passphrase.
 */
BC_API ec_secret unlock_secret(const encrypted_private_key& encrypted_secret,
    const std::string& passphrase);

#endif

} // namespace bip38
} // namespace libbitcoin

#endif
