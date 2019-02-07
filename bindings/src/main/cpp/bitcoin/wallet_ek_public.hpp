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
#ifndef LIBBITCOIN__WALLET_EK_PUBLIC_HPP
#define LIBBITCOIN__WALLET_EK_PUBLIC_HPP

#include <iostream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/wallet/ek_public.hpp>
#include <wallet_encrypted_public.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/// Use to pass an encrypted public key.
class BC_API wallet_ek_public
{
public:
    /// Constructors.
    wallet_ek_public();
    wallet_ek_public(const std::string& encoded);
    wallet_ek_public(const wallet_encrypted_public& key);
    wallet_ek_public(const wallet_ek_public& other);

    /// Operators.
//    bool operator<(const wallet_ek_public& other) const;
    bool lt(const wallet_ek_public& other) const;
//    bool operator==(const wallet_ek_public& other) const;
    bool eq(const wallet_ek_public& other) const;
//    bool operator!=(const wallet_ek_public& other) const;
//    wallet_ek_public& operator=(const wallet_ek_public& other);
    wallet_ek_public& assign(const wallet_ek_public& other);
//    friend std::istream& operator>>(std::istream& in, wallet_ek_public& to);
//    friend std::ostream& operator<<(std::ostream& out, const wallet_ek_public& of);

    /// Cast operators.
//    operator bool() const;
    bool to_bool() const;
//    operator const wallet_encrypted_public&() const;
    const wallet_encrypted_public& to_wallet_encrypted_public() const;

    /// Serializer.
    std::string encoded() const;

    /// Accessors.
    const wallet_encrypted_public& public_key() const;

    wallet::ek_public getValue() {
		return value;
	}

	void setValue(wallet::ek_public value) {
		this->value = value;
	}
private:
	wallet::ek_public value;

//    /// Factories.
//    static ek_public from_string(const std::string& encoded);
//
//    /// Members.
//    /// These should be const, apart from the need to implement assignment.
//    bool valid_;
//    encrypted_public public_;
};

} // namespace wallet
} // namespace libbitcoin

#endif
