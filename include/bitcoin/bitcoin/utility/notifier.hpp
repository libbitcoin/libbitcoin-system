/**
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
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
#ifndef  LIBBITCOIN_NOTIFIER_HPP
#define  LIBBITCOIN_NOTIFIER_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/enable_shared_from_base.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>
////#include <bitcoin/bitcoin/utility/track.hpp>

namespace libbitcoin {

template <typename Key, typename... Args>
class notifier
  : public enable_shared_from_base<notifier<Key, Args...>>
    /*, track<notifier<Key, Args...>>*/
{
public:
    typedef std::function<bool (Args...)> handler;
    typedef std::shared_ptr<notifier<Key, Args...>> ptr;

    /// Construct an instance.
    /// A limit of zero is unlimited, the class_name is for debugging.
    notifier(threadpool& pool, size_t limit, const std::string& class_name);
    notifier(threadpool& pool, const std::string& class_name);
    ~notifier();

    /// Enable new subscriptions.
    void start();

    /// Prevent new subscriptions.
    void stop();

    /// Subscribe to notifications for the specified amount of time.
    /// Return true from the handler to resubscribe to notifications.
    /// If key is matched the existing subscription is extended by duration.
    /// If stopped this will invoke the hander with the specified arguments.
    void subscribe(handler handler, const Key& key,
        const asio::duration& duration, Args... stopped_args);

    /// Remove the subscription matching the specified key.
    /// If subscribed this invokes notification with the specified arguments.
    void unsubscribe(const Key& key, Args... unsubscribed_args);

    /// Remove any expired subscriptions (blocking).
    /// Invokes expiration notification with the specified arguments.
    void purge(Args... expired_args);

    /// Invoke all handlers sequentially (blocking).
    void invoke(Args... args);

    /// Invoke all handlers sequentially (non-blocking).
    void relay(Args... args);

private:
    typedef struct { handler notify; asio::time_point expires; } value;
    typedef std::unordered_map<Key, value> map;

    void do_invoke(Args... args);

    const size_t limit_;
    bool stopped_;
    map subscriptions_;
    dispatcher dispatch_;
    mutable upgrade_mutex invoke_mutex_;
    mutable upgrade_mutex subscribe_mutex_;
};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/notifier.ipp>

#endif
