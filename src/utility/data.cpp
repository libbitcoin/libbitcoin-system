/**
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
#include <bitcoin/bitcoin/utility/data.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

data_chunk slice(data_slice bytes, size_t start, size_t end)
{
    BITCOIN_ASSERT(start <= bytes.size());

    const auto& data = bytes.data();
    return
    {
        &data[start], &data[end]
    };
}

data_chunk slice(data_slice bytes, const bounds& range)
{
    return slice(bytes, range.start, range.end);
}

void split(data_chunk& lower, data_chunk& upper, data_slice bytes, size_t size)
{
    BITCOIN_ASSERT(bytes.size() == size);

    const size_t front = size / 2;
    const size_t rest = size - front;
    lower.assign(std::begin(bytes), std::end(bytes) - front);
    upper.assign(std::begin(bytes) + rest, std::end(bytes));
}

data_chunk to_data_chunk(data_slice bytes)
{
    return data_chunk(std::begin(bytes), std::end(bytes));
}

data_chunk xor_data(data_slice bytes1, data_slice bytes2, size_t offset,
    size_t length)
{
    return xor_data(bytes1, bytes2, offset, offset, length);
}

data_chunk xor_data(data_slice bytes1, data_slice bytes2, size_t offset1,
    size_t offset2, size_t length)
{
    BITCOIN_ASSERT(offset1 + length <= bytes1.size());
    BITCOIN_ASSERT(offset2 + length <= bytes2.size());

    const auto& data1 = bytes1.data();
    const auto& data2 = bytes2.data();
    data_chunk out;
    for (size_t i = 0; i < length; i++)
        out.push_back(data1[i + offset1] ^ data2[i + offset2]);

    return out;
}

} // namespace libbitcoin
