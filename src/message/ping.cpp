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
#include <bitcoin/bitcoin/message/ping.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::ping::satoshi_command = "ping";

ping ping::factory_from_data(const data_chunk& data)
{
    ping instance;
    instance.from_data(data);
    return instance;
}

ping ping::factory_from_data(std::istream& stream)
{
    ping instance;
    instance.from_data(stream);
    return instance;
}

ping ping::factory_from_data(reader& source)
{
    ping instance;
    instance.from_data(source);
    return instance;
}

uint64_t ping::satoshi_fixed_size()
{
    return nonce_::satoshi_fixed_size();
}

ping::ping()
  : ping(0)
{
}

ping::ping(const uint64_t nonce)
  : nonce_(nonce)
{
}

} // namspace message
} // namspace libbitcoin
