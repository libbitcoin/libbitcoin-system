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
#ifndef  LIBBITCOIN_SUBSCRIBER_HPP
#define  LIBBITCOIN_SUBSCRIBER_HPP

#include <functional>
#include <memory>
#include <vector>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

template <typename... Args>
class subscriber
{
public:
    typedef std::function<void (Args...)> subscription_handler;

    subscriber(threadpool& pool)
      : strand_(pool)
    {
    }

    void subscribe(subscription_handler handler)
    {
        auto dispatch_subscribe =
            strand_.wrap(&subscriber<Args...>::do_subscribe,
                this, handler);

        dispatch_subscribe();
    }

    void relay(Args... params)
    {
        auto dispatch_relay =
            strand_.wrap(&subscriber<Args...>::do_relay,
                this, std::forward<Args>(params)...);

        dispatch_relay();
    }

private:
    typedef std::vector<subscription_handler> subscription_list;

    void do_subscribe(subscription_handler notifier)
    {
        subscriptions_.push_back(notifier);
    }

    void do_relay(Args... params)
    {
        if (subscriptions_.empty())
            return;

        const auto subscriptions = subscriptions_;
        subscriptions_.clear();
        for (const auto notifier: subscriptions)
            notifier(params...);
    }

    async_strand strand_;
    subscription_list subscriptions_;
};

} // namespace libbitcoin

#endif
