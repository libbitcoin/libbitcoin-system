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
#include <bitcoin/bitcoin/utility/threadpool.hpp>

#include <memory>
#include <new>
#include <thread>
#include <bitcoin/bitcoin/network/asio.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

threadpool::threadpool(size_t number_threads, thread_priority priority)
{
    spawn(number_threads, priority);
}

threadpool::~threadpool()
{
    shutdown();
}

void threadpool::spawn(size_t number_threads, thread_priority priority)
{
    for (size_t i = 0; i < number_threads; ++i)
        spawn_once(priority);
}

void threadpool::spawn_once(thread_priority priority)
{
    // Work prevents the service from running out of work and terminating.
    if (!work_)
        work_ = std::make_unique<asio::service::work>(service_);

    const auto action = [this, priority]
    {
        set_thread_priority(priority);
        service_.run();
    };

    threads_.push_back(std::thread(action));
}

void threadpool::stop()
{
    // TODO: don't call this on shutdown, instead signal the application.
    // Signals the service to stop. Calls will return immediately without
    // invoking any handlers.
    service_.stop();
}

void threadpool::shutdown()
{
    // This does not terminate outstanding calls, it just allows service to
    // terminate once all outstanding calls are complete.
    work_ = nullptr;
}

void threadpool::join()
{
    // TODO: join the threadpool on destruct?
    for (auto& thread: threads_)
        if (thread.joinable())
            thread.join();
}

asio::service& threadpool::service()
{
    return service_;
}

const asio::service& threadpool::service() const
{
    return service_;
}

} // namespace libbitcoin

