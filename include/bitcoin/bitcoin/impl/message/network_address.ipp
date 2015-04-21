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
#ifndef LIBBITCOIN_MESSAGE_NETWORK_ADDRESS_IPP
#define LIBBITCOIN_MESSAGE_NETWORK_ADDRESS_IPP

namespace libbitcoin {
namespace message {

template <typename Iterator, bool SafeCheckLast>
void network_address::deserialize(
    deserializer<Iterator, SafeCheckLast>& deserial)
{
    services_ = deserial.read_8_bytes();
    // Read IP address
    ip_ = deserial.template read_bytes<16>();
    port_ = deserial.template read_big_endian<uint16_t>();
}

template <typename Iterator, bool SafeCheckLast>
network_address::network_address(
    deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
network_address::network_address(const Iterator begin, const Iterator end)
{
    auto deserial = make_deserializer(begin, end);
    deserialize(deserial);

    BITCOIN_ASSERT(deserial.iterator() == begin + satoshi_size());
    BITCOIN_ASSERT(deserial.iterator() == end);
}

} // end message
} // end libbitcoin

#endif
