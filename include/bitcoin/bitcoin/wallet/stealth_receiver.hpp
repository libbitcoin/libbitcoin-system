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
#ifndef LIBBITCOIN_WALLET_STEALTH_RECEIVER_HPP
#define LIBBITCOIN_WALLET_STEALTH_RECEIVER_HPP

#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/wallet/stealth_address.hpp>

namespace libbitcoin {
namespace wallet {

/// Experimental.
class BC_API stealth_receiver
{
public:
    /// Constructors.
    stealth_receiver(const ec_secret& scan_private,
        const ec_secret& spend_private,
        uint8_t version=payment_address::mainnet_p2kh);

    /// Create a stealth address.
    stealth_address generate_stealth_address() const;

    /// Derive a payment address to compare against the blockchain.
    payment_address derive_address(
        const ec_compressed& ephemeral_public) const;

    /// Once address is discovered, derive the private spend key.
    ec_secret derive_private(const ec_compressed& ephemeral_public) const;

private:
    const ec_secret& scan_private_;
    const ec_secret& spend_private_;
    const uint8_t version_;
    ec_compressed spend_public_;
};
    
} // namespace wallet
} // namespace libbitcoin

#endif
