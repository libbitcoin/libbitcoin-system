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
#include <bitcoin/bitcoin/wallet/stealth_receiver.hpp>

#include <cstdint>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/math/stealth.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {
namespace wallet {

// BUGBUG: this assumes a single spender.
stealth_receiver::stealth_receiver(const ec_secret& scan_private,
    const ec_secret& spend_private, uint8_t version)
  : scan_private_(scan_private), spend_private_(spend_private),
    version_(version)
{
    // BUGBUG: error suppression.
    DEBUG_ONLY(auto success =) secret_to_public(spend_public_, spend_private_);
    BITCOIN_ASSERT(success);
}

stealth_address stealth_receiver::stealth_address() const
{
    // BUGBUG: error suppression.
    ec_compressed scan_public;
    DEBUG_ONLY(auto success =) secret_to_public(scan_public, scan_private_);
    BITCOIN_ASSERT(success);

    // BUGBUG: constrained to using only the first spend key.
    // BUGBUG: no filter (must test all stealth outputs in blockchain).
    return{ {}, scan_public, { spend_public_ } };
}

payment_address stealth_receiver::derive_address(
    const ec_compressed& ephemeral_public) const
{
    // BUGBUG: error suppression.
    ec_compressed receiver_public;
    DEBUG_ONLY(auto success =) uncover_stealth(receiver_public,
        ephemeral_public, scan_private_, spend_public_);
    BITCOIN_ASSERT(success);
    return{ receiver_public, version_ };
}

ec_secret stealth_receiver::derive_private(
    const ec_compressed& ephemeral_public) const
{
    // BUGBUG: error suppression.
    ec_secret receiver_private;
    DEBUG_ONLY(auto success =) uncover_stealth(receiver_private,
        ephemeral_public, scan_private_, spend_private_);
    BITCOIN_ASSERT(success);
    return receiver_private;
}

} // namespace wallet
} // namespace libbitcoin
