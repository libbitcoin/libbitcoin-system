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
#ifndef LIBBITCOIN_THREADPOOL_HPP
#define LIBBITCOIN_THREADPOOL_HPP

#include <memory>
#include <functional>
#include <thread>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/asio.hpp>
#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {

/**
 * A collection of threads which can be passed operations through io_service.
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
     * Abandon outstanding operations without dispatching handlers.
     * Terminate threads once work is complete.
     * Prevents the enqueuing of new handlers.
     * Caller should next call join, which will block until complete.
     *
     * WARNING: This can result in leaks in the case of heap allocated objects
     * referenced captured in handlers that may not execute.
     */
    void abort();

    /**
     * Allow outstanding operations and handlers to finish normally.
     * Terminate threads once work is complete.
     * Allows the enqueuing of new handlers.
     * Caller should next call join, which will block until complete.
     */
    void shutdown();

    /**
     * Wait for all threads in the pool to terminate.
     *
     * WARNING: Do not call this within any of the threads owned by this
     * threadpool. Doing so will cause a resource deadlock and an
     * std::system_error exception will be thrown.
     */
    void join();

    /**
     * Underlying boost::io_service object.
     */
    asio::service& service();

    /**
     * Underlying boost::io_service object.
     */
    const asio::service& service() const;

private:
    void spawn_once(thread_priority priority=thread_priority::normal);

    asio::service service_;
    std::vector<asio::thread> threads_;
    std::shared_ptr<asio::service::work> work_;
};

} // namespace libbitcoin

#endif

