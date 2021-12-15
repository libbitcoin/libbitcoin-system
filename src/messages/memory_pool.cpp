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
#include <bitcoin/system/messages/memory_pool.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

const std::string memory_pool::command = "mempool";
const identifier memory_pool::id = identifier::memory_pool;
const uint32_t memory_pool::version_minimum = version::level::bip35;
const uint32_t memory_pool::version_maximum = version::level::maximum;

// static
size_t memory_pool::size(uint32_t)
{
    return zero;
}

memory_pool memory_pool::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    return {};
}

void memory_pool::serialize(uint32_t DEBUG_ONLY(version),
    writer& DEBUG_ONLY(sink)) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)
    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
