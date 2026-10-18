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

BOOST_AUTO_TEST_SUITE(execution_tests)

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

// thread_pool

BOOST_AUTO_TEST_CASE(execution__thread_pool__zero_workers__one)
{
    thread_pool pool{ 0 };
    BOOST_REQUIRE_EQUAL(pool.size(), one);
}

BOOST_AUTO_TEST_CASE(execution__thread_pool__three_workers__three)
{
    thread_pool pool{ 3 };
    BOOST_REQUIRE_EQUAL(pool.size(), 3u);
}

BOOST_AUTO_TEST_CASE(execution__thread_pool__get__at_least_one_worker)
{
    BOOST_REQUIRE_GE(workers, one);
    BOOST_REQUIRE_EQUAL(&thread_pool::get(), &thread_pool::get());
}

BOOST_AUTO_TEST_CASE(execution__thread_pool__submit__future_completes_after_run)
{
    thread_pool pool{ 2 };
    std::atomic_size_t ran{};
    auto future = pool.submit([&]() NOEXCEPT { ++ran; });
    future.get();
    BOOST_REQUIRE_EQUAL(ran.load(), one);
}

BOOST_AUTO_TEST_CASE(execution__thread_pool__submit_throwing__future_rethrows)
{
    thread_pool pool{ 1 };
    auto future = pool.submit([]() { throw std::runtime_error{ "task" }; });
    BOOST_REQUIRE_THROW(future.get(), std::runtime_error);
}

BOOST_AUTO_TEST_CASE(execution__thread_pool__destruct__drains_queue)
{
    std::atomic_size_t ran{};
    {
        thread_pool pool{ 1 };
        for (size_t index{}; index < count; ++index)
            pool.submit([&]() NOEXCEPT { ++ran; });
    }

    BOOST_REQUIRE_EQUAL(ran.load(), count);
}

// policy

BOOST_AUTO_TEST_CASE(execution__policy__seq__not_parallel_floor_one)
{
    BOOST_REQUIRE(!seq.parallel());
    BOOST_REQUIRE_EQUAL(seq.floor(), one);
}

BOOST_AUTO_TEST_CASE(execution__policy__par__parallel_floor_one)
{
    BOOST_REQUIRE(par.parallel());
    BOOST_REQUIRE_EQUAL(par.floor(), one);
}

BOOST_AUTO_TEST_CASE(execution__policy__par_if__expected)
{
    BOOST_REQUIRE(!par_if(false).parallel());
    BOOST_REQUIRE(par_if(true).parallel());
    BOOST_REQUIRE_EQUAL(par_if(true).floor(), one);
    BOOST_REQUIRE_EQUAL(par_if(true, 32).floor(), 32u);
    BOOST_REQUIRE_EQUAL(par_if(false, 32).floor(), 32u);
}

BOOST_AUTO_TEST_CASE(execution__policy__zero_floor__one)
{
    BOOST_REQUIRE_EQUAL(par_if(true, 0).floor(), one);
}

// chunks

BOOST_AUTO_TEST_CASE(execution__chunks__empty__one)
{
    BOOST_REQUIRE_EQUAL(chunks(par, 0), one);
    BOOST_REQUIRE_EQUAL(chunks(par_if(true, 32), 0), one);
}

BOOST_AUTO_TEST_CASE(execution__chunks__within_floor__one)
{
    BOOST_REQUIRE_EQUAL(chunks(par_if(true, 32), 1), one);
    BOOST_REQUIRE_EQUAL(chunks(par_if(true, 32), 32), one);
    BOOST_REQUIRE_EQUAL(chunks(par_if(true, 32), 63), one);
}

BOOST_AUTO_TEST_CASE(execution__chunks__floor_multiples__floored_quotient_to_workers)
{
    BOOST_REQUIRE_EQUAL(chunks(par_if(true, 32), 64), std::min(2_size, workers));
    BOOST_REQUIRE_EQUAL(chunks(par_if(true, 32), 95), std::min(2_size, workers));
    BOOST_REQUIRE_EQUAL(chunks(par_if(true, 32), 96), std::min(3_size, workers));
    BOOST_REQUIRE_EQUAL(chunks(par_if(true, 10), count), std::min(100_size, workers));
}

BOOST_AUTO_TEST_CASE(execution__chunks__floor_one__count_to_workers)
{
    BOOST_REQUIRE_EQUAL(chunks(par, 1), one);
    BOOST_REQUIRE_EQUAL(chunks(par, 2), std::min(2_size, workers));
    BOOST_REQUIRE_EQUAL(chunks(par, count), std::min(count, workers));
}

BOOST_AUTO_TEST_CASE(execution__chunks__large_count__never_exceeds_workers)
{
    BOOST_REQUIRE_EQUAL(chunks(par, max_size_t), workers);
}

// serial

BOOST_AUTO_TEST_CASE(execution__serial__seq__always)
{
    const auto values = sequence(count);
    BOOST_REQUIRE(serial(seq, values.begin(), values.end()));
    BOOST_REQUIRE(serial(seq, values.begin(), values.begin()));
}

BOOST_AUTO_TEST_CASE(execution__serial__par_empty_or_one__true)
{
    const auto values = sequence(2);
    BOOST_REQUIRE(serial(par, values.begin(), values.begin()));
    BOOST_REQUIRE(serial(par, values.begin(), std::next(values.begin())));
}

BOOST_AUTO_TEST_CASE(execution__serial__par_two__only_when_one_worker)
{
    const auto values = sequence(2);
    BOOST_REQUIRE_EQUAL(serial(par, values.begin(), values.end()), workers < two);
}

BOOST_AUTO_TEST_CASE(execution__serial__floor__below_two_tasks)
{
    const auto values = sequence(64);
    const auto policy = par_if(true, 32);
    BOOST_REQUIRE(serial(policy, values.begin(), std::next(values.begin(), 32)));
    BOOST_REQUIRE(serial(policy, values.begin(), std::next(values.begin(), 63)));
    BOOST_REQUIRE_EQUAL(serial(policy, values.begin(), values.end()), workers < two);
}

// bounds

BOOST_AUTO_TEST_CASE(execution__bounds__empty__first_twice)
{
    const std::vector<size_t> values{};
    const auto edges = bounds(par, values.begin(), values.end());
    BOOST_REQUIRE_EQUAL(edges.size(), two);
    BOOST_REQUIRE(edges.front() == values.begin());
    BOOST_REQUIRE(edges.back() == values.end());
}

BOOST_AUTO_TEST_CASE(execution__bounds__floor__contiguous_balanced_not_below_floor)
{
    constexpr size_t floor = 7;
    const auto values = sequence(count);
    const auto policy = par_if(true, floor);
    const auto edges = bounds(policy, values.begin(), values.end());
    BOOST_REQUIRE_EQUAL(edges.size(), add1(chunks(policy, count)));
    BOOST_REQUIRE(edges.front() == values.begin());
    BOOST_REQUIRE(edges.back() == values.end());

    size_t smallest = max_size_t;
    size_t largest = zero;
    for (size_t index{}; index < sub1(edges.size()); ++index)
    {
        const auto width = to_unsigned(std::distance(edges.at(index), edges.at(add1(index))));
        smallest = std::min(smallest, width);
        largest = std::max(largest, width);
    }

    BOOST_REQUIRE_GE(smallest, floor);
    BOOST_REQUIRE_LE(largest - smallest, one);
}

BOOST_AUTO_TEST_CASE(execution__bounds__remainder__goes_to_leading_tasks)
{
    const auto values = sequence(5);
    const auto policy = par_if(true, 2);
    const auto edges = bounds(policy, values.begin(), values.end());
    const auto tasks = std::min(2_size, workers);
    BOOST_REQUIRE_EQUAL(edges.size(), add1(tasks));
    BOOST_REQUIRE_EQUAL(std::distance(edges.at(0), edges.at(1)), tasks == 2u ? 3 : 5);
}

// chunked

BOOST_AUTO_TEST_CASE(execution__chunked__floor__each_element_once_with_task_index)
{
    const auto values = sequence(count);
    const auto policy = par_if(true, 3);
    const auto edges = bounds(policy, values.begin(), values.end());
    std::vector<size_t> seen(count);
    std::vector<size_t> indexes(count);
    chunked(policy, values.begin(), values.end(), [&](auto from, auto to, size_t index) NOEXCEPT
    {
        for (auto it = from; it != to; ++it)
        {
            seen.at(*it - one) += one;
            indexes.at(*it - one) = index;
        }
    });

    BOOST_REQUIRE(std::all_of(seen.begin(), seen.end(), [](size_t once) NOEXCEPT { return once == one; }));
    for (size_t index{}; index < sub1(edges.size()); ++index)
        for (auto it = edges.at(index); it != edges.at(add1(index)); ++it)
            BOOST_REQUIRE_EQUAL(indexes.at(*it - one), index);
}

BOOST_AUTO_TEST_CASE(execution__chunked__throwing_task__rethrows)
{
    const auto values = sequence(count);
    BOOST_REQUIRE_THROW(chunked(par, values.begin(), values.end(), [](auto, auto, size_t) { throw std::runtime_error{ "chunk" }; }), std::runtime_error);
}

// for_each

BOOST_AUTO_TEST_CASE(execution__for_each__policies__same_as_serial)
{
    const auto values = sequence(count);
    size_t expected{};
    std::for_each(values.begin(), values.end(), [&](size_t value) NOEXCEPT { expected += value; });

    for (const auto& policy: { seq, par, par_if(true, 7), par_if(true, count) })
    {
        std::atomic_size_t sum{};
        std::for_each(policy, values.begin(), values.end(), [&](size_t value) NOEXCEPT { sum += value; });
        BOOST_REQUIRE_EQUAL(sum.load(), expected);
    }
}

BOOST_AUTO_TEST_CASE(execution__for_each__empty__no_calls)
{
    const std::vector<size_t> values{};
    std::atomic_size_t calls{};
    std::for_each(par, values.begin(), values.end(), [&](size_t) NOEXCEPT { ++calls; });
    BOOST_REQUIRE_EQUAL(calls.load(), zero);
}

// transform

BOOST_AUTO_TEST_CASE(execution__transform__unary_policies__same_as_serial)
{
    const auto values = sequence(count);
    const auto square = [](size_t value) NOEXCEPT { return value * value; };
    std::vector<size_t> expected(count);
    std::transform(values.begin(), values.end(), expected.begin(), square);

    for (const auto& policy: { seq, par, par_if(true, 7), par_if(true, count) })
    {
        std::vector<size_t> out(count);
        const auto end = std::transform(policy, values.begin(), values.end(), out.begin(), square);
        BOOST_REQUIRE(end == out.end());
        BOOST_REQUIRE(out == expected);
    }
}

BOOST_AUTO_TEST_CASE(execution__transform__binary_policies__same_as_serial)
{
    const auto left = sequence(count);
    const auto right = scrambled(count);
    const auto add = [](size_t one, size_t two) NOEXCEPT { return one + two; };
    std::vector<size_t> expected(count);
    std::transform(left.begin(), left.end(), right.begin(), expected.begin(), add);

    for (const auto& policy: { seq, par, par_if(true, 7), par_if(true, count) })
    {
        std::vector<size_t> out(count);
        const auto end = std::transform(policy, left.begin(), left.end(), right.begin(), out.begin(), add);
        BOOST_REQUIRE(end == out.end());
        BOOST_REQUIRE(out == expected);
    }
}

// reduce

BOOST_AUTO_TEST_CASE(execution__reduce__policies__same_as_serial)
{
    const auto values = sequence(count);
    const auto expected = std::reduce(values.begin(), values.end(), 42_size, std::plus<size_t>{});

    for (const auto& policy: { seq, par, par_if(true, 7), par_if(true, count) })
        BOOST_REQUIRE_EQUAL(std::reduce(policy, values.begin(), values.end(), 42_size, std::plus<size_t>{}), expected);
}

BOOST_AUTO_TEST_CASE(execution__reduce__init_applied_once)
{
    const auto values = sequence(count);
    const auto expected = std::reduce(values.begin(), values.end(), 1_size, std::multiplies<size_t>{});
    BOOST_REQUIRE_EQUAL(std::reduce(par_if(true, 7), values.begin(), values.end(), 2_size, [](size_t left, size_t right) NOEXCEPT { return left + right; }), count * add1(count) / two + two);
    BOOST_REQUIRE_EQUAL(std::reduce(par_if(true, 7), values.begin(), values.end(), 1_size, std::multiplies<size_t>{}), expected);
}

BOOST_AUTO_TEST_CASE(execution__reduce__empty__init)
{
    const std::vector<size_t> values{};
    BOOST_REQUIRE_EQUAL(std::reduce(par, values.begin(), values.end(), 42_size, std::plus<size_t>{}), 42u);
}

// transform_reduce

BOOST_AUTO_TEST_CASE(execution__transform_reduce__policies__same_as_serial)
{
    const auto values = sequence(count);
    const auto square = [](size_t value) NOEXCEPT { return value * value; };
    const auto expected = std::transform_reduce(values.begin(), values.end(), 42_size, std::plus<size_t>{}, square);

    for (const auto& policy: { seq, par, par_if(true, 7), par_if(true, count) })
        BOOST_REQUIRE_EQUAL(std::transform_reduce(policy, values.begin(), values.end(), 42_size, std::plus<size_t>{}, square), expected);
}

BOOST_AUTO_TEST_CASE(execution__transform_reduce__empty__init)
{
    const std::vector<size_t> values{};
    BOOST_REQUIRE_EQUAL(std::transform_reduce(par, values.begin(), values.end(), 42_size, std::plus<size_t>{}, [](size_t value) NOEXCEPT { return value; }), 42u);
}

// all_of

BOOST_AUTO_TEST_CASE(execution__all_of__policies__same_as_serial)
{
    const auto values = sequence(count);
    const auto positive = [](size_t value) NOEXCEPT { return value > zero; };
    const auto small = [](size_t value) NOEXCEPT { return value < count; };
    const auto tiny = [](size_t value) NOEXCEPT { return value < 10u; };

    for (const auto& policy: { seq, par, par_if(true, 7), par_if(true, count) })
    {
        BOOST_REQUIRE_EQUAL(std::all_of(policy, values.begin(), values.end(), positive), std::all_of(values.begin(), values.end(), positive));
        BOOST_REQUIRE_EQUAL(std::all_of(policy, values.begin(), values.end(), small), std::all_of(values.begin(), values.end(), small));
        BOOST_REQUIRE_EQUAL(std::all_of(policy, values.begin(), values.end(), tiny), std::all_of(values.begin(), values.end(), tiny));
    }
}

BOOST_AUTO_TEST_CASE(execution__all_of__empty__true)
{
    const std::vector<size_t> values{};
    BOOST_REQUIRE(std::all_of(par, values.begin(), values.end(), [](size_t) NOEXCEPT { return false; }));
}

// sort

BOOST_AUTO_TEST_CASE(execution__sort__policies__same_as_serial)
{
    const auto greater = [](size_t left, size_t right) NOEXCEPT { return left > right; };
    auto expected = scrambled(count);
    std::sort(expected.begin(), expected.end(), greater);

    for (const auto& policy: { seq, par, par_if(true, 7), par_if(true, count) })
    {
        auto values = scrambled(count);
        std::sort(policy, values.begin(), values.end(), greater);
        BOOST_REQUIRE(values == expected);
    }
}

BOOST_AUTO_TEST_CASE(execution__sort__odd_task_counts__sorted)
{
    for (const auto size: { 3u, 5u, 7u, 9u, 11u, 13u, 17u, 100u, 101u })
    {
        auto expected = scrambled(size);
        std::sort(expected.begin(), expected.end());
        auto values = scrambled(size);
        std::sort(par, values.begin(), values.end(), std::less<size_t>{});
        BOOST_REQUIRE(values == expected);
    }
}

BOOST_AUTO_TEST_CASE(execution__sort__empty__empty)
{
    std::vector<size_t> values{};
    std::sort(par, values.begin(), values.end(), std::less<size_t>{});
    BOOST_REQUIRE(values.empty());
}

BOOST_AUTO_TEST_SUITE_END()
