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
#include <bitcoin/bitcoin/network/connections.hpp>

#include <algorithm>
#include <mutex>
#include <bitcoin/bitcoin/config/authority.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

#define NAME "connections"

connections::connections(threadpool& pool)
    : dispatch_(pool, NAME)
{
}

connections::~connections()
{
    BITCOIN_ASSERT_MSG(channels_.empty(), "Connections was not cleared.");
}

connections::list connections::safe_copy()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    return channels_;
    ///////////////////////////////////////////////////////////////////////////
}

void connections::stop(const code& ec)
{
    // The list is copied, which protects the iteration without a lock.
    auto channels = safe_copy();

    for (auto channel: channels)
        channel->stop(ec);
}

bool connections::safe_exists(const authority& address)
{
    const auto match = [&address](channel::ptr entry)
    {
        return entry->authority() == address;
    };

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    const auto it = std::find_if(channels_.begin(), channels_.end(), match);
    return it != channels_.end();
    ///////////////////////////////////////////////////////////////////////////
}

void connections::exists(const authority& address, truth_handler handler)
{
    handler(safe_exists(address));
}

bool connections::safe_remove(channel::ptr channel)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    const auto it = std::find(channels_.begin(), channels_.end(), channel);
    const auto found = it != channels_.end();
    if (found)
        channels_.erase(it);

    return found;
    ///////////////////////////////////////////////////////////////////////////
}

void connections::remove(channel::ptr channel, result_handler handler)
{
    handler(safe_remove(channel) ? error::success : error::not_found);
}

bool connections::safe_store(channel::ptr channel)
{
    const auto address = channel->authority();
    const auto match = [&address](channel::ptr entry)
    {
        return entry->authority() == address;
    };

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    const auto it = std::find_if(channels_.begin(), channels_.end(), match);
    const auto found = it != channels_.end();

    if (!found)
        channels_.push_back(channel);

    return found;
    ///////////////////////////////////////////////////////////////////////////
}

void connections::store(channel::ptr channel, result_handler handler)
{
    handler(safe_store(channel) ? error::address_in_use : error::success);
}

size_t connections::safe_count()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    return channels_.size();
    ///////////////////////////////////////////////////////////////////////////
}

void connections::count(count_handler handler)
{
    handler(safe_count());
}

} // namespace network
} // namespace libbitcoin
