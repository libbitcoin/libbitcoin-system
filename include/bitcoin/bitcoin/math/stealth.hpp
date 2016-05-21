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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_STEALTH_HPP
#define LIBBITCOIN_STEALTH_HPP

#include <cstdint>
#include <bitcoin/bitcoin/chain/script/script.hpp>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>

namespace libbitcoin {

static BC_CONSTEXPR uint8_t ephemeral_public_key_sign = 0x02;

/// Determine if the script is a null-data script of at least 32 data bytes.
BC_API bool is_stealth_script(const chain::script& script);

/// Convert a stealth info script to a prefix usable for stealth.
BC_API bool to_stealth_prefix(uint32_t& out_prefix,
    const chain::script& script);

/// Create a valid stealth ephemeral private key from the provided seed.
BC_API bool create_ephemeral_key(ec_secret& out_secret,
    const data_chunk& seed);

/// Create an ephemeral public key from the provided seed with the
/// null-data script data value that produces the desired filter prefix.
BC_API bool create_stealth_data(data_chunk& out_stealth_data,
    ec_secret& out_secret, const binary& filter, const data_chunk& seed);

/// Extract the stealth ephemeral public key from an output script.
BC_API bool extract_ephemeral_key(ec_compressed& out_ephemeral_public_key,
    const chain::script& script);

/// Extract the unsigned stealth ephemeral public key from an output script.
BC_API bool extract_ephemeral_key(hash_digest& out_unsigned_ephemeral_key,
    const chain::script& script);

/// Calculate the shared secret.
BC_API bool shared_secret(ec_secret& out_shared, const ec_secret& secret,
    const ec_compressed& point);

/// Uncover the stealth public key.
BC_API bool uncover_stealth(ec_compressed& out_stealth,
    const ec_compressed& ephemeral_or_scan, const ec_secret& scan_or_ephemeral,
    const ec_compressed& spend);

/// Uncover the stealth secret.
BC_API bool uncover_stealth(ec_secret& out_stealth,
    const ec_compressed& ephemeral_or_scan, const ec_secret& scan_or_ephemeral,
    const ec_secret& spend);

} // namespace libbitcoin

#endif
