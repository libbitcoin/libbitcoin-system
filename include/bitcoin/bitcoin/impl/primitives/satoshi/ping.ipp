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
#ifndef LIBBITCOIN_SATOSHI_PRIMITIVE_PING_IPP
#define LIBBITCOIN_SATOSHI_PRIMITIVE_PING_IPP

namespace libbitcoin {

template <typename Iterator, bool SafeCheckLast>
void nonce_type::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    nonce = deserial.read_8_bytes();

}

template <typename Iterator, bool SafeCheckLast>
nonce_type::nonce_type(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
nonce_type::nonce_type(const Iterator begin, const Iterator end)
{
    auto deserial = make_deserializer(begin, end);
    deserialize(deserial);

    BITCOIN_ASSERT(deserial.iterator() == begin + satoshi_size());
    BITCOIN_ASSERT(deserial.iterator() == end);
}

template <typename Iterator, bool SafeCheckLast>
ping_type::ping_type(deserializer<Iterator, SafeCheckLast>& deserial)
: nonce_type(deserial)
{
}

template<typename Iterator>
ping_type::ping_type(const Iterator begin, const Iterator end)
: nonce_type(begin, end)
{
}

template <typename Iterator, bool SafeCheckLast>
pong_type::pong_type(deserializer<Iterator, SafeCheckLast>& deserial)
: nonce_type(deserial)
{
}

template<typename Iterator>
pong_type::pong_type(const Iterator begin, const Iterator end)
: nonce_type(begin, end)
{
}

}

#endif
