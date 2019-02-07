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
#ifndef LIBBITCOIN__WALLET_EK_PRIVATE_HPP
#define LIBBITCOIN__WALLET_EK_PRIVATE_HPP

#include <iostream>
#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/wallet/ek_private.hpp>
#include <wallet_encrypted_private.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/// Use to pass an encrypted private key.
class BC_API wallet_ek_private
{
public:
    /// Constructors.
    wallet_ek_private();
    wallet_ek_private(const std::string& encoded);
    wallet_ek_private(const wallet_encrypted_private& key);
    wallet_ek_private(const wallet_ek_private& other);

    /// Operators.
//    bool operator<(const wallet_ek_private& other) const;
    bool lt(const wallet_ek_private& other) const;
//    bool operator==(const wallet_ek_private& other) const;
    bool eq(const wallet_ek_private& other) const;
//    bool operator!=(const wallet_ek_private& other) const;
    bool operatorne(const wallet_ek_private& other) const;
//    wallet_ek_private& operator=(const wallet_ek_private& other);
    wallet_ek_private& assign(const wallet_ek_private& other);
//    friend std::istream& operator>>(std::istream& in, wallet_ek_private& to);
//    friend std::ostream& operator<<(std::ostream& out, const wallet_ek_private& of);

    /// Cast operators.
//    operator bool() const;
    bool toBoolean() const;
//    operator const encrypted_private&() const;
    wallet_encrypted_private& get_encrypted_private() const;

    /// Serializer.
    std::string encoded() const;

    /// Accessors.
    const wallet_encrypted_private& private_key() const;

    wallet::ek_private getValue() {
		return value;
	}

	void setValue(wallet::ek_private value) {
		this->value = value;
	}
private:
	wallet::ek_private value;

//    /// Factories.
//    static ek_private from_string(const std::string& encoded);
//
//    /// Members.
//    /// These should be const, apart from the need to implement assignment.
//    bool valid_;
//    encrypted_private private_;
};

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif
