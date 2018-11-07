/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_WORK_HPP
#define LIBBITCOIN_WORK_HPP

#include <functional>
#include <string>
#include <memory>
#include <utility>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/error.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/monitor.hpp>
#include <bitcoin/bitcoin/utility/noncopyable.hpp>
#include <bitcoin/bitcoin/utility/sequencer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

#define ORDERED "ordered"
#define UNORDERED "unordered"
#define CONCURRENT "concurrent"
#define SEQUENCE "sequence"

#define FORWARD_ARGS(args) \
    std::forward<Args>(args)...
#define FORWARD_HANDLER(handler) \
    std::forward<Handler>(handler)
#define BIND_HANDLER(handler, args) \
    std::bind(FORWARD_HANDLER(handler), FORWARD_ARGS(args))

/// This  class is thread safe.
/// boost asio class wrapper to enable work heap management.
class BC_API work
  : noncopyable
{
public:
    typedef std::shared_ptr<work> ptr;

    /// Create an instance.
    work(threadpool& pool, const std::string& name);

    /// Local execution for any operation, equivalent to std::bind.
    template <typename Handler, typename... Args>
    static void bound(Handler&& handler, Args&&... args)
    {
        BIND_HANDLER(handler, args)();
    }

    /// Concurrent execution for any operation.
    template <typename Handler, typename... Args>
    void concurrent(Handler&& handler, Args&&... args)
    {
        // Service post ensures the job does not execute in the current thread.
        service_.post(BIND_HANDLER(handler, args));
        ////service_.post(inject(BIND_HANDLER(handler, args), CONCURRENT,
        ////    concurrent_));
    }

    /// Sequential execution for synchronous operations.
    template <typename Handler, typename... Args>
    void ordered(Handler&& handler, Args&&... args)
    {
        // Use a strand to prevent concurrency and post vs. dispatch to ensure
        // that the job is not executed in the current thread.
        strand_.post(BIND_HANDLER(handler, args));
        ////strand_.post(inject(BIND_HANDLER(handler, args), ORDERED, ordered_));
    }

    /// Non-concurrent execution for synchronous operations.
    template <typename Handler, typename... Args>
    void unordered(Handler&& handler, Args&&... args)
    {
        // Use a strand wrapper to prevent concurrency and a service post
        // to deny ordering while ensuring execution on another thread.
        service_.post(strand_.wrap(BIND_HANDLER(handler, args)));
        ////service_.post(strand_.wrap(inject(BIND_HANDLER(handler, args),
        ////    UNORDERED, unordered_)));
    }

    /// Begin sequential execution for a set of asynchronous operations.
    /// The operation will be queued until the lock is free and then executed.
    template <typename Handler, typename... Args>
    void lock(Handler&& handler, Args&&... args)
    {
        // Use a sequence to track the asynchronous operation to completion,
        // ensuring each asynchronous op executes independently and in order.
        sequence_.lock(BIND_HANDLER(handler, args));
        ////sequence_.lock(inject(BIND_HANDLER(handler, args), SEQUENCE,
        ////    sequence_));
    }

    /// Complete sequential execution.
    void unlock()
    {
        sequence_.unlock();
    }

    ////size_t ordered_backlog();
    ////size_t unordered_backlog();
    ////size_t concurrent_backlog();
    ////size_t sequential_backlog();
    ////size_t combined_backlog();

private:
    ////template <typename Handler>
    ////auto inject(Handler&& handler, const std::string& context,
    ////    monitor::count_ptr counter) -> std::function<void()>
    ////{
    ////    auto label = name_ + "_" + context;
    ////    auto capture = std::make_shared<monitor>(counter, std::move(label));
    ////    return [=]() { capture->invoke(handler); };

    ////    //// TODO: use this to prevent handler copy into closure.
    ////    ////return std::bind(&monitor::invoke<Handler>, capture,
    ////    ////    std::forward<Handler>(handler));
    ////}

    // These are thread safe.
    const std::string name_;
    ////monitor::count_ptr ordered_;
    ////monitor::count_ptr unordered_;
    ////monitor::count_ptr concurrent_;
    ////monitor::count_ptr sequential_;
    asio::service& service_;
    asio::service::strand strand_;
    sequencer sequence_;
};

#undef FORWARD_ARGS
#undef FORWARD_HANDLER
#undef BIND_HANDLER

} // namespace libbitcoin

#endif
