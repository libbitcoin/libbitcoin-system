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
#ifndef LIBBITCOIN__WALLET_EK_TOKEN_HPP
#define LIBBITCOIN__WALLET_EK_TOKEN_HPP

//#include <iostream>
//#include <string>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/wallet/ek_token.hpp>
#include <wallet_encrypted_token.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/**
 * Serialization helper to convert between base58 string and bip38 token.
 */
class BC_API wallet_ek_token
{
public:
    /// Constructors.
    wallet_ek_token();
    wallet_ek_token(const std::string& encoded);
    wallet_ek_token(const wallet_encrypted_token& key);
    wallet_ek_token(const wallet_ek_token& other);

    /// Operators.
//    bool operator<(const wallet_ek_token& other) const;
    bool lt(const wallet_ek_token& other) const;
//    bool operator==(const wallet_ek_token& other) const;
    bool eq(const wallet_ek_token& other) const;
//    bool operator!=(const wallet_ek_token& other) const;
//    wallet_ek_token& operator=(const wallet_ek_token& other);
    wallet_ek_token& assign(const wallet_ek_token& other);
//    friend std::istream& operator>>(std::istream& in, wallet_ek_token& to);
//    friend std::ostream& operator<<(std::ostream& out, const wallet_ek_token& of);

    /// Cast operators.
//    operator bool() const;
    bool toBoolean() const;
//    operator const encrypted_token&() const;
    const wallet_encrypted_token& to_encrypted_token() const;

    /// Serializer.
    std::string encoded() const;

    /// Accessors.
    const wallet_encrypted_token& token() const;

    wallet::ek_token getValue() {
		return value;
	}

	void setValue(wallet::ek_token value) {
		this->value = value;
	}
private:
	wallet::ek_token value;

//    /// Factories.
//    static wallet_ek_token from_string(const std::string& encoded);
//
//    /// Members.
//    /// These should be const, apart from the need to implement assignment.
//    bool valid_;
//    encrypted_token token_;
};

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif
