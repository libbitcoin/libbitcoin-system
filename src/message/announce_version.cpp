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
#include <bitcoin/bitcoin/message/announce_version.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

announce_version::announce_version()
    : version_(), services_(), timestamp_(), address_me_(), address_you_(),
      nonce_(), user_agent_(), start_height_()
{
}

announce_version::announce_version(const uint32_t version,
    const uint64_t services, const uint64_t timestamp,
    const network_address& address_me, const network_address& address_you,
    const uint64_t nonce, const std::string& user_agent,
    const uint32_t start_height)
    : version_(version), services_(services), timestamp_(timestamp),
      address_me_(address_me), address_you_(address_you), nonce_(nonce),
      user_agent_(user_agent), start_height_(start_height)
{
}

announce_version::announce_version(std::istream& stream)
{
    version_ = read_4_bytes(stream);
    services_ = read_8_bytes(stream);
    timestamp_ = read_8_bytes(stream);

    address_me_ = network_address(stream);

    // Ignored field
    address_me_.timestamp(0);

    if (version_ < 106)
    {
        return;
    }

    address_you_ = network_address(stream);

    // Ignored field
    address_you_.timestamp(0);

    nonce_ = read_8_bytes(stream);
    user_agent_ = read_string(stream);

    if (version_ < 209)
    {
        return;
    }

    start_height_ = read_4_bytes(stream);

    if (stream.fail())
        throw std::ios_base::failure("announce_version");
}

announce_version::announce_version(const data_chunk& value)
: announce_version(value.begin(), value.end())
{
}

uint32_t announce_version::version() const
{
    return version_;
}

void announce_version::version(uint32_t value)
{
    version_ = value;
}

uint64_t announce_version::services() const
{
    return services_;
}

void announce_version::services(uint64_t value)
{
    services_ = value;
}

uint64_t announce_version::timestamp() const
{
    return timestamp_;
}

void announce_version::timestamp(uint64_t value)
{
    timestamp_ = value;
}

network_address& announce_version::address_me()
{
    return address_me_;
}

const network_address& announce_version::address_me() const
{
    return address_me_;
}

void announce_version::address_me(const network_address& value)
{
    address_me_ = value;
}

network_address& announce_version::address_you()
{
    return address_you_;
}

const network_address& announce_version::address_you() const
{
    return address_you_;
}

void announce_version::address_you(const network_address& value)
{
    address_you_ = value;
}

uint64_t announce_version::nonce() const
{
    return nonce_;
}

void announce_version::nonce(uint64_t value)
{
    nonce_ = value;
}

std::string& announce_version::user_agent()
{
    return user_agent_;
}

const std::string& announce_version::user_agent() const
{
    return user_agent_;
}

void announce_version::user_agent(const std::string& value)
{
    user_agent_ = value;
}

uint32_t announce_version::start_height() const
{
    return start_height_;
}

void announce_version::start_height(uint32_t value)
{
    start_height_ = value;
}

announce_version::operator const data_chunk() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_4_bytes(version_);
    serial.write_8_bytes(services_);
    serial.write_8_bytes(timestamp_);
    data_chunk raw_address_me = address_me_;
    serial.write_data(raw_address_me);
    data_chunk raw_address_you = address_you_;
    serial.write_data(raw_address_you);
    serial.write_8_bytes(nonce_);
    serial.write_string(user_agent_);
    serial.write_4_bytes(start_height_);
    return result;
}

size_t announce_version::satoshi_size() const
{
    return 84 +
        variable_uint_size(user_agent_.size()) + user_agent_.size();
}

} // end message
} // end libbitcoin
