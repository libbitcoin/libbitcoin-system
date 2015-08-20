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
#ifndef LIBBITCOIN_DISPATCHER_HPP
#define LIBBITCOIN_DISPATCHER_HPP

#include <functional>
#include <thread>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

template <typename Handler>
struct dispatch_impl
{
    Handler handler;
    boost::asio::io_service::strand& strand;

    template <typename... Args>
    void operator()(Args&&... args)
    {
        strand.dispatch(std::bind(handler, std::forward<Args>(args)...));
    }
};

/**
 * Convenience class for objects wishing to synchronize operations.
 */
class BC_API dispatcher
{
public:
    dispatcher(threadpool& pool);

    /**
     * Returns a new handler that guarantees that the handler it encapsulates
     * will not execute concurrently with other handlers on the strand. Does
     * not guarantee sequential calling order.
     */
    template <typename Handler, typename... Args>
    auto sync(Handler&& handler, Args&&... args) ->
        dispatch_impl<decltype(std::bind(std::forward<Handler>(handler),
            std::forward<Args>(args)...))>
    {
        auto bound = std::bind(std::forward<Handler>(handler),
            std::forward<Args>(args)...);
        return { bound, strand_ };
    }

    /**
     * Guarantees that any handlers passed to it will not execute concurrently
     * with other handlers on the strand. Guarantees sequential calling order.
     */
    template <typename... Args>
    void queue(Args&&... args)
    {
        strand_.post(std::bind(std::forward<Args>(args)...));
    }

    /**
     * Guarantees that any handlers passed to it will not execute concurrently
     * with other handlers on the strand. Does not guarantee sequential calling
     * order.
     */
    template <typename... Args>
    void randomly_queue(Args&&... args)
    {
        ios_.post(strand_.wrap(std::bind(std::forward<Args>(args)...)));
    }

    /**
     * Offers no synchronization guarantees (i.e. may execute concurrently with
     * other posts).
     */
    template <typename... Args>
    void async(Args&&... args)
    {
        ios_.post(std::bind(std::forward<Args>(args)...));
    }

private:
    boost::asio::io_service& ios_;
    boost::asio::io_service::strand strand_;
};

} // namespace libbitcoin

#endif
