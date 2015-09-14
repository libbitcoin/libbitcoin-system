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
#include "parse_ek_public.hpp"

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>
#include "parse_ek_key.hpp"
#include "parse_ek_key.hpp"

namespace libbitcoin {
namespace wallet {

// This prefix results in the prefix "cfrm" in the base58 encoding but is
// modified when the payment address is Bitcoin mainnet (0).
const byte_array<parse_ek_public::magic_size> parse_ek_public::magic
{
    { 0x3b, 0xf6, 0xa8 }
};

byte_array<parse_ek_public::prefix_size> parse_ek_public::prefix_factory(
    uint8_t address)
{
    const auto check = address == default_address_version ?
        default_key_version : address;
    return splice(to_array(check), magic, to_array(only_context));
}

parse_ek_public::parse_ek_public(const ek_public& key)
  : parse_ek_key<default_key_version, prefix_size>(
        slice<0, 5>(key),
        slice<5, 6>(key),
        slice<6, 10>(key),
        slice<10, 18>(key)),
    sign_(slice<18, 19>(key)),
    data_(slice<19, 51>(key))
{
    valid(valid() && verify_magic() && verify_context() &&
        verify_checksum(key));
}

uint8_t parse_ek_public::address_version() const
{
    const auto check = version();
    return check == default_key_version ? default_address_version : check;
}

hash_digest parse_ek_public::data() const
{
    return data_;
}

one_byte parse_ek_public::sign() const
{
    return sign_;
}

bool parse_ek_public::verify_context() const
{
    return context() == only_context;
}

bool parse_ek_public::verify_magic() const
{
    return slice<1, prefix_size - 1>(prefix()) == magic;
}

} // namespace wallet
} // namespace libbitcoin
