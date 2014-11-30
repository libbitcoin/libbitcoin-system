/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_URI_HPP
#define LIBBITCOIN_URI_HPP

#include <cstdint>
#include <string>
#include <sstream>
#include <boost/optional.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/wallet/address.hpp>
#include <bitcoin/bitcoin/wallet/stealth_address.hpp>

namespace libbitcoin {

/**
 * The URI parser calls these methods each time it extracts a URI component.
 */
class BC_API uri_visitor
{
public:
    virtual bool got_address(std::string& address) = 0;
    virtual bool got_param(std::string& key, std::string& value) = 0;
};

/**
 * A decoded bitcoin URI corresponding to BIP 21 and BIP 72.
 * All string members are UTF-8.
 */
class BC_API uri_parse_result
  : public uri_visitor
{
public:
    typedef boost::optional<payment_address> optional_address;
    typedef boost::optional<stealth_address> optional_stealth;
    typedef boost::optional<uint64_t> optional_amount;
    typedef boost::optional<std::string> optional_string;

    optional_address address;
    optional_stealth stealth;
    optional_amount amount;
    optional_string label;
    optional_string message;
    optional_string r;

    bool got_address(std::string& address);
    bool got_param(std::string& key, std::string& value);
};

/**
 * Parses a URI string into its individual components.
 * @param strict Only accept properly-escaped parameters. Some bitcoin
 * software does not properly escape URI parameters, and setting strict to
 * false allows these malformed URI's to parse anyhow.
 * @return false if the URI is malformed.
 */
BC_API bool uri_parse(const std::string& uri,
    uri_visitor& result, bool strict=true);

/**
 * Assembles a bitcoin URI string.
 */
class uri_writer
{
public:
    BC_API uri_writer();

    // Formatted:
    BC_API void write_address(const payment_address& address);
    BC_API void write_address(const stealth_address& stealth);
    BC_API void write_amount(uint64_t satoshis);
    BC_API void write_label(const std::string& label);
    BC_API void write_message(const std::string& message);
    BC_API void write_r(const std::string& r);

    // Raw:
    BC_API void write_address(const std::string& address);
    BC_API void write_param(const std::string& key, const std::string& value);

    BC_API std::string string() const;

private:
    std::ostringstream stream_;
    bool first_param_;
};

} // namespace libbitcoin

#endif

