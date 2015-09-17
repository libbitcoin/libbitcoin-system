/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/bitcoin_uri.hpp>

#include <cstdint>
#include <iomanip>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base10.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/formats/base58.hpp>
#include <bitcoin/bitcoin/wallet/stealth_address.hpp>
#include <bitcoin/bitcoin/wallet/uri.hpp>

namespace libbitcoin {
namespace wallet {

bool uri_parse(const std::string& in, uri_visitor& result,
    bool strict)
{
    uri parsed;
    if (!parsed.decode(in, strict))
        return false;

    // Check the scheme:
    if (parsed.scheme() != "bitcoin")
        return false;

    // Check the address:
    auto address = parsed.path();
    if (parsed.has_authority())
    {
        if (strict || address.size())
            return false;
        // Using "bitcoin://" instead of "bitcoin:" is a common mistake:
        address = parsed.authority();
    }
    if (!is_base58(address))
        return false;
    if (!address.empty() && !result.got_address(address))
        return false;

    // Check the parameters:
    auto q = parsed.decode_query();
    for (const auto& i: q)
    {
        auto key = i.first;
        auto value = i.second;
        if (!key.empty() && !result.got_param(key, value))
            return false;
    }
    return true;
}

bool uri_parse_result::got_address(std::string& address)
{
    payment_address payaddr;
    if (payaddr.from_string(address))
    {
        this->address.reset(payaddr);
        this->stealth.reset();
        return true;
    }

    stealth_address stealthaddr;
    if (stealthaddr.from_string(address))
    {
        this->stealth.reset(stealthaddr);
        this->address.reset();
        return true;
    }

    return false;
}

bool uri_parse_result::got_param(std::string& key, std::string& value)
{
    if (key == "amount")
    {
        uint64_t amount;
        if (!decode_base10(amount, value, btc_decimal_places))
            return false;
        this->amount.reset(amount);
    }
    else if (key == "label")
        label.reset(value);
    else if (key == "message")
        message.reset(value);
    else if (key == "r")
        r.reset(value);
    else if (!key.compare(0, 4, "req-"))
        return false;
    return true;
}

void uri_writer::write_address(const payment_address& address)
{
    address_ = address.to_string();
}

void uri_writer::write_address(const stealth_address& address)
{
    address_ = address.to_string();
}

void uri_writer::write_amount(uint64_t satoshis)
{
    query_["amount"] = encode_base10(satoshis, btc_decimal_places);
}

void uri_writer::write_label(const std::string& label)
{
    query_["label"] = label;
}

void uri_writer::write_message(const std::string& message)
{
    query_["message"] = message;
}

void uri_writer::write_r(const std::string& r)
{
    query_["r"] = r;
}

void uri_writer::write_address(const std::string& address)
{
    address_ = address;
}

void uri_writer::write_param(const std::string& key,
    const std::string& value)
{
    query_[key] = value;
}

std::string uri_writer::string() const
{
    uri out;
    out.set_scheme("bitcoin");
    out.set_path(address_);
    out.encode_query(query_);
    return out.encode();
}

} // namespace wallet
} // namespace libbitcoin
