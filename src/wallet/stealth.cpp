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
#include <bitcoin/bitcoin/wallet/stealth.hpp>

#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>

namespace libbitcoin {
namespace wallet {

binary_type calculate_stealth_prefix(const chain::script& stealth_script)
{
    const hash_digest index = bitcoin_hash(stealth_script.to_data(false));
    const size_t bitsize = binary_type::bits_per_block * sizeof(uint32_t);
    return binary_type(bitsize, index);
}

bool extract_stealth_info(stealth_info& info,
    const chain::script& output_script)
{
    if (output_script.type() != chain::payment_type::stealth_info)
        return false;

    info.bitfield = calculate_stealth_prefix(output_script);

    if (output_script.operations.size() < 2)
        return false;

    const auto& data = output_script.operations[1].data;

    if (data.size() < hash_size)
        return false;

    std::copy(data.begin(), data.begin() + hash_size,
        info.ephem_pubkey_hash.begin());
    return true;
}

bool shared_secret(ec_secret& out_shared, const ec_secret& secret,
    const ec_compressed& point)
{
    auto final = point;
    if (!ec_multiply(final, secret))
        return false;

    out_shared = sha256_hash(final);
    return true;
}

bool uncover_stealth(ec_compressed& out_stealth,
    const ec_compressed& ephemeral, const ec_secret& scan,
    const ec_compressed& spend)
{
    ec_secret shared;
    if (!shared_secret(shared, scan, ephemeral))
        return false;

    auto final = spend;
    if (!ec_add(final, shared))
        return false;

    out_stealth = final;
    return true;
}

bool uncover_stealth(ec_secret& out_stealth,
    const ec_compressed& ephemeral, const ec_secret& scan,
    const ec_secret& spend)
{
    ec_secret shared;
    if (!shared_secret(shared, scan, ephemeral))
        return false;

    auto final = spend;
    if (!ec_add(final, shared))
        return false;

    out_stealth = final;
    return true;
}

} // namespace wallet
} // namespace libbitcoin
