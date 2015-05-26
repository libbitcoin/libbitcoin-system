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
#ifndef LIBBITCOIN_THREADPOOL_HPP
#define LIBBITCOIN_THREADPOOL_HPP

#include <functional>
#include <thread>
#include <boost/asio.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

/**
 * Threadpool represents a collection of running threads which can
 * be passed operations through their io_service.
 *
 * @code
 *  void task_foo();
 *
 *  // Run threadpool with a single running thread.
 *  threadpool pool(1);
 *  // Post operation to threadpool.
 *  // task_foo() will be called inside threadpool.
 *  pool.service.post(task_foo);
 * @endcode
 */
class BC_API threadpool
{
public:

    /**
     * Threadpool constructor. spawn()s number_threads threads.
     * @param[in]   number_threads  Number of threads to spawn.
     * @param[in]   priority        Priority of threads to spawn.
     */
     threadpool(size_t number_threads=0, 
        thread_priority priority=thread_priority::normal);

    ~threadpool();

    threadpool(const threadpool&) = delete;
    void operator=(const threadpool&) = delete;

    /**
     * Add n threads to this threadpool.
     * @param[in]   number_threads  Number of threads to add.
     * @param[in]   priority        Priority of threads to add.
     */
    void spawn(size_t number_threads=1, 
        thread_priority priority=thread_priority::normal);

    /**
     * Stop the threadpool. All remaining operations on the queue are dropped.
     */
    void stop();

    /**
     * Finish executing all remaining operations in the queue.
     * Adding more operations keeps the threadpool running.
     */
    void shutdown();

    /**
     * Join all the threads in this threadpool with the current thread,
     * This method will hang until all the threads in this threadpool have
     * finished their running tasks and exited.
     *
     * Attempting to join() from within any of the threads owned by this
     * threadpool is invalid.
     *
     * WARNING: Do not call this within any of the threads owned by this
     * threadpool. Doing so will cause a resource deadlock and an
     * std::system_error exception will be thrown.
     *
     * @code
     *  // Threadpool with 4 running threads.
     *  threadpool pool(4);
     *  // Stop all 4 threads.
     *  pool.stop();
     *  // Wait for them to finish gracefully.
     *  pool.join();
     * @endcode
     */
    void join();

    template <typename... Args>
    void push(Args&&... args)
    {
        ios_.post(std::bind(std::forward<Args>(args)...));
    }

    template <typename... Args>
    void dispatch(Args&&... args)
    {
        ios_.dispatch(std::bind(std::forward<Args>(args)...));
    }

    /**
     * Underlying boost::io_service object.
     */
    boost::asio::io_service& service();

    /**
     * Underlying boost::io_service object.
     */
    const boost::asio::io_service& service() const;

private:
    void spawn_once(thread_priority priority=thread_priority::normal);

    boost::asio::io_service ios_;
    boost::asio::io_service::work* work_;
    std::vector<std::thread> threads_;
};

} // namespace libbitcoin

#endif

