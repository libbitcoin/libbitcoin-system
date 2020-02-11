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
#ifndef LIBBITCOIN__RESUBSCRIBER_HPP
#define LIBBITCOIN__RESUBSCRIBER_HPP

//#include <functional>
//#include <memory>
//#include <string>
//#include <vector>
//#include <bitcoin/bitcoin/utility/dispatcher.hpp>
//#include <bitcoin/bitcoin/utility/enable_shared_from_base.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
//#include <bitcoin/bitcoin/utility/threadpool.hpp>
////#include <bitcoin/bitcoin/utility/track.hpp>
#include <bitcoin/utility_resubscriber_handler.hpp>
#include <bitcoin/utility_threadpool.hpp>

namespace libbitcoin {
namespace api {

template <typename... Args>
class utility_resubscriber
		  : public enable_shared_from_base<utility_resubscriber<Args...>>
{
public:
//    typedef std::function<bool (Args...)> handler;
//    typedef std::shared_ptr<resubscriber<Args...>> ptr;

    /// Construct an instance. The class_name is for debugging.
    utility_resubscriber(utility_threadpool& pool, const std::string& class_name);
    virtual ~utility_resubscriber();

    /// Enable new subscriptions.
    void start();

    /// Prevent new subscriptions.
    void stop();

    /// Subscribe to notifications with an option to resubscribe.
    /// Return true from the handler to resubscribe to notifications.
    // FIXME later
//    void subscribe(utility_resubscriber_handler<Args>&& notify, Args... stopped_args);

    /// Invoke all handlers sequentially (blocking).
    void invoke(Args... args);

    /// Invoke all handlers sequentially (non-blocking).
    void relay(Args... args);

//private:
//    typedef std::vector<handler> list;
//
//    void do_invoke(Args... args);
//
//    bool stopped_;
//    list subscriptions_;
//    dispatcher dispatch_;
//    mutable upgrade_mutex invoke_mutex_;
//    mutable upgrade_mutex subscribe_mutex_;
};

} // namespace api
} // namespace libbitcoin

//#include <bitcoin/bitcoin/impl/utility/resubscriber.ipp>

#endif
