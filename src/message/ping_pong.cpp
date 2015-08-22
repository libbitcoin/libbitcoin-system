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
#include <bitcoin/bitcoin/message/ping_pong.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

bool ping_pong::is_valid() const
{
    return (nonce != 0);
}

void ping_pong::reset()
{
    nonce = 0;
}

bool ping_pong::from_data(const data_chunk& data)
{
    data_source istream(data);
    return from_data(istream);
}

bool ping_pong::from_data(std::istream& stream)
{
    istream_reader source(stream);
    return from_data(source);
}

bool ping_pong::from_data(reader& source)
{
    reset();

    nonce = source.read_8_bytes_little_endian();

    if (!source)
        reset();

    return source;
}

data_chunk ping_pong::to_data() const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == satoshi_size());
    return data;
}

void ping_pong::to_data(std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(sink);
}

void ping_pong::to_data(writer& sink) const
{
    sink.write_8_bytes_little_endian(nonce);
}

uint64_t ping_pong::satoshi_size() const
{
    return ping_pong::satoshi_fixed_size();
}

uint64_t ping_pong::satoshi_fixed_size()
{
    return 8;
}

bool operator==(const ping_pong& left, const ping_pong& right)
{
    return (left.nonce == right.nonce);
}

bool operator!=(const ping_pong& left, const ping_pong& right)
{
    return !(left == right);
}

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
    return ping_pong::satoshi_fixed_size();
}

ping::ping()
{
}

ping::ping(const uint64_t nonce)
{
    this->nonce = nonce;
}

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
    return ping_pong::satoshi_fixed_size();
}

pong::pong()
{
}

pong::pong(const uint64_t nonce)
{
    this->nonce = nonce;
}

} // namspace message
} // namspace libbitcoin
