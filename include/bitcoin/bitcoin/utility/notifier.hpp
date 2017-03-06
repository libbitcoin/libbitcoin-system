/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_NOTIFIER_HPP
#define LIBBITCOIN_NOTIFIER_HPP

#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <bitcoin/bitcoin/utility/asio.hpp>
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

    /// Construct an instance, the class_name is for debugging.
    notifier(threadpool& pool, const std::string& class_name);
    virtual ~notifier();

    /// Enable new subscriptions.
    void start();

    /// Prevent new subscriptions.
    void stop();

    /// True if there is not currently space for the subscription/renewal.
    bool limited(const Key& key, size_t limit) const;

    /// Subscribe to notifications for the specified amount of time.
    /// If key is matched the existing subscription is extended by duration.
    /// If stopped this will invoke the hander with the specified arguments.
    void subscribe(handler&& notify, const Key& key,
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
    typedef typename std::decay<handler>::type decay_handler;
    typedef struct
    {
        decay_handler notify;
        asio::time_point expires;
    } value;

    typedef std::unordered_map<Key, value> map;

    void do_invoke(Args... args);

    bool stopped_;
    map subscriptions_;
    dispatcher dispatch_;
    mutable upgrade_mutex invoke_mutex_;
    mutable upgrade_mutex subscribe_mutex_;
};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/notifier.ipp>

#endif
