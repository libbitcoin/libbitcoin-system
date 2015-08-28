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
#ifndef LIBBITCOIN_DELEGATES_HPP
#define LIBBITCOIN_DELEGATES_HPP

#include <functional>
#include <bitcoin/bitcoin/network/asio.hpp>

namespace libbitcoin {
namespace delegate {

#define BIND_HANDLER_ARGS(handler, args) \
    std::bind(handler, std::forward<Args>(args)...)

/**
 * Asynchronous delegate (function object).
 */
template <typename Handler>
struct concurrent
{
    template <typename... Args>
    void operator()(Args&&... args)
    {
        // Service post ensures the job does not execute in the current thread.
        service.post(BIND_HANDLER_ARGS(handler, args));
    }

    Handler handler;
    asio::service& service;
};

/**
 * Ordered synchornous delegate (function object).
 */
template <typename Handler>
struct ordered
{
    template <typename... Args>
    void operator()(Args&&... args)
    {
        // We use a strand to prevent concurrency and post vs. dispatch to
        // ensure that the job is not executed in the current thread.
        strand.post(BIND_HANDLER_ARGS(handler, args));
    }

    Handler handler;
    asio::service::strand& strand;
};

/**
 * Unordered synchornous delegate (function object).
 */
template <typename Handler>
struct unordered
{
    template <typename... Args>
    void operator()(Args&&... args)
    {
        // We use a strand wrapper ro prevent concurrency and a service post
        // to deny ordering while ensuring execution on another thread.
        service.post(strand.wrap(BIND_HANDLER_ARGS(handler, args)));
    }

    Handler handler;
    asio::service& service;
    asio::service::strand& strand;
};

#undef BIND_HANDLER_ARGS

} // namespace delegate
} // namespace libbitcoin

#endif
