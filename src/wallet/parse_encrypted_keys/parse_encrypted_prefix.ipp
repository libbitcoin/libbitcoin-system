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
#ifndef LIBBITCOIN_PARSE_ENCRYPTED_PREFIX_IPP
#define LIBBITCOIN_PARSE_ENCRYPTED_PREFIX_IPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>

namespace libbitcoin {
namespace wallet {
    
template<uint8_t Version, size_t Size>
parse_encrypted_prefix<Version, Size>::parse_encrypted_prefix(
    const byte_array<Size>& value)
  : version_(slice<0, 1>(value)),
    magic_(slice<1, Size - 1>(value)),
    context_(slice<Size - 1, Size>(value)),
    valid_(verify_version())
{
}

template<uint8_t Version, size_t Size>
uint8_t parse_encrypted_prefix<Version, Size>::context() const
{
    return context_.front();
}

template<uint8_t Version, size_t Size>
byte_array<Size> parse_encrypted_prefix<Version, Size>::prefix() const
{
    return splice(version_, magic_, context_);
}

template<uint8_t Version, size_t Size>
bool parse_encrypted_prefix<Version, Size>::valid() const
{
    return valid_;
}

template<uint8_t Version, size_t Size>
void parse_encrypted_prefix<Version, Size>::valid(bool value)
{
    valid_ = value;
}

template<uint8_t Version, size_t Size>
bool parse_encrypted_prefix<Version, Size>::verify_version() const
{
    // All version numbers are valid.
    return true;
}

template<uint8_t Version, size_t Size>
uint8_t parse_encrypted_prefix<Version, Size>::version() const
{
    return version_.front();
}

} // namespace wallet
} // namespace libbitcoin

#endif
