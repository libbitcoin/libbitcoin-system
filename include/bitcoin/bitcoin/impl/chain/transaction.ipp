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
#ifndef LIBBITCOIN_CHAIN_TRANSACTION_IPP
#define LIBBITCOIN_CHAIN_TRANSACTION_IPP

namespace libbitcoin {
namespace chain {

template <typename Iterator, bool SafeCheckLast>
void transaction::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    version_ = deserial.read_4_bytes();

    uint64_t tx_in_count = deserial.read_variable_uint();

    for (size_t tx_in_i = 0; tx_in_i < tx_in_count; ++tx_in_i)
    {
        transaction_input input(deserial);
        inputs_.push_back(input);
    }

    uint64_t tx_out_count = deserial.read_variable_uint();

    for (size_t tx_out_i = 0; tx_out_i < tx_out_count; ++tx_out_i)
    {
        transaction_output output(deserial);
        outputs_.push_back(output);
    }

    locktime_ = deserial.read_4_bytes();
}

template <typename Iterator, bool SafeCheckLast>
transaction::transaction(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
transaction::transaction(const Iterator begin, const Iterator end)
{
    deserializer<Iterator, true> deserial = make_deserializer(begin, end);
    deserialize(deserial);

    BITCOIN_ASSERT(std::distance(begin, deserial.iterator()) == satoshi_size());
}

} // end chain
} // end libbitcoin

#endif
