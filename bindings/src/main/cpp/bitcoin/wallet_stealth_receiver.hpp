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
#ifndef LIBBITCOIN__WALLET_STEALTH_RECEIVER_HPP
#define LIBBITCOIN__WALLET_STEALTH_RECEIVER_HPP

//#include <cstdint>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/wallet/stealth_receiver.hpp>
//#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
//#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/bitcoin/wallet/payment_address.hpp>
//#include <bitcoin/bitcoin/wallet/stealth_address.hpp>
#include <utility_binary.hpp>
#include <math_ec_secret.hpp>
#include <wallet_payment_address.hpp>
#include <wallet_stealth_address.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/// This class does not support multisignature stealth addresses.
class BC_API wallet_stealth_receiver
{
public:
    /// Constructors.
    wallet_stealth_receiver(const math_ec_secret& scan_private,
        const math_ec_secret& spend_private, const utility_binary& filter,
        uint8_t version=wallet::payment_address::mainnet_p2kh);

    /// Caller must test after construct.
//    operator bool() const;
    bool to_bool() const;

    /// Get the stealth address.
    const wallet_stealth_address& stealth_address() const;

    /// Derive a payment address to compare against the blockchain.
    bool derive_address(wallet_payment_address& out_address,
        const math_ec_compressed& ephemeral_public) const;

    /// Once address is discovered, derive the private spend key.
    bool derive_private(math_ec_secret& out_private,
        const math_ec_compressed& ephemeral_public) const;

    wallet::stealth_receiver getValue() {
		return value;
	}

	void setValue(wallet::stealth_receiver value) {
		this->value = value;
	}
private:
	wallet::stealth_receiver value;
//    const uint8_t version_;
//    const ec_secret scan_private_;
//    const ec_secret spend_private_;
//    ec_compressed spend_public_;
//    wallet::stealth_address address_;
};

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif
