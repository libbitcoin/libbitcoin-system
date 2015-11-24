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
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/network/channel.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {
namespace network {

#define NAME "pending"

pending::pending(threadpool& pool)
  : dispatch_(pool, NAME)
{
}

pending::~pending()
{
    BITCOIN_ASSERT_MSG(buffer_.empty(), "Pending buffer not empty.");
}

pending::iterator pending::find(const uint64_t version_nonce) const
{
    const auto found = [version_nonce](const channel::ptr& entry)
    {
        return entry->nonce() == version_nonce;
    };

    return std::find_if(buffer_.begin(), buffer_.end(), found);
}

pending::iterator pending::find(const channel::ptr& channel) const
{
    return std::find(buffer_.begin(), buffer_.end(), channel);
}

void pending::clear(const code& ec)
{
    dispatch_.ordered(&pending::do_clear,
        this, ec);
}

void pending::do_clear(const code& ec)
{
    for (auto channel: buffer_)
        channel->stop(ec);

    buffer_.clear();
}

void pending::exists(uint64_t version_nonce, truth_handler handler)
{
    // This is an optimization that requires we always set a non-zero nonce.
    if (version_nonce == 0)
    {
        handler(false);
        return;
    }
        
    dispatch_.ordered(&pending::do_exists,
        this, version_nonce, handler);
}

void pending::do_exists(uint64_t version_nonce, truth_handler handler) const
{
    const auto found = find(version_nonce) != buffer_.end();
    handler(found);
}

void pending::remove(const channel::ptr& channel, result_handler handler)
{
    dispatch_.ordered(&pending::do_remove,
        this, channel, handler);
}

void pending::do_remove(const channel::ptr& channel, result_handler handler)
{
    auto it = std::find(buffer_.begin(), buffer_.end(), channel);
    if (it == buffer_.end())
    {
        handler(error::not_found);
        return;
    }

    buffer_.erase(it);
    handler(error::success);
}

void pending::store(const channel::ptr& channel, result_handler handler)
{
    dispatch_.ordered(&pending::do_store,
        this, channel, handler);
}

void pending::do_store(const channel::ptr& channel, result_handler handler)
{
    const auto found = find(channel->nonce()) != buffer_.end();
    if (found)
    {
        handler(error::address_in_use);
        return;
    }

    buffer_.push_back(channel);
    handler(error::success);
}

void pending::count(count_handler handler)
{
    dispatch_.ordered(&pending::do_count,
        this, handler);
}

void pending::do_count(count_handler handler) const
{
    handler(buffer_.size());
}

} // namespace network
} // namespace libbitcoin
