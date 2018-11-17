/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_MESSAGE_HPP
#define LIBBITCOIN_SYSTEM_WALLET_MESSAGE_HPP

#include <string>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>

namespace libbitcoin {
namespace wallet {

/**
 * A message signature is an EC signature with one prefix byte.
 */
static BC_CONSTEXPR size_t message_signature_size = 1 + ec_signature_size;
typedef byte_array<message_signature_size> message_signature;

/**
 * Hashes a messages in preparation for signing.
 */
BC_API hash_digest hash_message(data_slice message);

/**
 * Signs a message using deterministic signature.
 * @param[in] out_signature The signature in Bitcoin's own format.
 * This should be base64 encoded for presentation to the user.
 * @return true if wif is valid and signature encoding is successful.
 */
BC_API bool sign_message(message_signature& signature, data_slice message,
    const ec_private& secret);

/**
 * Signs a message using deterministic signature.
 * @param[in] out_signature The signature in Bitcoin's own format.
 * This should be base64 encoded for presentation to the user.
 * @return true if wif is valid and signature encoding is successful.
 */
BC_API bool sign_message(message_signature& out_signature, data_slice message,
    const std::string& wif);

/**
 * Signs a message using deterministic signature.
 * @param[in] out_signature The signature in Bitcoin's own format.
 * This should be base64 encoded for presentation to the user.
 * @param[in] compressed true if the bitcoin address derived from the
 * private key is in compressed format.
 * @return true if signature encoding is successful.
 */
BC_API bool sign_message(message_signature& out_signature, data_slice message,
    const ec_secret& secret, bool compressed=true);

/**
 * Verifies a message.
 * @param signature a message signature in Bitcoin's own format.
 * The user will generally provide this as a base64 string,
 * which the user interface must decode.
 * @return false if the signature does not match the address or if there are
 * any errors in the signature encoding.
 */
BC_API bool verify_message(data_slice message, const payment_address& address,
    const message_signature& signature);

/// Exposed primarily for independent testability.
BC_API bool recovery_id_to_magic(uint8_t& out_magic, uint8_t recovery_id,
    bool compressed);

/// Exposed primarily for independent testability.
BC_API bool magic_to_recovery_id(uint8_t& out_recovery_id, bool& out_compressed,
    uint8_t magic);

} // namespace wallet
} // namespace libbitcoin

#endif
