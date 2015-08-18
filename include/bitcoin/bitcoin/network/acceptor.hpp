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
#ifndef LIBBITCOIN_NETWORK_ACCEPTOR_HPP
#define LIBBITCOIN_NETWORK_ACCEPTOR_HPP

#include <functional>
#include <memory>
#include <system_error>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/channel_proxy.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

class acceptor;
typedef std::shared_ptr<acceptor> acceptor_ptr;

class BC_API acceptor
  : public std::enable_shared_from_this<acceptor>
{
public:

    typedef std::shared_ptr<boost::asio::ip::tcp::acceptor> tcp_acceptor_ptr;

    typedef std::function<void (const std::error_code&, channel_ptr)>
        accept_handler;

    typedef std::function<void(const std::error_code&, acceptor_ptr)>
        listen_handler;

    acceptor(threadpool& pool, tcp_acceptor_ptr accept,
        const timeout& timeouts=timeout::defaults);

    /// This class is not copyable.
    acceptor(const acceptor&) = delete;
    void operator=(const acceptor&) = delete;

    void accept(accept_handler handle_accept);

private:
    void create_channel(const boost::system::error_code& ec,
        socket_ptr socket, accept_handler handle_accept);

    threadpool& pool_;
    const timeout& timeouts_;
    tcp_acceptor_ptr tcp_acceptor_;
};

} // namespace network
} // namespace libbitcoin

#endif

