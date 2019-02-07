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
#ifndef LIBBITCOIN__WALLET_EC_PUBLIC_HPP
#define LIBBITCOIN__WALLET_EC_PUBLIC_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/wallet/ec_public.hpp>
#include <math_ec_point.hpp>
#include <math_ec_compressed.hpp>
#include <math_ec_uncompressed.hpp>
#include <utility_data_chunk.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

//class wallet_ec_private;
//class wallet_payment_address;

/// Use to pass an ec point as either ec_compressed or ec_uncompressed.
/// ec_public doesn't carry a version for address creation or base58 encoding.
class BC_API wallet_ec_public
{
public:
    static const uint8_t mainnet_p2kh;

    /// Constructors.
    wallet_ec_public();
    wallet_ec_public(const wallet_ec_public& other);
    wallet_ec_public(const math_ec_point& point);
    wallet_ec_public(const wallet_ec_private& secret);
    wallet_ec_public(const utility_data_chunk& decoded);
    wallet_ec_public(const std::string& base16);
    wallet_ec_public(const math_ec_compressed& compressed, bool compress=true);
    wallet_ec_public(const math_ec_uncompressed& uncompressed, bool compress=false);

    /// Operators.
//    wallet_ec_public& operator=(wallet_ec_public other);
    wallet_ec_public& assign(wallet_ec_public other);
//    bool operator<(const wallet_ec_public& other) const;
    bool lt(const wallet_ec_public& other) const;
//    bool operator==(const wallet_ec_public& other) const;
    bool eq(const wallet_ec_public& other) const;
//    bool operator!=(const wallet_ec_public& other) const;
    bool ne(const wallet_ec_public& other) const;
//    friend std::istream& operator>>(std::istream& in, ec_public& to);
//    friend std::ostream& operator<<(std::ostream& out, const ec_public& of);

    // Swap implementation required to properly handle base class.
//    friend void swap(wallet_ec_public& left, wallet_ec_public& right);

    /// Serializer.
    std::string encoded() const;

    /// Accessors.
    bool compressed() const;

    /// Methods.
    bool to_data(utility_data_chunk& out) const;
    bool to_uncompressed(math_ec_uncompressed& out) const;
    wallet_payment_address to_payment_address(uint8_t version=mainnet_p2kh) const;

    wallet::ec_public getValue() {
		return value;
	}

	void setValue(wallet::ec_public value) {
		this->value = value;
	}

private:
	wallet::ec_public value;
//    /// Validators.
//    static bool is_point(data_slice decoded);
//
//    /// Factories.
//    static ec_public from_data(const data_chunk& decoded);
//    static ec_public from_private(const ec_private& secret);
//    static ec_public from_string(const std::string& base16);
//    static ec_public from_point(const ec_uncompressed& point, bool compress);
//
//    /// Members.
//    /// This should be const, apart from the need to implement assignment.
//    bool compress_;
};

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif
