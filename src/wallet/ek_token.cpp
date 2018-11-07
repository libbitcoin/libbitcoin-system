/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/wallet/ek_token.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <boost/program_options.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/formats/base_58.hpp>
#include <bitcoin/bitcoin/math/checksum.hpp>

namespace libbitcoin {
namespace wallet {

ek_token::ek_token()
  : valid_(false), token_()
{
}

ek_token::ek_token(const std::string& encoded)
  : ek_token(from_string(encoded))
{
}

ek_token::ek_token(const ek_token& other)
  : valid_(other.valid_), token_(other.token_)
{
}

ek_token::ek_token(const encrypted_token& value)
  : valid_(true), token_(value)
{
}

// Factories.
// ----------------------------------------------------------------------------

ek_token ek_token::from_string(const std::string& encoded)
{
    // TODO: incorporate existing parser here, setting new members.

    encrypted_token key;
    return decode_base58(key, encoded) && verify_checksum(key) ?
        ek_token(key) : ek_token();
}

// Cast operators.
// ----------------------------------------------------------------------------

ek_token::operator bool() const
{
    return valid_;
}

ek_token::operator const encrypted_token&() const
{
    return token_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string ek_token::encoded() const
{
    return encode_base58(token_);
}

// Accessors.
// ----------------------------------------------------------------------------

const encrypted_token& ek_token::token() const
{
    return token_;
}

// Operators.
// ----------------------------------------------------------------------------

ek_token& ek_token::operator=(const ek_token& other)
{
    valid_ = other.valid_;
    token_ = other.token_;
    return *this;
}

bool ek_token::operator<(const ek_token& other) const
{
    return encoded() < other.encoded();
}

bool ek_token::operator==(const ek_token& other) const
{
    return valid_ == other.valid_ && token_ == other.token_;
}

bool ek_token::operator!=(const ek_token& other) const
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, ek_token& to)
{
    std::string value;
    in >> value;
    to = ek_token(value);

    if (!to)
    {
        using namespace boost::program_options;
        BOOST_THROW_EXCEPTION(invalid_option_value(value));
    }

    return in;
}

std::ostream& operator<<(std::ostream& out, const ek_token& of)
{
    out << of.encoded();
    return out;
}

} // namespace wallet
} // namespace libbitcoin
