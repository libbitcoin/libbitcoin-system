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
#include <bitcoin/bitcoin/message/verack.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::verack::command = "verack";

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

verack verack::factory_from_data(reader& source)
{
    verack instance;
    instance.from_data(source);
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
    data_source istream(data);
    return from_data(istream);
}

bool verack::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool verack::from_data(reader& source)
{
    reset();
    return source;
}

data_chunk verack::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size());
    return data;
}

void verack::to_data(std::ostream& stream) const
{
}

uint64_t verack::serialized_size() const
{
    return verack::satoshi_fixed_size();
}


uint64_t verack::satoshi_fixed_size()
{
    return 0;
}

} // namspace message
} // namspace libbitcoin
