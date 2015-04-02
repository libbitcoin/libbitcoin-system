/*
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
#ifndef LIBBITCOIN_MESSAGE_GET_BLOCKS_IPP
#define LIBBITCOIN_MESSAGE_GET_BLOCKS_IPP

namespace libbitcoin {
namespace message {

template <typename Iterator, bool SafeCheckLast>
void get_blocks::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    // Discard protocol version because it is stupid
    deserial.read_4_bytes();

    // Note: changed to uint64_t to preclude possible loss of data.
    uint64_t count = deserial.read_variable_uint();

    for (uint64_t i = 0; i < count; ++i)
    {
        hash_digest start_hash = deserial.read_hash();
        start_hashes.push_back(start_hash);
    }

    hash_stop = deserial.read_hash();
}

template <typename Iterator, bool SafeCheckLast>
get_blocks::get_blocks(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
get_blocks::get_blocks(const Iterator begin, const Iterator end)
{
    auto deserial = make_deserializer(begin, end);
    deserialize(deserial);
    BITCOIN_ASSERT(deserial.iterator() == begin + satoshi_size());
    BITCOIN_ASSERT(deserial.iterator() == end);
}

} // end message
} // end libbitcoin

#endif
