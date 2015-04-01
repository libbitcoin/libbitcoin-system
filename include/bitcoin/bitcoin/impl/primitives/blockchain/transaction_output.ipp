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
#ifndef LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_TRANSACTION_OUTPUT_IPP
#define LIBBITCOIN_BLOCKCHAIN_PRIMITIVE_TRANSACTION_OUTPUT_IPP

namespace libbitcoin {

template <typename Iterator, bool SafeCheckLast>
void transaction_output_type::deserialize(deserializer<Iterator, SafeCheckLast>& deserial)
{
    value = deserial.read_8_bytes();
    script = script_type(deserial);
}

template <typename Iterator, bool SafeCheckLast>
transaction_output_type::transaction_output_type(deserializer<Iterator, SafeCheckLast>& deserial)
{
    deserialize(deserial);
}

template<typename Iterator>
transaction_output_type::transaction_output_type(const Iterator begin, const Iterator end)
{
    deserializer<Iterator, true> deserial = make_deserializer(begin, end);
    deserialize(deserial);

    BITCOIN_ASSERT(std::distance(begin, deserial.iterator()) == satoshi_size());
}

}

#endif
