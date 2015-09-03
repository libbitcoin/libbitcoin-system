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

uint8_t last_byte(data_slice buffer)
{
    if (buffer.empty())
        return 0x00;

    return buffer.data()[buffer.size() - 1];
}

data_chunk xor_data(data_slice buffer1, data_slice buffer2, size_t offset,
    size_t length)
{
    return xor_offset(buffer1, buffer2, offset, offset, length);
}

data_chunk xor_offset(data_slice buffer1, data_slice buffer2,
    size_t buffer1_offset, size_t buffer2_offset, size_t length)
{
    BITCOIN_ASSERT(buffer1_offset + length <= buffer1.size());
    BITCOIN_ASSERT(buffer2_offset + length <= buffer2.size());

    const auto& data1 = buffer1.data();
    const auto& data2 = buffer2.data();
    data_chunk out;
    for(size_t i = 0; i < length; i++)
        out.push_back(data1[i + buffer1_offset] ^ data2[i + buffer2_offset]);

    return out;
}

} // namespace libbitcoin
