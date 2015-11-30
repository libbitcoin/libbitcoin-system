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
#ifndef LIBBITCOIN_NETWORK_PENDING_HPP
#define LIBBITCOIN_NETWORK_PENDING_HPP

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>

namespace libbitcoin {
namespace network {

/// Class to manage a pending channel pool, thread safe.
class BC_API pending
{
public:
    typedef std::function<void(bool)> truth_handler;
    typedef std::function<void(const code&)> result_handler;
    
    pending();
    ~pending();

    /// This class is not copyable.
    pending(const pending&) = delete;
    void operator=(const pending&) = delete;

    void exists(uint64_t version_nonce, truth_handler handler);
    void store(const channel::ptr& channel, result_handler handler);
    void remove(const channel::ptr& channel, result_handler handler);

private:
    typedef std::vector<channel::ptr> list;

    list buffer_;
    std::mutex buffer_mutex_;
};

} // namespace network
} // namespace libbitcoin

#endif

