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
#include <string>
#include <utility>
#include <vector>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/utility/delegates.hpp>
#include <bitcoin/bitcoin/utility/synchronizer.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>

namespace libbitcoin {

using std::placeholders::_1;

#define FORWARD_ARGS(args) \
    std::forward<Args>(args)...
#define FORWARD_HANDLER(handler) \
    std::forward<Handler>(handler)
#define BIND_ARGS(args) \
    std::bind(FORWARD_ARGS(args))
#define BIND_HANDLER(handler, args) \
    std::bind(FORWARD_HANDLER(handler), FORWARD_ARGS(args))
#define BIND_ELEMENT(args, call, element) \
    std::bind(FORWARD_ARGS(args), call, element)

/**
 * Convenience class for objects wishing to synchronize operations.
 * If the ios service is stopped jobs will not be dispatched.
 */
class BC_API dispatcher
{
public:
    dispatcher(threadpool& pool);

    /**
     * Posts a job to the service. Concurrent and not ordered.
     */
    template <typename... Args>
    void concurrent(Args&&... args)
    {
        service_.post(BIND_ARGS(args));
    }

    /**
     * Post a job to the strand. Ordered and not concurrent.
     */
    template <typename... Args>
    void ordered(Args&&... args)
    {
        strand_.post(BIND_ARGS(args));
    }

    /**
     * Posts a strand-wrapped job to the service. Not ordered or concurrent.
     * The wrap provides non-concurrency, order is prevented by service post.
     */
    template <typename... Args>
    void unordered(Args&&... args)
    {
        service_.post(strand_.wrap(BIND_ARGS(args)));
    }

    /**
     * Executes the job against each member of a collection concurrently.
     */
    template <typename Element, typename Handler, typename... Args>
    void parallel(const std::vector<Element>& collection,
        const std::string& name, Handler&& handler, Args&&... args)
    {
        const auto call = synchronize(FORWARD_HANDLER(handler),
            collection.size(), name);

        for (const auto& element: collection)
            concurrent(BIND_ELEMENT(args, element, call));
    }

    /**
     * Disperses the job against each member of a collection without order.
     */
    template <typename Element, typename Handler, typename... Args>
    void disperse(const std::vector<Element>& collection,
        const std::string& name, Handler&& handler, Args&&... args)
    {
        const auto call = synchronize(FORWARD_HANDLER(handler),
            collection.size(), name);

        for (const auto& element: collection)
            unordered(BIND_ELEMENT(args, element, call));
    }

    /**
     * Disperses the job against each member of a collection with order.
     */
    template <typename Element, typename Handler, typename... Args>
    void serialize(const std::vector<Element>& collection,
        const std::string& name, Handler&& handler, Args&&... args)
    {
        const auto call = synchronize(FORWARD_HANDLER(handler),
            collection.size(), name);

        for (const auto& element: collection)
            ordered(BIND_ELEMENT(args, element, call));
    }

    /**
     * Returns a delegate that will post the job via the service.
     */
    template <typename Handler, typename... Args>
    auto concurrent_delegate(Handler&& handler, Args&&... args) ->
        delegate::concurrent<decltype(BIND_HANDLER(handler, args))>
    {
        return
        {
            BIND_HANDLER(handler, args),
            service_
        };
    }

    /**
     * Returns a delegate that will post the job via the strand.
     */
    template <typename Handler, typename... Args>
    auto ordered_delegate(Handler&& handler, Args&&... args) ->
        delegate::ordered<decltype(BIND_HANDLER(handler, args))>
    {
        return
        {
            BIND_HANDLER(handler, args),
            strand_
        };
    }

    /**
     * Returns a delegate that will post a wrapped job via the service.
     */
    template <typename Handler, typename... Args>
    auto unordered_delegate(Handler&& handler, Args&&... args) ->
        delegate::unordered<decltype(BIND_HANDLER(handler, args))>
    {
        return
        {
            BIND_HANDLER(handler, args),
            service_,
            strand_
        };
    }

private:
    asio::service& service_;
    asio::service::strand strand_;
};

#undef FORWARD_ARGS
#undef FORWARD_HANDLER
#undef BIND_ARGS
#undef BIND_HANDLER
#undef BIND_ELEMENT

// These are declared to improve binding readability in derived classes.
// These require that CLASS be defined in each source file where used,
// and that the base of the derive class implement the necessary methods.

#define BIND1(method, p1) \
    bind<CLASS>(&CLASS::method, p1)
#define BIND2(method, p1, p2) \
    bind<CLASS>(&CLASS::method, p1, p2)
#define BIND3(method, p1, p2, p3) \
    bind<CLASS>(&CLASS::method, p1, p2, p3)
#define BIND4(method, p1, p2, p3, p4) \
    bind<CLASS>(&CLASS::method, p1, p2, p3, p4)
#define BIND5(method, p1, p2, p3, p4, p5) \
    bind<CLASS>(&CLASS::method, p1, p2, p3, p4, p5)

#define ORDERED2(method, p1, p2) \
    ordered<CLASS>(&CLASS::method, p1, p2)
#define ORDERED3(method, p1, p2, p3) \
    ordered<CLASS>(&CLASS::method, p1, p2, p3)
#define ORDERED4(method, p1, p2, p3, p4) \
    ordered<CLASS>(&CLASS::method, p1, p2, p3, p4)
#define ORDERED5(method, p1, p2, p3, p4, p5) \
    ordered<CLASS>(&CLASS::method, p1, p2, p3, p4, p5)
#define ORDERED6(method, p1, p2, p3, p4, p5, p6) \
    ordered<CLASS>(&CLASS::method, p1, p2, p3, p4, p5, p6)

#define CALL1(method, p1) \
    call<CLASS>(&CLASS::method, p1)

#define SEND1(message, method, p1) \
    send<CLASS>(message, &CLASS::method, p1)
#define SEND2(message, method, p1, p2) \
    send<CLASS>(message, &CLASS::method, p1, p2)

#define SUBSCRIBE_STOP1(method, p1) \
    subscribe_stop<CLASS>(&CLASS::method, p1)

#define SUBSCRIBE2(message, method, p1, p2) \
    subscribe<CLASS, message>(&CLASS::method, p1, p2)

#define SUBSCRIBE3(message, method, p1, p2, p3) \
    subscribe<CLASS, message>(&CLASS::method, p1, p2, p3)

} // namespace libbitcoin

#endif
