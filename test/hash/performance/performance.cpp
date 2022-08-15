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

#if defined(HAVE_PERFORMANCE_TESTS)

BOOST_AUTO_TEST_SUITE(performance_tests)

using namespace performance;
using rmd160a          = rmd160_parameters<false>;
using rmd160c          = rmd160_parameters<true>;
using sha256a          = sha256_parameters<true, false, true,  false>;
using sha256c_cached   = sha256_parameters<true, false, true,  true>;
using sha256c_uncached = sha256_parameters<true, false, false, true>;

using sha256a_both     = sha256_parameters<true,  true,  true, false>;
using sha256a_comp     = sha256_parameters<true,  false, true, false>;
using sha256a_vect     = sha256_parameters<false, true,  true, false>;
using sha256a_none     = sha256_parameters<false, false, true, false>;

using namespace baseline;
using base_rmd160a     = base::parameters<CRIPEMD160, false>;
using base_rmd160c     = base::parameters<CRIPEMD160, true>;
using base_sha256a     = base::parameters<CSHA256, false>;
using base_sha256c     = base::parameters<CSHA256, true>;

struct v0
{
    static constexpr size_t c = 1024;
    static constexpr size_t s = 1024 * 1024;
};

struct v1
{
    static constexpr size_t c = 1;
    static constexpr size_t s = 1024 * 1024 * 1024;
};

struct v2
{
    static constexpr size_t c = 8 * 1024 * 1024;
    static constexpr size_t s = 128;
};

struct v3
{
    static constexpr size_t c = 16 * 1024 * 1024;
    static constexpr size_t s = 64;
};

struct v4
{
    static constexpr size_t c = 32 * 1024 * 1024;
    static constexpr size_t s = 32;
};

BOOST_AUTO_TEST_CASE(performance__base_sha256a)
{
    auto complete = true;
    complete = base::test_hash<base_sha256a, v0::c, v0::s>(std::cout);
    complete = base::test_hash<base_sha256a, v1::c, v1::s>(std::cout);
    complete = base::test_hash<base_sha256a, v2::c, v2::s>(std::cout);
    complete = base::test_hash<base_sha256a, v3::c, v3::s>(std::cout);
    complete = base::test_hash<base_sha256a, v4::c, v4::s>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_CASE(performance__sha256a_both)
{
    auto complete = true;
    complete = test_accumulator<sha256a_both, v0::c, v0::s>(std::cout);
    complete = test_accumulator<sha256a_both, v1::c, v1::s>(std::cout);
    complete = test_accumulator<sha256a_both, v2::c, v2::s>(std::cout);
    complete = test_accumulator<sha256a_both, v3::c, v3::s>(std::cout);
    complete = test_accumulator<sha256a_both, v4::c, v4::s>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_CASE(performance__sha256a_comp)
{
    auto complete = true;
    complete = test_accumulator<sha256a_comp, v0::c, v0::s>(std::cout);
    complete = test_accumulator<sha256a_comp, v1::c, v1::s>(std::cout);
    complete = test_accumulator<sha256a_comp, v2::c, v2::s>(std::cout);
    complete = test_accumulator<sha256a_comp, v3::c, v3::s>(std::cout);
    complete = test_accumulator<sha256a_comp, v4::c, v4::s>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_CASE(performance__sha256a_vect)
{
    auto complete = true;
    complete = test_accumulator<sha256a_vect, v0::c, v0::s>(std::cout);
    complete = test_accumulator<sha256a_vect, v1::c, v1::s>(std::cout);
    complete = test_accumulator<sha256a_vect, v2::c, v2::s>(std::cout);
    complete = test_accumulator<sha256a_vect, v3::c, v3::s>(std::cout);
    complete = test_accumulator<sha256a_vect, v4::c, v4::s>(std::cout);
    BOOST_CHECK(complete);
}

BOOST_AUTO_TEST_CASE(performance__sha256a_none)
{
    auto complete = true;
    complete = test_accumulator<sha256a_none, v0::c, v0::s>(std::cout);
    complete = test_accumulator<sha256a_none, v1::c, v1::s>(std::cout);
    complete = test_accumulator<sha256a_none, v2::c, v2::s>(std::cout);
    complete = test_accumulator<sha256a_none, v3::c, v3::s>(std::cout);
    complete = test_accumulator<sha256a_none, v4::c, v4::s>(std::cout);
    BOOST_CHECK(complete);
}

////BOOST_AUTO_TEST_CASE(performance__sha256__baseline)
////{
////    auto complete = true;
////    complete = base::test_hash<base_sha256a, v0::c, v0::s>(std::cout);
////    complete = base::test_hash<base_sha256a, v1::c, v1::s>(std::cout);
////    complete = base::test_hash<base_sha256a, v2::c, v2::s>(std::cout);
////    complete = base::test_hash<base_sha256a, v3::c, v3::s>(std::cout);
////    complete = base::test_hash<base_sha256a, v4::c, v4::s>(std::cout);
////    BOOST_CHECK(complete);
////}
////
////BOOST_AUTO_TEST_CASE(performance__rmd160__baseline)
////{
////    auto complete = true;
////    complete = base::test_hash<base_rmd160a, v0::c, v0::s>(std::cout);
////    complete = base::test_hash<base_rmd160a, v1::c, v1::s>(std::cout);
////    complete = base::test_hash<base_rmd160a, v2::c, v2::s>(std::cout);
////    complete = base::test_hash<base_rmd160a, v3::c, v3::s>(std::cout);
////    complete = base::test_hash<base_rmd160a, v4::c, v4::s>(std::cout);
////    BOOST_CHECK(complete);
////}

////BOOST_AUTO_TEST_CASE(performance__sha256__accumulator)
////{
////    auto complete = true;
////    complete = test_accumulator<sha256a, v0::c, v0::s>(std::cout);
////    complete = test_accumulator<sha256a, v1::c, v1::s>(std::cout);
////    complete = test_accumulator<sha256a, v2::c, v2::s>(std::cout);
////    complete = test_accumulator<sha256a, v3::c, v3::s>(std::cout);
////    complete = test_accumulator<sha256a, v4::c, v4::s>(std::cout);
////    BOOST_CHECK(complete);
////}
////
////BOOST_AUTO_TEST_CASE(performance__rmd160__accumulator)
////{
////    auto complete = true;
////    complete = test_accumulator<rmd160a, v0::c, v0::s>(std::cout);
////    complete = test_accumulator<rmd160a, v1::c, v1::s>(std::cout);
////    complete = test_accumulator<rmd160a, v2::c, v2::s>(std::cout);
////    complete = test_accumulator<rmd160a, v3::c, v3::s>(std::cout);
////    complete = test_accumulator<rmd160a, v4::c, v4::s>(std::cout);
////    BOOST_CHECK(complete);
////}

////BOOST_AUTO_TEST_CASE(performance__sha256c_cached__accumulator)
////{
////    auto complete = true;
////    complete = test_accumulator<sha256c_cached, v0::c, v0::s>(std::cout);
////    complete = test_accumulator<sha256c_cached, v1::c, v1::s>(std::cout);
////    complete = test_accumulator<sha256c_cached, v2::c, v2::s>(std::cout);
////    complete = test_accumulator<sha256c_cached, v3::c, v3::s>(std::cout);
////    complete = test_accumulator<sha256c_cached, v4::c, v4::s>(std::cout);
////    BOOST_CHECK(complete);
////}
////
////BOOST_AUTO_TEST_CASE(performance__sha256c_uncached__accumulator)
////{
////    auto complete = true;
////    complete = test_accumulator<sha256c_uncached, v0::c, v0::s>(std::cout);
////    complete = test_accumulator<sha256c_uncached, v1::c, v1::s>(std::cout);
////    complete = test_accumulator<sha256c_uncached, v2::c, v2::s>(std::cout);
////    complete = test_accumulator<sha256c_uncached, v3::c, v3::s>(std::cout);
////    complete = test_accumulator<sha256c_uncached, v4::c, v4::s>(std::cout);
////    BOOST_CHECK(complete);
////}

//////BOOST_AUTO_TEST_CASE(performance__rmd160__algorithm)
//////{
//////    auto complete = true;
//////    ////complete = test_algorithm<rmd160a, v0::c, v0::s>(std::cout);
//////    ////complete = test_algorithm<rmd160a, v1::c, v1::s>(std::cout);
//////    /////complete = test_algorithm<rmd160a, v2::c, v2::s>(std::cout);
//////    complete = test_algorithm<rmd160a, v3::c, v3::s>(std::cout);
//////    complete = test_algorithm<rmd160a, v4::c, v4::s>(std::cout);
//////    BOOST_CHECK(complete);
//////}

BOOST_AUTO_TEST_SUITE_END()

#endif
