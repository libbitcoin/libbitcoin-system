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
#ifndef LIBBITCOIN_DISPATCHER_HPP
#define LIBBITCOIN_DISPATCHER_HPP

#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/delegates.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>
#include <bitcoin/bitcoin/utility/work.hpp>

namespace libbitcoin {

#define FORWARD_ARGS(args) \
    std::forward<Args>(args)...
#define FORWARD_HANDLER(handler) \
    std::forward<Handler>(handler)
#define BIND_HANDLER(handler, args) \
    std::bind(FORWARD_HANDLER(handler), FORWARD_ARGS(args))
#define BIND_ARGS(args) \
    std::bind(FORWARD_ARGS(args))

// Collection dispatch doesn't forward args as move args can only forward once.
#define BIND_RACE(args, call) \
    std::bind(args..., call)
#define BIND_ELEMENT(args, element, call) \
    std::bind(args..., element, call)

/// This  class is thread safe.
/// If the ios service is stopped jobs will not be dispatched.
class BC_API dispatcher
{
public:
    dispatcher(threadpool& pool, const std::string& name);

    size_t ordered_backlog();
    size_t unordered_backlog();
    size_t concurrent_backlog();
    size_t combined_backlog();

    /// Invokes a job on the current thread.
    template <typename... Args>
    static void bound(Args&&... args)
    {
        BIND_ARGS(args)();
    }

    /// Posts a job to the service. Concurrent and not ordered.
    template <typename... Args>
    void concurrent(Args&&... args)
    {
        heap_.concurrent(BIND_ARGS(args));
    }

    /// Post a job to the strand. Ordered and not concurrent.
    template <typename... Args>
    void ordered(Args&&... args)
    {
        heap_.ordered(BIND_ARGS(args));
    }

    /// Posts a strand-wrapped job to the service. Not ordered or concurrent.
    /// The wrap provides non-concurrency, order is prevented by service post.
    template <typename... Args>
    void unordered(Args&&... args)
    {
        heap_.unordered(BIND_ARGS(args));
    }

    /// Returns a delegate that will execute the job on the current thread.
    template <typename... Args>
    static auto bound_delegate(Args&&... args) ->
        delegates::bound<decltype(BIND_ARGS(args))>
    {
        return
        {
            BIND_ARGS(args)
        };
    }

    /// Returns a delegate that will post the job via the service.
    template <typename... Args>
    auto concurrent_delegate(Args&&... args) ->
        delegates::concurrent<decltype(BIND_ARGS(args))>
    {
        return
        {
            BIND_ARGS(args),
            heap_
        };
    }

    /// Returns a delegate that will post the job via the strand.
    template <typename... Args>
    auto ordered_delegate(Args&&... args) ->
        delegates::ordered<decltype(BIND_ARGS(args))>
    {
        return
        {
            BIND_ARGS(args),
            heap_
        };
    }

    /// Returns a delegate that will post a wrapped job via the service.
    template <typename... Args>
    auto unordered_delegate(Args&&... args) ->
        delegates::unordered<decltype(BIND_ARGS(args))>
    {
        return
        {
            BIND_ARGS(args),
            heap_
        };
    }

    /// Executes multiple identical jobs concurrently until one completes.
    template <typename Count, typename Handler, typename... Args>
    void race(Count count, const std::string& name, Handler&& handler,
        Args... args)
    {
        // The first fail will also terminate race and return the code.
        static const size_t clearance_count = 1;
        const auto call = synchronize(FORWARD_HANDLER(handler),
            clearance_count, name, false);

        for (Count iteration = 0; iteration < count; ++iteration)
            concurrent(BIND_RACE(args, call));
    }

    /// Executes the job against each member of a collection concurrently.
    template <typename Element, typename Handler, typename... Args>
    void parallel(const std::vector<Element>& collection,
        const std::string& name, Handler&& handler, Args... args)
    {
        // Failures are suppressed, success always returned to handler.
        const auto call = synchronize(FORWARD_HANDLER(handler),
            collection.size(), name, true);

        for (const auto& element: collection)
            concurrent(BIND_ELEMENT(args, element, call));
    }

    /// Disperses the job against each member of a collection without order.
    template <typename Element, typename Handler, typename... Args>
    void disperse(const std::vector<Element>& collection,
        const std::string& name, Handler&& handler, Args... args)
    {
        // Failures are suppressed, success always returned to handler.
        const auto call = synchronize(FORWARD_HANDLER(handler),
            collection.size(), name, true);

        for (const auto& element: collection)
            unordered(BIND_ELEMENT(args, element, call));
    }

    /// Disperses the job against each member of a collection with order.
    template <typename Element, typename Handler, typename... Args>
    void serialize(const std::vector<Element>& collection,
        const std::string& name, Handler&& handler, Args... args)
    {
        // Failures are suppressed, success always returned to handler.
        const auto call = synchronize(FORWARD_HANDLER(handler),
            collection.size(), name, true);

        for (const auto& element: collection)
            ordered(BIND_ELEMENT(args, element, call));
    }

private:

    // This is thread safe.
    work heap_;
};

#undef FORWARD_ARGS
#undef FORWARD_HANDLER
#undef BIND_HANDLER
#undef BIND_ARGS
#undef BIND_RACE
#undef BIND_ELEMENT

} // namespace libbitcoin

#endif
