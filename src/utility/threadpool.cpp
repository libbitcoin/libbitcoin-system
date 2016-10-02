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
#include <bitcoin/bitcoin/utility/threadpool.hpp>

#include <memory>
#include <new>
#include <thread>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

threadpool::threadpool(size_t number_threads, thread_priority priority)
{
    spawn(number_threads, priority);
}

threadpool::~threadpool()
{
    shutdown();
    join();
}

// Should not be called during spawn.
size_t threadpool::size() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    shared_lock(threads_mutex);

    return threads_.size();
    ///////////////////////////////////////////////////////////////////////////
}

// Not thread safe.
void threadpool::spawn(size_t number_threads, thread_priority priority)
{
    for (size_t i = 0; i < number_threads; ++i)
        spawn_once(priority);
}

// Not thread safe.
void threadpool::spawn_once(thread_priority priority)
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    work_mutex_.lock_upgrade();

    // Work prevents the service from running out of work and terminating.
    if (!work_)
    {
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        work_mutex_.unlock_upgrade_and_lock();
        work_ = std::make_shared<asio::service::work>(service_);
        work_mutex_.unlock_and_lock_upgrade();
        //-----------------------------------------------------------------
    }

    work_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    const auto action = [this, priority]
    {
        set_thread_priority(priority);
        service_.run();
    };

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock(threads_mutex);

    threads_.push_back(asio::thread(action));
    ///////////////////////////////////////////////////////////////////////////
}

void threadpool::abort()
{
    service_.stop();
}

void threadpool::shutdown()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(work_mutex_);

    work_ = nullptr;
    ///////////////////////////////////////////////////////////////////////////
}

// Not thread safe.
void threadpool::join()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    threads_mutex_.lock_upgrade();

    for (auto& thread: threads_)
    {
        if (thread.joinable())
        {
            //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            threads_mutex_.unlock_upgrade_and_lock();
            thread.join();
            threads_mutex_.unlock_and_lock_upgrade();
            //-----------------------------------------------------------------
        }
    }

    // This allows the pool to be cleanly restarted by calling spawn.
    threads_.clear();
    service_.reset();

    threads_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////
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
