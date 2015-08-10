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
#include <bitcoin/bitcoin/network/timeout.hpp>

#include <cstdint>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/system/error_code.hpp>

namespace libbitcoin {
namespace network {

using boost::posix_time::seconds;
using boost::posix_time::minutes;

const timeout timeout::defaults;

timeout::timeout(
    uint32_t connect_timeout_seconds,
    uint32_t channel_handshake_minutes,
    uint32_t channel_revival_minutes,
    uint32_t channel_heartbeat_minutes,
    uint32_t channel_inactivity_minutes,
    uint32_t channel_expiration_minutes)
  : connect(0, 0, connect_timeout_seconds),
    handshake(0, channel_handshake_minutes, 0),
    revival(0, channel_revival_minutes, 0),
    heartbeat(0, channel_heartbeat_minutes, 0),
    inactivity(0, channel_inactivity_minutes, 0),
    expiration(0, channel_expiration_minutes, 0)
{
}

// TODO: wrap boost timers with our own and map error codes internally.
bool timeout::canceled(const boost::system::error_code& ec)
{
    return ec == boost::asio::error::operation_aborted;
}

} // namespace network
} // namespace libbitcoin
