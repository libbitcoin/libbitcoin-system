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
#include <bitcoin/bitcoin/utility/threadpool.hpp>
#include <system_error>

namespace libbitcoin {

using boost::asio::io_service;

threadpool::threadpool()
  : work_(nullptr)
{
}

threadpool::threadpool(size_t number_threads)
  : work_(nullptr)
{
    spawn(number_threads);
}

threadpool::~threadpool()
{
    delete work_;
}

void threadpool::spawn(size_t number_threads)
{
    for (size_t i = 0; i < number_threads; ++i)
        spawn_once();
}
void threadpool::spawn_once()
{
    if (!work_)
        work_ = new io_service::work(ios_);
    threads_.push_back(std::thread([this] { ios_.run(); }));
}

void threadpool::stop()
{
    ios_.stop();
}
void threadpool::shutdown()
{
    delete work_;
    work_ = nullptr;
}
void threadpool::join()
{
    for (std::thread& t: threads_)
    {
        try
        {
            t.join();
        }
        catch (const std::system_error&)
        {
            // other than logging, or altering the signature to
            // return the error to the join caller, not sure what
            // to do here.
        }
    }
}

io_service& threadpool::service()
{
    return ios_;
}
const io_service& threadpool::service() const
{
    return ios_;
}

async_strand::async_strand(threadpool& pool)
  : ios_(pool.service()), strand_(ios_)
{
}

} // namespace libbitcoin

