/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_SYSTEM_CONCURRENCY_RESUBSCRIBER_IPP
#define LIBBITCOIN_SYSTEM_CONCURRENCY_RESUBSCRIBER_IPP

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <bitcoin/system/assert.hpp>
#include <bitcoin/system/concurrency/dispatcher.hpp>
#include <bitcoin/system/concurrency/thread.hpp>
#include <bitcoin/system/concurrency/threadpool.hpp>
////#include <bitcoin/system/concurrency/track.hpp>

namespace libbitcoin {
namespace system {

template <typename... Args>
resubscriber<Args...>::resubscriber(threadpool& pool,
    const std::string& class_name)
  : stopped_(true), dispatch_(pool, class_name)
    /*, track<resubscriber<Args...>>(class_name)*/
{
}

template <typename... Args>
resubscriber<Args...>::~resubscriber()
{
    BITCOIN_ASSERT_MSG(subscriptions_.empty(), "resubscriber not cleared");
}

template <typename... Args>
void resubscriber<Args...>::start()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    subscribe_mutex_.lock_upgrade();

    if (stopped_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        subscribe_mutex_.unlock_upgrade_and_lock();
        stopped_ = false;
        subscribe_mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    subscribe_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

template <typename... Args>
void resubscriber<Args...>::stop()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    subscribe_mutex_.lock_upgrade();

    if (!stopped_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        subscribe_mutex_.unlock_upgrade_and_lock();
        stopped_ = true;
        subscribe_mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    subscribe_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

template <typename... Args>
void resubscriber<Args...>::subscribe(handler&& notify, Args... stopped_args)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    subscribe_mutex_.lock_upgrade();

    if (!stopped_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        subscribe_mutex_.unlock_upgrade_and_lock();
        subscriptions_.push_back(std::forward<handler>(notify));
        subscribe_mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    subscribe_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    notify(stopped_args...);
}

template <typename... Args>
void resubscriber<Args...>::invoke(Args... args)
{
    do_invoke(args...);
}

template <typename... Args>
void resubscriber<Args...>::relay(Args... args)
{
    // This enqueues work while maintaining order.
    dispatch_.ordered(&resubscriber<Args...>::do_invoke,
        this->shared_from_this(), args...);
}

// private
template <typename... Args>
void resubscriber<Args...>::do_invoke(Args... args)
{
    // Critical Section (prevent concurrent handler execution)
    ///////////////////////////////////////////////////////////////////////////
    unique_lock lock(invoke_mutex_);

    // Critical Section (protect stop)
    ///////////////////////////////////////////////////////////////////////////
    subscribe_mutex_.lock();

    // Move subscribers from the member list to a temporary list.
    list subscriptions;
    std::swap(subscriptions, subscriptions_);

    subscribe_mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    // Subscriptions may be created while this loop is executing.
    // Invoke subscribers from temporary list and resubscribe as indicated.
    for (const auto& handler: subscriptions)
    {
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        // DEADLOCK RISK, handler must not return to invoke.
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        if (handler(args...))
        {
            // Critical Section
            ///////////////////////////////////////////////////////////////////
            subscribe_mutex_.lock_upgrade();

            if (stopped_)
            {
                subscribe_mutex_.unlock_upgrade();
                //-------------------------------------------------------------
                continue;
            }

            subscribe_mutex_.unlock_upgrade_and_lock();
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            subscriptions_.push_back(handler);

            subscribe_mutex_.unlock();
            ///////////////////////////////////////////////////////////////////
        }
    }

    ///////////////////////////////////////////////////////////////////////////
}

} // namespace system
} // namespace libbitcoin

#endif
