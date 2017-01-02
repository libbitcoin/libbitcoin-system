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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/wallet/stealth.hpp>

#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/math/stealth.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>

namespace libbitcoin {
namespace wallet {

using namespace libbitcoin::machine;

// Stealth receiver
// ----------------------------------------------------------------------------

stealth_receiver::stealth_receiver(
    const ec_secret& scan_private, const ec_secret& spend_private,
    uint8_t version)
  : scan_private_(scan_private), spend_private_(spend_private),
    version_(version)
{
    DEBUG_ONLY(bool success =) secret_to_public(spend_public_, spend_private_);
    BITCOIN_ASSERT(success);
}

const stealth_address stealth_receiver::generate_stealth_address()
{
    ec_compressed scan_public;
    DEBUG_ONLY(bool success =) secret_to_public(scan_public, scan_private_);
    BITCOIN_ASSERT(success);

    const binary empty;

    const point_list spend_keys = { spend_public_ };

    return stealth_address(empty, scan_public, spend_keys);
}

const payment_address stealth_receiver::derive_address(
    const ec_compressed& ephemeral_public)
{
    ec_compressed receiver_public;
    DEBUG_ONLY(bool success =) uncover_stealth(
        receiver_public, ephemeral_public, scan_private_, spend_public_);
    BITCOIN_ASSERT(success);

    return payment_address(receiver_public, version_);
}

const ec_secret stealth_receiver::derive_private(
    const ec_compressed& ephemeral_public)
{
    ec_secret receiver_private;
    DEBUG_ONLY(bool success =) uncover_stealth(
        receiver_private, ephemeral_public, scan_private_, spend_private_);
    BITCOIN_ASSERT(success);
    return receiver_private;
}

// Stealth sender
// ----------------------------------------------------------------------------

stealth_sender::stealth_sender(uint8_t version)
  : version_(version)
{
}

void stealth_sender::send_to_stealth_address(
    const stealth_address& stealth_addr)
{
    ec_secret ephemeral_private;
    data_chunk seed(12);
    pseudo_random_fill(seed);
    DEBUG_ONLY(bool success =) create_ephemeral_key(ephemeral_private, seed);
    BITCOIN_ASSERT(success);
    send_to_stealth_address(stealth_addr, ephemeral_private);
}
void stealth_sender::send_to_stealth_address(const stealth_address& stealth_addr,
    const ec_secret& ephemeral_private)
{
    ec_compressed ephemeral_public;
    DEBUG_ONLY(bool success =) secret_to_public(
        ephemeral_public, ephemeral_private);
    BITCOIN_ASSERT(success);

    const auto& spend_keys = stealth_addr.spend_keys();
    BITCOIN_ASSERT(!spend_keys.empty());
    // Sender derives stealth public, requiring ephemeral private.
    ec_compressed sender_public;
    uncover_stealth(
        sender_public, stealth_addr.scan_key(),
        ephemeral_private, spend_keys[0]);

    send_address_ = payment_address(sender_public, version_);

    data_chunk output_data(
        ephemeral_public.begin() + 1, ephemeral_public.end());
    BITCOIN_ASSERT(output_data.size() == 32);
    data_chunk random_padding(4);
    pseudo_random_fill(random_padding);
    extend_data(output_data, random_padding);
    BITCOIN_ASSERT(output_data.size() == 40);

    meta_script_.from_operations({
        operation(opcode::return_),
        operation(output_data)
    });
}

const chain::script& stealth_sender::meta_script() const
{
    return meta_script_;
}
const payment_address& stealth_sender::send_address() const
{
    return send_address_;
}

} // namespace wallet
} // namespace libbitcoin

