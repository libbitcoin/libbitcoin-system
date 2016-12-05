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
#include <bitcoin/bitcoin/utility/socket.hpp>

#include <memory>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

socket::socket()
  : thread_(1),
    socket_(thread_.service())
    /*, CONSTRUCT_TRACK(socket) */
{
}

socket::~socket()
{
    close();
}

config::authority socket::authority() const
{
    boost_code ec;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section.
    mutex_.lock_shared();

    const auto endpoint = socket_.remote_endpoint(ec);

    mutex_.unlock_shared();
    ///////////////////////////////////////////////////////////////////////////

    return ec ? config::authority() : config::authority(endpoint);
}

asio::socket& socket::get()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section.
    shared_lock lock(mutex_);

    return socket_;
    ///////////////////////////////////////////////////////////////////////////
}

threadpool& socket::thread()
{
    return thread_;
}

void socket::stop()
{
    // Handling socket error codes creates exception safety.
    boost_code ignore;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section.
    mutex_.lock();

    // Signal the end of oustanding async socket functions (read).
    socket_.shutdown(asio::socket::shutdown_both, ignore);

    // BUGBUG: this is documented to fail on Windows XP and Server 2003.
    socket_.cancel(ignore);

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    // Signal the end of oustanding work and no new work.
    thread_.shutdown();
}

void socket::close()
{
    stop();

    // Handling socket error codes creates exception safety.
    boost_code ignore;

    // DO NOT CLOSE THE SOCKET, IT TERMINATES WORK IMMEDIATELY REUSLTING IN LEAKS
    ///////////////////////////////////////////////////////////////////////////////
    ////// Critical Section.
    ////mutex_.lock();
    ////socket_.close(ignore);
    ////mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////////

    thread_.join();
}

} // namespace libbitcoin
