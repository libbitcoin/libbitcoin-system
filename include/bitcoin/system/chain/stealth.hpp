/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_CHAIN_STEALTH_HPP
#define LIBBITCOIN_CHAIN_STEALTH_HPP

#include <cstdint>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/math/elliptic_curve.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {
namespace chain {

/// This structure is used in the client-server protocol in v2/v3.
/// The stealth row excludes the sign byte (0x02) of the ephemeral public key.
struct BC_API stealth_compact
{
    typedef std::vector<stealth_compact> list;

    hash_digest ephemeral_public_key_hash;
    short_hash public_key_hash;
    hash_digest transaction_hash;
};

/// This structure is used between client and API callers in v2/v3.
/// The normal stealth row includes the sign byte of the ephemeral public key.
struct BC_API stealth
{
    typedef std::vector<stealth> list;

    ec_compressed ephemeral_public_key;
    short_hash public_key_hash;
    hash_digest transaction_hash;
};

} // namespace chain
} // namespace libbitcoin

#endif
