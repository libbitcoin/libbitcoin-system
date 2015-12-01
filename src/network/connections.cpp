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

namespace libbitcoin {
namespace network {

// It is not possible for this class to produce a deadlock.

connections::connections()
{
}

connections::~connections()
{
    BITCOIN_ASSERT_MSG(buffer_.empty(), "Connections was not cleared.");
}

connections::list connections::copy()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(buffer_mutex_);

    return buffer_;
    ///////////////////////////////////////////////////////////////////////////
}

void connections::stop(const code& ec)
{
    // TODO: dispatch this onto a concurrent thread (handlers).
    // This will stop all channels stored at the start of the call.
    auto buffer = copy();
    for (auto channel: buffer)
        channel->stop(ec);
}

void connections::exists(const authority& address, truth_handler handler)
{
    bool found;
    const auto match = [&address](const channel::ptr& entry)
    {
        return entry->authority() == address;
    };

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);

        const auto it = std::find_if(buffer_.begin(), buffer_.end(), match);
        found = it != buffer_.end();
    }
    ///////////////////////////////////////////////////////////////////////////

    handler(found);
}

void connections::remove(const channel::ptr& channel, result_handler handler)
{
    bool found;

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);

        auto it = std::find(buffer_.begin(), buffer_.end(), channel);
        found = it != buffer_.end();
        if (found)
            buffer_.erase(it);
    }
    ///////////////////////////////////////////////////////////////////////////

    handler(found ? error::success : error::not_found);
}

void connections::store(const channel::ptr& channel, result_handler handler)
{
    bool found;
    const auto address = channel->authority();
    const auto match = [&address](const channel::ptr& entry)
    {
        return entry->authority() == address;
    };

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);

        const auto it = std::find_if(buffer_.begin(), buffer_.end(), match);
        found = it != buffer_.end();

        if (!found)
            buffer_.push_back(channel);
    }
    ///////////////////////////////////////////////////////////////////////////

    handler(found ? error::address_in_use : error::success);
}

void connections::count(count_handler handler)
{
    size_t size;

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    if (true)
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);

        size = buffer_.size();
    }
    ///////////////////////////////////////////////////////////////////////////

    handler(size);
}

} // namespace network
} // namespace libbitcoin
