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
#include <bitcoin/bitcoin/message/get_address.hpp>
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>

namespace libbitcoin {
namespace message {

void get_address::reset()
{
}

bool get_address::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool get_address::from_data(std::istream& stream)
{
    reset();
    return stream;
}

data_chunk get_address::to_data() const
{
    data_chunk result(satoshi_size());
    return result;
}

uint64_t get_address::satoshi_size() const
{
    return get_address::satoshi_fixed_size();
}

uint64_t get_address::satoshi_fixed_size()
{
    return 0;
}

} // end message
} // end libbitcoin
