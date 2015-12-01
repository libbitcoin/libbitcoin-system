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

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/network/network_settings.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

/// Create inbound socket connections, thread and lock safe.
class BC_API acceptor
  : public std::enable_shared_from_this<acceptor>, track<acceptor>
{
public:
    typedef std::shared_ptr<acceptor> ptr;
    typedef std::function<void(const code&)> result_handler;
    typedef std::function<void(const code&, channel::ptr)> accept_handler;

    /// Construct an instance.
    acceptor(threadpool& pool, const settings& settings);

    /// Validate acceptor stopped.
    ~acceptor();

    /// This class is not copyable.
    acceptor(const acceptor&) = delete;
    void operator=(const acceptor&) = delete;

    /// Start the listener on the specified port.
    void listen(uint16_t port, result_handler handler);

    /// Accept the next connection available, until canceled.
    void accept(accept_handler handler);

    /// Cancel the listener and all outstanding accept attempts.
    void stop();

private:
    bool stopped();
    void handle_accept(const boost_code& ec, asio::socket_ptr socket,
        accept_handler handler);

    std::atomic<bool> stopped_;
    threadpool& pool_;
    const settings& settings_;
    dispatcher dispatch_;
    asio::acceptor_ptr acceptor_;
    std::mutex mutex_;
};

} // namespace network
} // namespace libbitcoin

#endif
