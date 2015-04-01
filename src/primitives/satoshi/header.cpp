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
#include <bitcoin/bitcoin/primitives/satoshi/header.hpp>

namespace libbitcoin {

header_type::header_type()
{
}

header_type::header_type(const data_chunk& value)
: header_type(value.begin(), value.end())
{
}

header_type::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_4_bytes(magic);
    serial.write_fixed_string(command, command_size);
    serial.write_4_bytes(payload_length);
    if (checksum != 0)
        serial.write_4_bytes(checksum);
    return result;
}

size_t header_type::satoshi_size() const
{
    return 20 + (checksum == 0 ? 0 : 4);
}

}
