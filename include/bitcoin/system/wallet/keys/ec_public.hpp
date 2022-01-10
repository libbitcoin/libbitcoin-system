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
#ifndef LIBBITCOIN_SYSTEM_WALLET_KEYS_EC_PUBLIC_HPP
#define LIBBITCOIN_SYSTEM_WALLET_KEYS_EC_PUBLIC_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/keys/ec_point.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

class ec_private;
class payment_address;

/// Use to pass an ec point as either ec_compressed or ec_uncompressed.
/// ec_public doesn't carry a version for address creation or base58 encoding.
class BC_API ec_public
  : public ec_point
{
public:
    static const uint8_t mainnet_p2kh;
    static const uint8_t testnet_p2kh;

    /// Constructors.
    ec_public() noexcept;
    ec_public(const ec_public& other) noexcept;
    ec_public(const ec_point& point) noexcept;
    ec_public(const ec_private& secret) noexcept;
    ec_public(const data_chunk& decoded) noexcept;
    ec_public(const std::string& base16) noexcept;
    ec_public(const ec_compressed& compressed, bool compress=true) noexcept;
    ec_public(const ec_uncompressed& uncompressed,
        bool compress=false) noexcept;

    /// Operators.
    ec_public& operator=(ec_public other) noexcept;
    bool operator<(const ec_public& other) const noexcept;
    bool operator==(const ec_public& other) const noexcept;
    bool operator!=(const ec_public& other) const noexcept;
    friend std::istream& operator>>(std::istream& in, ec_public& to);
    friend std::ostream& operator<<(std::ostream& out,
        const ec_public& of) noexcept;

    // Swap implementation required to properly handle base class.
    friend void swap(ec_public& left, ec_public& right) noexcept;

    /// Serializer.
    std::string encoded() const noexcept;

    /// Accessors.
    bool compressed() const noexcept;

    /// Methods.
    bool to_data(data_chunk& out) const noexcept;
    bool to_uncompressed(ec_uncompressed& out) const noexcept;
    payment_address to_payment_address(
        uint8_t version=mainnet_p2kh) const noexcept;

private:
    /// Validators.
    static bool is_point(const data_slice& decoded) noexcept;

    /// Factories.
    static ec_public from_data(const data_chunk& decoded) noexcept;
    static ec_public from_private(const ec_private& secret) noexcept;
    static ec_public from_string(const std::string& base16) noexcept;
    static ec_public from_point(const ec_uncompressed& point,
        bool compress) noexcept;

    /// Members.
    /// This should be const, apart from the need to implement assignment.
    bool compress_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
