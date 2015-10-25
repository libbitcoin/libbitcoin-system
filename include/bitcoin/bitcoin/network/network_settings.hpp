/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
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
#ifndef LIBBITCOIN_NETWORK_SETTINGS_HPP
#define LIBBITCOIN_NETWORK_SETTINGS_HPP

#include <cstdint>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/config/endpoint.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace network {

// This file is network_settings.hpp to deconflict with wallet::settings.hpp.
// This is in network vs. config because plan to move to libbitcoin-network.
struct BC_API settings
{
    uint32_t threads;
    uint32_t identifier;
    uint16_t inbound_port;
    uint32_t inbound_connection_limit;
    uint32_t outbound_connections;
    uint16_t connect_attempts;
    uint32_t connect_timeout_seconds;
    uint32_t channel_handshake_seconds;
    uint32_t channel_revival_minutes;
    uint32_t channel_heartbeat_minutes;
    uint32_t channel_inactivity_minutes;
    uint32_t channel_expiration_minutes;
    uint32_t channel_germination_seconds;
    uint32_t host_pool_capacity;
    bool relay_transactions;
    boost::filesystem::path hosts_file;
    boost::filesystem::path debug_file;
    boost::filesystem::path error_file;
    config::authority self;
    config::authority::list blacklists;
    config::endpoint::list seeds;

    boost::posix_time::time_duration connect_timeout() const
    {
        using namespace boost::posix_time;
        return time_duration(0, 0, connect_timeout_seconds);
    }

    boost::posix_time::time_duration channel_handshake() const
    {
        using namespace boost::posix_time;
        return time_duration(0, 0, channel_handshake_seconds);
    }

    boost::posix_time::time_duration channel_revival() const
    {
        using namespace boost::posix_time;
        return time_duration(0, channel_revival_minutes, 0);
    }

    boost::posix_time::time_duration channel_heartbeat() const
    {
        using namespace boost::posix_time;
        return time_duration(0, channel_heartbeat_minutes, 0);
    }

    boost::posix_time::time_duration channel_inactivity() const
    {
        using namespace boost::posix_time;
        return time_duration(0, channel_inactivity_minutes, 0);
    }

    boost::posix_time::time_duration channel_expiration() const
    {
        using namespace boost::posix_time;
        return time_duration(0, channel_expiration_minutes, 0);
    }

    boost::posix_time::time_duration channel_germination() const
    {
        using namespace boost::posix_time;
        return time_duration(0, 0, channel_germination_seconds);
    }
};

} // namespace network
} // namespace libbitcoin

#endif
