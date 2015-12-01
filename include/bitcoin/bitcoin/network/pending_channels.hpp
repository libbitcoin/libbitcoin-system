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
#ifndef LIBBITCOIN_NETWORK_PENDING_CHANNELS_HPP
#define LIBBITCOIN_NETWORK_PENDING_CHANNELS_HPP

#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>

namespace libbitcoin {
namespace network {

/// Class to manage a pending channel pool, thread and lock safe.
class BC_API pending_channels
{
public:
    typedef std::function<void(bool)> truth_handler;
    typedef std::function<void(const code&)> result_handler;
    
    pending_channels();
    ~pending_channels();

    /// This class is not copyable.
    pending_channels(const pending_channels&) = delete;
    void operator=(const pending_channels&) = delete;

    void exists(uint64_t version_nonce, truth_handler handler);
    void store(channel::ptr channel, result_handler handler);
    void remove(channel::ptr channel, result_handler handler);

private:
    typedef std::vector<channel::ptr> list;

    bool safe_store(channel::ptr channel);
    bool safe_remove(channel::ptr channel);
    bool safe_exists(uint64_t version_nonce);

    list channels_;
    std::mutex mutex_;
};

} // namespace network
} // namespace libbitcoin

#endif

