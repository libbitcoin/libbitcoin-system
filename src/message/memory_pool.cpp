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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/memory_pool.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string memory_pool::command = "mempool";
const uint32_t memory_pool::version_minimum = version::level::bip35;
const uint32_t memory_pool::version_maximum = version::level::maximum;

memory_pool memory_pool::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    memory_pool instance;
    instance.from_data(version, data);
    return instance;
}

memory_pool memory_pool::factory_from_data(uint32_t version,
    std::istream& stream)
{
    memory_pool instance;
    instance.from_data(version, stream);
    return instance;
}

memory_pool memory_pool::factory_from_data(uint32_t version,
    reader& source)
{
    memory_pool instance;
    instance.from_data(version, source);
    return instance;
}

memory_pool::memory_pool()
{
    reset();
}

bool memory_pool::is_valid() const
{
    return !insufficient_version_;
}

void memory_pool::reset()
{
    insufficient_version_ = false;
}

bool memory_pool::from_data(uint32_t version, const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(version, istream);
}

bool memory_pool::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool memory_pool::from_data(uint32_t version, reader& source)
{
    reset();
    insufficient_version_ = (version < memory_pool::version_minimum);
    return !insufficient_version_;
}

data_chunk memory_pool::to_data(uint32_t version) const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void memory_pool::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void memory_pool::to_data(uint32_t version, writer& sink) const
{
}

uint64_t memory_pool::serialized_size(uint32_t version) const
{
    return memory_pool::satoshi_fixed_size(version);
}

uint64_t memory_pool::satoshi_fixed_size(uint32_t version)
{
    return 0;
}

} // end message
} // end libbitcoin
