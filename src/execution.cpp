/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/execution.hpp>

#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
namespace execution {

// static
thread_pool& thread_pool::get() NOEXCEPT
{
    static thread_pool pool{};
    return pool;
}

thread_pool::thread_pool(size_t workers) NOEXCEPT
{
    const auto count = std::max(workers, one);
    workers_.reserve(count);
    for (size_t index{}; index < count; ++index)
        workers_.emplace_back(&thread_pool::work, this);
}

thread_pool::~thread_pool() NOEXCEPT
{
    {
        std::lock_guard<std::mutex> lock{ mutex_ };
        stopped_ = true;
        condition_.notify_all();
    }

    for (auto& worker: workers_)
        worker.join();
}

size_t thread_pool::size() const NOEXCEPT
{
    return workers_.size();
}

// A stopped worker drains the queue before exiting.
void thread_pool::work() NOEXCEPT
{
    while (true)
    {
        std::unique_lock<std::mutex> lock{ mutex_ };
        condition_.wait(lock, [this]() NOEXCEPT
        {
            return stopped_ || !tasks_.empty();
        });

        if (tasks_.empty())
            return;

        auto task = std::move(tasks_.front());
        tasks_.pop();
        lock.unlock();
        task();
    }
}

} // namespace execution
} // namespace system
} // namespace libbitcoin
