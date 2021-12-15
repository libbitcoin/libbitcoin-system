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
#include <bitcoin/system/messages/alert.hpp>

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/assert.hpp>
////#include <bitcoin/system/messages/alert_item.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {
    
const std::string alert::command = "alert";
const identifier alert::id = identifier::alert;
const uint32_t alert::version_minimum = version::level::minimum;
const uint32_t alert::version_maximum = version::level::maximum;

// static
alert alert::deserialize(uint32_t version, reader& source)
{
    if (version < version_minimum || version > version_maximum)
        source.invalidate();

    return
    {
        source.read_bytes(source.read_size(chain::max_block_size)),
        source.read_bytes(source.read_size(chain::max_block_size))
    };
}

void alert::serialize(uint32_t DEBUG_ONLY(version), writer& sink) const
{
    DEBUG_ONLY(const auto bytes = size(version);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    sink.write_variable(payload.size());
    sink.write_bytes(payload);
    sink.write_variable(signature.size());
    sink.write_bytes(signature);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

size_t alert::size(uint32_t) const
{
    return variable_size(payload.size()) + payload.size()
        + variable_size(signature.size()) + signature.size();
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
