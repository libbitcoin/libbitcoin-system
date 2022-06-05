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

#include <cstdint>
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
    bitcoin_uri() noexcept;
    bitcoin_uri(const bitcoin_uri& other) noexcept;
    bitcoin_uri(const std::string& uri, bool strict=true) noexcept;

    /// Operators.
    bool operator<(const bitcoin_uri& other) const noexcept;
    bool operator==(const bitcoin_uri& other) const noexcept;
    bool operator!=(const bitcoin_uri& other) const noexcept;
    bitcoin_uri& operator=(const bitcoin_uri& other) noexcept;
    friend std::istream& operator>>(std::istream& in, bitcoin_uri& to);
    friend std::ostream& operator<<(std::ostream& out,
        const bitcoin_uri& from) noexcept;

    /// Test whether the URI has been initialized.
    operator bool() const noexcept;

    /// Get the serialized URI representation.
    std::string encoded() const noexcept;

    /// Property getters.
    uint64_t amount() const noexcept;
    std::string label() const noexcept;
    std::string message() const noexcept;
    std::string r() const noexcept;
    std::string address() const noexcept;
    payment_address payment() const noexcept;
    stealth_address stealth() const noexcept;
    std::string parameter(const std::string& key) const noexcept;

    /// Property setters.
    void set_amount(uint64_t satoshis) noexcept;
    void set_label(const std::string& label) noexcept;
    void set_message(const std::string& message) noexcept;
    void set_r(const std::string& r) noexcept;
    bool set_address(const std::string& address) noexcept;
    void set_address(const payment_address& payment) noexcept;
    void set_address(const stealth_address& stealth) noexcept;

    /// uri_reader implementation.
    void set_strict(bool strict) noexcept;
    bool set_scheme(const std::string& scheme) noexcept;
    bool set_authority(const std::string& authority) noexcept;
    bool set_path(const std::string& path) noexcept;
    bool set_fragment(const std::string& fragment) noexcept;
    bool set_parameter(const std::string& key, const std::string& value) noexcept;

private:
    /// Private helpers.
    bool set_amount(const std::string& satoshis) noexcept;

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

