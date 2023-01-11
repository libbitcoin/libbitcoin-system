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
#include <bitcoin/system/wallet/keys/ek_public.hpp>

#include <iostream>
#include <sstream>
#include <string>
#include <bitcoin/system/crypto/crypto.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/radix/radix.hpp>

namespace libbitcoin {
namespace system {
namespace wallet {

ek_public::ek_public() NOEXCEPT
  : valid_(false), public_()
{
}

ek_public::ek_public(const std::string& encoded) NOEXCEPT
  : ek_public(from_string(encoded))
{
}

ek_public::ek_public(const encrypted_public& value) NOEXCEPT
  : valid_(true), public_(value)
{
}

// Factories.
// ----------------------------------------------------------------------------

ek_public ek_public::from_string(const std::string& encoded) NOEXCEPT
{
    // TODO: incorporate existing parser here, setting new members.

    encrypted_public key;
    return decode_base58(key, encoded) && verify_checksum(key) ?
        ek_public{ key } : ek_public{};
}

// Cast operators.
// ----------------------------------------------------------------------------

ek_public::operator bool() const NOEXCEPT
{
    return valid_;
}

ek_public::operator const encrypted_public&() const NOEXCEPT
{
    return public_;
}

// Serializer.
// ----------------------------------------------------------------------------

std::string ek_public::encoded() const NOEXCEPT
{
    return encode_base58(public_);
}

// Accessors.
// ----------------------------------------------------------------------------

const encrypted_public& ek_public::public_key() const NOEXCEPT
{
    return public_;
}

// Operators.
// ----------------------------------------------------------------------------

bool ek_public::operator<(const ek_public& other) const NOEXCEPT
{
    return encoded() < other.encoded();
}

bool ek_public::operator==(const ek_public& other) const NOEXCEPT
{
    return valid_ == other.valid_ && public_ == other.public_;
}

bool ek_public::operator!=(const ek_public& other) const NOEXCEPT
{
    return !(*this == other);
}

std::istream& operator>>(std::istream& in, ek_public& to)
{
    std::string value;
    in >> value;
    to = ek_public(value);

    if (!to)
        throw istream_exception(value);

    return in;
}

std::ostream& operator<<(std::ostream& out, const ek_public& of) NOEXCEPT
{
    out << of.encoded();
    return out;
}

} // namespace wallet
} // namespace system
} // namespace libbitcoin
