/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(input_tests)

using namespace system::chain;

const auto input_data = base16_chunk(
    "54b755c39207d443fd96a8d12c94446a1c6f66e39c95e894c23418d7501f681b01000"
    "0006b48304502203267910f55f2297360198fff57a3631be850965344370f732950b4"
    "7795737875022100f7da90b82d24e6e957264b17d3e5042bab8946ee5fc676d15d915"
    "da450151d36012103893d5a06201d5cf61400e96fa4a7514fc12ab45166ace618d68b"
    "8066c9c585f9ffffffff");

const input expected_input(input_data);

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid

BOOST_AUTO_TEST_CASE(input__constructor__default__valid)
{
    input instance;
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(input__constructor__move__expected)
{
    auto copy = expected_input;
    input instance(std::move(copy));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.point() == expected_input.point());
    BOOST_REQUIRE(instance.script() == expected_input.script());
    BOOST_REQUIRE(instance.witness() == expected_input.witness());
    BOOST_REQUIRE(instance.sequence() == expected_input.sequence());
}

BOOST_AUTO_TEST_CASE(input__constructor__copy__expected)
{
    input instance(expected_input);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.point() == expected_input.point());
    BOOST_REQUIRE(instance.script() == expected_input.script());
    BOOST_REQUIRE(instance.witness() == expected_input.witness());
    BOOST_REQUIRE(instance.sequence() == expected_input.sequence());
}

BOOST_AUTO_TEST_CASE(input__constructor__move_parameters__expected)
{
    auto point_copy = expected_input.point();
    auto script_copy = expected_input.script();
    input instance(std::move(point_copy), std::move(script_copy), expected_input.sequence());
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.point() == expected_input.point());
    BOOST_REQUIRE(instance.script() == expected_input.script());
    BOOST_REQUIRE(instance.witness().empty());
    BOOST_REQUIRE(instance.sequence() == expected_input.sequence());
}

BOOST_AUTO_TEST_CASE(input__constructor__copy_parameters__expected)
{
    input instance(expected_input.point(), expected_input.script(), expected_input.sequence());
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.point() == expected_input.point());
    BOOST_REQUIRE(instance.script() == expected_input.script());
    BOOST_REQUIRE(instance.witness().empty());
    BOOST_REQUIRE(instance.sequence() == expected_input.sequence());
}

BOOST_AUTO_TEST_CASE(input__constructor__move_parameters_witness__expected)
{
    auto point_copy = expected_input.point();
    auto script_copy = expected_input.script();
    auto witness_copy = expected_input.witness();
    input instance(std::move(point_copy), std::move(script_copy), std::move(witness_copy), expected_input.sequence());
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.point() == expected_input.point());
    BOOST_REQUIRE(instance.script() == expected_input.script());
    BOOST_REQUIRE(instance.witness() == expected_input.witness());
    BOOST_REQUIRE(instance.sequence() == expected_input.sequence());
}

BOOST_AUTO_TEST_CASE(input__constructor__copy_parameters_witness__expected)
{
    input instance(expected_input.point(), expected_input.script(), expected_input.witness(), expected_input.sequence());
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.point() == expected_input.point());
    BOOST_REQUIRE(instance.script() == expected_input.script());
    BOOST_REQUIRE(instance.witness() == expected_input.witness());
    BOOST_REQUIRE(instance.sequence() == expected_input.sequence());
}

BOOST_AUTO_TEST_CASE(input__from_data__junk_data__valid)
{
    // Any set of bytes is a valid script.
    const auto data = base16_chunk("000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0");
    input instance(data);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(input__constructor__data__expected)
{
    const input instance(input_data);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_input);
}

BOOST_AUTO_TEST_CASE(input__constructor__stream__success)
{
    stream::in::copy stream(input_data);
    const input instance(stream);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_input);
}

BOOST_AUTO_TEST_CASE(input__constructor__reader__success)
{
    read::bytes::copy source(input_data);
    const input instance(source);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_input);
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(input__assign__move__expected)
{
    const auto& alpha = expected_input;
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(input__assign__copy__expected)
{
    const auto& alpha = expected_input;
    const auto beta = alpha;
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(input__equality__same__true)
{
    const auto& alpha = expected_input;
    const input beta(alpha);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(input__equality__different__false)
{
    const auto& alpha = expected_input;
    const input beta;
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(input__inequality__same__false)
{
    const auto& alpha = expected_input;
    const input beta(alpha);
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(input__inequality__different__true)
{
    const auto& alpha = expected_input;
    const input beta;
    BOOST_REQUIRE(alpha != beta);
}

// from_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(input__from_data__insufficient_data__invalid)
{
    data_chunk insufficient_data(2);
    input instance;
    BOOST_REQUIRE(!instance.from_data(insufficient_data));
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(input__from_data__data__valid)
{
    input instance;
    BOOST_REQUIRE(instance.from_data(input_data));
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(input__from_data__stream__valid)
{
    input instance;
    stream::in::copy stream(input_data);
    BOOST_REQUIRE(instance.from_data(stream));
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(input__from_data__reader__valid)
{
    input instance;
    read::bytes::copy source(input_data);
    BOOST_REQUIRE(instance.from_data(source));
    BOOST_REQUIRE(instance.is_valid());
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(input__to_data__data__expected)
{
    const auto size = expected_input.to_data().size();
    BOOST_REQUIRE_EQUAL(size, expected_input.serialized_size(false));
}

BOOST_AUTO_TEST_CASE(input__to_data__stream__expected)
{
    // Write input to stream.
    std::stringstream iostream;
    expected_input.to_data(iostream);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const input copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_input);
}

BOOST_AUTO_TEST_CASE(input__to_data__writer__expected)
{
    // Write input to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_input.to_data(out);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const input copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_input);
}

// properties
// ----------------------------------------------------------------------------

// serialized_size

// methods
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(input__is_final__max_input_sequence__true)
{
    const input instance({}, {}, max_input_sequence);
    BOOST_REQUIRE(instance.is_final());
}

BOOST_AUTO_TEST_CASE(input__is_final__sequence_zero__false)
{
    const input instance({}, {}, 0);
    BOOST_REQUIRE(!instance.is_final());
}

BOOST_AUTO_TEST_CASE(input__is_locked__enabled_block_sequence_age_equals_minimum__false)
{
    static const auto age = 7u;
    static const auto sequence_enabled_block_type_minimum = age;
    input instance({}, {}, sequence_enabled_block_type_minimum);
    auto& prevout = instance.prevout;
    prevout.height = 42;
    BOOST_REQUIRE(!instance.is_locked(prevout.height + age, 0));
}

BOOST_AUTO_TEST_CASE(input__is_locked__enabled_block_type_sequence_age_above_minimum__false)
{
    static const auto age = 7u;
    static const auto sequence_enabled_block_type_minimum = sub1(age);
    input instance({}, {}, sequence_enabled_block_type_minimum);
    auto& prevout = instance.prevout;
    prevout.height = 42;
    BOOST_REQUIRE(!instance.is_locked(prevout.height + age, 0));
}

BOOST_AUTO_TEST_CASE(input__is_locked__enabled_block_type_sequence_age_below_minimum__true)
{
    static const auto age = 7u;
    static const auto sequence_enabled_block_type_minimum = add1(age);
    input instance({}, {}, sequence_enabled_block_type_minimum);
    auto& prevout = instance.prevout;
    prevout.height = 42;
    BOOST_REQUIRE(instance.is_locked(prevout.height + age, 0));
}

BOOST_AUTO_TEST_CASE(input__is_locked__disabled_block_type_sequence_age_below_minimum__false)
{
    static const auto age = 7u;
    static const auto sequence_disabled_block_type_minimum = bit_right<uint32_t>(relative_locktime_disabled_bit) | add1(age);
    input instance({}, {}, sequence_disabled_block_type_minimum);
    auto& prevout = instance.prevout;
    prevout.height = 42;
    BOOST_REQUIRE(!instance.is_locked(prevout.height + age, 0));
}

BOOST_AUTO_TEST_CASE(input__is_locked__enabled_time_type_sequence_age_equals_minimum__false)
{
    static const auto age = 7u;
    static const auto age_seconds = 7u << relative_locktime_seconds_shift_left;
    static const auto sequence_enabled_time_type_minimum = bit_right<uint32_t>(relative_locktime_time_locked_bit) | age;
    input instance({}, {}, sequence_enabled_time_type_minimum);
    auto& prevout = instance.prevout;
    prevout.median_time_past = 42;
    BOOST_REQUIRE(!instance.is_locked(0, prevout.median_time_past + age_seconds));
}

BOOST_AUTO_TEST_CASE(input__is_locked__enabled_time_type_sequence_age_above_minimum__false)
{
    static const auto age = 7u;
    static const auto age_seconds = 7u << relative_locktime_seconds_shift_left;
    static const auto sequence_enabled_time_type_minimum = bit_right<uint32_t>(relative_locktime_time_locked_bit) | sub1(age);
    input instance({}, {}, sequence_enabled_time_type_minimum);
    auto& prevout = instance.prevout;
    prevout.median_time_past = 42;
    BOOST_REQUIRE(!instance.is_locked(0, prevout.median_time_past + age_seconds));
}

BOOST_AUTO_TEST_CASE(input__is_locked__enabled_time_type_sequence_age_below_minimum__true)
{
    static const auto age = 7u;
    static const auto age_seconds = 7u << relative_locktime_seconds_shift_left;
    static const auto sequence_enabled_time_type_minimum = bit_right<uint32_t>(relative_locktime_time_locked_bit) | add1(age);
    input instance({}, {}, sequence_enabled_time_type_minimum);
    auto& prevout = instance.prevout;
    prevout.median_time_past = 42;
    BOOST_REQUIRE(instance.is_locked(0, prevout.median_time_past + age_seconds));
}

BOOST_AUTO_TEST_CASE(input__is_locked__disabled_time_type_sequence_age_below_minimum__false)
{
    static const auto age = 7u;
    static const auto age_seconds = 7u << relative_locktime_seconds_shift_left;
    static const auto sequence_disabled_time_type_minimum = bit_right<uint32_t>(relative_locktime_disabled_bit) | bit_right<uint32_t>(relative_locktime_time_locked_bit) | add1(age);
    input instance({}, {}, sequence_disabled_time_type_minimum);
    auto& prevout = instance.prevout;
    prevout.median_time_past = 42;
    BOOST_REQUIRE(!instance.is_locked(0, prevout.median_time_past + age_seconds));
}

// reserved_hash

BOOST_AUTO_TEST_CASE(input__signature_operations__bips_inactive__script_sigops)
{
    script script;
    BOOST_REQUIRE(script.from_data(base16_chunk("02acad"), true));

    input instance{ {}, script, chain::max_input_sequence };
    BOOST_REQUIRE_EQUAL(script.sigops(false), instance.signature_operations(false, false));
}

BOOST_AUTO_TEST_CASE(input__signature_operations__bip141_inactive__script_sigops)
{
    script script;
    BOOST_REQUIRE(script.from_data(base16_chunk("02acad"), true));

    input instance{ {}, script, chain::max_input_sequence };
    BOOST_REQUIRE_EQUAL(instance.signature_operations(true, false), script.sigops(false));
    BOOST_REQUIRE_EQUAL(instance.signature_operations(false, false), script.sigops(false));
}

BOOST_AUTO_TEST_CASE(input__signature_operations__bip141_active_missing_prevout__max_size_t_sigops)
{
    script script;
    BOOST_REQUIRE(script.from_data(base16_chunk("02acad"), true));

    input instance{ {}, script, chain::max_input_sequence };
    BOOST_REQUIRE_EQUAL(instance.signature_operations(true, true), max_size_t);
    BOOST_REQUIRE_EQUAL(instance.signature_operations(false, true), max_size_t);
}

BOOST_AUTO_TEST_SUITE_END()
