/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN__UTILITY_THREADPOOL_HPP
#define LIBBITCOIN__UTILITY_THREADPOOL_HPP

#include <atomic>
#include <cstddef>
#include <memory>
#include <functional>
#include <thread>
#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/threadpool.hpp>
//#include <bitcoin/bitcoin/utility/thread.hpp>
#include <bitcoin/utility_service.hpp>

namespace libbitcoin {
namespace api {

/**
 * This class and the asio service it exposes are thread safe.
 * A collection of threads which can be passed operations through io_service.
 */
class BC_API utility_threadpool
  : public libbitcoin::threadpool
{
public:

    /**
     * Threadpool constructor, spawns the specified number of threads.
     * @param[in]   number_threads  Number of threads to spawn.
     * @param[in]   priority        Priority of threads to spawn.
     */
     utility_threadpool(size_t number_threads=0,
        thread_priority priority=thread_priority::normal);

    virtual ~utility_threadpool();

    /**
     * There are no threads configured in the utility_threadpool.
     */
    bool empty() const;

    /**
     * The number of threads configured in the utility_threadpool.
     */
    size_t size() const;

    /**
     * Add the specified number of threads to this utility_threadpool.
     * @param[in]   number_threads  Number of threads to add.
     * @param[in]   priority        Priority of threads to add.
     */
    void spawn(size_t number_threads=1,
        thread_priority priority=thread_priority::normal);

    /**
     * Abandon outstanding operations without dispatching handlers.
     * WARNING: This call is unsave and should be avoided.
     */
    void abort();

    /**
     * Destroy the work keep alive, allowing threads be joined.
     * Caller should next call join.
     */
    void shutdown();

    /**
     * Wait for all threads in the pool to terminate.
     * This is safe to call from any thread in the utility_threadpool or otherwise.
     */
    void join();

    /**
     * Underlying boost::io_service object.
     */
    libbitcoin::api::utility_service& service();

    /**
     * Underlying boost::io_service object.
     */
//    const asio::service& service() const;

//private:
//    void spawn_once(thread_priority priority=thread_priority::normal);
//
//    // This is thread safe.
//    asio::service service_;
//
//    // These are protected by mutex.
//
//    std::atomic<size_t> size_;
//    std::vector<asio::thread> threads_;
//    mutable upgrade_mutex threads_mutex_;
//
//    std::shared_ptr<asio::service::work> work_;
//    mutable upgrade_mutex work_mutex_;
};

} // namespace api
} // namespace libbitcoin

#endif

