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

BC_AUTO_TEST_SUITE(context_tests)

using namespace system::chain;

BC_AUTO_TEST_CASE(context__is_enabled__set_flag__true)
{
    const context instance{ flags::bip16_rule | flags::bip65_rule, 0, 0, 0, 0, 0, 0 };
    BC_REQUIRE(instance.is_enabled(flags::bip16_rule));
    BC_REQUIRE(instance.is_enabled(flags::bip65_rule));
    BC_REQUIRE(!instance.is_enabled(flags::bip66_rule));
}

BC_AUTO_TEST_CASE(context__is_insufficient_version__below_minimum__true)
{
    const context instance{ 0, 0, 0, 0, 4, 0, 0 };
    BC_REQUIRE(instance.is_insufficient_version(3));
    BC_REQUIRE(!instance.is_insufficient_version(4));
    BC_REQUIRE(!instance.is_insufficient_version(5));
}

// CONSENSUS: the version comparison is signed, a negative version is below
// any positive minimum.
BC_AUTO_TEST_CASE(context__is_insufficient_version__negative_version__true)
{
    const context instance{ 0, 0, 0, 0, 4, 0, 0 };
    BC_REQUIRE(instance.is_insufficient_version(0x80000000));
}

BC_AUTO_TEST_CASE(context__is_insufficient_version__zero_minimum__false)
{
    const context instance{ 0, 0, 0, 0, 0, 0, 0 };
    BC_REQUIRE(!instance.is_insufficient_version(0));
    BC_REQUIRE(!instance.is_insufficient_version(0x80000000));
}

BC_AUTO_TEST_CASE(context__is_anachronistic_timestamp__at_median__true)
{
    const context instance{ 0, 0, 100, 0, 0, 0, 0 };
    BC_REQUIRE(instance.is_anachronistic_timestamp(100));
    BC_REQUIRE(instance.is_anachronistic_timestamp(99));
    BC_REQUIRE(!instance.is_anachronistic_timestamp(101));
}

BC_AUTO_TEST_CASE(context__equality__retarget_timestamp__compared)
{
    const context left{ 0, 0, 0, 0, 0, 0, 0, 1 };
    const context right{ 0, 0, 0, 0, 0, 0, 0, 2 };
    const context same{ 0, 0, 0, 0, 0, 0, 0, 1 };
    BC_REQUIRE(left != right);
    BC_REQUIRE(left == same);
}

BC_AUTO_TEST_CASE(context__is_invalid_work__mismatch__true)
{
    const context instance{ 0, 0, 0, 0, 0, 0x1d00ffff, 0 };
    BC_REQUIRE(instance.is_invalid_work(0x1d00fffe));
    BC_REQUIRE(!instance.is_invalid_work(0x1d00ffff));
}

// is_early_timestamp
// ----------------------------------------------------------------------------
// BIP94 (testnet4) bounds how far back a retarget block timestamp may reach.

BC_AUTO_TEST_CASE(context__is_early_timestamp__patch_disabled__false)
{
    const context instance{ flags::no_rules, 0, 0, 2016, 0, 0, 10000 };
    BC_REQUIRE(!instance.is_early_timestamp(2016));
}

BC_AUTO_TEST_CASE(context__is_early_timestamp__not_retarget_height__false)
{
    const context instance{ flags::time_warp_patch, 0, 0, 2015, 0, 0, 10000 };
    BC_REQUIRE(!instance.is_early_timestamp(2016));
}

// The bound is exclusive, so the limit itself is not early.
BC_AUTO_TEST_CASE(context__is_early_timestamp__at_limit__false)
{
    constexpr auto limit = possible_narrow_cast<uint32_t>(10000u - max_timewarp);
    const context instance{ flags::time_warp_patch, limit, 0, 2016, 0, 0, 10000 };
    BC_REQUIRE(!instance.is_early_timestamp(2016));
}

BC_AUTO_TEST_CASE(context__is_early_timestamp__below_limit__true)
{
    constexpr auto limit = possible_narrow_cast<uint32_t>(10000u - max_timewarp);
    const context instance{ flags::time_warp_patch, sub1(limit), 0, 2016, 0, 0, 10000 };
    BC_REQUIRE(instance.is_early_timestamp(2016));
}

// The subtraction is floored, so a low previous timestamp cannot underflow.
BC_AUTO_TEST_CASE(context__is_early_timestamp__floored_previous__false)
{
    const context instance{ flags::time_warp_patch, 0, 0, 2016, 0, 0, 42 };
    BC_REQUIRE(!instance.is_early_timestamp(2016));
}

// bip54 bounds the retarget block timestamp to two hours before its parent.
BC_AUTO_TEST_CASE(context__is_early_timestamp__bip54_at_limit__false)
{
    constexpr auto limit = possible_narrow_cast<uint32_t>(10000u - max_timewarp_bip54);
    const context instance{ flags::bip54_timewarp_rule, limit, 0, 2016, 0, 0, 10000 };
    BC_REQUIRE(!instance.is_early_timestamp(2016));
}

BC_AUTO_TEST_CASE(context__is_early_timestamp__bip54_below_limit__true)
{
    constexpr auto limit = possible_narrow_cast<uint32_t>(10000u - max_timewarp_bip54);
    const context instance{ flags::bip54_timewarp_rule, sub1(limit), 0, 2016, 0, 0, 10000 };
    BC_REQUIRE(instance.is_early_timestamp(2016));
}

BC_AUTO_TEST_CASE(context__is_early_timestamp__bip54_not_retarget_height__false)
{
    const context instance{ flags::bip54_timewarp_rule, 0, 0, 2015, 0, 0, 10000 };
    BC_REQUIRE(!instance.is_early_timestamp(2016));
}

// The bip94 bound takes precedence where both apply.
BC_AUTO_TEST_CASE(context__is_early_timestamp__bip54_and_patch__patch_limit)
{
    constexpr auto limit = possible_narrow_cast<uint32_t>(10000u - max_timewarp);
    const context instance{ flags::bip54_timewarp_rule | flags::time_warp_patch, sub1(limit), 0, 2016, 0, 0, 10000 };
    BC_REQUIRE(instance.is_early_timestamp(2016));
}

// is_negative_timespan
// ----------------------------------------------------------------------------
// bip54 requires the last block of a period at/after the period's first.

BC_AUTO_TEST_CASE(context__is_negative_timespan__not_last_height__false)
{
    const context instance{ flags::bip54_timespan_rule, 99, 0, 2014, 0, 0, 0, 100 };
    BC_REQUIRE(!instance.is_negative_timespan(2016));
    const context next{ flags::bip54_timespan_rule, 99, 0, 2016, 0, 0, 0, 100 };
    BC_REQUIRE(!next.is_negative_timespan(2016));
}

BC_AUTO_TEST_CASE(context__is_negative_timespan__at_first__false)
{
    const context instance{ flags::bip54_timespan_rule, 100, 0, 2015, 0, 0, 0, 100 };
    BC_REQUIRE(!instance.is_negative_timespan(2016));
}

BC_AUTO_TEST_CASE(context__is_negative_timespan__below_first__true)
{
    const context instance{ flags::bip54_timespan_rule, 99, 0, 2015, 0, 0, 0, 100 };
    BC_REQUIRE(instance.is_negative_timespan(2016));
    const context later{ flags::bip54_timespan_rule, 99, 0, 4031, 0, 0, 0, 100 };
    BC_REQUIRE(later.is_negative_timespan(2016));
}

BC_AUTO_TEST_SUITE_END()
