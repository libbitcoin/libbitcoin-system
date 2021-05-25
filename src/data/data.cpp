/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/data/data.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <bitcoin/system/data/data_slice.hpp>

namespace libbitcoin {
namespace system {

one_byte to_array(uint8_t byte)
{
    return { { byte } };
}

data_chunk to_chunk(uint8_t byte)
{
    return { byte };
}

data_chunk to_chunk(const data_slice& bytes)
{
    return bytes.to_chunk();
}

data_chunk build_chunk(const data_loaf& slices, size_t extra_reserve)
{
    size_t size = 0;
    for (const auto& slice: slices)
        size += slice.size();

    data_chunk out;
    out.reserve(size + extra_reserve);

    for (const auto& slice: slices)
        out.insert(out.end(), slice.begin(), slice.end());

    return out;
}

data_chunk splice(const data_slice& left, const data_slice& right)
{
    data_chunk out(left.size() + right.size());
    std::copy(right.begin(), right.end(),
        std::copy(left.begin(), left.end(), out.begin()));

    return out;
}

} // namespace system
} // namespace libbitcoin
