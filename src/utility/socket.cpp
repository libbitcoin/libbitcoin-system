/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/system/utility/socket.hpp>

#include <memory>
#include <bitcoin/system/config/authority.hpp>
#include <bitcoin/system/utility/asio.hpp>
#include <bitcoin/system/utility/thread.hpp>
#include <bitcoin/system/utility/threadpool.hpp>

namespace libbitcoin {
namespace system {

socket::socket(threadpool& thread)
  : thread_(thread),
    socket_(thread_.service())
    /*, CONSTRUCT_TRACK(socket) */
{
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

void socket::stop()
{
    // Handling socket error codes creates exception safety.
    boost_code ignore;

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section.
    unique_lock lock(mutex_);

    // Signal the end of oustanding async socket functions (read).
    socket_.shutdown(asio::socket::shutdown_both, ignore);

    // BUGBUG: this is documented to fail on Windows XP and Server 2003.
    // DO NOT CLOSE SOCKET, IT TERMINATES WORK IMMEDIATELY RESULTING IN LEAKS
    socket_.cancel(ignore);
    ///////////////////////////////////////////////////////////////////////////
}

} // namespace system
} // namespace libbitcoin
