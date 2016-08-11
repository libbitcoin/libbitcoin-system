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
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string ping::command = "ping";
const uint32_t ping::version_minimum = version::level::minimum;
const uint32_t ping::version_maximum = version::level::maximum;

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
    return version < version::level::bip31 ? 0 : sizeof(nonce);
}

ping::ping()
  : ping(0)
{
}

ping::ping(uint64_t nonce)
  : nonce(nonce), valid_(nonce != 0)
{
}

bool ping::from_data(uint32_t version, const data_chunk& data)
{
    data_source istream(data);
    return from_data(version, istream);
}

bool ping::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool ping::from_data(uint32_t version, reader& source)
{
    reset();

    if (version >= version::level::bip31)
        nonce = source.read_8_bytes_little_endian();

    // Must track valid because is_valid doesn't include version parameter.
    // Otherwise when below bip31 then the object would always be invalid.
    valid_ = source;

    if (!valid_)
        reset();

    return valid_;
}

data_chunk ping::to_data(uint32_t version) const
{
    data_chunk data;
    data_sink ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void ping::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void ping::to_data(uint32_t version, writer& sink) const
{
    if (version >= version::level::bip31)
        sink.write_8_bytes_little_endian(nonce);
}

bool ping::is_valid() const
{
    return valid_;
}

void ping::reset()
{
    nonce = 0;
}

uint64_t ping::serialized_size(uint32_t version) const
{
    return satoshi_fixed_size(version);
}

bool operator==(const ping& left, const ping& right)
{
    // Nonce should be zero if not used.
    return (left.nonce == right.nonce);
}

bool operator!=(const ping& left, const ping& right)
{
    // Nonce should be zero if not used.
    return !(left == right);
}

} // namspace message
} // namspace libbitcoin
