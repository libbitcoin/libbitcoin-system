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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SUBSCRIBER_IPP
#define LIBBITCOIN_SUBSCRIBER_IPP

#include <functional>
#include <memory>
#include <string>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>
////#include <bitcoin/bitcoin/utility/track.hpp>

namespace libbitcoin {

template <typename... Args>
subscriber<Args...>::subscriber(threadpool& pool,
    const std::string& class_name)
  : stopped_(true), dispatch_(pool, class_name)
    /*, track<subscriber<Args...>>(class_name)*/
{
}

template <typename... Args>
subscriber<Args...>::~subscriber()
{
    BITCOIN_ASSERT_MSG(subscriptions_.empty(), "subscriber not cleared");
}

template <typename... Args>
void subscriber<Args...>::start()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    if (stopped_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();
        stopped_ = false;
        mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

template <typename... Args>
void subscriber<Args...>::stop()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    if (!stopped_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();
        stopped_ = true;
        mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

template <typename... Args>
void subscriber<Args...>::subscribe(handler handler, Args... stopped_args)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    if (!stopped_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();
        subscriptions_.emplace_back(handler);
        mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    handler(stopped_args...);
}

template <typename... Args>
void subscriber<Args...>::invoke(Args... args)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    unique_lock(invoke_mutex_);
    do_invoke(args...);
    ///////////////////////////////////////////////////////////////////////////
}

template <typename... Args>
void subscriber<Args...>::relay(Args... args)
{
    // The ordered dispatch prevents concurrent do_invoke.
    dispatch_.ordered(&subscriber<Args...>::do_invoke,
        this->shared_from_this(), args...);
}

// private
template <typename... Args>
void subscriber<Args...>::do_invoke(Args... args)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock();

    // Move subscribers from the member list to a temporary list.
    list subscriptions;
    std::swap(subscriptions, subscriptions_);

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    // Subscriptions may be created while this loop is executing.
    // Invoke subscribers from temporary list, without subscription renewal.
    for (const auto& handler: subscriptions)
        handler(args...);
}

} // namespace libbitcoin

#endif
