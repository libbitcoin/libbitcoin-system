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
#ifndef LIBBITCOIN_WORK_HPP
#define LIBBITCOIN_WORK_HPP

#include <functional>
#include <string>
#include <utility>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/monitor.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

#define ORDERED "ordered"
#define UNORDERED "unordered"
#define CONCURRENT "concurrent"
    
#define FORWARD_ARGS(args) \
    std::forward<Args>(args)...
#define FORWARD_HANDLER(handler) \
    std::forward<Handler>(handler)
#define BIND_HANDLER(handler, args) \
    std::bind(FORWARD_HANDLER(handler), FORWARD_ARGS(args))

/// This  class is thread safe.
/// boost asio class wrapper to enable work heap management.
class BC_API work
{
public:
    /// Create an instance.
    work(threadpool& pool, const std::string& name);

    /// This class is not copyable.
    work(const work&) = delete;
    void operator=(const work&) = delete;

    /// Execute the job on the current thread.
    template <typename Handler, typename... Args>
    static void bound(Handler&& handler, Args&&... args)
    {
        BIND_HANDLER(handler, args)();
    }

    /// Service post ensures the job does not execute in the current thread.
    template <typename Handler, typename... Args>
    void concurrent(Handler&& handler, Args&&... args)
    {
        service_.post(inject(BIND_HANDLER(handler, args), CONCURRENT,
            concurrent_));
    }

    /// Use a strand to prevent concurrency and post vs. dispatch to
    /// ensure that the job is not executed in the current thread.
    template <typename Handler, typename... Args>
    void ordered(Handler&& handler, Args&&... args)
    {
        strand_.post(inject(BIND_HANDLER(handler, args), ORDERED, ordered_));
    }

    /// Use a strand wrapper to prevent concurrency and a service post
    /// to deny ordering while ensuring execution on another thread.
    template <typename Handler, typename... Args>
    void unordered(Handler&& handler, Args&&... args)
    {
        service_.post(strand_.wrap(inject(BIND_HANDLER(handler, args),
            UNORDERED, unordered_)));
    }

    size_t ordered_backlog();
    size_t unordered_backlog();
    size_t concurrent_backlog();
    size_t combined_backlog();

private:
    template <typename Handler>
    auto inject(Handler handler, const std::string& context,
        monitor::count_ptr counter) -> std::function<void()>
    {
        const auto label = name_ + "_" + context;
        const auto capture = std::make_shared<monitor>(counter, label);
        return [=]() { capture->invoke(handler); };

        //// TODO: determine how to define return type across platforms.
        ////return std::bind(&monitor::invoke<Handler>, capture, handler);
    }

    // These are thread safe.
    monitor::count_ptr ordered_;
    monitor::count_ptr unordered_;
    monitor::count_ptr concurrent_;
    asio::service& service_;
    asio::service::strand strand_;
    const std::string name_;
};

#undef FORWARD_ARGS
#undef FORWARD_HANDLER
#undef BIND_HANDLER

} // namespace libbitcoin

#endif
