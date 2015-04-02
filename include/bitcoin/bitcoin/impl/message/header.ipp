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
#ifndef LIBBITCOIN_MESSAGE_HEADER_IPP
#define LIBBITCOIN_MESSAGE_HEADER_IPP

#include <bitcoin/bitcoin/constants.hpp>

namespace libbitcoin {
namespace message {

template <typename Iterator, bool SafeCheckLast>
void header::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    magic = deserial.read_4_bytes();
    command = deserial.read_fixed_string(command_size);
    payload_length = deserial.read_4_bytes();
    checksum = 0;
}

template <typename Iterator, bool SafeCheckLast>
header::header(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
header::header(const Iterator begin, const Iterator end)
{
    auto deserial = make_deserializer(begin, end);
    deserialize(deserial);
}

} // end message
} // end libbitcoin

#endif
