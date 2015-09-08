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

data_chunk xor_data(data_slice buffer1, data_slice buffer2, size_t offset,
    size_t length)
{
    return xor_data(buffer1, buffer2, offset, offset, length);
}

data_chunk xor_data(data_slice buffer1, data_slice buffer2, size_t offset1,
    size_t offset2, size_t length)
{
    BITCOIN_ASSERT(offset1 + length <= buffer1.size());
    BITCOIN_ASSERT(offset2 + length <= buffer2.size());

    const auto& data1 = buffer1.data();
    const auto& data2 = buffer2.data();
    data_chunk out;
    for (size_t i = 0; i < length; i++)
        out.push_back(data1[i + offset1] ^ data2[i + offset2]);

    return out;
}

} // namespace libbitcoin
