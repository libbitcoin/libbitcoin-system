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

// TODO: The out parameter may be modified in the case of a failure.
// This can lead to unexpected results if the visitor is reused.
// This is not easily resolved since the parse result can't be cleared,
// nor is that ideal (it should only be modified if successful). A temporary
// copy is also hard, since the out parameter is a vitual base and therefore
// copy construction from a local concrete copy is problematic. Templatizing
// the out parameter type would probably work, but seems like overkill/ugly.
bool uri_parse(uri_visitor& out, const std::string& uri, bool strict)
{
    wallet::uri parsed;
    if (!parsed.decode(uri, strict))
        return false;

    // Check the scheme:
    if (parsed.scheme() != "bitcoin")
        return false;

    // Check the address:
    auto address = parsed.path();
    if (parsed.has_authority())
    {
        if (strict || !address.empty())
            return false;

        // Using "bitcoin://" instead of "bitcoin:" is a common mistake:
        address = parsed.authority();
    }

    if (!address.empty() && !out.set_address(address))
        return false;

    // Check the parameters:
    const auto query = parsed.decode_query();
    for (const auto& term: query)
    {
        const auto& key = term.first;
        const auto& value = term.second;
        if (!key.empty() && !out.set_param(key, value))
            return false;
    }

    return true;
}

bool uri_parse_result::set_address(const std::string& address)
{
    payment_address try_payment;
    if (try_payment.from_string(address))
    {
        this->address.reset(try_payment);
        stealth.reset();
        return true;
    }

    stealth_address try_stealth;
    if (try_stealth.from_string(address))
    {
        stealth.reset(try_stealth);
        this->address.reset();
        return true;
    }

    return false;
}

bool uri_parse_result::set_param(const std::string& key, const std::string& value)
{
    if (key == "amount")
    {
        uint64_t decoded;
        if (!decode_base10(decoded, value, btc_decimal_places))
            return false;

        amount.reset(decoded);
    }
    else if (key == "label")
        label.reset(value);
    else if (key == "message")
        message.reset(value);
    else if (key == "r")
        r.reset(value);
    else
        return key.substr(0, 4) != "req-";

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

void uri_writer::write_param(const std::string& key, const std::string& value)
{
    query_[key] = value;
}

std::string uri_writer::encoded() const
{
    uri out;
    out.set_scheme("bitcoin");
    out.set_path(address_);
    out.encode_query(query_);
    return out.encoded();
}

} // namespace wallet
} // namespace libbitcoin
