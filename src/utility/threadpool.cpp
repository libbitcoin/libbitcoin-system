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

#include <algorithm>
#include <memory>
#include <new>
#include <thread>
#include <utility>
#include <bitcoin/bitcoin/log/sink.hpp>
#include <bitcoin/bitcoin/log/source.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>
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

// This is not thread safe.
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

void threadpool::join()
{
    ///////////////////////////////////////////////////////////////////////////
    // Critical Section
    unique_lock lock(threads_mutex_);

    const auto joiner = [this]()
    {
        for (auto& thread: threads_)
        {
            // Since we do not use default thread construction only joined
            // threads will be non-joinable. But we always clear after joining
            // so should not be possible to observe a non-joinable thread here.
            BITCOIN_ASSERT(thread.joinable());

            thread.join();
        }
    };

    const auto id = boost::this_thread::get_id();
    const auto self = [id](const asio::thread& thread)
    {
        return id == thread.get_id();
    };

    // Deadlock would occur if we attempted to join our own thread. However if
    // we do not block until each thread join we will allow a thread to
    // continue after this call, allowing the caller to terminate process or
    // free resources still in use by the thread. Also, when threads_ is
    // deleted it terminates all executing threads.
    const auto any_self = std::any_of(threads_.begin(), threads_.end(), self);

    if (any_self)
    {
        asio::thread dispatch(joiner);
        dispatch.join();
    }
    else
    {
        joiner();
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
