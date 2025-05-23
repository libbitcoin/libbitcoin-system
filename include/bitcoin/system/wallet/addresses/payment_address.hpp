/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_PAYMENT_ADDRESS_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_PAYMENT_ADDRESS_HPP

#include <memory>
#include <bitcoin/system/chain/chain.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/wallet/addresses/checked.hpp>
#include <bitcoin/system/wallet/keys/ec_private.hpp>
#include <bitcoin/system/wallet/keys/ec_public.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

typedef checked<1, short_hash_size, 4> payment;

/// A class for working with non-witness payment addresses.
/// For witness addresses, see payment_witness.
class BC_API payment_address
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(payment_address);

    static const uint8_t mainnet_p2kh;
    static const uint8_t mainnet_p2sh;

    static const uint8_t testnet_p2kh;
    static const uint8_t testnet_p2sh;

    typedef std_vector<payment_address> list;
    typedef std::shared_ptr<payment_address> ptr;

    /// Extract a payment address from an input or output script.
    static list extract(const chain::script& script,
        uint8_t p2kh_prefix=mainnet_p2kh,
        uint8_t p2sh_prefix=mainnet_p2sh) NOEXCEPT;
    static list extract_input(const chain::script& script,
        uint8_t p2kh_prefix=mainnet_p2kh,
        uint8_t p2sh_prefix=mainnet_p2sh) NOEXCEPT;
    static payment_address extract_output(const chain::script& script,
        uint8_t p2kh_prefix=mainnet_p2kh,
        uint8_t p2sh_prefix=mainnet_p2sh) NOEXCEPT;

    /// Constructors.
    payment_address() NOEXCEPT;
    payment_address(payment&& decoded) NOEXCEPT;
    payment_address(const payment& decoded) NOEXCEPT;

    payment_address(const std::string& address) NOEXCEPT;
    payment_address(const ec_private& secret) NOEXCEPT;
    payment_address(const ec_public& point,
        uint8_t prefix=mainnet_p2kh) NOEXCEPT;
    payment_address(const short_hash& hash,
        uint8_t prefix=mainnet_p2kh) NOEXCEPT;
    payment_address(const chain::script& script,
        uint8_t prefix=mainnet_p2sh) NOEXCEPT;

    /// Operators.
    bool operator<(const payment_address& other) const NOEXCEPT;
    friend std::istream& operator>>(std::istream& in, payment_address& to);
    friend std::ostream& operator<<(std::ostream& out,
        const payment_address& of) NOEXCEPT;

    /// Cast operators.
    operator bool() const NOEXCEPT;

    /// Serializer.
    std::string encoded() const NOEXCEPT;

    /// Properties.
    uint8_t prefix() const NOEXCEPT;
    short_hash hash() const NOEXCEPT;
    chain::script output_script() const NOEXCEPT;

    /// Methods.
    const payment& to_payment() const NOEXCEPT;

private:
    // Factories.
    static payment_address from_string(const std::string& address) NOEXCEPT;
    static payment_address from_private(const ec_private& secret) NOEXCEPT;
    static payment_address from_public(const ec_public& point,
        uint8_t prefix) NOEXCEPT;
    static payment_address from_script(const chain::script& script,
        uint8_t prefix) NOEXCEPT;

    // This should be const, apart from the need to implement assignment.
    payment payment_;
};

BC_API bool operator==(const payment_address& left,
    const payment_address& right) NOEXCEPT;
BC_API bool operator!=(const payment_address& left,
    const payment_address& right) NOEXCEPT;

} // namespace wallet
} // namespace system
} // namespace libbitcoin

// Allow payment_address to be in indexed in std::*map classes.
namespace std
{
template <>
struct hash<bc::system::wallet::payment_address>
{
    size_t operator()(
        const bc::system::wallet::payment_address& address) const NOEXCEPT
    {
        return std::hash<bc::system::short_hash>()(address.hash());
    }
};

} // namespace std

#endif
