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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_BITCOIN_URI_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_BITCOIN_URI_HPP

#include <iostream>
#include <map>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/wallet/addresses/payment_address.hpp>
#include <bitcoin/system/wallet/addresses/stealth_address.hpp>
#include <bitcoin/system/wallet/addresses/uri_reader.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

/// A bitcoin URI corresponding to BIP21 and BIP72.
/// The object is not constant, setters can change state after construction.
class BC_API bitcoin_uri
  : public uri_reader
{
public:
    /// Constructors.
    bitcoin_uri() NOEXCEPT;
    bitcoin_uri(const bitcoin_uri& other) NOEXCEPT;
    bitcoin_uri(const std::string& uri, bool strict=true) NOEXCEPT;

    /// Operators.
    bool operator<(const bitcoin_uri& other) const NOEXCEPT;
    bool operator==(const bitcoin_uri& other) const NOEXCEPT;
    bool operator!=(const bitcoin_uri& other) const NOEXCEPT;
    bitcoin_uri& operator=(const bitcoin_uri& other) NOEXCEPT;
    friend std::istream& operator>>(std::istream& in, bitcoin_uri& to);
    friend std::ostream& operator<<(std::ostream& out,
        const bitcoin_uri& from) NOEXCEPT;

    /// Test whether the URI has been initialized.
    operator bool() const NOEXCEPT;

    /// Get the serialized URI representation.
    std::string encoded() const NOEXCEPT;

    /// Property getters.
    uint64_t amount() const NOEXCEPT;
    std::string label() const NOEXCEPT;
    std::string message() const NOEXCEPT;
    std::string r() const NOEXCEPT;
    std::string address() const NOEXCEPT;
    payment_address payment() const NOEXCEPT;
    stealth_address stealth() const NOEXCEPT;
    std::string parameter(const std::string& key) const NOEXCEPT;

    /// Property setters.
    void set_amount(uint64_t satoshis) NOEXCEPT;
    void set_label(const std::string& label) NOEXCEPT;
    void set_message(const std::string& message) NOEXCEPT;
    void set_r(const std::string& r) NOEXCEPT;
    bool set_address(const std::string& address) NOEXCEPT;
    void set_address(const payment_address& payment) NOEXCEPT;
    void set_address(const stealth_address& stealth) NOEXCEPT;

    /// uri_reader implementation.
    void set_strict(bool strict) NOEXCEPT;
    bool set_scheme(const std::string& scheme) NOEXCEPT;
    bool set_authority(const std::string& authority) NOEXCEPT;
    bool set_path(const std::string& path) NOEXCEPT;
    bool set_fragment(const std::string& fragment) NOEXCEPT;
    bool set_parameter(const std::string& key, const std::string& value) NOEXCEPT;

private:
    /// Private helpers.
    bool set_amount(const std::string& satoshis) NOEXCEPT;

    /// Member state.
    bool strict_;
    std::string scheme_;
    std::string address_;
    std::map<std::string, std::string> query_;
};

} // namespace wallet
} // namespace system
} // namespace libbitcoin

#endif

