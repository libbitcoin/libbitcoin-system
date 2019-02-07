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
#ifndef LIBBITCOIN__WALLET_BITCOIN_URI_HPP
#define LIBBITCOIN__WALLET_BITCOIN_URI_HPP

//#include <cstdint>
//#include <iostream>
//#include <map>
//#include <string>
//#include <boost/optional.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/wallet/bitcoin_uri.hpp>
#include <wallet_payment_address.hpp>
#include <wallet_stealth_address.hpp>
//#include <bitcoin/bitcoin/wallet/uri_reader.hpp>

namespace libbitcoin {
//namespace wallet {
namespace api {

/// A bitcoin URI corresponding to BIP 21 and BIP 72.
/// The object is not constant, setters can change state after construction.
class BC_API wallet_bitcoin_uri
//: public wallet::bitcoin_uri
//  : public uri_reader
{
public:
    /// Constructors.
    wallet_bitcoin_uri();
    wallet_bitcoin_uri(const wallet_bitcoin_uri& other);
    wallet_bitcoin_uri(const std::string& uri, bool strict=true);
    virtual ~wallet_bitcoin_uri();

    /// Operators.
//    bool operator<(const wallet_bitcoin_uri& other) const;
    bool lt(const wallet_bitcoin_uri& other) const;
//    bool operator==(const wallet_bitcoin_uri& other) const;
    bool eq(const wallet_bitcoin_uri& other) const;
//    bool operator!=(const wallet_bitcoin_uri& other) const;
//    wallet_bitcoin_uri& operator=(const wallet_bitcoin_uri& other);
    wallet_bitcoin_uri& assign(const wallet_bitcoin_uri& other);
//    friend std::istream& operator>>(std::istream& in, wallet_bitcoin_uri& to);
//    friend std::ostream& operator<<(std::ostream& out,
//        const wallet_bitcoin_uri& from);

    /// Test whether the URI has been initialized.
//    operator bool() const;
    bool toBoolean() const;

    /// Get the serialized URI representation.
    std::string encoded() const;

    /// Property getters.
    uint64_t amount() const;
    std::string label() const;
    std::string message() const;
    std::string r() const;
    std::string address() const;
    wallet_payment_address payment() const;
    wallet_stealth_address stealth() const;
    std::string parameter(const std::string& key) const;

    /// Property setters.
    void set_amount(uint64_t satoshis);
    void set_label(const std::string& label);
    void set_message(const std::string& message);
    void set_r(const std::string& r);
    bool set_address(const std::string& address);
    void set_address(const wallet_payment_address& payment);
    void set_address(const wallet_stealth_address& stealth);

    /// uri_reader implementation.
    void set_strict(bool strict);
    bool set_scheme(const std::string& scheme);
    bool set_authority(const std::string& authority);
    bool set_path(const std::string& path);
    bool set_fragment(const std::string& fragment);
    bool set_parameter(const std::string& key, const std::string& value);

    wallet::bitcoin_uri getValue() {
    	return value_;
    }

    void setValue(wallet::bitcoin_uri value) {
    	value_ = value;
    }

private:
    wallet::bitcoin_uri value_;
//    /// Private helpers.
//    bool set_amount(const std::string& satoshis);
//
//    /// Member state.
//    bool strict_;
//    std::string scheme_;
//    std::string address_;
//    std::map<std::string, std::string> query_;
};

} // namespace api
//} // namespace wallet
} // namespace libbitcoin

#endif

