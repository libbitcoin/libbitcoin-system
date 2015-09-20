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
#ifndef LIBBITCOIN_WALLET_STEALTH_HPP
#define LIBBITCOIN_WALLET_STEALTH_HPP

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/chain/script.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>
#include <bitcoin/bitcoin/math/ec_keys.hpp>

namespace libbitcoin {
namespace wallet {

struct BC_API stealth_info
{
    hash_digest ephem_pubkey_hash;
    binary_type bitfield;
};

/**
 * Calculate the stealth binary prefix from the RETURN output.
 */
BC_API binary_type calculate_stealth_prefix(
    const chain::script& stealth_script);

BC_API bool extract_stealth_info(stealth_info& info,
    const chain::script& output_script);

BC_API bool shared_secret(ec_secret& out_shared, const ec_secret& secret,
    const ec_compressed& point);

BC_API bool uncover_stealth(ec_compressed& out_stealth,
    const ec_compressed& ephemeral, const ec_secret& scan,
    const ec_compressed& spend);

BC_API bool uncover_stealth(ec_secret& out_stealth,
    const ec_compressed& ephemeral, const ec_secret& scan,
    const ec_secret& spend);

} // namespace wallet
} // namespace libbitcoin

#endif
