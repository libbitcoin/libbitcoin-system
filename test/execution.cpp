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
#include "test.hpp"
#include <atomic>
#include <numeric>

BC_AUTO_TEST_SUITE(execution_tests)

using namespace execution;

constexpr size_t count = 1000;
static const size_t workers = thread_pool::get().size();

static std::vector<size_t> sequence(size_t size) NOEXCEPT
{
    std::vector<size_t> out(size);
    std::iota(out.begin(), out.end(), one);
    return out;
}

static std::vector<size_t> scrambled(size_t size) NOEXCEPT
{
    std::vector<size_t> out(size);
    for (size_t index{}; index < size; ++index)
        out.at(index) = (index * 7919) % size;

    return out;
}

static size_t drained(size_t tasks) NOEXCEPT
{
    std::atomic_size_t ran{};
    {
        thread_pool pool{ 1 };
        for (size_t index{}; index < tasks; ++index)
            pool.submit([&]() NOEXCEPT { ++ran; });
    }

    return ran.load();
}

template <typename Edges>
static size_t width(const Edges& edges, size_t index) NOEXCEPT
{
    return to_unsigned(std::distance(edges.at(index), edges.at(add1(index))));
}

template <typename Edges>
static size_t smallest_width(const Edges& edges) NOEXCEPT
{
    size_t smallest = max_size_t;
    for (size_t index{}; index < sub1(edges.size()); ++index)
        smallest = std::min(smallest, width(edges, index));

    return smallest;
}

template <typename Edges>
static size_t largest_width(const Edges& edges) NOEXCEPT
{
    size_t largest = zero;
    for (size_t index{}; index < sub1(edges.size()); ++index)
        largest = std::max(largest, width(edges, index));

    return largest;
}

static ptrdiff_t leading_width(size_t tasks) NOEXCEPT
{
    return tasks == 2u ? 3 : 5;
}

static bool chunked_once_by_index(const policy& policy) NOEXCEPT
{
    const auto values = sequence(count);
    const auto edges = bounds(policy, values.cbegin(), values.cend());
    std::vector<size_t> seen(count);
    std::vector<size_t> indexes(count);
    chunked(policy, values.cbegin(), values.cend(),
        [&](auto from, auto to, size_t index) NOEXCEPT
        {
            for (auto it = from; it != to; ++it)
            {
                seen.at(*it - one) += one;
                indexes.at(*it - one) = index;
            }
        });

    for (const auto once: seen)
        if (once != one)
            return false;

    for (size_t index{}; index < sub1(edges.size()); ++index)
        for (auto it = edges.at(index); it != edges.at(add1(index)); ++it)
            if (indexes.at(*it - one) != index)
                return false;

    return true;
}

static bool for_each_serial(const policy& policy) NOEXCEPT
{
    const auto values = sequence(count);
    size_t expected{};
    std::for_each(values.cbegin(), values.cend(),
        [&](size_t value) NOEXCEPT { expected += value; });

    std::atomic_size_t sum{};
    std::for_each(policy, values.cbegin(), values.cend(),
        [&](size_t value) NOEXCEPT { sum += value; });

    return sum.load() == expected;
}

static bool transform_unary_serial(const policy& policy) NOEXCEPT
{
    const auto values = sequence(count);
    const auto square = [](size_t value) NOEXCEPT { return value * value; };
    std::vector<size_t> expected(count);
    std::transform(values.cbegin(), values.cend(), expected.begin(), square);

    std::vector<size_t> out(count);
    const auto end = std::transform(policy, values.cbegin(), values.cend(),
        out.begin(), square);

    return end == out.cend() && out == expected;
}

static bool transform_binary_serial(const policy& policy) NOEXCEPT
{
    const auto left = sequence(count);
    const auto right = scrambled(count);
    const auto add = [](size_t one, size_t two) NOEXCEPT { return one + two; };
    std::vector<size_t> expected(count);
    std::transform(left.cbegin(), left.cend(), right.cbegin(),
        expected.begin(), add);

    std::vector<size_t> out(count);
    const auto end = std::transform(policy, left.cbegin(), left.cend(),
        right.cbegin(), out.begin(), add);

    return end == out.cend() && out == expected;
}

static bool reduce_serial(const policy& policy) NOEXCEPT
{
    const auto values = sequence(count);
    const auto expected = std::reduce(values.cbegin(), values.cend(), 42_size,
        std::plus<size_t>{});

    return std::reduce(policy, values.cbegin(), values.cend(), 42_size,
        std::plus<size_t>{}) == expected;
}

static bool transform_reduce_serial(const policy& policy) NOEXCEPT
{
    const auto values = sequence(count);
    const auto square = [](size_t value) NOEXCEPT { return value * value; };
    const auto expected = std::transform_reduce(values.cbegin(),
        values.cend(), 42_size, std::plus<size_t>{}, square);

    return std::transform_reduce(policy, values.cbegin(), values.cend(),
        42_size, std::plus<size_t>{}, square) == expected;
}

template <typename Predicate>
static bool all_of_serial(const policy& policy,
    const Predicate& predicate) NOEXCEPT
{
    const auto values = sequence(count);
    const auto expected = std::all_of(values.cbegin(), values.cend(),
        predicate);

    return std::all_of(policy, values.cbegin(), values.cend(), predicate) ==
        expected;
}

static bool sort_serial(const policy& policy) NOEXCEPT
{
    const auto greater = [](size_t left, size_t right) NOEXCEPT
    {
        return left > right;
    };

    auto expected = scrambled(count);
    std::sort(expected.begin(), expected.end(), greater);
    auto values = scrambled(count);
    std::sort(policy, values.begin(), values.end(), greater);
    return values == expected;
}

static bool sorts(size_t size) NOEXCEPT
{
    auto expected = scrambled(size);
    std::sort(expected.begin(), expected.end());
    auto values = scrambled(size);
    std::sort(par, values.begin(), values.end(), std::less<size_t>{});
    return values == expected;
}

// thread_pool

BC_AUTO_TEST_CASE(execution__thread_pool__zero_workers__one)
{
    thread_pool pool{ 0 };
    BC_REQUIRE_EQUAL(pool.size(), one);
}

BC_AUTO_TEST_CASE(execution__thread_pool__three_workers__three)
{
    thread_pool pool{ 3 };
    BC_REQUIRE_EQUAL(pool.size(), 3u);
}

BC_AUTO_TEST_CASE(execution__thread_pool__get__at_least_one_worker)
{
    BC_REQUIRE_GE(workers, one);
    BC_REQUIRE_EQUAL(&thread_pool::get(), &thread_pool::get());
}

BC_AUTO_TEST_CASE(execution__thread_pool__submit__future_completes_after_run)
{
    thread_pool pool{ 2 };
    std::atomic_size_t ran{};
    auto future = pool.submit([&]() NOEXCEPT { ++ran; });
    future.get();
    BC_REQUIRE_EQUAL(ran.load(), one);
}

BC_AUTO_TEST_CASE(execution__thread_pool__submit_throwing__future_rethrows)
{
    thread_pool pool{ 1 };
    auto future = pool.submit([]() { throw std::runtime_error{ "task" }; });
    BC_REQUIRE_THROW(future.get(), std::runtime_error);
}

BC_AUTO_TEST_CASE(execution__thread_pool__destruct__drains_queue)
{
    BC_REQUIRE_EQUAL(drained(count), count);
}

// policy

BC_AUTO_TEST_CASE(execution__policy__seq__not_parallel_floor_one)
{
    BC_REQUIRE(!seq.parallel());
    BC_REQUIRE_EQUAL(seq.floor(), one);
}

BC_AUTO_TEST_CASE(execution__policy__par__parallel_floor_one)
{
    BC_REQUIRE(par.parallel());
    BC_REQUIRE_EQUAL(par.floor(), one);
}

BC_AUTO_TEST_CASE(execution__policy__par_if__expected)
{
    BC_REQUIRE(!par_if(false).parallel());
    BC_REQUIRE(par_if(true).parallel());
    BC_REQUIRE_EQUAL(par_if(true).floor(), one);
    BC_REQUIRE_EQUAL(par_if(true, 32).floor(), 32u);
    BC_REQUIRE_EQUAL(par_if(false, 32).floor(), 32u);
}

BC_AUTO_TEST_CASE(execution__policy__zero_floor__one)
{
    BC_REQUIRE_EQUAL(par_if(true, 0).floor(), one);
}

// chunks

BC_AUTO_TEST_CASE(execution__chunks__empty__one)
{
    BC_REQUIRE_EQUAL(chunks(par, 0), one);
    BC_REQUIRE_EQUAL(chunks(par_if(true, 32), 0), one);
}

BC_AUTO_TEST_CASE(execution__chunks__within_floor__one)
{
    BC_REQUIRE_EQUAL(chunks(par_if(true, 32), 1), one);
    BC_REQUIRE_EQUAL(chunks(par_if(true, 32), 32), one);
    BC_REQUIRE_EQUAL(chunks(par_if(true, 32), 63), one);
}

BC_AUTO_TEST_CASE(execution__chunks__floor_multiples__floored_quotient_to_workers)
{
    BC_REQUIRE_EQUAL(chunks(par_if(true, 32), 64), std::min(2_size, workers));
    BC_REQUIRE_EQUAL(chunks(par_if(true, 32), 95), std::min(2_size, workers));
    BC_REQUIRE_EQUAL(chunks(par_if(true, 32), 96), std::min(3_size, workers));
    BC_REQUIRE_EQUAL(chunks(par_if(true, 10), count), std::min(100_size, workers));
}

BC_AUTO_TEST_CASE(execution__chunks__floor_one__count_to_workers)
{
    BC_REQUIRE_EQUAL(chunks(par, 1), one);
    BC_REQUIRE_EQUAL(chunks(par, 2), std::min(2_size, workers));
    BC_REQUIRE_EQUAL(chunks(par, count), std::min(count, workers));
}

BC_AUTO_TEST_CASE(execution__chunks__large_count__never_exceeds_workers)
{
    BC_REQUIRE_EQUAL(chunks(par, max_size_t), workers);
}

// serial

BC_AUTO_TEST_CASE(execution__serial__seq__always)
{
    const auto values = sequence(count);
    BC_REQUIRE(serial(seq, values.cbegin(), values.cend()));
    BC_REQUIRE(serial(seq, values.cbegin(), values.cbegin()));
}

BC_AUTO_TEST_CASE(execution__serial__par_empty_or_one__true)
{
    const auto values = sequence(2);
    BC_REQUIRE(serial(par, values.cbegin(), values.cbegin()));
    BC_REQUIRE(serial(par, values.cbegin(), std::next(values.cbegin())));
}

BC_AUTO_TEST_CASE(execution__serial__par_two__only_when_one_worker)
{
    const auto values = sequence(2);
    BC_REQUIRE_EQUAL(serial(par, values.cbegin(), values.cend()), workers < two);
}

BC_AUTO_TEST_CASE(execution__serial__floor__below_two_tasks)
{
    const auto values = sequence(64);
    const auto policy = par_if(true, 32);
    BC_REQUIRE(serial(policy, values.cbegin(), std::next(values.cbegin(), 32)));
    BC_REQUIRE(serial(policy, values.cbegin(), std::next(values.cbegin(), 63)));
    BC_REQUIRE_EQUAL(serial(policy, values.cbegin(), values.cend()), workers < two);
}

// bounds

BC_AUTO_TEST_CASE(execution__bounds__empty__first_twice)
{
    const std::vector<size_t> values{};
    const auto edges = bounds(par, values.cbegin(), values.cend());
    BC_REQUIRE_EQUAL(edges.size(), two);
    BC_REQUIRE(edges.front() == values.cbegin());
    BC_REQUIRE(edges.back() == values.cend());
}

BC_AUTO_TEST_CASE(execution__bounds__floor__contiguous_balanced_not_below_floor)
{
    constexpr size_t floor = 7;
    const auto values = sequence(count);
    const auto policy = par_if(true, floor);
    const auto edges = bounds(policy, values.cbegin(), values.cend());
    BC_REQUIRE_EQUAL(edges.size(), add1(chunks(policy, count)));
    BC_REQUIRE(edges.front() == values.cbegin());
    BC_REQUIRE(edges.back() == values.cend());
    BC_REQUIRE_GE(smallest_width(edges), floor);
    BC_REQUIRE_LE(largest_width(edges) - smallest_width(edges), one);
}

BC_AUTO_TEST_CASE(execution__bounds__remainder__goes_to_leading_tasks)
{
    const auto values = sequence(5);
    const auto policy = par_if(true, 2);
    const auto edges = bounds(policy, values.cbegin(), values.cend());
    const auto tasks = std::min(2_size, workers);
    BC_REQUIRE_EQUAL(edges.size(), add1(tasks));
    BC_REQUIRE_EQUAL(std::distance(edges.at(0), edges.at(1)), leading_width(tasks));
}

// chunked

BC_AUTO_TEST_CASE(execution__chunked__floor__each_element_once_with_task_index)
{
    BC_REQUIRE(chunked_once_by_index(par_if(true, 3)));
}

BC_AUTO_TEST_CASE(execution__chunked__throwing_task__rethrows)
{
    const auto values = sequence(count);
    BC_REQUIRE_THROW(chunked(par, values.cbegin(), values.cend(), [](auto, auto, size_t) { throw std::runtime_error{ "chunk" }; }), std::runtime_error);
}

// for_each

BC_AUTO_TEST_CASE(execution__for_each__policies__same_as_serial)
{
    BC_REQUIRE(for_each_serial(seq));
    BC_REQUIRE(for_each_serial(par));
    BC_REQUIRE(for_each_serial(par_if(true, 7)));
    BC_REQUIRE(for_each_serial(par_if(true, count)));
}

BC_AUTO_TEST_CASE(execution__for_each__empty__no_calls)
{
    const std::vector<size_t> values{};
    std::atomic_size_t calls{};
    std::for_each(par, values.cbegin(), values.cend(), [&](size_t) NOEXCEPT { ++calls; });
    BC_REQUIRE_EQUAL(calls.load(), zero);
}

// transform

BC_AUTO_TEST_CASE(execution__transform__unary_policies__same_as_serial)
{
    BC_REQUIRE(transform_unary_serial(seq));
    BC_REQUIRE(transform_unary_serial(par));
    BC_REQUIRE(transform_unary_serial(par_if(true, 7)));
    BC_REQUIRE(transform_unary_serial(par_if(true, count)));
}

BC_AUTO_TEST_CASE(execution__transform__binary_policies__same_as_serial)
{
    BC_REQUIRE(transform_binary_serial(seq));
    BC_REQUIRE(transform_binary_serial(par));
    BC_REQUIRE(transform_binary_serial(par_if(true, 7)));
    BC_REQUIRE(transform_binary_serial(par_if(true, count)));
}

// reduce

BC_AUTO_TEST_CASE(execution__reduce__policies__same_as_serial)
{
    BC_REQUIRE(reduce_serial(seq));
    BC_REQUIRE(reduce_serial(par));
    BC_REQUIRE(reduce_serial(par_if(true, 7)));
    BC_REQUIRE(reduce_serial(par_if(true, count)));
}

BC_AUTO_TEST_CASE(execution__reduce__init_applied_once)
{
    const auto values = sequence(count);
    const auto expected = std::reduce(values.cbegin(), values.cend(), 1_size, std::multiplies<size_t>{});
    BC_REQUIRE_EQUAL(std::reduce(par_if(true, 7), values.cbegin(), values.cend(), 2_size, [](size_t left, size_t right) NOEXCEPT { return left + right; }), count * add1(count) / two + two);
    BC_REQUIRE_EQUAL(std::reduce(par_if(true, 7), values.cbegin(), values.cend(), 1_size, std::multiplies<size_t>{}), expected);
}

BC_AUTO_TEST_CASE(execution__reduce__empty__init)
{
    const std::vector<size_t> values{};
    BC_REQUIRE_EQUAL(std::reduce(par, values.cbegin(), values.cend(), 42_size, std::plus<size_t>{}), 42u);
}

// transform_reduce

BC_AUTO_TEST_CASE(execution__transform_reduce__policies__same_as_serial)
{
    BC_REQUIRE(transform_reduce_serial(seq));
    BC_REQUIRE(transform_reduce_serial(par));
    BC_REQUIRE(transform_reduce_serial(par_if(true, 7)));
    BC_REQUIRE(transform_reduce_serial(par_if(true, count)));
}

BC_AUTO_TEST_CASE(execution__transform_reduce__empty__init)
{
    const std::vector<size_t> values{};
    BC_REQUIRE_EQUAL(std::transform_reduce(par, values.cbegin(), values.cend(), 42_size, std::plus<size_t>{}, [](size_t value) NOEXCEPT { return value; }), 42u);
}

// all_of

BC_AUTO_TEST_CASE(execution__all_of__policies__same_as_serial)
{
    const auto positive = [](size_t value) NOEXCEPT { return value > zero; };
    const auto small = [](size_t value) NOEXCEPT { return value < count; };
    const auto tiny = [](size_t value) NOEXCEPT { return value < 10u; };
    BC_REQUIRE(all_of_serial(seq, positive));
    BC_REQUIRE(all_of_serial(seq, small));
    BC_REQUIRE(all_of_serial(seq, tiny));
    BC_REQUIRE(all_of_serial(par, positive));
    BC_REQUIRE(all_of_serial(par, small));
    BC_REQUIRE(all_of_serial(par, tiny));
    BC_REQUIRE(all_of_serial(par_if(true, 7), positive));
    BC_REQUIRE(all_of_serial(par_if(true, 7), small));
    BC_REQUIRE(all_of_serial(par_if(true, 7), tiny));
    BC_REQUIRE(all_of_serial(par_if(true, count), positive));
    BC_REQUIRE(all_of_serial(par_if(true, count), small));
    BC_REQUIRE(all_of_serial(par_if(true, count), tiny));
}

BC_AUTO_TEST_CASE(execution__all_of__empty__true)
{
    const std::vector<size_t> values{};
    BC_REQUIRE(std::all_of(par, values.cbegin(), values.cend(), [](size_t) NOEXCEPT { return false; }));
}

// sort

BC_AUTO_TEST_CASE(execution__sort__policies__same_as_serial)
{
    BC_REQUIRE(sort_serial(seq));
    BC_REQUIRE(sort_serial(par));
    BC_REQUIRE(sort_serial(par_if(true, 7)));
    BC_REQUIRE(sort_serial(par_if(true, count)));
}

BC_AUTO_TEST_CASE(execution__sort__odd_task_counts__sorted)
{
    BC_REQUIRE(sorts(3));
    BC_REQUIRE(sorts(5));
    BC_REQUIRE(sorts(7));
    BC_REQUIRE(sorts(9));
    BC_REQUIRE(sorts(11));
    BC_REQUIRE(sorts(13));
    BC_REQUIRE(sorts(17));
    BC_REQUIRE(sorts(100));
    BC_REQUIRE(sorts(101));
}

BC_AUTO_TEST_CASE(execution__sort__empty__empty)
{
    std::vector<size_t> values{};
    std::sort(par, values.begin(), values.end(), std::less<size_t>{});
    BC_REQUIRE(values.empty());
}

BC_AUTO_TEST_SUITE_END()
