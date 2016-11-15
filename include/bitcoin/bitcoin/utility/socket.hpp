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
#ifndef LIBBITCOIN_SOCKET_HPP
#define LIBBITCOIN_SOCKET_HPP

#include <memory>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
////#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

/// The underlying socket is not thread safe and can fail if used concurrently
/// for any combination of concurrent read/write calls. All calls to async_read
/// and async_write must be guarded to ensure non-concurrent execution only.
/// A concurrent write may become interleaved with another by asio due to the
/// manner in which it breaks up messages. By passing a lock to the write 
/// completion handler other writes on the socket are blocked until all message
/// parts have been sent.
class BC_API socket
    /*: public track<socket>*/
{
public:
    typedef std::shared_ptr<socket> ptr;

    /// Construct an instance.
    socket(threadpool& threadpool);

    /// This class is not copyable.
    socket(const socket&) = delete;
    void operator=(const socket&) = delete;

    /// The underlying socket.
    asio::socket& get();

    /// Obtain the authority of the remote endpoint.
    config::authority get_authority() const;

    /// Signal cancel of all outstanding work on the socket.
    virtual void stop();

private:
    // This is partially protected by mutex.
    asio::socket socket_;
};

} // namespace libbitcoin

#endif
