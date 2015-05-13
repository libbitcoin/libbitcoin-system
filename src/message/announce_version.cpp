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
#include <bitcoin/bitcoin/utility/data_stream_host.hpp>
#include <bitcoin/bitcoin/utility/istream.hpp>
#include <bitcoin/bitcoin/utility/serializer.hpp>

namespace libbitcoin {
namespace message {

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

void announce_version::reset()
{
    version_ = 0;
    services_ = 0;
    timestamp_ = 0;
    address_me_.reset();
    address_you_.reset();
    nonce_ = 0;
    user_agent_.clear();
}

bool announce_version::from_data(const data_chunk& data)
{
    data_chunk_stream_host host(data);
    return from_data(host.stream);
}

bool announce_version::from_data(std::istream& stream)
{
    bool result = 0;

    reset();

    version_ = read_4_bytes(stream);
    services_ = read_8_bytes(stream);
    timestamp_ = read_8_bytes(stream);
    result = !stream.fail();

    if (result)
        result = address_me_.from_data(stream, false);

    if (result && (version_ >= 106))
    {
        result = address_you_.from_data(stream, false);

        nonce_ = read_8_bytes(stream);
        user_agent_ = read_string(stream);
        result = !stream.fail();

        if (version_ >= 209)
            start_height_ = read_4_bytes(stream);

        result &= !stream.fail();
    }

    if (!result)
        reset();

    return result;
}

data_chunk announce_version::to_data() const
{
    data_chunk result(satoshi_size());
    auto serial = make_serializer(result.begin());
    serial.write_4_bytes(version_);
    serial.write_8_bytes(services_);
    serial.write_8_bytes(timestamp_);
    serial.write_data(address_me_.to_data(false));
    serial.write_data(address_you_.to_data(false));
    serial.write_8_bytes(nonce_);
    serial.write_string(user_agent_);
    serial.write_4_bytes(start_height_);
    return result;
}

uint64_t announce_version::satoshi_size() const
{
    return 32 + (2 * network_address::satoshi_fixed_size(false)) +
        variable_uint_size(user_agent_.size()) + user_agent_.size();
}

} // end message
} // end libbitcoin
