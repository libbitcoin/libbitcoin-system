/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_STEALTH_HPP
#define LIBBITCOIN_STEALTH_HPP

#include <bitcoin/types.hpp>

namespace libbitcoin {

typedef uint32_t stealth_bitfield;

struct BC_API stealth_prefix
{
    uint8_t number_bits;
    stealth_bitfield bitfield;
};

BC_API bool stealth_match(
    const stealth_prefix& prefix, const uint8_t* raw_bitfield);

BC_API stealth_bitfield calculate_stealth_bitfield(
    const data_chunk& stealth_data);

} // namespace libbitcoin

#endif

