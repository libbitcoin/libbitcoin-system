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
#ifndef LIBBITCOIN_PARSE_ENCRYPTED_KEY_IPP
#define LIBBITCOIN_PARSE_ENCRYPTED_KEY_IPP

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>
#include "parse_encrypted_prefix.hpp"

namespace libbitcoin {
namespace wallet {

template<uint8_t Version, size_t Size>
parse_encrypted_key<Version, Size>::parse_encrypted_key(
    const byte_array<Size>& prefix, const one_byte& flags,
    const wallet::salt& salt, const wallet::entropy& entropy)
  : parse_encrypted_prefix<Version, Size>(prefix),
    flags_(flags), salt_(salt), entropy_(entropy)
{
}

template<uint8_t Version, size_t Size>
bool parse_encrypted_key<Version, Size>::compressed() const
{
    return (flags() & encrypted_key_flag::ec_compressed) != 0;
}

template<uint8_t Version, size_t Size>
wallet::entropy parse_encrypted_key<Version, Size>::entropy() const
{
    // The owner salt + lot-sequence or owner entropy.
    return entropy_;
}

template<uint8_t Version, size_t Size>
uint8_t parse_encrypted_key<Version, Size>::flags() const
{
    return flags_.front();
}

template<uint8_t Version, size_t Size>
bool parse_encrypted_key<Version, Size>::lot_sequence() const
{
    return (flags() & encrypted_key_flag::lot_sequence) != 0;
}

template<uint8_t Version, size_t Size>
data_chunk parse_encrypted_key<Version, Size>::owner_salt() const
{
    // Either 4 or 8 bytes, depending on the lot sequence flags.
    if (lot_sequence())
        return to_chunk(slice<0, salt_size>(entropy()));
    else
        return to_chunk(entropy());
}

template<uint8_t Version, size_t Size>
wallet::salt parse_encrypted_key<Version, Size>::salt() const
{
    // The address hash salt.
    return salt_;
}

} // namespace wallet
} // namespace libbitcoin

#endif
