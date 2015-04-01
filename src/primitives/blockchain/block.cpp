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
#include <bitcoin/bitcoin/primitives/blockchain/block.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {

block_type::block_type()
{
}

block_type::block_type(const data_chunk& value)
    : block_type(value.begin(), value.end())
{
}

block_type::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());

    data_chunk raw_header = header;
    serial.write_data(raw_header);

    serial.write_variable_uint(transactions.size());

    for (const transaction_type& tx : transactions)
    {
        data_chunk raw_tx = tx;
        serial.write_data(raw_tx);
    }

    return result;
}

size_t block_type::satoshi_size() const
{
    size_t block_size = header.satoshi_size()
        + variable_uint_size(transactions.size());

    for (const transaction_type& tx : transactions)
        block_size += tx.satoshi_size();

    return block_size;
}

}
