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

#include <mutex>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/sequencer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

template <typename... Args>
resubscriber<Args...>::resubscriber(threadpool& pool)
  : strand_(pool), stopped_(false)
{
}

template <typename... Args>
resubscriber<Args...>::~resubscriber()
{
    BITCOIN_ASSERT_MSG(stopped_, "resubscriber not stopped");
    BITCOIN_ASSERT_MSG(subscriptions_.empty(), "resubscriber not cleared");
}

template <typename... Args>
void resubscriber<Args...>::stop()
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    stopped_ = true;
    ///////////////////////////////////////////////////////////////////////////
}

template <typename... Args>
void resubscriber<Args...>::subscribe(resubscription_handler notifier)
{
    // Critical Section
    ///////////////////////////////////////////////////////////////////////////
    std::lock_guard<std::mutex> lock(mutex_);

    if (!stopped_)
        strand_.queue(&resubscriber<Args...>::do_subscribe,
            this->shared_from_this(), notifier);
    ///////////////////////////////////////////////////////////////////////////
}

template <typename... Args>
void resubscriber<Args...>::relay(Args... args)
{
    strand_.queue(&resubscriber<Args...>::do_relay,
        this->shared_from_this(), args...);
}

template <typename... Args>
void resubscriber<Args...>::do_subscribe(resubscription_handler notifier)
{
    subscriptions_.push_back(notifier);
}

template <typename... Args>
void resubscriber<Args...>::do_relay(Args... args)
{
    if (subscriptions_.empty())
        return;

    const auto subscriptions_copy = subscriptions_;
    subscriptions_.clear();

    for (const auto notifier : subscriptions_copy)
    {
        const auto renew = notifier(args...);

        // Critical Section
        ///////////////////////////////////////////////////////////////////////
        std::lock_guard<std::mutex> lock(mutex_);

        if (renew && !stopped_)
            subscriptions_.push_back(notifier);
        ///////////////////////////////////////////////////////////////////////
    }
}

} // namespace libbitcoin

#endif
