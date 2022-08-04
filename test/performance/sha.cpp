/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include "../test.hpp"
#include "performance.hpp"

#if defined(HAVE_PERFORMANCE_TESTS)

BOOST_AUTO_TEST_SUITE(performance_sha)

using precision = std::chrono::nanoseconds;
using timer = perf::timer<precision, std::chrono::steady_clock>;
constexpr auto rounds = 1'000'000_u64;

template <typename P>
auto total(uint64_t time) noexcept
{
    return (1.0f * time) / P::period::den;
}

template <typename P>
auto round(uint64_t time, uint64_t rounds) noexcept
{
    return total<P>(time) / rounds;
}

template <typename P>
auto datum(uint64_t time, uint64_t rounds, size_t size) noexcept
{
    return round<P>(time, rounds) / size;
}

template <typename P>
void console(std::ostream& out, uint64_t time, size_t size, uint64_t rounds) noexcept
{
    std::cout << std::endl
        << TEST_NAME << std::endl
        << "   total (sec) : " << serialize(total<P>(time)) << std::endl
        << "   round (sec) : " << serialize(round<P>(time, rounds)) << std::endl
        << "   byte  (sec) : " << serialize(datum<P>(time, rounds, size)) << std::endl;
}

BOOST_AUTO_TEST_CASE(performance__bitcoin_hash__one_block__baseline)
{
    uint64_t time{};
    constexpr auto size = 64_size;

    for (uint64_t seed = 0; seed < rounds; ++seed)
    {
        const auto chunk = perf::get_chunk<size>(seed);
        time += timer::execution(&bitcoin_hash<size>, chunk);
    }

    console<precision>(std::cout, time, size, rounds);
}

BOOST_AUTO_TEST_CASE(performance__bitcoin_hash__half_block__baseline)
{
    uint64_t time{};
    constexpr auto size = 32_size;

    for (uint64_t seed = 0; seed < rounds; ++seed)
    {
        const auto chunk = perf::get_chunk<size>(seed);
        time += timer::execution(&bitcoin_hash<size>, chunk);
    }

    console<precision>(std::cout, time, size, rounds);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
