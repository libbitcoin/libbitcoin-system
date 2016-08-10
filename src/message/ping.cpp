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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/ping.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string message::ping::command = "ping";
const uint32_t message::ping::version_minimum = peer_minimum_version;
const uint32_t message::ping::version_maximum = protocol_version;

ping ping::factory_from_data(uint32_t version, const data_chunk& data)
{
    ping instance;
    instance.from_data(version, data);
    return instance;
}

ping ping::factory_from_data(uint32_t version, std::istream& stream)
{
    ping instance;
    instance.from_data(version, stream);
    return instance;
}

ping ping::factory_from_data(uint32_t version, reader& source)
{
    ping instance;
    instance.from_data(version, source);
    return instance;
}

uint64_t ping::satoshi_fixed_size(uint32_t version)
{
    return nonce_::satoshi_fixed_size(version);
}

ping::ping()
  : ping(0)
{
}

ping::ping(uint64_t nonce)
  : nonce_(nonce)
{
}

} // namspace message
} // namspace libbitcoin
