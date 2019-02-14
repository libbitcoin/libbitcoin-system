/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_PAYMENT_ADDRESS_HPP
#define LIBBITCOIN_SYSTEM_WALLET_PAYMENT_ADDRESS_HPP

#include <algorithm>
#include <iostream>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <bitcoin/system/chain/script.hpp>
#include <bitcoin/system/compat.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/checksum.hpp>
#include <bitcoin/system/math/elliptic_curve.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/wallet/ec_private.hpp>
#include <bitcoin/system/wallet/ec_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

static BC_CONSTEXPR size_t payment_size = 1u + short_hash_size + checksum_size;
typedef byte_array<payment_size> payment;

/// A class for working with non-stealth payment addresses.
class BC_API payment_address
{
public:
    static const uint8_t mainnet_p2kh;
    static const uint8_t mainnet_p2sh;

    static const uint8_t testnet_p2kh;
    static const uint8_t testnet_p2sh;

    typedef std::vector<payment_address> list;
    typedef std::shared_ptr<payment_address> ptr;

    /// Extract a payment address list from an input or output script.
    static list extract(const chain::script& script,
        uint8_t p2kh_version=mainnet_p2kh, uint8_t p2sh_version=mainnet_p2sh);
    static list extract_input(const chain::script& script,
        uint8_t p2kh_version=mainnet_p2kh, uint8_t p2sh_version=mainnet_p2sh);
    static list extract_output(const chain::script& script,
        uint8_t p2kh_version=mainnet_p2kh, uint8_t p2sh_version=mainnet_p2sh);

    /// Constructors.
    payment_address();
    payment_address(payment_address&& other);
    payment_address(const payment_address& other);
    payment_address(const payment& decoded);
    payment_address(const ec_private& secret);
    payment_address(const std::string& address);
    payment_address(short_hash&& hash, uint8_t version=mainnet_p2kh);
    payment_address(const short_hash& hash, uint8_t version=mainnet_p2kh);
    payment_address(const ec_public& point, uint8_t version=mainnet_p2kh);
    payment_address(const chain::script& script, uint8_t version=mainnet_p2sh);

    /// Operators.
    bool operator<(const payment_address& other) const;
    bool operator==(const payment_address& other) const;
    bool operator!=(const payment_address& other) const;
    payment_address& operator=(const payment_address& other);
    friend std::istream& operator>>(std::istream& in, payment_address& to);
    friend std::ostream& operator<<(std::ostream& out,
        const payment_address& of);

    /// Cast operators.
    operator bool() const;
    operator const short_hash&() const;

    /// Serializer.
    std::string encoded() const;

    /// Accessors.
    uint8_t version() const;
    const short_hash& hash() const;

    /// Methods.
    payment to_payment() const;

private:
    /// Validators.
    static bool is_address(const data_slice& decoded);

    /// Factories.
    static payment_address from_string(const std::string& address);
    static payment_address from_payment(const payment& decoded);
    static payment_address from_private(const ec_private& secret);
    static payment_address from_public(const ec_public& point, uint8_t version);
    static payment_address from_script(const chain::script& script,
        uint8_t version);

    /// Members.
    /// These should be const, apart from the need to implement assignment.
    bool valid_;
    uint8_t version_;
    short_hash hash_;
};

/// The pre-encoded structure of a payment address or other similar data.
struct BC_API wrapped_data
{
    uint8_t version;
    data_chunk payload;
    uint32_t checksum;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

// Allow payment_address to be in indexed in std::*map classes.
namespace std
{
template <>
struct hash<bc::system::wallet::payment_address>
{
    size_t operator()(const bc::system::wallet::payment_address& address) const
    {
        return std::hash<bc::system::short_hash>()(address.hash());
    }
};

} // namespace std

#endif
