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
#include <string>
#include <vector>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/enable_shared_from_base.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>
////#include <bitcoin/bitcoin/utility/track.hpp>

namespace libbitcoin {

template <typename... Args>
class subscriber
  : public enable_shared_from_base<subscriber<Args...>>/*,
    track<subscriber<Args...>>*/
{
public:
    typedef std::function<void (Args...)> handler;
    typedef std::shared_ptr<subscriber<Args...>> ptr;

    subscriber(threadpool& pool, const std::string& class_name);
    ~subscriber();

    /// Call stop to prevent new subscriptions.
    void stop();

    /// Use the resubscriber template to resubscribe in the handler.
    void subscribe(handler notifier);

    /// Call to invoke all handlers, which clears them.
    void relay(Args... args);

    /// Invoke all handlers in order on the current thread.
    void do_relay(Args... args);

private:
    typedef std::vector<handler> list;

    bool stopped_;
    dispatcher dispatch_;
    list subscriptions_;
    mutable shared_mutex mutex_;
};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/subscriber.ipp>

#endif
