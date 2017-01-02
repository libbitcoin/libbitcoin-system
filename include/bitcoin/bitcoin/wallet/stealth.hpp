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
#ifndef LIBBITCOIN_WALLET_STEALTH_HPP
#define LIBBITCOIN_WALLET_STEALTH_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/wallet/stealth_address.hpp>

namespace libbitcoin {
namespace wallet {

class stealth_receiver
{
public:
    /// Constructors.
    stealth_receiver(
        const ec_secret& scan_private, const ec_secret& spend_private,
        uint8_t version=payment_address::mainnet_p2kh);

    /// Step 1. Receiver creates a stealth address
    const stealth_address generate_stealth_address();

    /// Step 3. Receiver generates address and compares it to the
    /// one in the blockchain.
    const payment_address derive_address(const ec_compressed& ephemeral_public);

    /// Final step. Once address is confirmed, derive the private spend key.
    /// This can again be converted to a public key for double checking.
    const ec_secret derive_private(const ec_compressed& ephemeral_public);

private:
    const ec_secret& scan_private_;
    const ec_secret& spend_private_;
    const uint8_t version_;

    ec_compressed spend_public_;
};

class stealth_sender
{
public:
    /// Constructors.
    stealth_sender(uint8_t version=payment_address::mainnet_p2kh);

    /// Step 2. Sender generates a send address from the stealth address
    /// This version generates a random ephemeral_private.
    void send_to_stealth_address(const stealth_address& stealth_addr);
    /// Use a given ephemeral_private instead.
    void send_to_stealth_address(const stealth_address& stealth_addr,
        const ec_secret& ephemeral_private);

    /// Accessors.
    /// Attach this script to the output before the send output.
    const chain::script& meta_script() const;
    /// Send money to this address.
    const payment_address& send_address() const;

private:
    const uint8_t version_;
    chain::script meta_script_;
    payment_address send_address_;
};
    
} // namespace wallet
} // namespace libbitcoin

#endif

