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
#include <bitcoin/bitcoin/message/merkle_block.hpp>

#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/message/version.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace message {

const std::string merkle_block::command = "merkleblock";
const uint32_t merkle_block::version_minimum = version::level::bip37;
const uint32_t merkle_block::version_maximum = version::level::maximum;

merkle_block merkle_block::factory_from_data(uint32_t version,
    const data_chunk& data)
{
    merkle_block instance;
    instance.from_data(version, data);
    return instance;
}

merkle_block merkle_block::factory_from_data(uint32_t version,
    std::istream& stream)
{
    merkle_block instance;
    instance.from_data(version, stream);
    return instance;
}

merkle_block merkle_block::factory_from_data(uint32_t version,
    reader& source)
{
    merkle_block instance;
    instance.from_data(version, source);
    return instance;
}

bool merkle_block::is_valid() const
{
    return !hashes.empty() || !flags.empty() || header.is_valid();
}

void merkle_block::reset()
{
    header.reset();
    hashes.clear();
    hashes.shrink_to_fit();
    flags.clear();
    flags.shrink_to_fit();
}

bool merkle_block::from_data(uint32_t version, const data_chunk& data)
{
    boost::iostreams::stream<byte_source<data_chunk>> istream(data);
    return from_data(version, istream);
}

bool merkle_block::from_data(uint32_t version, std::istream& stream)
{
    istream_reader source(stream);
    return from_data(version, source);
}

bool merkle_block::from_data(uint32_t version, reader& source)
{
    reset();

    bool result = !(version < merkle_block::version_minimum);
    uint64_t hash_count = 0;

    if (result)
        result = header.from_data(source, true);

    if (result)
    {
        hash_count = source.read_variable_uint_little_endian();
        result = source;
    }

    if (result)
        hashes.reserve(hash_count);

    for (uint64_t i = 0; (i < hash_count) && result; ++i)
    {
        hashes.push_back(source.read_hash());
        result = source;
    }

    if (result)
    {
        const auto size = source.read_variable_uint_little_endian();
        BITCOIN_ASSERT(size <= bc::max_size_t);
        const auto flag_count = static_cast<size_t>(size);
        flags = source.read_data(flag_count);
        result = source && (flags.size() == flag_count);
    }

    if (!result)
        reset();

    return result;
}

data_chunk merkle_block::to_data(uint32_t version) const
{
    data_chunk data;
    boost::iostreams::stream<byte_sink<data_chunk>> ostream(data);
    to_data(version, ostream);
    ostream.flush();
    BITCOIN_ASSERT(data.size() == serialized_size(version));
    return data;
}

void merkle_block::to_data(uint32_t version, std::ostream& stream) const
{
    ostream_writer sink(stream);
    to_data(version, sink);
}

void merkle_block::to_data(uint32_t version, writer& sink) const
{
    header.to_data(sink, true);

    sink.write_variable_uint_little_endian(hashes.size());

    for (const auto& hash : hashes)
        sink.write_hash(hash);

    sink.write_variable_uint_little_endian(flags.size());
    sink.write_data(flags);
}

uint64_t merkle_block::serialized_size(uint32_t version) const
{
    return header.serialized_size(true) +
        variable_uint_size(hashes.size()) + (hash_size * hashes.size()) +
        variable_uint_size(flags.size()) + flags.size();
}

bool operator==(const merkle_block& block_a,
    const merkle_block& block_b)
{
    bool result = (block_a.header == block_b.header) &&
        (block_a.hashes.size() == block_b.hashes.size()) &&
        (block_a.flags.size() == block_b.flags.size());

    for (hash_list::size_type i = 0; i < block_a.hashes.size() && result; i++)
        result = (block_a.hashes[i] == block_b.hashes[i]);

    for (data_chunk::size_type i = 0; i < block_a.flags.size() && result; i++)
        result = (block_a.flags[i] == block_b.flags[i]);

    return result;
}

bool operator!=(const merkle_block& block_a,
    const merkle_block& block_b)
{
    return !(block_a == block_b);
}

} // end message
} // end libbitcoin
