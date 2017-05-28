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
#include <bitcoin/bitcoin/wallet/address_status.hpp>

#include <bitcoin/bitcoin/wallet/payment_address.hpp>
#include <bitcoin/bitcoin/wallet/stealth_address.hpp>

namespace libbitcoin {
namespace wallet {

address_status_type payment_address_type(const uint8_t version)
{
    if (version == payment_address::mainnet_p2kh)
        return address_status_type::mainnet_p2kh;
    else if (version == payment_address::mainnet_p2sh)
        return address_status_type::mainnet_p2sh;
    else if (version == payment_address::testnet_p2kh)
        return address_status_type::testnet_p2kh;
    else if (version == payment_address::testnet_p2sh)
        return address_status_type::testnet_p2sh;
    else
        return address_status_type::invalid;
}

address_status_type address_status(const std::string& address)
{
    const payment_address payaddr(address);
    const stealth_address stealth(address);
    if (payaddr.is_valid())
        return payment_address_type(payaddr.version());
    else if (stealth.is_valid())
        return address_status_type::stealth;
    else
        return address_status_type::invalid;
}

bool is_mainnet_address(const std::string& address)
{
    const auto status = address_status(address);
    return status == address_status_type::mainnet_p2kh ||
           status == address_status_type::mainnet_p2sh;
}
bool is_testnet_address(const std::string& address)
{
    const auto status = address_status(address);
    return status == address_status_type::testnet_p2kh ||
           status == address_status_type::testnet_p2sh;
}
bool is_stealth_address(const std::string& address)
{
    const auto status = address_status(address);
    return status == address_status_type::stealth;
}

bool is_valid_address(const std::string& address)
{
    return address_status(address) != address_status_type::invalid;
}

} // namespace wallet
} // namespace libbitcoin

