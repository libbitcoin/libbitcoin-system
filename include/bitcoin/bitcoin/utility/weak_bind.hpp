/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_UTILITY_WEAK_BIND_HPP
#define LIBBITCOIN_UTILITY_WEAK_BIND_HPP

#include <functional>
#include <memory>

namespace libbitcoin {

template <typename WeakPtr, typename Handler>
struct weak_wrapper_impl
{
    WeakPtr weak_ref;
    Handler handler;

    template <typename... Args>
    void operator()(Args&&... args)
    {
        auto raii_lock = weak_ref.lock();
        if (raii_lock)
            handler(std::forward<Args>(args)...);
    }
};

template <typename WeakPtr, typename Handler>
weak_wrapper_impl<
    WeakPtr,
    typename std::decay<Handler>::type
>
weak_wrapper(WeakPtr weak_ref, Handler handler)
{
    return {weak_ref, handler};
}

template <typename MemberFunction, typename Class, typename... Args>
auto weak_bind(MemberFunction member_func,
    std::shared_ptr<Class> ref, Args&&... args)
    -> decltype(
        weak_wrapper(
            std::weak_ptr<Class>(ref),
            std::bind(member_func, ref.get(), std::forward<Args>(args)...)
        ))
{
    std::weak_ptr<Class> weak_ref = ref;
    auto handler =
        std::bind(member_func, ref.get(), std::forward<Args>(args)...);
    return weak_wrapper(weak_ref, handler);
}

} // libbitcoin

#endif

