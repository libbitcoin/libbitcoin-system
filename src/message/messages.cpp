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
#include <bitcoin/system/message/message.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/serialization/endian.hpp>

namespace libbitcoin {
namespace system {
namespace message {

// The little-endian encoding is a private data format.
uint32_t network_checksum(const data_slice& data)
{
    return from_little_endian<uint32_t>(bitcoin_hash(data));
}

size_t variable_uint_size(uint64_t value)
{
    if (value < 0xfd)
        return 1;
    else if (value <= 0xffff)
        return 3;
    else if (value <= 0xffffffff)
        return 5;
    else
        return 9;
}

} // namespace message
} // namespace system
} // namespace libbitcoin
