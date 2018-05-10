/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_RING_SIGNATURE_HPP
#define LIBBITCOIN_RING_SIGNATURE_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {

typedef std::vector<point_list> key_rings;
typedef std::vector<ec_secret> secret_list;

typedef std::vector<secret_list> s_values_type;

struct ring_signature
{
    ec_secret e;
    s_values_type s;
};

/**
 * message: the message to sign
 * rings: an array rings; each ring is an array of pubkeys
 * secret_keys: an array of signing keys
 */
BC_API bool sign(
    ring_signature& out,
    const secret_list& secrets,
    const key_rings& rings,
    const data_slice message,
    const data_slice seed);

BC_API bool verify(
    const key_rings& rings,
    const data_slice message,
    const ring_signature& signature);

} // namespace libbitcoin

#endif

