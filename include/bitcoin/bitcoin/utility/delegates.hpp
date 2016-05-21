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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_DELEGATES_HPP
#define LIBBITCOIN_DELEGATES_HPP

#include <functional>
#include <bitcoin/bitcoin/utility/work.hpp>

namespace libbitcoin {
namespace delegates {
    
#define FORWARD_ARGS(args) \
    std::forward<Args>(args)...
#define FORWARD_HANDLER(handler) \
    std::forward<Handler>(handler)
#define BIND_HANDLER(handler, args) \
    std::bind(FORWARD_HANDLER(handler), FORWARD_ARGS(args))

/// Binding delegate (current thread).
template <typename Handler>
struct bound
{
    template <typename... Args>
    void operator()(Args&&... args)
    {
        work::bound(BIND_HANDLER(handler, args));
    }

    Handler handler;
};

/// Asynchronous delegate.
template <typename Handler>
struct concurrent
{
    template <typename... Args>
    void operator()(Args&&... args)
    {
        heap.concurrent(BIND_HANDLER(handler, args));
    }

    Handler handler;
    work& heap;
};

/// Ordered synchronous delegate.
template <typename Handler>
struct ordered
{
    template <typename... Args>
    void operator()(Args&&... args)
    {
        heap.ordered(BIND_HANDLER(handler, args));
    }

    Handler handler;
    work& heap;
};

/// Unordered synchronous delegate.
template <typename Handler>
struct unordered
{
    template <typename... Args>
    void operator()(Args&&... args)
    {
        heap.unordered(BIND_HANDLER(handler, args));
    }

    Handler handler;
    work& heap;
};

#undef FORWARD_ARGS
#undef FORWARD_HANDLER
#undef BIND_HANDLER

} // namespace delegates
} // namespace libbitcoin

#endif
