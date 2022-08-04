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

// TODO: parallelize buffer creation and preparation for dependent sha blocks
// TODO: in sha::algorithm::hash(set). Use Concurrent template arg to
// TODO: isolate for testing here (conditioned on HAVE_EXECUTION).

BOOST_AUTO_TEST_SUITE(performance_sha_tests)

BOOST_AUTO_TEST_CASE(performance__sha256)
{
    // test____________: performance__sha256__one_block_array__baseline
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // k_rounds________: 500
    // bytes_per_round_: 1024
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 3.24749
    // ms_per_round____: 0.00649497
    // ms_per_byte_____: 6.34275e-06

    // test____________: performance__sha256__one_block_array__baseline
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // k_rounds________: 500
    // bytes_per_round_: 1024
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 3.19064
    // ms_per_round____: 0.00638128
    // ms_per_byte_____: 6.23172e-06

    // test____________: performance__sha256__one_block_array__baseline
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // k_rounds________: 500
    // bytes_per_round_: 1024
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 3.1829
    // ms_per_round____: 0.00636579
    // ms_per_byte_____: 6.21659e-06

    // optimize array of nblocks (-.05 .. -.10 sec)

    // test____________: performance__sha256__one_block_array__baseline
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // k_rounds________: 500
    // bytes_per_round_: 1024
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 3.14241
    // ms_per_round____: 0.00628481
    // ms_per_byte_____: 6.13751e-06

    // .24 sec increase for partial block (double buffering).

    // test____________: performance__sha256__one_block_array__baseline
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // k_rounds________: 500
    // bytes_per_round_: 1023
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 3.38437
    // ms_per_round____: 0.00676875
    // ms_per_byte_____: 6.61657e-06

    const auto complete = perf::hash<256, 1024>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
