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
#ifndef  LIBBITCOIN_RESUBSCRIBER_HPP
#define  LIBBITCOIN_RESUBSCRIBER_HPP

#include <functional>
#include <memory>
#include <vector>
#include <bitcoin/bitcoin/utility/assert.hpp>
#include <bitcoin/bitcoin/utility/dispatcher.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

template <typename... Args>
class resubscriber
  : public std::enable_shared_from_this<resubscriber<Args...>>/*,
    track<resubscriber<Args...>>*/
{
public:
    typedef std::function<bool (Args...)> handler;
    typedef std::shared_ptr<resubscriber<Args...>> ptr;

    resubscriber(threadpool& pool, const std::string& class_name);

    void subscribe(handler notifier);
    void relay(Args... args);

private:
    typedef std::vector<handler> list;

    void do_subscribe(handler notifier);
    void do_relay(Args... args);

    dispatcher dispatch_;
    list subscriptions_;
};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/resubscriber.ipp>

#endif
