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
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

// TODO: use Args&&, list, ptr.
template <typename... Args>
class subscriber
  : public std::enable_shared_from_this<subscriber<Args...>>
{
public:
    typedef std::function<void (Args...)> subscription_handler;
    typedef std::shared_ptr<subscriber<Args...>> ptr;

    subscriber(threadpool& pool);
    ~subscriber();

    void subscribe(subscription_handler handler);
    void relay(Args... args);

private:
    typedef std::vector<subscription_handler> subscription_list;

    void do_subscribe(subscription_handler notifier);
    void do_relay(Args... args);

    dispatcher dispatch_;
    subscription_list subscriptions_;
};

template <typename... Args>
using subscriber_ptr = std::shared_ptr<subscriber<Args...>>;

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/subscriber.ipp>

#endif
