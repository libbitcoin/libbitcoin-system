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
#ifndef LIBBITCOIN_NETWORK_PENDING_SOCKETS_HPP
#define LIBBITCOIN_NETWORK_PENDING_SOCKETS_HPP

#include <mutex>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>

namespace libbitcoin {
namespace network {

/// Class to manage a pending socket pool, thread and lock safe.
class BC_API pending_sockets
{
public:    
    pending_sockets();
    ~pending_sockets();

    /// This class is not copyable.
    pending_sockets(const pending_sockets&) = delete;
    void operator=(const pending_sockets&) = delete;

    void clear();
    void store(asio::socket_ptr socket);
    void remove(asio::socket_ptr socket);

private:
    typedef std::vector<asio::socket_ptr> list;

    bool safe_clear();
    bool safe_store(asio::socket_ptr socket);
    bool safe_remove(asio::socket_ptr socket);

    list sockets_;
    std::mutex mutex_;
};

} // namespace network
} // namespace libbitcoin

#endif

