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
#ifndef LIBBITCOIN_MESSAGE_ANNOUNCE_VERSION_IPP
#define LIBBITCOIN_MESSAGE_ANNOUNCE_VERSION_IPP

namespace libbitcoin {
namespace message {

template <typename Iterator, bool SafeCheckLast>
void announce_version::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    version_ = deserial.read_4_bytes();
    services_ = deserial.read_8_bytes();
    timestamp_ = deserial.read_8_bytes();

    address_me_ = network_address(deserial);

    // Ignored field
    address_me_.timestamp(0);

    if (version_ < 106)
    {
        return;
    }

    address_you_ = network_address(deserial);

    // Ignored field
    address_you_.timestamp(0);

    nonce_ = deserial.read_8_bytes();
    user_agent_ = deserial.read_string();

    if (version_ < 209)
    {
        return;
    }

    start_height_ = deserial.read_4_bytes();
}

template <typename Iterator, bool SafeCheckLast>
announce_version::announce_version(
    deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
announce_version::announce_version(const Iterator begin, const Iterator end)
{
    auto deserial = make_deserializer(begin, end);
    deserialize(deserial);

    if (version_ < 106)
    {
        BITCOIN_ASSERT(std::distance(begin, end) >= 46);
        return;
    }

    if (version_ < 209)
    {
        BITCOIN_ASSERT(std::distance(begin, end) >= 46 + 26 + 8 + 1);
        return;
    }

    BITCOIN_ASSERT(deserial.iterator() == begin + satoshi_size());
    BITCOIN_ASSERT(deserial.iterator() == end);
}

} // end message
} // end libbitcoin

#endif
