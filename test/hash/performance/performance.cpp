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

#if defined(HAVE_PERFORMANCE_TESTS)

BOOST_AUTO_TEST_SUITE(hash_performance_tests)

using namespace performance;

// sha256
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(hash_performance__sha256__stream_1m)
{
    run_sha<sha::h256<>, stream_1m>("sha256 stream 1m", 100);
}

BOOST_AUTO_TEST_CASE(hash_performance__sha256__stream_32)
{
    run_sha<sha::h256<>, stream_32>("sha256 stream 32", 1'000'000);
}

BOOST_AUTO_TEST_CASE(hash_performance__sha256__stream_32_uncached)
{
    const auto expected = stream_32<sha256_scalar>::run("sha256 stream 32 scalar cached", 1'000'000);
    BOOST_CHECK_EQUAL(stream_32<sha256_uncached>::run("sha256 stream 32 scalar uncached", 1'000'000), expected);
}

BOOST_AUTO_TEST_CASE(hash_performance__sha256__fixed_32)
{
    run_sha<sha::h256<>, fixed_32>("sha256 fixed 32", 1'000'000);
}

BOOST_AUTO_TEST_CASE(hash_performance__sha256__double_64)
{
    constexpr size_t calls = 500;
    const auto expected = double_64<sha256_scalar>("sha256 double 64 x 1024 scalar", calls);

    if constexpr (sha256_native::native)
    {
        BOOST_CHECK_EQUAL(double_64<sha256_native>("sha256 double 64 x 1024 native", calls), expected);
        BOOST_CHECK_EQUAL(double_64_native("sha256 double 64 x 1024 native 2 lanes", calls), expected);
    }

    run_double_64_lanes<xint128_t>("sha256 double 64 x 1024 4 lanes", calls, expected);
    run_double_64_lanes<xint256_t>("sha256 double 64 x 1024 8 lanes", calls, expected);
    run_double_64_lanes<xint512_t>("sha256 double 64 x 1024 16 lanes", calls, expected);
}

BOOST_AUTO_TEST_CASE(hash_performance__sha256__merkle_root)
{
    run_sha<sha::h256<>, root_9001>("sha256 merkle root 9001", 100);
}

// other
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(hash_performance__sha160__stream_1m)
{
    run_sha<sha::h160, stream_1m>("sha160 stream 1m", 100);
}

BOOST_AUTO_TEST_CASE(hash_performance__sha512__stream_1m)
{
    run_sha<sha::h512<>, stream_1m>("sha512 stream 1m", 100);
}

BOOST_AUTO_TEST_CASE(hash_performance__rmd160__stream_1m)
{
    run_rmd160("rmd160 stream 1m", 100);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
