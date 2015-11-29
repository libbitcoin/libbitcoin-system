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
    BITCOIN_ASSERT_MSG(buffer_.empty(), "Connections was not cleared.");
}

connections::iterator connections::find(const authority& authority) const
{
    const auto found = [&authority](const channel::ptr& entry)
    {
        return entry->authority() == authority;
    };

    return std::find_if(buffer_.begin(), buffer_.end(), found);
}

connections::iterator connections::find(const channel::ptr& channel) const
{
    return std::find(buffer_.begin(), buffer_.end(), channel);
}

void connections::stop(const code& ec)
{
    // Safe to change context only if this completes before work suspension.
    dispatch_.ordered(&connections::do_stop,
        this, ec);
}

void connections::do_stop(const code& ec)
{
    for (auto channel: buffer_)
        channel->stop(ec);

    ////buffer_.clear();
}

void connections::exists(const authority& address, truth_handler handler)
{
    dispatch_.ordered(&connections::do_exists,
        this, address, handler);
}

void connections::do_exists(const authority& address,
    truth_handler handler) const
{
    const auto found = find(address) != buffer_.end();
    handler(found);
}

void connections::remove(const channel::ptr& channel, result_handler handler)
{
    dispatch_.ordered(&connections::do_remove,
        this, channel, handler);
}

void connections::do_remove(const channel::ptr& channel,
    result_handler handler)
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

void connections::store(const channel::ptr& channel, result_handler handler)
{
    dispatch_.ordered(&connections::do_store,
        this, channel, handler);
}

void connections::do_store(const channel::ptr& channel, result_handler handler)
{
    const auto found = find(channel->authority()) != buffer_.end();
    if (found)
    {
        handler(error::address_in_use);
        return;
    }

    buffer_.push_back(channel);
    handler(error::success);
}

void connections::count(count_handler handler)
{
    dispatch_.ordered(&connections::do_count,
        this, handler);
}

void connections::do_count(count_handler handler) const
{
    handler(buffer_.size());
}

} // namespace network
} // namespace libbitcoin
