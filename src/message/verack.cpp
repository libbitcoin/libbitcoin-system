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
#include <bitcoin/bitcoin/message/verack.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

verack verack::factory_from_data(const data_chunk& data)
{
    verack instance;
    instance.from_data(data);
    return instance;
}

verack verack::factory_from_data(std::istream& stream)
{
    verack instance;
    instance.from_data(stream);
    return instance;
}

bool verack::is_valid() const
{
    return true;
}

void verack::reset()
{
}

bool verack::from_data(const data_chunk& data)
{
    byte_source<data_chunk> source(data);
    boost::iostreams::stream<byte_source<data_chunk>> istream(source);
    return from_data(istream);
}

bool verack::from_data(std::istream& stream)
{
    reset();
    return stream;
}

data_chunk verack::to_data() const
{
    data_chunk result(0);
    return result;
}

uint64_t verack::satoshi_size() const
{
    return verack::satoshi_fixed_size();
}


uint64_t verack::satoshi_fixed_size()
{
    return 0;
}

} // end message
} // end libbitcoin
