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
#include "../../test.hpp"
#include "performance.hpp"

// One block in 4 lanes (Intel)
// veracrypt.fr/code/VeraCrypt/plain/src/Crypto/sha256_sse4_x64.asm
// intel.com/content/dam/www/public/us/en/documents/white-papers/
// sha-256-implementations-paper.pdf

// TODO: compare all script hashing scenarios:
// TODO: rmd160, sha160, sha256, rmd160(sha256), sha256(sha256)
// TODO: compare incremental hash accumulation.
// TODO: test runner/baseline for stream read/write.
// TODO: test runner/baseline for signature hashing.
// TODO: test runner/baseline for signature validation.
// TODO: test runner/baseline for script execution.

#if !defined(HAVE_PERFORMANCE_TESTS)

BOOST_AUTO_TEST_SUITE(performance_tests)

using namespace performance;
using rmd160a          = rmd160_parameters<false>;
using rmd160c          = rmd160_parameters<true>;
using sha256a_cached   = sha256_parameters<true, true, true,  false>;
using sha256c_cached   = sha256_parameters<true, true, true,  true>;
using sha256a_uncached = sha256_parameters<true, true, false, false>;
using sha256c_uncached = sha256_parameters<true, true, false, true>;

using namespace baseline;
using base_rmd160a     = base::parameters<CRIPEMD160, false>;
using base_rmd160c     = base::parameters<CRIPEMD160, true>;
using base_sha256a     = base::parameters<CSHA256, false>;
using base_sha256c     = base::parameters<CSHA256, true>;

BOOST_AUTO_TEST_CASE(performance__base_sha256a)
{
#if !defined(VISIBLE)
    // test____________: performance__baseline__sha256
    // algorithm_______: baseline::CSHA256
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 5.79635
    // mib_per_second__: 176.663
    // cycles_per_byte_: 16.1948
    // ms_per_round____: 5796.35
    // ms_per_byte_____: 5.39827e-06

    // test____________: performance__baseline__sha256
    // algorithm_______: baseline::CSHA256
    // test_rounds_____: 1024
    // bytes_per_round_: 1'048'576 [GiB]
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 5.92684
    // mib_per_second__: 172.773
    // cycles_per_byte_: 16.5594
    // ms_per_round____: 5.78793
    // ms_per_byte_____: 5.5198e-06

    // test____________: performance__baseline__sha256
    // algorithm_______: baseline::CSHA256
    // test_rounds_____: 1'048'576 [GiB]
    // bytes_per_round_: 1024
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.32796
    // mib_per_second__: 161.822
    // cycles_per_byte_: 17.6801
    // ms_per_round____: 0.00603481
    // ms_per_byte_____: 5.89337e-06

    // test____________: performance__baseline__sha256
    // algorithm_______: baseline::CSHA256
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 5.8362
    // mib_per_second__: 175.457
    // cycles_per_byte_: 16.3062
    // ms_per_round____: 5836.2
    // ms_per_byte_____: 5.43539e-06
    // remote compilation unit.

    // test____________: performance__baseline__sha256
    // algorithm_______: baseline::CSHA256
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 5.97054
    // mib_per_second__: 171.509
    // cycles_per_byte_: 16.6815
    // ms_per_round____: 5970.54
    // ms_per_byte_____: 5.5605e-06

    // [Compiler optimizations disabled]
    // test____________: performance__baseline__sha256
    // algorithm_______: baseline::CSHA256
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 17.9835
    // mib_per_second__: 56.9411
    // cycles_per_byte_: 50.2453
    // ms_per_round____: 17983.5
    // ms_per_byte_____: 1.67484e-05
#endif

    auto complete = true;
    complete = base::test_hash<base_sha256a, 1024*1024, 1024>(std::cout);
    complete = base::test_hash<base_sha256a, 16*1024*1024, 64>(std::cout);
    complete = base::test_hash<base_sha256a, 32*1024*1024, 32>(std::cout);
    complete = base::test_hash<base_sha256a, 1, 1024*1024*1024>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_CASE(performance__sha256a_cached)
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
    // bytes_per_round_: 1'048'576 [GiB]
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

    // After movement of k to schedule and compression (no expected change).

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.72823
    // mib_per_second__: 152.194
    // cycles_per_byte_: 18.7985
    // ms_per_round____: 0.00641654
    // ms_per_byte_____: 6.26616e-06

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.98735
    // mib_per_second__: 146.551
    // cycles_per_byte_: 19.5224
    // ms_per_round____: 0.00666366
    // ms_per_byte_____: 6.50748e-06

    // baselining
    // ------------------------------------------------------------------------

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.21839
    // mib_per_second__: 164.673
    // cycles_per_byte_: 17.374
    // ms_per_round____: 6218.39
    // ms_per_byte_____: 5.79133e-06
    // 107% of baseline.

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1024
    // bytes_per_round_: 1'048'576 [GiB]
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.28161
    // mib_per_second__: 163.016
    // cycles_per_byte_: 17.5506
    // ms_per_round____: 6.13438
    // ms_per_byte_____: 5.8502e-06
    // 106% of baseline.

    // test____________: performance__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1'048'576 [GiB]
    // bytes_per_round_: 1024
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.71416
    // mib_per_second__: 152.514
    // cycles_per_byte_: 18.7591
    // ms_per_round____: 0.00640312
    // ms_per_byte_____: 6.25305e-06
    // 106% of baseline.

    // test____________: performance__optimal__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.22669
    // mib_per_second__: 164.453
    // cycles_per_byte_: 17.3972
    // ms_per_round____: 6226.69
    // ms_per_byte_____: 5.79906e-06
    // 107%

    // test____________: performance__optimal__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.20114
    // mib_per_second__: 165.131
    // cycles_per_byte_: 17.3258
    // ms_per_round____: 6201.14
    // ms_per_byte_____: 5.77526e-06
    // 103%

    // [Compiler optimizations disabled]
    // test____________: performance__optimal__sha256
    // algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 1
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 84.4734
    // mib_per_second__: 12.1222
    // cycles_per_byte_: 236.016
    // ms_per_round____: 84473.4
    // ms_per_byte_____: 7.86719e-05
    // 470%
#endif

    auto complete = true;
    complete = test_hash<sha256a_cached, 1024*1024, 1024>(std::cout);
    complete = test_hash<sha256a_cached, 16*1024*1024, 64>(std::cout);
    complete = test_hash<sha256a_cached, 32*1024*1024, 32>(std::cout);
    complete = test_hash<sha256a_cached, 1, 1024*1024*1024>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_CASE(performance__base_rmd160a)
{
#if !defined(VISIBLE)
    // test____________: performance__baseline__rmd160
    // algorithm_______: baseline::CRIPEMD160
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 3.42184
    // mib_per_second__: 299.254
    // cycles_per_byte_: 9.56052
    // ms_per_round____: 3421.84
    // ms_per_byte_____: 3.18684e-06

    // test____________: performance__baseline__rmd160
    // algorithm_______: baseline::CRIPEMD160
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 3.43346
    // mib_per_second__: 298.241
    // cycles_per_byte_: 9.59299
    // ms_per_round____: 3433.46
    // ms_per_byte_____: 3.19766e-06

    // [Compiler optimizations disabled]
    // test____________: performance__baseline__rmd160
    // algorithm_______: baseline::CRIPEMD160
    // test_rounds_____: 1
    // bytes_per_round_: 1'073'741'824 [GiB]
    // compressed______: 0
    // vectorized______: 0
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 23.5469
    // mib_per_second__: 43.4877
    // cycles_per_byte_: 65.7893
    // ms_per_round____: 23546.9
    // ms_per_byte_____: 2.19298e-05
#endif

    auto complete = true;
    complete = base::test_hash<base_rmd160a, 1024*1024, 1024>(std::cout);
    complete = base::test_hash<base_rmd160a, 16*1024*1024, 64>(std::cout);
    complete = base::test_hash<base_rmd160a, 32*1024*1024, 32>(std::cout);
    complete = base::test_hash<base_rmd160a, 1, 1024*1024*1024>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_CASE(performance__rmd160a)
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
    // bytes_per_round_: 1'048'576 [MiB]
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

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,1,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024
    // compressed______: 0
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.32906
    // mib_per_second__: 161.793
    // cycles_per_byte_: 17.6832
    // ms_per_round____: 0.00603587
    // ms_per_byte_____: 5.8944e-06

    // test____________: performance__rmd160
    // algorithm_______: rmd::algorithm<rmd::h160<160,1>,1,0,1>
    // test_rounds_____: 1'048'576
    // bytes_per_round_: 1024
    // compressed______: 0
    // vectorized______: 1
    // concurrent______: 0
    // chunked_________: 0
    // seconds_total___: 6.54346
    // mib_per_second__: 156.492
    // cycles_per_byte_: 18.2822
    // ms_per_round____: 0.00624033
    // ms_per_byte_____: 6.09407e-06

    // baselining
    // ------------------------------------------------------------------------

#endif

    auto complete = true;
    complete = test_hash<rmd160a, 1024*1024, 1024>(std::cout);
    complete = test_hash<rmd160a, 16*1024*1024, 64>(std::cout);
    complete = test_hash<rmd160a, 32*1024*1024, 32>(std::cout);
    complete = test_hash<rmd160a, 1, 1024*1024*1024>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_SUITE_END()

#endif

// 1GiB one round (before full inlining)
// ----------------------------------------------------------------------------

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 5.72079
// mib_per_second__: 178.996
// cycles_per_byte_: 15.9837
// ms_per_round____: 5720.79
// ms_per_byte_____: 5.3279e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824 [GiB]
// compressed______: 1
// vectorized______: 1
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 6.1639
// mib_per_second__: 166.129
// cycles_per_byte_: 17.2217
// ms_per_round____: 6163.9
// ms_per_byte_____: 5.74058e-06
// 107.7% of baseline

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.36403
// mib_per_second__: 304.397
// cycles_per_byte_: 9.39899
// ms_per_round____: 3364.03
// ms_per_byte_____: 3.133e-06

// rmd160?

// 1GiB one round (after full inlining)
// ----------------------------------------------------------------------------

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 5.7408
// mib_per_second__: 178.372
// cycles_per_byte_: 16.0396
// ms_per_round____: 5740.8
// ms_per_byte_____: 5.34654e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824 [GiB]
// compressed______: 1
// vectorized______: 1
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 6.02041
// mib_per_second__: 170.088
// cycles_per_byte_: 16.8208
// ms_per_round____: 6020.41
// ms_per_byte_____: 5.60695e-06
// 104.8% of baseline.

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.37158
// mib_per_second__: 303.715
// cycles_per_byte_: 9.42008
// ms_per_round____: 3371.58
// ms_per_byte_____: 3.14003e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 8.91146
// mib_per_second__: 114.908
// cycles_per_byte_: 24.8983
// ms_per_round____: 8911.46
// ms_per_byte_____: 8.29944e-06
// 264% of baseline

// single blocks (array)
// ----------------------------------------------------------------------------

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 16'777'216
// bytes_per_round_: 64 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 13.195
// mib_per_second__: 77.6052
// cycles_per_byte_: 36.8664
// ms_per_round____: 0.000786483
// ms_per_byte_____: 1.22888e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
// test_rounds_____: 16'777'216
// bytes_per_round_: 64 [GiB]
// compressed______: 1
// vectorized______: 1
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 11.7934
// mib_per_second__: 86.8281
// cycles_per_byte_: 32.9504
// ms_per_round____: 0.000702943
// ms_per_byte_____: 1.09835e-05
// 89% of baseline

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 16'777'216
// bytes_per_round_: 64 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.63579
// mib_per_second__: 134.105
// cycles_per_byte_: 21.3342
// ms_per_round____: 0.000455129
// ms_per_byte_____: 7.11139e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 16'777'216
// bytes_per_round_: 64 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 18.1742
// mib_per_second__: 56.3437
// cycles_per_byte_: 50.778
// ms_per_round____: 0.00108327
// ms_per_byte_____: 1.6926e-05
// 238% of baseline

// half blocks (array)
// ----------------------------------------------------------------------------

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33'554'432
// bytes_per_round_: 32 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 13.5378
// mib_per_second__: 75.6399
// cycles_per_byte_: 37.8243
// ms_per_round____: 0.000403459
// ms_per_byte_____: 1.26081e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,1,1,0,1>
// test_rounds_____: 33'554'432
// bytes_per_round_: 32 [GiB]
// compressed______: 1
// vectorized______: 1
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 13.6308
// mib_per_second__: 75.1241
// cycles_per_byte_: 38.0839
// ms_per_round____: 0.000406229
// ms_per_byte_____: 1.26946e-05
// 100.6% of baseline

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 33'554'432
// bytes_per_round_: 32 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 8.274
// mib_per_second__: 123.761
// cycles_per_byte_: 23.1173
// ms_per_round____: 0.000246584
// ms_per_byte_____: 7.70576e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 33'554'432
// bytes_per_round_: 32 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 17.3754
// mib_per_second__: 58.9339
// cycles_per_byte_: 48.5463
// ms_per_round____: 0.000517827
// ms_per_byte_____: 1.61821e-05
// 210% of baseline.

// half/full blocks (chunk) - not optimized for 32/64 byte chunks.
// ----------------------------------------------------------------------------

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.70286
// mib_per_second__: 179.559
// cycles_per_byte_: 15.9336
// ms_per_round____: 5702.86
// ms_per_byte_____: 5.3112e-06

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.31779
// mib_per_second__: 162.082
// cycles_per_byte_: 17.6517
// ms_per_round____: 0.00602511
// ms_per_byte_____: 5.8839e-06

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 12.9945
// mib_per_second__: 78.8025
// cycles_per_byte_: 36.3063
// ms_per_round____: 0.000774534
// ms_per_byte_____: 1.21021e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.6866
// mib_per_second__: 74.8178
// cycles_per_byte_: 38.2399
// ms_per_round____: 0.000407892
// ms_per_byte_____: 1.27466e-05

// vs.

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.0937
// mib_per_second__: 168.042
// cycles_per_byte_: 17.0256
// ms_per_round____: 6093.7
// ms_per_byte_____: 5.6752e-06
// 106.9%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.62771
// mib_per_second__: 154.503
// cycles_per_byte_: 18.5176
// ms_per_round____: 0.00632068
// ms_per_byte_____: 6.17254e-06
// 104.9%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 15.258
// mib_per_second__: 67.1125
// cycles_per_byte_: 42.6303
// ms_per_round____: 0.000909445
// ms_per_byte_____: 1.42101e-05
// 117%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 14.8867
// mib_per_second__: 68.7863
// cycles_per_byte_: 41.5929
// ms_per_round____: 0.000443658
// ms_per_byte_____: 1.38643e-05
// 115%

// half/full blocks (chunk) - optimized for 32/64 byte chunks.
// ----------------------------------------------------------------------------

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.91178
// mib_per_second__: 173.214
// cycles_per_byte_: 16.5173
// ms_per_round____: 5911.78
// ms_per_byte_____: 5.50577e-06

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.54015
// mib_per_second__: 156.571
// cycles_per_byte_: 18.273
// ms_per_round____: 0.00623717
// ms_per_byte_____: 6.09099e-06

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.2283
// mib_per_second__: 77.41
// cycles_per_byte_: 36.9594
// ms_per_round____: 0.000788466
// ms_per_byte_____: 1.23198e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.6421
// mib_per_second__: 75.0619
// cycles_per_byte_: 38.1155
// ms_per_round____: 0.000406566
// ms_per_byte_____: 1.27052e-05


// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.09393
// mib_per_second__: 168.036
// cycles_per_byte_: 17.0262
// ms_per_round____: 6093.93
// ms_per_byte_____: 5.67542e-06
// 103%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.87179
// mib_per_second__: 149.015
// cycles_per_byte_: 19.1995
// ms_per_round____: 0.00655345
// ms_per_byte_____: 6.39985e-06
// 105% 

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 11.7258
// mib_per_second__: 87.3288
// cycles_per_byte_: 32.7615
// ms_per_round____: 0.000698913
// ms_per_byte_____: 1.09205e-05
// 88.6% [prescheduled padding (not in baseline)]

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.8562
// mib_per_second__: 73.9018
// cycles_per_byte_: 38.7139
// ms_per_round____: 0.000412948
// ms_per_byte_____: 1.29046e-05
// 101.6% [predefined padding (not in baseline) - not as effective as single block]

// RMD (underperforming - well over 200%)

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 3.39857
// mib_per_second__: 301.303
// cycles_per_byte_: 9.49549
// ms_per_round____: 3398.57
// ms_per_byte_____: 3.16516e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 3.64351
// mib_per_second__: 281.048
// cycles_per_byte_: 10.1798
// ms_per_round____: 0.00347472
// ms_per_byte_____: 3.39328e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 7.55811
// mib_per_second__: 135.484
// cycles_per_byte_: 21.1171
// ms_per_round____: 0.000450499
// ms_per_byte_____: 7.03904e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 8.28738
// mib_per_second__: 123.561
// cycles_per_byte_: 23.1547
// ms_per_round____: 0.000246983
// ms_per_byte_____: 7.71822e-06

// vs.

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 8.67182
// mib_per_second__: 118.084
// cycles_per_byte_: 24.2288
// ms_per_round____: 8671.82
// ms_per_byte_____: 8.07626e-06
// 238%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 9.29137
// mib_per_second__: 110.21
// cycles_per_byte_: 25.9598
// ms_per_round____: 0.00886094
// ms_per_byte_____: 8.65326e-06
// 255%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 18.1823
// mib_per_second__: 56.3186
// cycles_per_byte_: 50.8007
// ms_per_round____: 0.00108375
// ms_per_byte_____: 1.69336e-05
// 241%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 17.5007
// mib_per_second__: 58.512
// cycles_per_byte_: 48.8963
// ms_per_round____: 0.000521561
// ms_per_byte_____: 1.62988e-05
// 211%

// 8GiB block
// ----------------------------------------------------------------------------

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 8589934592
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 46.196
// mib_per_second__: 177.331
// cycles_per_byte_: 16.1338
// ms_per_round____: 46196
// ms_per_byte_____: 5.37792e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 8589934592
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 48.8777
// mib_per_second__: 167.602
// cycles_per_byte_: 17.0703
// ms_per_round____: 48877.7
// ms_per_byte_____: 5.69011e-06
// 105% [same gap as 1Gib, so it's in the input/schedule/compress loop]
// most endian is 3x greater, with 2 movs for each byteswap.

// 8GiB block (optimize endian)
// ----------------------------------------------------------------------------

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 8589934592
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 46.1251
// mib_per_second__: 177.604
// cycles_per_byte_: 16.109
// ms_per_round____: 46125.1
// ms_per_byte_____: 5.36967e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 8589934592
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 49.2061
// mib_per_second__: 166.483
// cycles_per_byte_: 17.185
// ms_per_round____: 49206.1
// ms_per_byte_____: 5.72835e-06
// 106.7%

// after "optimizing" removing pointer indirections

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.94902
// mib_per_second__: 172.129
// cycles_per_byte_: 16.6214
// ms_per_round____: 5949.02
// ms_per_byte_____: 5.54045e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1'073'741'824 [GiB]
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.99015
// mib_per_second__: 170.947
// cycles_per_byte_: 16.7363
// ms_per_round____: 5990.15
// ms_per_byte_____: 5.57876e-06
// 100.7%

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1024
// bytes_per_round_: 1048576
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.87768
// mib_per_second__: 174.218
// cycles_per_byte_: 16.4221
// ms_per_round____: 5.73993
// ms_per_byte_____: 5.47402e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1024
// bytes_per_round_: 1048576
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.81637
// mib_per_second__: 176.055
// cycles_per_byte_: 16.2508
// ms_per_round____: 5.68005
// ms_per_byte_____: 5.41692e-06
// 98.9%

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.4368
// mib_per_second__: 159.085
// cycles_per_byte_: 17.9842
// ms_per_round____: 0.00613861
// ms_per_byte_____: 5.99474e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.56616
// mib_per_second__: 155.951
// cycles_per_byte_: 18.3456
// ms_per_round____: 0.00626198
// ms_per_byte_____: 6.11521e-06
// 102.0%

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.2434
// mib_per_second__: 77.3216
// cycles_per_byte_: 37.0016
// ms_per_round____: 0.000789368
// ms_per_byte_____: 1.23339e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 11.8265
// mib_per_second__: 86.5851
// cycles_per_byte_: 33.0429
// ms_per_round____: 0.000704915
// ms_per_byte_____: 1.10143e-05
// 89.5%

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.6888
// mib_per_second__: 74.8056
// cycles_per_byte_: 38.2461
// ms_per_round____: 0.000407959
// ms_per_byte_____: 1.27487e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 14.0118
// mib_per_second__: 73.081
// cycles_per_byte_: 39.1486
// ms_per_round____: 0.000417585
// ms_per_byte_____: 1.30495e-05
// 102.4%

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.29475
// mib_per_second__: 162.675
// cycles_per_byte_: 17.5873
// ms_per_round____: 0.00600314
// ms_per_byte_____: 5.86244e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 6.43846
// mib_per_second__: 159.044
// cycles_per_byte_: 17.9889
// ms_per_round____: 0.0061402
// ms_per_byte_____: 5.99629e-06
// 102.3%

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 3.66938
// mib_per_second__: 279.066
// cycles_per_byte_: 10.2521
// ms_per_round____: 0.0034994
// ms_per_byte_____: 3.41738e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 10.651
// mib_per_second__: 96.1416
// cycles_per_byte_: 29.7584
// ms_per_round____: 0.0101575
// ms_per_byte_____: 9.91948e-06
// 290%

// Replace std::rotr/rotl with native/intrinsic implementation.

// sha

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.27023
// mib_per_second__: 163.311
// cycles_per_byte_: 17.5188
// ms_per_round____: 0.00597975
// ms_per_byte_____: 5.8396e-06

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 12.8026
// mib_per_second__: 79.9838
// cycles_per_byte_: 35.77
// ms_per_round____: 0.000763094
// ms_per_byte_____: 1.19233e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.6005
// mib_per_second__: 75.2915
// cycles_per_byte_: 37.9993
// ms_per_round____: 0.000405326
// ms_per_byte_____: 1.26664e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.86734
// mib_per_second__: 174.526
// cycles_per_byte_: 16.3931
// ms_per_round____: 5867.34
// ms_per_byte_____: 5.46438e-06

// vs.

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 6.45534
// mib_per_second__: 158.628
// cycles_per_byte_: 18.036
// ms_per_round____: 0.00615629
// ms_per_byte_____: 6.012e-06
// 103%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 11.5386
// mib_per_second__: 88.7459
// cycles_per_byte_: 32.2384
// ms_per_round____: 0.000687752
// ms_per_byte_____: 1.07461e-05
// 90%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 13.6375
// mib_per_second__: 75.087
// cycles_per_byte_: 38.1028
// ms_per_round____: 0.00040643
// ms_per_byte_____: 1.27009e-05
// 106.5% [this should be better, verify it is bypassing the stream buffer]

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 5.8418
// mib_per_second__: 175.288
// cycles_per_byte_: 16.3218
// ms_per_round____: 5841.8
// ms_per_byte_____: 5.4406e-06
// 99.5%

// rmd

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.60676
// mib_per_second__: 283.911
// cycles_per_byte_: 10.0772
// ms_per_round____: 0.00343968
// ms_per_byte_____: 3.35906e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.42915
// mib_per_second__: 137.835
// cycles_per_byte_: 20.7568
// ms_per_round____: 0.000442812
// ms_per_byte_____: 6.91894e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 8.2665
// mib_per_second__: 123.873
// cycles_per_byte_: 23.0963
// ms_per_round____: 0.000246361
// ms_per_byte_____: 7.69878e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.359
// mib_per_second__: 304.852
// cycles_per_byte_: 9.38495
// ms_per_round____: 3359
// ms_per_byte_____: 3.12832e-06

// vs.

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 6.30333
// mib_per_second__: 162.454
// cycles_per_byte_: 17.6113
// ms_per_round____: 0.00601132
// ms_per_byte_____: 5.87043e-06
// 175%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 11.4104
// mib_per_second__: 89.7427
// cycles_per_byte_: 31.8803
// ms_per_round____: 0.000680113
// ms_per_byte_____: 1.06268e-05
// 154%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 11.937
// mib_per_second__: 85.7838
// cycles_per_byte_: 33.3515
// ms_per_round____: 0.00035575
// ms_per_byte_____: 1.11172e-05
// 111%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 5.85506
// mib_per_second__: 174.892
// cycles_per_byte_: 16.3588
// ms_per_round____: 5855.06
// ms_per_byte_____: 5.45295e-06
// 174%

// after combining rmd rounds into pairs

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 4.17131
// mib_per_second__: 245.486
// cycles_per_byte_: 11.6545
// ms_per_round____: 0.00397807
// ms_per_byte_____: 3.88484e-06
// 115%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.90491
// mib_per_second__: 262.234
// cycles_per_byte_: 10.9102
// ms_per_round____: 0.00372401
// ms_per_byte_____: 3.63673e-06
// 108%

// rmd

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.81294
// mib_per_second__: 268.56
// cycles_per_byte_: 10.6532
// ms_per_round____: 0.0036363
// ms_per_byte_____: 3.55107e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.68182
// mib_per_second__: 133.302
// cycles_per_byte_: 21.4628
// ms_per_round____: 0.000457872
// ms_per_byte_____: 7.15425e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 8.49558
// mib_per_second__: 120.533
// cycles_per_byte_: 23.7364
// ms_per_round____: 0.000253188
// ms_per_byte_____: 7.91213e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.39469
// mib_per_second__: 301.647
// cycles_per_byte_: 9.48466
// ms_per_round____: 3394.69
// ms_per_byte_____: 3.16155e-06

// vs.

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.98596
// mib_per_second__: 256.902
// cycles_per_byte_: 11.1366
// ms_per_round____: 0.0038013
// ms_per_byte_____: 3.71221e-06
// 104.5% [1kb rmd unlikely]

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.06955
// mib_per_second__: 144.847
// cycles_per_byte_: 19.7521
// ms_per_round____: 0.000421378
// ms_per_byte_____: 6.58403e-06
// 92% [next most likely scenario]

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.5783
// mib_per_second__: 135.123
// cycles_per_byte_: 21.1735
// ms_per_round____: 0.000225851
// ms_per_byte_____: 7.05784e-06
// 89% [this is the only std script use - rmd160(sha256))]

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.6296
// mib_per_second__: 282.125
// cycles_per_byte_: 10.141
// ms_per_round____: 3629.6
// ms_per_byte_____: 3.38032e-06
// 106.9% [this is a scenario Bitcoin will never see]

// endianness on 32 bit read turned off.
// conversion takes 3 movs for each endian.
// this along brought us to 99% from 106.9%.
// the movs are 75% of the input chunk cost (3/4 ops).

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 14.4879
// mib_per_second__: 70.6796
// cycles_per_byte_: 40.4788
// ms_per_round____: 0.000431774
// ms_per_byte_____: 1.34929e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 14.599
// mib_per_second__: 70.1419
// cycles_per_byte_: 40.7891
// ms_per_round____: 0.000435084
// ms_per_byte_____: 1.35964e-05
// 99% [no endian for 32 bytes - test]

// After unrolling endian.

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 14.8519
// mib_per_second__: 68.9474
// cycles_per_byte_: 41.4957
// ms_per_round____: 0.000442621
// ms_per_byte_____: 1.38319e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 13.8573
// mib_per_second__: 73.8959
// cycles_per_byte_: 38.7169
// ms_per_round____: 0.000412981
// ms_per_byte_____: 1.29056e-05
// 93%

// After inlining batch endian for rmd (not unrolled as in sha - testing difference).

// sha

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.61414
// mib_per_second__: 154.82
// cycles_per_byte_: 18.4797
// ms_per_round____: 0.00630774
// ms_per_byte_____: 6.1599e-06

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 12.9995
// mib_per_second__: 78.772
// cycles_per_byte_: 36.3203
// ms_per_round____: 0.000774833
// ms_per_byte_____: 1.21068e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 14.0653
// mib_per_second__: 72.8034
// cycles_per_byte_: 39.2979
// ms_per_round____: 0.000419178
// ms_per_byte_____: 1.30993e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.89485
// mib_per_second__: 173.711
// cycles_per_byte_: 16.47
// ms_per_round____: 5894.85
// ms_per_byte_____: 5.49001e-06

// vs.

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 6.53018
// mib_per_second__: 156.81
// cycles_per_byte_: 18.2451
// ms_per_round____: 0.00622766
// ms_per_byte_____: 6.0817e-06
// 98.73%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 11.3404
// mib_per_second__: 90.2964
// cycles_per_byte_: 31.6848
// ms_per_round____: 0.000675942
// ms_per_byte_____: 1.05616e-05
// 87.24%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 13.6447
// mib_per_second__: 75.0477
// cycles_per_byte_: 38.1227
// ms_per_round____: 0.000406643
// ms_per_byte_____: 1.27076e-05
// 97.01%

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 5.85662
// mib_per_second__: 174.845
// cycles_per_byte_: 16.3632
// ms_per_round____: 5856.62
// ms_per_byte_____: 5.45441e-06
// 99.35%

// rmd

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.76513
// mib_per_second__: 271.969
// cycles_per_byte_: 10.5196
// ms_per_round____: 0.00359071
// ms_per_byte_____: 3.50655e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.67836
// mib_per_second__: 133.362
// cycles_per_byte_: 21.4531
// ms_per_round____: 0.000457666
// ms_per_byte_____: 7.15103e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 8.39799
// mib_per_second__: 121.934
// cycles_per_byte_: 23.4637
// ms_per_round____: 0.00025028
// ms_per_byte_____: 7.82124e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.43666
// mib_per_second__: 297.964
// cycles_per_byte_: 9.60192
// ms_per_round____: 3436.66
// ms_per_byte_____: 3.20064e-06

// vs.

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.96693
// mib_per_second__: 258.134
// cycles_per_byte_: 11.0835
// ms_per_round____: 0.00378316
// ms_per_byte_____: 3.6945e-06
// 105.36%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.08053
// mib_per_second__: 144.622
// cycles_per_byte_: 19.7828
// ms_per_round____: 0.000422032
// ms_per_byte_____: 6.59425e-06
// 92.21%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.73246
// mib_per_second__: 132.429
// cycles_per_byte_: 21.6042
// ms_per_round____: 0.000230445
// ms_per_byte_____: 7.20142e-06
// 92.08%

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.66517
// mib_per_second__: 279.387
// cycles_per_byte_: 10.2404
// ms_per_round____: 3665.17
// ms_per_byte_____: 3.41346e-06
// 106.65%

// after inlining rmd endian as well.

// sha

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.51007
// mib_per_second__: 157.295
// cycles_per_byte_: 18.1889
// ms_per_round____: 0.00620849
// ms_per_byte_____: 6.06298e-06

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.4024
// mib_per_second__: 76.4044
// cycles_per_byte_: 37.4458
// ms_per_round____: 0.000798843
// ms_per_byte_____: 1.24819e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.8581
// mib_per_second__: 73.892
// cycles_per_byte_: 38.719
// ms_per_round____: 0.000413003
// ms_per_byte_____: 1.29063e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.8103
// mib_per_second__: 176.239
// cycles_per_byte_: 16.2338
// ms_per_round____: 5810.3
// ms_per_byte_____: 5.41126e-06

// vs.

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 6.51221
// mib_per_second__: 157.243
// cycles_per_byte_: 18.1949
// ms_per_round____: 0.00621053
// ms_per_byte_____: 6.06497e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 11.3148
// mib_per_second__: 90.5008
// cycles_per_byte_: 31.6132
// ms_per_round____: 0.000674415
// ms_per_byte_____: 1.05377e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 13.3367
// mib_per_second__: 76.7806
// cycles_per_byte_: 37.2623
// ms_per_round____: 0.000397465
// ms_per_byte_____: 1.24208e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 5.87648
// mib_per_second__: 174.254
// cycles_per_byte_: 16.4187
// ms_per_round____: 5876.48
// ms_per_byte_____: 5.4729e-06

// rmd

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.67529
// mib_per_second__: 278.618
// cycles_per_byte_: 10.2686
// ms_per_round____: 0.00350503
// ms_per_byte_____: 3.42288e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.52153
// mib_per_second__: 136.142
// cycles_per_byte_: 21.0149
// ms_per_round____: 0.000448318
// ms_per_byte_____: 7.00498e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 8.59016
// mib_per_second__: 119.206
// cycles_per_byte_: 24.0006
// ms_per_round____: 0.000256007
// ms_per_byte_____: 8.00021e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.88036
// mib_per_second__: 263.893
// cycles_per_byte_: 10.8416
// ms_per_round____: 3880.36
// ms_per_byte_____: 3.61387e-06

// vs.

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 4.13588
// mib_per_second__: 247.589
// cycles_per_byte_: 11.5555
// ms_per_round____: 0.00394428
// ms_per_byte_____: 3.85184e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.19124
// mib_per_second__: 142.395
// cycles_per_byte_: 20.0921
// ms_per_round____: 0.000428631
// ms_per_byte_____: 6.69737e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.88696
// mib_per_second__: 129.835
// cycles_per_byte_: 22.0359
// ms_per_round____: 0.00023505
// ms_per_byte_____: 7.3453e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.67731
// mib_per_second__: 278.464
// cycles_per_byte_: 10.2743
// ms_per_round____: 3677.31
// ms_per_byte_____: 3.42476e-06

// rmd - rerun1

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 4.08115
// mib_per_second__: 250.91
// cycles_per_byte_: 11.4026
// ms_per_round____: 0.00389209
// ms_per_byte_____: 3.80086e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 4.18407
// mib_per_second__: 244.738
// cycles_per_byte_: 11.6901
// ms_per_round____: 0.00399024
// ms_per_byte_____: 3.89672e-06

// rmd - rerun2 (4x)

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 4194304
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 15.3599
// mib_per_second__: 266.669
// cycles_per_byte_: 10.7287
// ms_per_round____: 0.00366208
// ms_per_byte_____: 3.57625e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 4194304
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 16.4044
// mib_per_second__: 249.69
// cycles_per_byte_: 11.4583
// ms_per_round____: 0.0039111
// ms_per_byte_____: 3.81944e-06

// if constexpr endianess exclusions

// sha

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 6.6612
// mib_per_second__: 153.726
// cycles_per_byte_: 18.6112
// ms_per_round____: 0.00635262
// ms_per_byte_____: 6.20373e-06

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 13.3701
// mib_per_second__: 76.589
// cycles_per_byte_: 37.3556
// ms_per_round____: 0.000796919
// ms_per_byte_____: 1.24519e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 14.4472
// mib_per_second__: 70.8785
// cycles_per_byte_: 40.3651
// ms_per_round____: 0.000430562
// ms_per_byte_____: 1.3455e-05

// test____________: performance__baseline__sha256
// algorithm_______: baseline::CSHA256
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 1
// seconds_total___: 5.92412
// mib_per_second__: 172.853
// cycles_per_byte_: 16.5518
// ms_per_round____: 5924.12
// ms_per_byte_____: 5.51726e-06

//vs.

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 6.58035
// mib_per_second__: 155.615
// cycles_per_byte_: 18.3853
// ms_per_round____: 0.00627551
// ms_per_byte_____: 6.12843e-06

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 11.3211
// mib_per_second__: 90.4506
// cycles_per_byte_: 31.6308
// ms_per_round____: 0.00067479
// ms_per_byte_____: 1.05436e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 13.8915
// mib_per_second__: 73.7141
// cycles_per_byte_: 38.8124
// ms_per_round____: 0.000413999
// ms_per_byte_____: 1.29375e-05

// test____________: performance__optimal__sha256
// algorithm_______: sha::algorithm<sha::h256<256,1>,0,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 5.92694
// mib_per_second__: 172.77
// cycles_per_byte_: 16.5597
// ms_per_round____: 5926.94
// ms_per_byte_____: 5.5199e-06

// rmd

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.71057
// mib_per_second__: 275.969
// cycles_per_byte_: 10.3672
// ms_per_round____: 0.00353867
// ms_per_byte_____: 3.45574e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.6064
// mib_per_second__: 134.623
// cycles_per_byte_: 21.252
// ms_per_round____: 0.000453377
// ms_per_byte_____: 7.08401e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 8.33945
// mib_per_second__: 122.79
// cycles_per_byte_: 23.3002
// ms_per_round____: 0.000248535
// ms_per_byte_____: 7.76672e-06

// test____________: performance__baseline__rmd160
// algorithm_______: baseline::CRIPEMD160
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.42782
// mib_per_second__: 298.732
// cycles_per_byte_: 9.57723
// ms_per_round____: 3427.82
// ms_per_byte_____: 3.19241e-06

// vs.

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1048576
// bytes_per_round_: 1024
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 4.01654
// mib_per_second__: 254.946
// cycles_per_byte_: 11.2221
// ms_per_round____: 0.00383047
// ms_per_byte_____: 3.7407e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 16777216
// bytes_per_round_: 64
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.0091
// mib_per_second__: 146.096
// cycles_per_byte_: 19.5832
// ms_per_round____: 0.000417775
// ms_per_byte_____: 6.52774e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 33554432
// bytes_per_round_: 32
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 7.64522
// mib_per_second__: 133.94
// cycles_per_byte_: 21.3605
// ms_per_round____: 0.000227845
// ms_per_byte_____: 7.12016e-06

// test____________: performance__optimal__rmd160
// algorithm_______: rmd::algorithm<rmd::h160<160,1>,0,0,1>
// test_rounds_____: 1
// bytes_per_round_: 1073741824
// compressed______: 0
// vectorized______: 0
// concurrent______: 0
// chunked_________: 0
// seconds_total___: 3.63785
// mib_per_second__: 281.485
// cycles_per_byte_: 10.164
// ms_per_round____: 3637.85
// ms_per_byte_____: 3.38801e-06