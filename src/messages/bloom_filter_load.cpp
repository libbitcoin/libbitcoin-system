/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/messages/bloom_filter_load.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string bloom_filter_load::command = "filterload";
const identifier bloom_filter_load::id = identifier::bloom_filter_load;
const uint32_t bloom_filter_load::version_minimum = version::level::bip37;
const uint32_t bloom_filter_load::version_maximum = version::level::maximum;

// static
bloom_filter_load bloom_filter_load::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    const auto read_hash_functions = [](reader& source)
    {
        const auto hash_functions = source.read_4_bytes_little_endian();

        if (hash_functions > max_filter_functions)
            source.invalidate();

        return hash_functions;
    };

    return
    {
        source.read_bytes(source.read_size(max_bloom_filter_load)),
        read_hash_functions(source),
        source.read_4_bytes_little_endian(),
        source.read_byte()
    };
}

void bloom_filter_load::serialize(uint32_t DEBUG_ONLY(version), writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_variable(filter.size());
    sink.write_bytes(filter);
    sink.write_4_bytes_little_endian(hash_functions);
    sink.write_4_bytes_little_endian(tweak);
    sink.write_byte(flags);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t bloom_filter_load::size(uint32_t) const
{
    return
        variable_size(filter.size()) + filter.size() +
        sizeof(uint32_t) +
        sizeof(uint32_t) +
        sizeof(uint8_t);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
