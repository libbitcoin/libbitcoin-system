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
#ifndef LIBBITCOIN_ASYNC_STRAND_HPP
#define LIBBITCOIN_ASYNC_STRAND_HPP

#include <functional>
#include <thread>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

template <typename Handler>
struct wrapped_handler_impl
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
 * Convenience class for objects wishing to synchronize operations around
 * shared data.
 *
 * queue() guarantees that any handlers passed to it will never
 * execute at the same time, and they will be called in sequential order.
 *
 * randomly_queue() guarantees that any handlers passed to it will never
 * execute at the same time.
 */
class BC_API async_strand
{
public:
    async_strand(threadpool& pool);

    /**
     * wrap() returns a new handler that guarantees that the handler it
     * encapsulates will never execute at the same time as another handler
     * passing through this class.
     */
    template <typename Handler, typename... Args>
    auto wrap(Handler&& handler, Args&&... args) ->
        wrapped_handler_impl<decltype(
            std::bind(
                std::forward<Handler>(handler), std::forward<Args>(args)...))>
    {
        auto bound = std::bind(
            std::forward<Handler>(handler), std::forward<Args>(args)...);
        return { bound, strand_ };
    }

    /**
     * queue() guarantees that any handlers passed to it will
     * never execute at the same time in sequential order.
     *
     * Guarantees sequential calling order.
     *
     * @code
     *   strand.queue(handler);
     * @endcode
     */
    template <typename... Args>
    void queue(Args&&... args)
    {
        strand_.post(std::bind(std::forward<Args>(args)...));
    }

    /**
     * randomly_queue() guarantees that any handlers passed to it will
     * never execute at the same time.
     *
     * Does not guarantee sequential calling order.
     *
     * @code
     *   strand.randomly_queue(handler);
     * @endcode
     */
    template <typename... Args>
    void randomly_queue(Args&&... args)
    {
        ios_.post(strand_.wrap(std::bind(std::forward<Args>(args)...)));
    }

private:
    boost::asio::io_service& ios_;
    boost::asio::io_service::strand strand_;
};

} // namespace libbitcoin

#endif

