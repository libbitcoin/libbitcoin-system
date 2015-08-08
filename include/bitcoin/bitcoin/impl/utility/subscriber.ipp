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
#ifndef LIBBITCOIN_SUBSCRIBER_IPP
#define LIBBITCOIN_SUBSCRIBER_IPP

#include <bitcoin/bitcoin/utility/subscriber.hpp>

#include <functional>
#include <memory>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/logger.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

//// REFCOUNT
//#include <atomic>
//#include <cstddef>
    
namespace libbitcoin {

//template <typename... Args>
//std::atomic<size_t> subscriber<Args...>::refcount = 0;

template <typename... Args>
subscriber<Args...>::subscriber(threadpool& pool)
  : strand_(pool)
{
    //log_info("LEAKCHECK") << "subscriber(" << ++refcount << ")";
}

template <typename... Args>
subscriber<Args...>::~subscriber()
{
    //log_info("LEAKCHECK") << "~subscriber(" << --refcount << ")";
}

template <typename... Args>
void subscriber<Args...>::subscribe(subscription_handler handler)
{
    strand_.wrap(&subscriber<Args...>::do_subscribe,
        shared_from_this(), handler)();
}

template <typename... Args>
void subscriber<Args...>::relay(Args... args)
{
    strand_.wrap(&subscriber<Args...>::do_relay,
        shared_from_this(), std::forward<Args>(args)...)();
}

template <typename... Args>
void subscriber<Args...>::do_subscribe(subscription_handler notifier)
{
    subscriptions_.push_back(notifier);
}

template <typename... Args>
void subscriber<Args...>::do_relay(Args... args)
{
    if (subscriptions_.empty())
        return;

    const auto subscriptions = subscriptions_;
    subscriptions_.clear();
    for (const auto notifier: subscriptions)
        notifier(args...);
}

} // namespace libbitcoin

#endif
