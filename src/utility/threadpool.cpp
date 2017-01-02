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
#include <stdexcept>
#include <thread>
#include <utility>
#include <bitcoin/bitcoin/log/sink.hpp>
#include <bitcoin/bitcoin/log/source.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

#define LOG_SYSTEM "system"

threadpool::threadpool(size_t number_threads, thread_priority priority)
{
    spawn(number_threads, priority);
}

threadpool::~threadpool()
{
    shutdown();
    join();
}

bool threadpool::empty() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    shared_lock lock(threads_mutex_);

    return threads_.size() != 0;
    ///////////////////////////////////////////////////////////////////////////
}

// Should not be called during spawn.
size_t threadpool::size() const
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    shared_lock lock(threads_mutex_);

    return threads_.size();
    ///////////////////////////////////////////////////////////////////////////
}

// Not thread safe.
void threadpool::spawn(size_t number_threads, thread_priority priority)
{
    // This allows the pool to be restarted.
    service_.reset();

    for (size_t i = 0; i < number_threads; ++i)
        spawn_once(priority);
}

void threadpool::spawn_once(thread_priority priority)
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    work_mutex_.lock_upgrade();

    // Work prevents the service from running out of work and terminating.
    if (!work_)
    {
        work_mutex_.unlock_upgrade_and_lock();
        //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        work_ = std::make_shared<asio::service::work>(service_);

        work_mutex_.unlock_and_lock_upgrade();
        //-----------------------------------------------------------------
    }

    work_mutex_.unlock_upgrade();
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(threads_mutex_);

    threads_.push_back(asio::thread([this, priority]()
    {
        set_thread_priority(priority);
        service_.run();
    }));
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

    work_.reset();
    ///////////////////////////////////////////////////////////////////////////
}

inline bool self(asio::thread& thread)
{
    return boost::this_thread::get_id() == thread.get_id();
}

void threadpool::join()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(threads_mutex_);

    for (auto& thread: threads_)
    {
        if (self(thread))
        {
            LOG_FATAL(LOG_SYSTEM) << "cannot join self";
            ////throw std::runtime_error("cannot join self");
        }

        if (!thread.joinable())
        {
            LOG_FATAL(LOG_SYSTEM) << "thread not joinable";
            ////throw std::runtime_error("thread not joinable");
        }

        if (!self(thread) && thread.joinable())
            thread.join();
    }

    threads_.clear();
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
