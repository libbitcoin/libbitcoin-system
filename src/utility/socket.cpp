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
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

socket::socket(threadpool& threadpool)
  : socket_(threadpool.service())
    /*, CONSTRUCT_TRACK(socket) */
{
}

config::authority socket::get_authority() const
{
    boost_code ec;
    const auto endpoint = socket_.remote_endpoint(ec);
    return ec ? config::authority() : config::authority(endpoint);
}

// This should be used for async reading, as there is no interleaving.
asio::socket& socket::get()
{
    return socket_;
}

void socket::stop()
{
    // Handling socket error codes creates exception safety.
    boost_code ignore;

    // Signal the end of oustanding async socket functions (read).
    socket_.shutdown(asio::socket::shutdown_both, ignore);

    // BUGBUG: this is documented to fail on Windows XP and Server 2003.
    socket_.cancel(ignore);
}

} // namespace libbitcoin
