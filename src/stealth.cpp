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
#include <bitcoin/constants.hpp>
#include <bitcoin/stealth.hpp>
#include <bitcoin/utility/assert.hpp>
#include <bitcoin/utility/hash.hpp>
#include <bitcoin/utility/serializer.hpp>

namespace libbitcoin {

bool stealth_match(const stealth_prefix& prefix, const uint8_t* raw_bitfield)
{
    for (size_t i = 0; i < prefix.size(); ++i)
    {
        if (i > 0 && i % byte_size == 0)
            ++raw_bitfield;
        size_t current_block = i / byte_size;
        size_t bit_offset = i - current_block * byte_size;
        BITCOIN_ASSERT(bit_offset < byte_size);
        bool value = (*raw_bitfield & (1 << bit_offset)) > 0;
        if (value != prefix[i])
            return false;
    }
    return true;
}

stealth_bitfield calculate_stealth_bitfield(const data_chunk& stealth_data)
{
    constexpr size_t bitfield_size = sizeof(stealth_bitfield);
    // Calculate stealth bitfield
    const hash_digest index = bitcoin_hash(stealth_data);
    auto deserial = make_deserializer(
        index.begin(), index.begin() + bitfield_size);
    auto bitfield = deserial.read_little_endian<stealth_bitfield>();
    return bitfield;
}

} // namespace libbitcoin

