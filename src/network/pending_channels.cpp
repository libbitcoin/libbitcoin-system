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
#include <bitcoin/bitcoin/network/pending_channels.hpp>

#include <algorithm>
#include <cstdint>
#include <functional>
#include <mutex>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {
namespace network {

pending_channels::pending_channels()
{
}

pending_channels::~pending_channels()
{
    BITCOIN_ASSERT_MSG(channels_.empty(), "Pending channels not cleared.");
}

bool pending_channels::safe_store(channel::ptr channel)
{
    const auto version_nonce = channel->nonce();
    const auto match = [version_nonce](const channel::ptr& entry)
    {
        return entry->nonce() == version_nonce;
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

void pending_channels::store(channel::ptr channel, result_handler handler)
{
    handler(safe_store(channel) ? error::address_in_use : error::success);
}

bool pending_channels::safe_remove(channel::ptr channel)
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

void pending_channels::remove(channel::ptr channel, result_handler handler)
{
    handler(safe_remove(channel) ? error::success : error::not_found);
}

bool pending_channels::safe_exists(uint64_t version_nonce)
{
    const auto match = [version_nonce](channel::ptr entry)
    {
        return entry->nonce() == version_nonce;
    };

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    const auto it = std::find_if(channels_.begin(), channels_.end(), match);
    return it != channels_.end();
    ///////////////////////////////////////////////////////////////////////////
}

void pending_channels::exists(uint64_t version_nonce, truth_handler handler)
{
    // This is an optimization that requires we always set a non-zero nonce.
    handler(version_nonce == 0 ? false : safe_exists(version_nonce));
}

} // namespace network
} // namespace libbitcoin
