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

#include <bitcoin/bitcoin/utility/deserializer.hpp>
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

network_address::network_address(const data_chunk& value)
{
    auto deserializer = make_deserializer(value.begin(), value.end());
    services_ = deserializer.read_8_bytes();
    // Read IP address
    ip_ = deserializer.read_bytes<16>();
    port_ = deserializer.read_big_endian<uint16_t>();
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

network_address::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_8_bytes(services_);
    serial.write_data(ip_);
    serial.write_big_endian<uint16_t>(port_);
    return result;
}

size_t network_address::satoshi_size() const
{
    return network_address::satoshi_fixed_size();
}

size_t network_address::satoshi_fixed_size()
{
    return 26;
}

} // end message
} // end libbitcoin
