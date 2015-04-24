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
#ifndef LIBBITCOIN_NETWORK_PEER_HPP
#define LIBBITCOIN_NETWORK_PEER_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <thread>
#include <boost/asio.hpp>
#include <boost/date_time.hpp>
#include <boost/utility.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/acceptor.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/network/connector.hpp>
#include <bitcoin/bitcoin/network/handshake.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class BC_API peer
{
public:
    peer(threadpool& pool, const timeout& timeouts=timeout::defaults);

    /// This class is not copyable.
    peer(const peer&) = delete;
    void operator=(const peer&) = delete;

    void listen(uint16_t port, acceptor::listen_handler handle_listen);
    void connect(const std::string& hostname, uint16_t port,
        connector::connect_handler handle_connect);

private:
    typedef std::shared_ptr<boost::asio::ip::tcp::resolver> resolver_ptr;
    typedef std::shared_ptr<boost::asio::ip::tcp::resolver::query> query_ptr;

    void resolve_handler(const boost::system::error_code& ec,
        boost::asio::ip::tcp::resolver::iterator endpoint_iterator,
        connector::connect_handler handle_connect, resolver_ptr, query_ptr);

    threadpool& pool_;
    const timeout& timeouts_;
};

} // namespace network
} // namespace libbitcoin

#endif
