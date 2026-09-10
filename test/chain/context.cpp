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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(context_tests)

using namespace system::chain;

BOOST_AUTO_TEST_CASE(context__is_enabled__set_flag__true)
{
    const context instance{ flags::bip16_rule | flags::bip65_rule, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE(instance.is_enabled(flags::bip16_rule));
    BOOST_REQUIRE(instance.is_enabled(flags::bip65_rule));
    BOOST_REQUIRE(!instance.is_enabled(flags::bip66_rule));
}

BOOST_AUTO_TEST_CASE(context__is_insufficient_version__below_minimum__true)
{
    const context instance{ 0, 0, 0, 0, 4, 0, 0 };
    BOOST_REQUIRE(instance.is_insufficient_version(3));
    BOOST_REQUIRE(!instance.is_insufficient_version(4));
    BOOST_REQUIRE(!instance.is_insufficient_version(5));
}

// CONSENSUS: the version comparison is signed, a negative version is below
// any positive minimum.
BOOST_AUTO_TEST_CASE(context__is_insufficient_version__negative_version__true)
{
    const context instance{ 0, 0, 0, 0, 4, 0, 0 };
    BOOST_REQUIRE(instance.is_insufficient_version(0x80000000));
}

BOOST_AUTO_TEST_CASE(context__is_insufficient_version__zero_minimum__false)
{
    const context instance{ 0, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE(!instance.is_insufficient_version(0));
    BOOST_REQUIRE(!instance.is_insufficient_version(0x80000000));
}

BOOST_AUTO_TEST_CASE(context__is_anachronistic_timestamp__at_median__true)
{
    const context instance{ 0, 0, 100, 0, 0, 0, 0 };
    BOOST_REQUIRE(instance.is_anachronistic_timestamp(100));
    BOOST_REQUIRE(instance.is_anachronistic_timestamp(99));
    BOOST_REQUIRE(!instance.is_anachronistic_timestamp(101));
}

BOOST_AUTO_TEST_CASE(context__is_invalid_work__mismatch__true)
{
    const context instance{ 0, 0, 0, 0, 0, 0x1d00ffff, 0 };
    BOOST_REQUIRE(instance.is_invalid_work(0x1d00fffe));
    BOOST_REQUIRE(!instance.is_invalid_work(0x1d00ffff));
}

// is_early_timestamp
// ----------------------------------------------------------------------------
// BIP94 (testnet4) bounds how far back a retarget block timestamp may reach.

BOOST_AUTO_TEST_CASE(context__is_early_timestamp__patch_disabled__false)
{
    const context instance{ flags::no_rules, 0, 0, 2016, 0, 0, 10000 };
    BOOST_REQUIRE(!instance.is_early_timestamp(2016));
}

BOOST_AUTO_TEST_CASE(context__is_early_timestamp__not_retarget_height__false)
{
    const context instance{ flags::time_warp_patch, 0, 0, 2015, 0, 0, 10000 };
    BOOST_REQUIRE(!instance.is_early_timestamp(2016));
}

// The bound is exclusive, so the limit itself is not early.
BOOST_AUTO_TEST_CASE(context__is_early_timestamp__at_limit__false)
{
    constexpr auto limit = possible_narrow_cast<uint32_t>(10000u - max_timewarp);
    const context instance{ flags::time_warp_patch, limit, 0, 2016, 0, 0, 10000 };
    BOOST_REQUIRE(!instance.is_early_timestamp(2016));
}

BOOST_AUTO_TEST_CASE(context__is_early_timestamp__below_limit__true)
{
    constexpr auto limit = possible_narrow_cast<uint32_t>(10000u - max_timewarp);
    const context instance{ flags::time_warp_patch, sub1(limit), 0, 2016, 0, 0, 10000 };
    BOOST_REQUIRE(instance.is_early_timestamp(2016));
}

// The subtraction is floored, so a low previous timestamp cannot underflow.
BOOST_AUTO_TEST_CASE(context__is_early_timestamp__floored_previous__false)
{
    const context instance{ flags::time_warp_patch, 0, 0, 2016, 0, 0, 42 };
    BOOST_REQUIRE(!instance.is_early_timestamp(2016));
}

BOOST_AUTO_TEST_SUITE_END()
