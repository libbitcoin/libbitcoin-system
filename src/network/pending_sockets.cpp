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
#include <bitcoin/bitcoin/network/pending_sockets.hpp>

#include <algorithm>
#include <mutex>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {
namespace network {

pending_sockets::pending_sockets()
{
}

pending_sockets::~pending_sockets()
{
    BITCOIN_ASSERT_MSG(sockets_.empty(), "Pending sockets not cleared.");
}

// BUGBUG: socket::cancel fails with error::operation_not_supported
// on Windows XP and Windows Server 2003, but handler invocation is required.
// We should enable BOOST_ASIO_ENABLE_CANCELIO and BOOST_ASIO_DISABLE_IOCP
// on these platforms only. See: bit.ly/1YC0p9e

void pending_sockets::clear()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    // This will asynchronously invoke the handler of each pending connect.
    for (auto socket : sockets_)
        socket->cancel();

    sockets_.clear();
    ///////////////////////////////////////////////////////////////////////////
}

void pending_sockets::store(asio::socket_ptr socket)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    // We could test the list for the socket, which should not exist.
    sockets_.push_back(socket);
    ///////////////////////////////////////////////////////////////////////////
}

void pending_sockets::remove(asio::socket_ptr socket)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = std::find(sockets_.begin(), sockets_.end(), socket);

    // Clear can occur before unpend, so the entry may not be found.
    if (it != sockets_.end())
        sockets_.erase(it);
    ///////////////////////////////////////////////////////////////////////////
}

} // namespace network
} // namespace libbitcoin
