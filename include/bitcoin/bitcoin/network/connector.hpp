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
#ifndef LIBBITCOIN_NETWORK_CONNECTOR_HPP
#define LIBBITCOIN_NETWORK_CONNECTOR_HPP

#include <cstdint>
#include <memory>
#include <boost/date_time.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/timeout.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/deadline.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/**
 * Class wrapper for boost::asio::async_connect.
 * This simplifies invocation and eliminates boost-speific error handling.
 */
class connector
  : public std::enable_shared_from_this<connector>, track<connector>
{
public:
    typedef std::shared_ptr<connector> ptr;
    typedef std::function<void(const code&, channel::ptr)> handler;

    /**
     * Construct a socket connector.
     * @param[in]  pool           The thread pool used by the connector.
     * @param[in]  network_magic  The magic bytes that identify the network.
     * @param[in]  timeout        The collection of network timeout settings.
     */
    connector(threadpool& pool, uint32_t network_magic,
        const timeout& timeout);

    /// This class is not copyable.
    connector(const connector&) = delete;
    void operator=(const connector&) = delete;

    /**
     * Connect to the specified endpoint.
     * @param[in]  endpoint_iterator The endpoint iterator to connect with.
     * @param[in]  handle_connect    Will be invoked upon expire or connection.
     */
    void connect(asio::iterator endpoint_iterator, handler handle_connect);

private:
    void create_channel(const boost_code& ec, asio::iterator,
        asio::socket_ptr socket, handler complete);
    void handle_timer(const code& ec, handler complete);

    threadpool& pool_;
    uint32_t magic_;
    const timeout& timeouts_;
    deadline::ptr deadline_;
};

} // namespace network
} // namespace libbitcoin

#endif
