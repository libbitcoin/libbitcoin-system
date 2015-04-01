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
#ifndef LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_BLOCK_IPP
#define LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_BLOCK_IPP

namespace libbitcoin {

template <typename Iterator, bool SafeCheckLast>
void block_type::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    header = block_header_type(deserial.read_data(
        block_header_type::satoshi_fixed_size()));

    uint64_t tx_count = deserial.read_variable_uint();

    for (size_t tx_i = 0; tx_i < tx_count; ++tx_i)
    {
        transaction_type tx(deserial);
        transactions.push_back(std::move(tx));
    }
}

template <typename Iterator, bool SafeCheckLast>
block_type::block_type(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
block_type::block_type(const Iterator begin, const Iterator end)
{
    deserializer<Iterator, true> deserial = make_deserializer(begin, end);
    deserialize(deserial);

    BITCOIN_ASSERT(std::distance(begin, deserial.iterator()) == satoshi_size());
}

}

#endif
