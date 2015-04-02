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
#ifndef LIBBITCOIN_CHAIN_BLOCK_HEADER_IPP
#define LIBBITCOIN_CHAIN_BLOCK_HEADER_IPP

namespace libbitcoin {
namespace chain {

template <typename Iterator, bool SafeCheckLast>
void block_header::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    version = deserial.read_4_bytes();
    previous_block_hash = deserial.read_hash();
    merkle = deserial.read_hash();
    timestamp = deserial.read_4_bytes();
    bits = deserial.read_4_bytes();
    nonce = deserial.read_4_bytes();
}

template <typename Iterator, bool SafeCheckLast>
block_header::block_header(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
block_header::block_header(const Iterator begin, const Iterator end)
{
    deserializer<Iterator, true> deserial = make_deserializer(begin, end);
    deserialize(deserial);

    BITCOIN_ASSERT(std::distance(begin, deserial.iterator()) == satoshi_size());
}

} // end chain
} // end libbitcoin

#endif
