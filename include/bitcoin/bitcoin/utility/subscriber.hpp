/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#include <stack>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/async_strand.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

template <typename... Args>
class subscriber
  : public std::enable_shared_from_this<subscriber<Args...>>
{
public:
    typedef std::function<void (Args...)> handler_type;
    typedef std::shared_ptr<subscriber<Args...>> ptr;

    subscriber(threadpool& pool)
      : strand_(pool)
    {
    }

    void subscribe(handler_type handle)
    {
        auto dispatch_subscribe =
            strand_.wrap(&subscriber<Args...>::do_subscribe,
                this->shared_from_this(), handle);

        dispatch_subscribe();
    }

    void relay(Args... params)
    {
        auto dispatch_relay =
            strand_.wrap(&subscriber<Args...>::do_relay,
                this->shared_from_this(), std::forward<Args>(params)...);

        dispatch_relay();
    }

private:
    typedef std::stack<handler_type> registry_stack;

    void do_subscribe(handler_type handle)
    {
        registry_.push(handle);
    }

    void do_relay(Args... params)
    {
        auto notify_copy = registry_;
        registry_ = registry_stack();
        while (!notify_copy.empty())
        {
            notify_copy.top()(params...);
            notify_copy.pop();
        }

        BITCOIN_ASSERT(notify_copy.empty());
    }

    async_strand strand_;
    registry_stack registry_;
};

// TODO: push into subscriber<Agrs...> (interface break).
template <typename... Args>
using subscriber_ptr = std::shared_ptr<subscriber<Args...>>;

} // namespace libbitcoin

#endif

