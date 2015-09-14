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
#include "parse_ek_private.hpp"

#include <cstdint>
#include <cstddef>
#include <bitcoin/bitcoin/math/checksum.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/wallet/encrypted_keys.hpp>
#include "parse_ek_key.hpp"

namespace libbitcoin {
namespace wallet {

const byte_array<parse_ek_private::magic_size> parse_ek_private::magic
{
    { 0x01 }
};

byte_array<parse_ek_private::prefix_size> parse_ek_private::prefix_factory(
    uint8_t address, bool multiplied)
{
    const auto baseline = multiplied ? multiplied_context : default_context;
    const auto context = baseline + address;
    return splice(magic, to_array(context));
}

parse_ek_private::parse_ek_private(const ek_private& key)
    : parse_ek_key<prefix_size>(
        slice<0, 2>(key),
        slice<2, 3>(key),
        slice<3, 7>(key),
        slice<7, 15>(key)),
    data1_(slice<15, 23>(key)),
    data2_(slice<23, 39>(key))
{
    valid(valid() && verify_checksum(key));
}

uint8_t parse_ek_private::address_version() const
{
    const auto baseline = multiplied() ? multiplied_context : default_context;
    return context() - baseline;
}

quarter_hash parse_ek_private::data1() const
{
    return data1_;
}

half_hash parse_ek_private::data2() const
{
    return data2_;
}

bool parse_ek_private::multiplied() const
{
    // This is a double negative (multiplied = not not multiplied).
    return (flags() & ek_flag::ec_non_multiplied) == 0;
}

} // namespace wallet
} // namespace libbitcoin
