/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__WALLET_STEALTH_SENDER_HPP
#define LIBBITCOIN__WALLET_STEALTH_SENDER_HPP

//#include <cstdint>
//#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/wallet/stealth_sender.hpp>
//#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
//#include <bitcoin/bitcoin/utility/binary.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
//#include <bitcoin/bitcoin/wallet/payment_address.hpp>
//#include <bitcoin/bitcoin/wallet/stealth_address.hpp>
#include <chain_script.hpp>
#include <utility_data_chunk.hpp>
#include <wallet_stealth_address.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/// This class does not support multisignature stealth addresses.
class BC_API wallet_stealth_sender
{
public:
    /// Constructors.
    /// Generate a send address from the stealth address.
    wallet_stealth_sender(const wallet_stealth_address& address, const utility_data_chunk& seed,
        const utility_binary& filter, uint8_t version=wallet::payment_address::mainnet_p2kh);

    /// Generate a send address from the stealth address.
    wallet_stealth_sender(const math_ec_secret& ephemeral_private,
        const wallet_stealth_address& address, const utility_data_chunk& seed,
        const utility_binary& filter, uint8_t version=wallet::payment_address::mainnet_p2kh);

    /// Caller must test after construct.
//    operator bool() const;
    bool to_bool() const;

    /// Attach this script to the output before the send output.
    const chain_script& stealth_script() const;

    /// The bitcoin payment address to which the payment will be made.
    const wallet_payment_address& payment_address() const;

    wallet::stealth_sender getValue() {
		return value;
	}

	void setValue(wallet::stealth_sender value) {
		this->value = value;
	}
private:
	wallet::stealth_sender value;
//    void initialize(const math_ec_secret& ephemeral_private,
//        const wallet_stealth_address& address, const utility_data_chunk& seed,
//        const utility_binary& filter);
//
//    const uint8_t version_;
//    chain_script script_;
//    wallet_payment_address address_;
};

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif

