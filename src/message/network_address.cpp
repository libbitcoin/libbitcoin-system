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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/message/network_address.hpp>
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

network_address::network_address()
    : timestamp_(), services_(), ip_(), port_()
{
}

network_address::network_address(const uint32_t timestamp,
    const uint64_t services, const ip_address& ip, const uint16_t port)
    : timestamp_(timestamp), services_(services), ip_(ip), port_(port)
{
}

uint32_t network_address::timestamp() const
{
    return timestamp_;
}

void network_address::timestamp(uint32_t value)
{
    timestamp_ = value;
}

uint64_t network_address::services() const
{
    return services_;
}

void network_address::services(uint64_t value)
{
    services_ = value;
}

ip_address& network_address::ip()
{
    return ip_;
}

const ip_address& network_address::ip() const
{
    return ip_;
}

void network_address::ip(const ip_address& value)
{
    ip_ = value;
}

uint16_t network_address::port() const
{
    return port_;
}

void network_address::port(uint16_t value)
{
    port_ = value;
}

void network_address::reset()
{
    timestamp_ = 0;
    services_ = 0;
    ip_.fill(0);
    port_ = 0;
}

bool network_address::from_data(const data_chunk& data, bool with_timestamp)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream, with_timestamp);
}

bool network_address::from_data(std::istream& stream, bool with_timestamp)
{
    bool result = true;

    reset();

    if (with_timestamp)
        timestamp_ = read_4_bytes(stream);

    services_ = read_8_bytes(stream);
    ip_ = read_bytes<16>(stream);
    port_ = read_big_endian<uint16_t>(stream);

    if (stream.fail())
    {
        reset();
        result = false;
    }

    return result;
}

data_chunk network_address::to_data(bool with_timestamp) const
{
    data_chunk result(satoshi_size(with_timestamp));
    auto serial = make_serializer(result.begin());

    if (with_timestamp)
        serial.write_4_bytes(timestamp_);

    serial.write_8_bytes(services_);
    serial.write_data(ip_);
    serial.write_big_endian<uint16_t>(port_);
    return result;
}

uint64_t network_address::satoshi_size(bool with_timestamp) const
{
    return network_address::satoshi_fixed_size(with_timestamp);
}

uint64_t network_address::satoshi_fixed_size(bool with_timestamp)
{
    uint64_t result = 26;

    if (with_timestamp)
        result += 4;

    return result;
}

} // end message
} // end libbitcoin
