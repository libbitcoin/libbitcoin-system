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
#include <bitcoin/bitcoin/network/pending.hpp>

#include <algorithm>
#include <cstdint>
#include <mutex>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {
namespace network {

// It is not possible for this class to produce a deadlock.

pending::pending()
{
}

pending::~pending()
{
    BITCOIN_ASSERT_MSG(buffer_.empty(), "Pending was not cleared.");
}

void pending::exists(uint64_t version_nonce, truth_handler handler)
{
    // This is an optimization that requires we always set a non-zero nonce.
    if (version_nonce == 0)
    {
        handler(false);
        return;
    }

    bool found;
    const auto match = [version_nonce](const channel::ptr& entry)
    {
        return entry->nonce() == version_nonce;
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

void pending::remove(const channel::ptr& channel, result_handler handler)
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

void pending::store(const channel::ptr& channel, result_handler handler)
{
    bool found;
    const auto version_nonce = channel->nonce();
    const auto match = [version_nonce](const channel::ptr& entry)
    {
        return entry->nonce() == version_nonce;
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

} // namespace network
} // namespace libbitcoin
