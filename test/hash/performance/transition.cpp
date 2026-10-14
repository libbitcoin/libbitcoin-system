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
#include "../../test.hpp"
#include "performance.hpp"

#if defined(HAVE_PERFORMANCE_TESTS) && defined(HAVE_AVX2)

#include <atomic>

BOOST_AUTO_TEST_SUITE(hash_performance_tests)

using namespace performance;

// External linkage and a signal fence retain each 256 bit store in the loop.
xint256_t dirty_sink{};

// Single block accumulation, each preceded by a 256 bit store.
template <typename Algorithm>
struct dirty_64
{
    static typename Algorithm::digest_t run(const std::string& name,
        size_t calls) NOEXCEPT
    {
        auto state = Algorithm::H::get;
        typename Algorithm::block_t block{};
        report(name, nanoseconds(calls, [&]() NOEXCEPT
        {
            f::store(dirty_sink, f::broadcast<xint256_t>(state.front()));
            std::atomic_signal_fence(std::memory_order_seq_cst);
            block.front() ^= narrow_cast<uint8_t>(state.front());
            Algorithm::accumulate(state, block);
        }), "block");

        return Algorithm::normalize(state);
    }
};

BOOST_AUTO_TEST_CASE(hash_performance__sha256__dirty_upper_64)
{
    run_sha<sha::h256<>, dirty_64>("sha256 dirty upper 64", 100'000);
}

BOOST_AUTO_TEST_CASE(hash_performance__sha160__dirty_upper_64)
{
    run_sha<sha::h160, dirty_64>("sha160 dirty upper 64", 100'000);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
