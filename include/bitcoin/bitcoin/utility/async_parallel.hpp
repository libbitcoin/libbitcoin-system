/**
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
#ifndef LIBBITCOIN_ASYNC_PARALLEL_HPP
#define LIBBITCOIN_ASYNC_PARALLEL_HPP

#include <atomic>
#include <cstddef>
#include <memory>
#include <system_error>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

/*
 * async_parallel(completion_handler, clearance_count)
 *
 * Returns a callback that will stop when:
 *
 * 1. An error is passed as the first argument.
 * 2. It has been called clearance_count number of times.
 *
 * This is useful when we have a piece of code that forks into several
 * asynchronous code paths. This utility ensures that all code paths
 * are completed before calling the completion_handler.
 *
 *   auto complete = [](std::error_code ec, std::string s) { ... };
 *   auto cb = async_parallel(complete, 3);
 *   cb(std::error_code, "hello");
 *   // Calling cb(error::service_stopped, "") here will
 *   // call complete(error::service_stopped, "")
 *   cb(std::error_code, "world");
 *   // Calls complete(std::error_code, "final")
 *   cb(std::error_code, "final");
 */

typedef std::atomic<size_t> atomic_counter;
typedef std::shared_ptr<atomic_counter> atomic_counter_ptr;

template <typename Handler>
struct async_parallel_dispatch
{
    Handler handler;
    size_t clearance_count;
    atomic_counter_ptr counter;

    template <typename... Args>
    void operator()(const std::error_code& code, Args&&... args)
    {
        BITCOIN_ASSERT(*counter <= clearance_count);
        if (*counter == clearance_count)
            return;

        if (code)
        {
            // Stop because of failure.
            *counter = clearance_count;
            handler(code, std::forward<Args>(args)...);
        }
        else if (++(*counter) == clearance_count)
        {
            // Finished executing multiple async paths.
            handler(code, std::forward<Args>(args)...);
        }
    }
};

template <typename Handler>
async_parallel_dispatch<typename std::decay<Handler>::type>
async_parallel(Handler&& handler, size_t clearance_count)
{
    return
    {
        handler,
        clearance_count,
        std::make_shared<atomic_counter>(0)
    };
}

} // libbitcoin

#endif

