/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/stealth_sender.hpp>

#include <cstdint>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/math/stealth.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/random.hpp>

namespace libbitcoin {
namespace wallet {

stealth_sender::stealth_sender(const stealth_address& address,
    uint8_t version)
  : version_(version)
{
    // BUGBUG: hardwired security RNG (generation of ephemeral private key).
    data_chunk seed(32);
    pseudo_random_fill(seed);

    // BUGBUG: error suppression.
    ec_secret ephemeral_private;
    DEBUG_ONLY(auto success =) create_ephemeral_key(ephemeral_private, seed);
    BITCOIN_ASSERT(success);

    initialize(address, ephemeral_private);
}

stealth_sender::stealth_sender(const stealth_address& address,
    const ec_secret& ephemeral_private, uint8_t version)
  : version_(version)
{
    initialize(address, ephemeral_private);
}

// private
void stealth_sender::initialize(const stealth_address& address,
    const ec_secret& ephemeral_private)
{
    // BUGBUG: error suppression.
    ec_compressed ephemeral_public;
    DEBUG_ONLY(auto success =) secret_to_public(ephemeral_public,
        ephemeral_private);
    BITCOIN_ASSERT(success);

    // Safe as spend_keys is guaranteed non-empty by stealth_address construct.
    const auto& spend_keys = address.spend_keys();
    BITCOIN_ASSERT(!spend_keys.empty());

    // BUGBUG: error suppression.
    // BUGBUG: constrained to using only the first spend key.
    ec_compressed sender_public;
    DEBUG_ONLY(success =) uncover_stealth(sender_public, address.scan_key(),
        ephemeral_private, spend_keys.front());
    BITCOIN_ASSERT(success);

    send_address_ = payment_address(sender_public, version_);

    // BUGBUG: no filter (must test all stealth outputs in blockchain).
    binary filter;

    // BUGBUG: hardwired privacy RNG (gneration of stealth script padding).
    data_chunk seed(32);
    pseudo_random_fill(seed);

    // BUGBUG: error suppression.
    DEBUG_ONLY(success =) create_stealth_script(stealth_script_,
        ephemeral_private, filter, seed);
    BITCOIN_ASSERT(success);
}

const chain::script& stealth_sender::stealth_script() const
{
    return stealth_script_;
}

const payment_address& stealth_sender::send_address() const
{
    return send_address_;
}

} // namespace wallet
} // namespace libbitcoin

