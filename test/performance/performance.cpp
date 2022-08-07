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

#if !defined(HAVE_PERFORMANCE_TESTS)

BOOST_AUTO_TEST_SUITE(performance_tests)

using namespace performance;

BOOST_AUTO_TEST_CASE(performance__sha256)
{
#if !defined(VISIBLE)
    // 1 round (lowest overhead, best measure of pure hashing)
    // --------------------------------------------------------

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // concurrent______: 1 [concurrent]
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 6.08308
    // [faulty_sum]____: 6229.08
    // cycles_per_byte_: 16.9959
    // ms_per_round____: 6083.08
    // ms_per_byte_____: 5.66531e-06

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 6.18803
    // [faulty_sum]____: 6336.54
    // cycles_per_byte_: 17.2892
    // ms_per_round____: 6188.03
    // ms_per_byte_____: 5.76305e-06

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'048'576 [MiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0060496
    // [faulty_sum]____: 0.0060496
    // cycles_per_byte_: 17.308
    // ms_per_round____: 6.0496
    // ms_per_byte_____: 5.76935e-06

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1024 [KiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 1.11e-05
    // [faulty_sum]____: 1.08398e-08
    // cycles_per_byte_: 32.5195
    // ms_per_round____: 0.0111
    // ms_per_byte_____: 1.08398e-05

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 64 [block]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 1.8e-06
    // [faulty_sum]____: 1.09863e-10
    // cycles_per_byte_: 84.375
    // ms_per_round____: 0.0018
    // ms_per_byte_____: 2.8125e-05

    // 1k bytes nonconcurrent
    // --------------------------------------------------------

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1'048'576 [GiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 6.21972
    // [faulty_sum]____: 6369
    // cycles_per_byte_: 17.3777
    // ms_per_round____: 6.07395
    // ms_per_byte_____: 5.79257e-06

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024 [GiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 6.82405
    // [faulty_sum]____: 6987.83
    // cycles_per_byte_: 19.0662
    // ms_per_round____: 0.00650792
    // ms_per_byte_____: 6.35539e-06

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,0,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1024 [MiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0091653
    // [faulty_sum]____: 0.0091653
    // cycles_per_byte_: 26.2221
    // ms_per_round____: 0.00895049
    // ms_per_byte_____: 8.74071e-06

    // 1k bytes concurrent
    // --------------------------------------------------------

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1'048'576 [GiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 7.10007
    // [faulty_sum]____: 7270.47
    // cycles_per_byte_: 19.8374
    // ms_per_round____: 6.93366
    // ms_per_byte_____: 6.61245e-06

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024 [GiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 11.6964
    // [faulty_sum]____: 11977.1
    // cycles_per_byte_: 32.6794
    // ms_per_round____: 0.0111546
    // ms_per_byte_____: 1.08931e-05

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1024 [MiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0158307
    // [faulty_sum]____: 0.0158307
    // cycles_per_byte_: 45.292
    // ms_per_round____: 0.0154597
    // ms_per_byte_____: 1.50973e-05

    // 1k bytes concurrent, stack allocated (modified impl)
    // --------------------------------------------------------

    // There is no measurable distinction between stack alloc of
    // the concurrency buffer and shared_ptr/heap alloc...

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024 [GiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 11.6033
    // [faulty_sum]____: 11881.7
    // cycles_per_byte_: 32.4192
    // ms_per_round____: 0.0110657
    // ms_per_byte_____: 1.08064e-05

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1048576
    // bytes_per_round_: 1024
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 11.7146
    // [faulty_sum]____: 11995.8
    // cycles_per_byte_: 32.7302
    // ms_per_round____: 0.0111719
    // ms_per_byte_____: 1.09101e-05

    // test variance is high at low iteration levels...

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1024 [MiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0120429
    // [faulty_sum]____: 0.0120429
    // cycles_per_byte_: 34.455
    // ms_per_round____: 0.0117606
    // ms_per_byte_____: 1.1485e-05

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1024 [MiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0102711
    // [faulty_sum]____: 0.0102711
    // cycles_per_byte_: 29.3859
    // ms_per_round____: 0.0100304
    // ms_per_byte_____: 9.79528e-06

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1024 [MiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0160313
    // [faulty_sum]____: 0.0160313
    // cycles_per_byte_: 45.8659
    // ms_per_round____: 0.0156556
    // ms_per_byte_____: 1.52886e-05
#endif

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.89518
    // mib_per_second__: 148.51
    // cycles_per_byte_: 19.2649
    // ms_per_round____: 0.00657575
    // ms_per_byte_____: 6.42164e-06

    const auto complete = performance::hash<sha256_optimal>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_CASE(performance__rmd160)
{
#if !defined(VISIBLE)
    // 1 round (lowest overhead, best measure of pure hashing)
    // --------------------------------------------------------

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,1,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // concurrent______: 1 [concurrent]
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 24.0652
    // [faulty_sum]____: 24642.8
    // cycles_per_byte_: 67.2375
    // ms_per_round____: 24065.2
    // ms_per_byte_____: 2.24125e-05

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 5.9627
    // [faulty_sum]____: 6105.81
    // cycles_per_byte_: 16.6596
    // ms_per_round____: 5962.7
    // ms_per_byte_____: 5.5532e-06

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 10'48'576 [MiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0106938
    // [faulty_sum]____: 0.0106938
    // cycles_per_byte_: 30.5952
    // ms_per_round____: 10.6938
    // ms_per_byte_____: 1.01984e-05

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1024 [KiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 2.87e-05
    // [faulty_sum]____: 2.80273e-08
    // cycles_per_byte_: 84.082
    // ms_per_round____: 0.0287
    // ms_per_byte_____: 2.80273e-05

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 64 [block]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 9.7e-06
    // [faulty_sum]____: 5.92041e-10
    // cycles_per_byte_: 454.688
    // ms_per_round____: 0.0097
    // ms_per_byte_____: 0.000151563

    // 1k bytes nonconcurrent
    // --------------------------------------------------------

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1'048'576 [GiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 5.96008
    // [faulty_sum]____: 6103.12
    // cycles_per_byte_: 16.6523
    // ms_per_round____: 5.82039
    // ms_per_byte_____: 5.55075e-06

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024 [GiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 6.4172
    // [faulty_sum]____: 6571.21
    // cycles_per_byte_: 17.9295
    // ms_per_round____: 0.00611992
    // ms_per_byte_____: 5.97648e-06

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1024 [MiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0063824
    // [faulty_sum]____: 0.0063824
    // cycles_per_byte_: 18.2602
    // ms_per_round____: 0.00623281
    // ms_per_byte_____: 6.08673e-06

    // 1k bytes concurrent
    // --------------------------------------------------------

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,1,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1'048'576 [GiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 23.8377
    // [faulty_sum]____: 24409.8
    // cycles_per_byte_: 66.6018
    // ms_per_round____: 23.279
    // ms_per_byte_____: 2.22006e-05

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,1,1>
    // test_rounds_____: 1'048'576 [GiB]
    // bytes_per_round_: 1024
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 25.3097
    // [faulty_sum]____: 25917.1
    // cycles_per_byte_: 70.7144
    // ms_per_round____: 0.0241372
    // ms_per_byte_____: 2.35715e-05

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,1,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1024 [MiB]
    // concurrent______: 1
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.030023
    // [faulty_sum]____: 0.030023
    // cycles_per_byte_: 85.8965
    // ms_per_round____: 0.0293193
    // ms_per_byte_____: 2.86322e-05

    // 1k bytes nonconcurrent (after overhead optimization)
    // --------------------------------------------------------

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1'048'576 [GiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 5.95985
    // mib_per_second__: 171.816
    // cycles_per_byte_: 16.6516
    // ms_per_round____: 5.82017
    // ms_per_byte_____: 5.55055e-06

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024 [GiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 6.38589
    // mib_per_second__: 160.354
    // cycles_per_byte_: 17.842
    // ms_per_round____: 0.00609006
    // ms_per_byte_____: 5.94732e-06

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1024 [MiB]
    // concurrent______: 0
    // vectorized______: 0
    // intrinsic_______: 0
    // chunked_________: 0
    // seconds_total___: 0.0060727
    // mib_per_second__: 164.671
    // cycles_per_byte_: 17.3741
    // ms_per_round____: 0.00593037
    // ms_per_byte_____: 5.79138e-06
#endif

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,1,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024
    // compressed______: 0
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.3798
    // mib_per_second__: 160.506
    // cycles_per_byte_: 17.825
    // ms_per_round____: 0.00608426
    // ms_per_byte_____: 5.94166e-06

    const auto complete = performance::hash<rmd160_optimal>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_SUITE_END()

#endif
