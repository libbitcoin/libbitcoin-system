/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
    DEFAULT_COPY_MOVE_DESTRUCT(ec_public);

    static const uint8_t mainnet_p2kh;
    static const uint8_t testnet_p2kh;

    /// Constructors.
    ec_public() NOEXCEPT;
    ec_public(const ec_point& point) NOEXCEPT;
    ec_public(const ec_private& secret) NOEXCEPT;
    ec_public(const data_chunk& decoded) NOEXCEPT;
    ec_public(const std::string& base16) NOEXCEPT;
    ec_public(const ec_compressed& compressed, bool compress=true) NOEXCEPT;
    ec_public(const ec_uncompressed& uncompressed,
        bool compress=false) NOEXCEPT;

    /// Operators.
    bool operator<(const ec_public& other) const NOEXCEPT;
    bool operator==(const ec_public& other) const NOEXCEPT;
    bool operator!=(const ec_public& other) const NOEXCEPT;
    friend std::istream& operator>>(std::istream& in, ec_public& to);
    friend std::ostream& operator<<(std::ostream& out,
        const ec_public& of) NOEXCEPT;

    /// Serializer.
    std::string encoded() const NOEXCEPT;

    /// Accessors.
    bool compressed() const NOEXCEPT;

    /// Methods.
    bool to_data(data_chunk& out) const NOEXCEPT;
    bool to_uncompressed(ec_uncompressed& out) const NOEXCEPT;
    payment_address to_payment_address(
        uint8_t version=mainnet_p2kh) const NOEXCEPT;

private:
    /// Validators.
    static bool is_point(const data_slice& decoded) NOEXCEPT;

    /// Factories.
    static ec_public from_data(const data_chunk& decoded) NOEXCEPT;
    static ec_public from_private(const ec_private& secret) NOEXCEPT;
    static ec_public from_string(const std::string& base16) NOEXCEPT;
    static ec_public from_point(const ec_uncompressed& point,
        bool compress) NOEXCEPT;

    /// Members.
    /// This should be const, apart from the need to implement assignment.
    bool compress_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif
