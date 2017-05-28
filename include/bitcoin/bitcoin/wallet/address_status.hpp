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
#ifndef LIBBITCOIN_WALLET_ADDRESS_STATUS_HPP
#define LIBBITCOIN_WALLET_ADDRESS_STATUS_HPP

#include <string>

namespace libbitcoin {
namespace wallet {

enum class address_status_type
{
    mainnet_p2kh,
    mainnet_p2sh,
    testnet_p2kh,
    testnet_p2sh,
    stealth,
    invalid
};

/// Return the type of address.
address_status_type address_status(const std::string& address);

/// Whether the string is a mainnet Bitcoin address.
bool is_mainnet_address(const std::string& address);
/// Whether the string is a testnet Bitcoin address.
bool is_testnet_address(const std::string& address);
/// Whether the string is a stealth address.
bool is_stealth_address(const std::string& address);

/// Whether the string is a valid address (status is not invalid).
bool is_valid_address(const std::string& address);

} // namespace wallet
} // namespace libbitcoin

#endif

