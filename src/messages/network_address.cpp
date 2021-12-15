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
#include <bitcoin/system/messages/network_address.hpp>

#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/messages/identifier.hpp>
#include <bitcoin/system/messages/message.hpp>
#include <bitcoin/system/messages/version.hpp>
#include <bitcoin/system/stream/stream.hpp>

namespace libbitcoin {
namespace system {
namespace messages {

constexpr auto ip_address_size = std::tuple_size<ip_address>::value;

// bytereader interface cannot expose templated method, so do here.
template <size_t Size>
data_array<Size> read_forward(reader& source)
{
    data_array<Size> out{};
    source.read_bytes(out.data(), Size);
    return out;
}

// static
size_t network_address::size(uint32_t, bool with_timestamp)
{
    return (with_timestamp ? sizeof(uint32_t) : zero)
        + sizeof(uint64_t)
        + ip_address_size
        + sizeof(uint16_t);
}

// static
network_address network_address::deserialize(uint32_t, reader& source,
    bool with_timestamp)
{
    return
    {
        with_timestamp ? source.read_4_bytes_little_endian() : 0u,
        source.read_8_bytes_little_endian(),
        read_forward<ip_address_size>(source),
        source.read_2_bytes_big_endian()
    };
}

void network_address::serialize(uint32_t DEBUG_ONLY(version), writer& sink,
    bool with_timestamp) const
{
    DEBUG_ONLY(const auto bytes = size(version, with_timestamp);)
    DEBUG_ONLY(const auto start = sink.get_position();)

    if (with_timestamp)
        sink.write_4_bytes_little_endian(timestamp);

    sink.write_8_bytes_little_endian(services);
    sink.write_bytes(ip.data(), ip.size());
    sink.write_2_bytes_big_endian(port);

    BITCOIN_ASSERT(sink && sink.get_position() - start == bytes);
}

} // namespace messages
} // namespace system
} // namespace libbitcoin
