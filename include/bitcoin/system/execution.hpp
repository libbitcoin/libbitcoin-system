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
#ifndef LIBBITCOIN_SYSTEM_EXECUTION_HPP
#define LIBBITCOIN_SYSTEM_EXECUTION_HPP

#include <atomic>
#include <condition_variable>
#include <future>
#include <mutex>
#include <numeric>
#include <queue>
#include <thread>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace execution {

/// Fixed set of workers consuming a queue of tasks.
class BC_API thread_pool final
{
public:
    DELETE_COPY_MOVE(thread_pool);

    /// The process-wide pool, one worker per hardware thread.
    static thread_pool& get() NOEXCEPT;

    /// Start the workers, at least one.
    thread_pool(size_t workers=std::thread::hardware_concurrency()) NOEXCEPT;

    /// Complete all queued tasks and join the workers.
    ~thread_pool() NOEXCEPT;

    /// The number of workers.
    size_t size() const NOEXCEPT;

    /// Queue a task and wake one worker, the future completes when it has run.
    template <typename Task>
    std::future<void> submit(Task&& task) NOEXCEPT
    {
        std::packaged_task<void()> packaged{ std::forward<Task>(task) };
        auto future = packaged.get_future();
        std::lock_guard<std::mutex> lock{ mutex_ };
        tasks_.push(std::move(packaged));
        condition_.notify_one();
        return future;
    }

private:
    void work() NOEXCEPT;

    std::vector<std::thread> workers_{};
    std::queue<std::packaged_task<void()>> tasks_{};
    std::mutex mutex_{};
    std::condition_variable condition_{};
    bool stopped_{};
};

/// Whether an algorithm runs on the pool, and the minimum elements per task.
class policy
{
public:
    explicit constexpr policy(bool parallel, size_t floor=one) NOEXCEPT
      : parallel_(parallel), floor_(std::max(floor, one))
    {
    }

    constexpr bool parallel() const NOEXCEPT
    {
        return parallel_;
    }

    constexpr size_t floor() const NOEXCEPT
    {
        return floor_;
    }

private:
    bool parallel_;
    size_t floor_;
};

constexpr policy seq{ false };
constexpr policy par{ true };

constexpr policy par_if(bool parallel, size_t floor=one) NOEXCEPT
{
    return policy{ parallel, floor };
}

/// Tasks for a count of elements, at most one per worker, none below the floor.
inline size_t chunks(const policy& policy, size_t count) NOEXCEPT
{
    return std::max(std::min(floored_divide(count, policy.floor()),
        thread_pool::get().size()), one);
}

/// A range that does not fill two tasks runs on the caller.
template <typename Iterator>
bool serial(const policy& policy, Iterator first, Iterator last) NOEXCEPT
{
    return !policy.parallel() ||
        chunks(policy, to_unsigned(std::distance(first, last))) < two;
}

/// Task boundaries over a range, balanced to within one element.
template <typename Iterator>
std::vector<Iterator> bounds(const policy& policy, Iterator first,
    Iterator last) NOEXCEPT
{
    const auto count = to_unsigned(std::distance(first, last));
    const auto tasks = chunks(policy, count);
    const auto width = floored_divide(count, tasks);
    const auto extra = floored_modulo(count, tasks);
    std::vector<Iterator> out{};
    out.reserve(add1(tasks));
    out.push_back(first);

    for (size_t index{}; index < tasks; ++index)
        out.push_back(std::next(out.back(), to_signed(width + (index < extra ? one : zero))));

    return out;
}

/// Run task(first, last, index) on the pool for each chunk and wait for all.
template <typename Iterator, typename Task>
void chunked(const policy& policy, Iterator first, Iterator last,
    Task task) THROWS
{
    auto& pool = thread_pool::get();
    const auto edges = bounds(policy, first, last);
    std::vector<std::future<void>> futures{};
    futures.reserve(sub1(edges.size()));

    for (size_t index{}; index < sub1(edges.size()); ++index)
    {
        const auto from = edges.at(index);
        const auto to = edges.at(add1(index));
        futures.push_back(pool.submit([=]() { task(from, to, index); }));
    }

    for (auto& future: futures)
        future.get();
}

} // namespace execution
} // namespace system
} // namespace libbitcoin

namespace poolstl {
namespace execution = libbitcoin::system::execution;
}

namespace std {

template <typename Iterator, typename Function>
void for_each(const bc::system::execution::policy& policy, Iterator first,
    Iterator last, Function function) THROWS
{
    if (bc::system::execution::serial(policy, first, last))
    {
        std::for_each(first, last, function);
        return;
    }

    bc::system::execution::chunked(policy, first, last,
        [&function](Iterator from, Iterator to, size_t)
        {
            std::for_each(from, to, function);
        });
}

template <typename Iterator, typename Output, typename Operation>
Output transform(const bc::system::execution::policy& policy, Iterator first,
    Iterator last, Output out, Operation operation) THROWS
{
    if (bc::system::execution::serial(policy, first, last))
        return std::transform(first, last, out, operation);

    bc::system::execution::chunked(policy, first, last,
        [&](Iterator from, Iterator to, size_t)
        {
            const auto at = std::next(out, std::distance(first, from));
            std::transform(from, to, at, operation);
        });

    return std::next(out, std::distance(first, last));
}

template <typename Iterator1, typename Iterator2, typename Output,
    typename Operation>
Output transform(const bc::system::execution::policy& policy, Iterator1 first1,
    Iterator1 last1, Iterator2 first2, Output out, Operation operation) THROWS
{
    if (bc::system::execution::serial(policy, first1, last1))
        return std::transform(first1, last1, first2, out, operation);

    bc::system::execution::chunked(policy, first1, last1,
        [&](Iterator1 from, Iterator1 to, size_t)
        {
            const auto offset = std::distance(first1, from);
            const auto with = std::next(first2, offset);
            const auto at = std::next(out, offset);
            std::transform(from, to, with, at, operation);
        });

    return std::next(out, std::distance(first1, last1));
}

template <typename Iterator, typename Value, typename Operation>
Value reduce(const bc::system::execution::policy& policy, Iterator first,
    Iterator last, Value init, Operation operation) THROWS
{
    if (bc::system::execution::serial(policy, first, last))
        return std::reduce(first, last, init, operation);

    const auto count = bc::system::to_unsigned(std::distance(first, last));
    std::vector<Value> partials(bc::system::execution::chunks(policy, count));
    bc::system::execution::chunked(policy, first, last,
        [&](Iterator from, Iterator to, size_t index)
        {
            const Value partial = *from;
            partials.at(index) = std::reduce(std::next(from), to, partial,
                operation);
        });

    return std::reduce(partials.begin(), partials.end(), init, operation);
}

template <typename Iterator, typename Value, typename Reduction,
    typename Transformation>
Value transform_reduce(const bc::system::execution::policy& policy,
    Iterator first, Iterator last, Value init, Reduction reduction,
    Transformation transformation) THROWS
{
    if (bc::system::execution::serial(policy, first, last))
        return std::transform_reduce(first, last, init, reduction,
            transformation);

    const auto count = bc::system::to_unsigned(std::distance(first, last));
    std::vector<Value> partials(bc::system::execution::chunks(policy, count));
    bc::system::execution::chunked(policy, first, last,
        [&](Iterator from, Iterator to, size_t index)
        {
            const Value partial = transformation(*from);
            partials.at(index) = std::transform_reduce(std::next(from), to,
                partial, reduction, transformation);
        });

    return std::reduce(partials.begin(), partials.end(), init, reduction);
}

template <typename Iterator, typename Predicate>
bool all_of(const bc::system::execution::policy& policy, Iterator first,
    Iterator last, Predicate predicate) THROWS
{
    if (bc::system::execution::serial(policy, first, last))
        return std::all_of(first, last, predicate);

    std::atomic_bool all{ true };
    bc::system::execution::chunked(policy, first, last,
        [&](Iterator from, Iterator to, size_t)
        {
            if (all && !std::all_of(from, to, predicate))
                all = false;
        });

    return all;
}

/// Sorted runs of one chunk each are merged pairwise until one remains.
template <typename Iterator, typename Compare>
void sort(const bc::system::execution::policy& policy, Iterator first,
    Iterator last, Compare compare) THROWS
{
    if (bc::system::execution::serial(policy, first, last))
    {
        std::sort(first, last, compare);
        return;
    }

    bc::system::execution::chunked(policy, first, last,
        [&](Iterator from, Iterator to, size_t)
        {
            std::sort(from, to, compare);
        });

    auto& pool = bc::system::execution::thread_pool::get();
    auto runs = bc::system::execution::bounds(policy, first, last);
    while (runs.size() > bc::two)
    {
        std::vector<std::future<void>> futures{};
        std::vector<Iterator> merged{ runs.front() };
        for (auto at = bc::two; at < runs.size(); at += bc::two)
        {
            const auto from = runs.at(at - bc::two);
            const auto middle = runs.at(bc::sub1(at));
            const auto to = runs.at(at);
            futures.push_back(pool.submit([=, &compare]()
            {
                std::inplace_merge(from, middle, to, compare);
            }));

            merged.push_back(to);
        }

        if (bc::is_even(runs.size()))
            merged.push_back(runs.back());

        for (auto& future: futures)
            future.get();

        runs = std::move(merged);
    }
}

} // namespace std

#endif
