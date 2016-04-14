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
#ifndef LIBBITCOIN_RESUBSCRIBER_IPP
#define LIBBITCOIN_RESUBSCRIBER_IPP

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>
////#include <bitcoin/bitcoin/utility/track.hpp>

namespace libbitcoin {

template <typename... Args>
resubscriber<Args...>::resubscriber(threadpool& pool,
    const std::string& class_name)
  : stopped_(true),
    dispatch_(pool, class_name)/*,
    track<resubscriber<Args...>>(class_name)*/
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
void resubscriber<Args...>::stop()
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
void resubscriber<Args...>::subscribe(handler notifier, Args... stopped_args)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    if (!stopped_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();
        subscriptions_.emplace_back(notifier);
        mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    notifier(stopped_args...);
}

template <typename... Args>
void resubscriber<Args...>::relay(Args... args)
{
    dispatch_.ordered(&resubscriber<Args...>::do_relay,
        this->shared_from_this(), args...);
}

template <typename... Args>
void resubscriber<Args...>::do_relay(Args... args)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock();

    // Move subscribers from the member list to a temporary list.
    list subscriptions;
    move_append(subscriptions, subscriptions_);

    mutex_.unlock();
    ///////////////////////////////////////////////////////////////////////////

    // Build list of renewals by moving from temporary list after invoking.
    list renewals;

    // Invoke subscribers from temporary list.
    for (const auto& notifier: subscriptions)
        if (notifier(args...))
            renewals.emplace_back(notifier);

    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    mutex_.lock_upgrade();

    if (!renewals.empty() && !stopped_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        mutex_.unlock_upgrade_and_lock();

        // Move renewals onto subscriptions member (which may have changed).
        move_append(subscriptions_, renewals);

        mutex_.unlock();
        //---------------------------------------------------------------------
        return;
    }

    mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
}

} // namespace libbitcoin

#endif
