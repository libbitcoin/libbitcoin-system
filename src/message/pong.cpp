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
#include <bitcoin/bitcoin/message/pong.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::pong::command = "pong";

pong pong::factory_from_data(const data_chunk& data)
{
    pong instance;
    instance.from_data(data);
    return instance;
}

pong pong::factory_from_data(std::istream& stream)
{
    pong instance;
    instance.from_data(stream);
    return instance;
}

pong pong::factory_from_data(reader& source)
{
    pong instance;
    instance.from_data(source);
    return instance;
}

uint64_t pong::satoshi_fixed_size()
{
    return nonce_::satoshi_fixed_size();
}

pong::pong()
  : pong(0)
{
}

pong::pong(uint64_t nonce)
  : nonce_(nonce)
{
}

} // namspace message
} // namspace libbitcoin
