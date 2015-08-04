/**
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
#ifndef LIBBITCOIN_TIMEOUT_HPP
#define LIBBITCOIN_TIMEOUT_HPP

#include <cstdint>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace network {

class BC_API timeout
{
public:
    static const timeout defaults;

    timeout();
    timeout(
        uint32_t channel_expiration_minutes,
        uint32_t channel_timeout_minutes,
        uint32_t channel_heartbeat_minutes,
        uint32_t channel_handshake_minutes,
        uint32_t channel_revival_minutes,
        uint32_t connect_timeout_seconds);

    boost::posix_time::time_duration expiration;
    boost::posix_time::time_duration inactivity;
    boost::posix_time::time_duration heartbeat;
    boost::posix_time::time_duration handshake;
    boost::posix_time::time_duration revival;
    boost::posix_time::time_duration connection;
};

} // namespace network
} // namespace libbitcoin

#endif

