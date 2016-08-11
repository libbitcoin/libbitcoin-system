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
#include <bitcoin/bitcoin/message/pong.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string pong::command = "pong";
const uint32_t pong::version_minimum = version::level::minimum;
const uint32_t pong::version_maximum = version::level::maximum;

pong pong::factory_from_data(uint32_t version, const data_chunk& data)
{
    pong instance;
    instance.from_data(version, data);
    return instance;
}

pong pong::factory_from_data(uint32_t version, std::istream& stream)
{
    pong instance;
    instance.from_data(version, stream);
    return instance;
}

pong pong::factory_from_data(uint32_t version, reader& source)
{
    pong instance;
    instance.from_data(version, source);
    return instance;
}

uint64_t pong::satoshi_fixed_size(uint32_t version)
{
    return sizeof(nonce);
}

bool pong::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool pong::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool pong::from_data(uint32_t version, reader& source)
{
    reset();

    nonce = source.read_8_bytes_little_endian();

    if (!source)
        reset();

    return source;
}

data_chunk pong::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void pong::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void pong::to_data(uint32_t version, writer& sink) const
{
    sink.write_8_bytes_little_endian(nonce);
}

bool pong::is_valid() const
{
    return (nonce != 0);
}

void pong::reset()
{
    nonce = 0;
}

uint64_t pong::serialized_size(uint32_t version) const
{
    return satoshi_fixed_size(version);
}

bool operator==(const pong& left, const pong& right)
{
    return (left.nonce == right.nonce);
}

bool operator!=(const pong& left, const pong& right)
{
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin
