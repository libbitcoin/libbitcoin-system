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
#include "parse_encrypted_private_key.hpp"

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>
#include "parse_encrypted_key.hpp"
#include "parse_encrypted_prefix.hpp"

namespace libbitcoin {
namespace wallet {

byte_array<parse_encrypted_private_key::prefix_size>
parse_encrypted_private_key::prefix(uint8_t address, bool multiplied)
{
    const auto check = address == mainnet ? default_version : address;
    const auto context = multiplied ? multiplied_context : default_context;
    return splice(to_array(check), to_array(context));
}

parse_encrypted_private_key::parse_encrypted_private_key(const private_key& key)
  : parse_encrypted_key<default_version, prefix_size>(
        slice<0, 2>(key),
        slice<2, 3>(key),
        slice<3, 7>(key),
        slice<7, 15>(key)),
    data1_(slice<15, 23>(key)),
    data2_(slice<23, 39>(key))
{
    valid(valid() && verify_context() && verify_flags() &&
        verify_checksum(key));
}

uint8_t parse_encrypted_private_key::address_version() const
{
    const auto check = version();
    return check == default_version ? mainnet : check;
}

quarter_hash parse_encrypted_private_key::data1() const
{
    return data1_;
}

half_hash parse_encrypted_private_key::data2() const
{
    return data2_;
}

bool parse_encrypted_private_key::multiplied() const
{
    // This is a double negative (multiplied = not not multiplied).
    return (flags() & encrypted_key_flag::ec_non_multiplied) == 0;
}

bool parse_encrypted_private_key::verify_flags() const
{
    // This guards against a conflict that can result from a redundancy in
    // the BIP38 specification - multiplied context exists in two places.
    return multiplied() == (context() == multiplied_context);
}

bool parse_encrypted_private_key::verify_context() const
{
    return context() == default_context || context() == multiplied_context;
}

} // namespace wallet
} // namespace libbitcoin
