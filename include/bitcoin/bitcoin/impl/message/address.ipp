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
#ifndef LIBBITCOIN_MESSAGE_ADDRESS_IPP
#define LIBBITCOIN_MESSAGE_ADDRESS_IPP

namespace libbitcoin {
namespace message {

template <typename Iterator, bool SafeCheckLast>
void address::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    uint64_t count = deserial.read_variable_uint();

    for (size_t i = 0; i < count; ++i)
    {
        uint32_t timestamp = deserial.read_4_bytes();
        network_address addr = deserial.read_network_address();
        addr.timestamp = timestamp;

        addresses.push_back(addr);
    }
}

template <typename Iterator, bool SafeCheckLast>
address::address(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
address::address(const Iterator begin, const Iterator end)
{
    deserializer<Iterator, true> deserial = make_deserializer(begin, end);
    deserialize(deserial);
}

} // end message
} // end libbitcoin

#endif
