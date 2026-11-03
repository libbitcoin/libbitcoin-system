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

BC_AUTO_TEST_SUITE(input_tests)

namespace json = system::json;
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

BC_AUTO_TEST_CASE(input__constructor__default__valid)
{
    const input instance;
    BC_REQUIRE(!instance.is_valid());
}

BC_AUTO_TEST_CASE(input__constructor__move__expected)
{
    auto copy = expected_input;
    const input instance(std::move(copy));
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_input);
}

BC_AUTO_TEST_CASE(input__constructor__copy__expected)
{
    const input instance(expected_input);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_input);
}

BC_AUTO_TEST_CASE(input__constructor__move_parameters__expected)
{
    auto point_copy = expected_input.point();
    auto script_copy = expected_input.script();
    const input instance(std::move(point_copy), std::move(script_copy), expected_input.sequence());
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance.point() == expected_input.point());
    BC_REQUIRE(instance.script() == expected_input.script());
    BC_REQUIRE(instance.witness().stack().empty());
    BC_REQUIRE(instance.sequence() == expected_input.sequence());
}

BC_AUTO_TEST_CASE(input__constructor__copy_parameters__expected)
{
    const input instance(expected_input.point(), expected_input.script(), expected_input.sequence());
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance.point() == expected_input.point());
    BC_REQUIRE(instance.script() == expected_input.script());
    BC_REQUIRE(instance.witness().stack().empty());
    BC_REQUIRE(instance.sequence() == expected_input.sequence());
}

BC_AUTO_TEST_CASE(input__constructor__move_parameters_witness__expected)
{
    auto point_copy = expected_input.point();
    auto script_copy = expected_input.script();
    auto witness_copy = expected_input.witness();
    const input instance(std::move(point_copy), std::move(script_copy), std::move(witness_copy), expected_input.sequence());
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_input);
}

BC_AUTO_TEST_CASE(input__constructor__copy_parameters_witness__expected)
{
    const input instance(expected_input.point(), expected_input.script(), expected_input.witness(), expected_input.sequence());
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_input);
}

BC_AUTO_TEST_CASE(input__from_data__junk_data__valid)
{
    // Any set of bytes is a valid script.
    const auto data = base16_chunk("000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0");
    const input instance(data);
    BC_REQUIRE(instance.is_valid());
}

BC_AUTO_TEST_CASE(input__constructor__data__expected)
{
    const input instance(input_data);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_input);
}

BC_AUTO_TEST_CASE(input__constructor__fast__success)
{
    stream::in::fast stream(input_data);
    const input instance(stream);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_input);
}

BC_AUTO_TEST_CASE(input__constructor__stream__success)
{
    stream::in::copy stream(input_data);
    const input instance(stream);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_input);
}

BC_AUTO_TEST_CASE(input__constructor__reader__success)
{
    read::bytes::copy source(input_data);
    const input instance(source);
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance == expected_input);
}

// operators
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(input__assign__move__expected)
{
    const auto& alpha = expected_input;
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(input__assign__copy__expected)
{
    const auto& alpha = expected_input;
    const auto beta = alpha;
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(input__equality__same__true)
{
    const auto& alpha = expected_input;
    const input beta(alpha);
    BC_REQUIRE(alpha == beta);
}

BC_AUTO_TEST_CASE(input__equality__different__false)
{
    const auto& alpha = expected_input;
    const input beta;
    BC_REQUIRE(!(alpha == beta));
}

BC_AUTO_TEST_CASE(input__inequality__same__false)
{
    const auto& alpha = expected_input;
    const input beta(alpha);
    BC_REQUIRE(!(alpha != beta));
}

BC_AUTO_TEST_CASE(input__inequality__different__true)
{
    const auto& alpha = expected_input;
    const input beta;
    BC_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(input__to_data__data__expected)
{
    const auto size = expected_input.to_data().size();
    BC_REQUIRE_EQUAL(size, expected_input.serialized_size(false));
}

BC_AUTO_TEST_CASE(input__to_data__stream__expected)
{
    // Write input to stream.
    std::stringstream iostream;
    expected_input.to_data(iostream);
    BC_REQUIRE(iostream);

    // Verify stream contents.
    const input copy(iostream);
    BC_REQUIRE(iostream);
    BC_REQUIRE(copy.is_valid());
    BC_REQUIRE(copy == expected_input);
}

BC_AUTO_TEST_CASE(input__to_data__writer__expected)
{
    // Write input to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_input.to_data(out);
    BC_REQUIRE(iostream);

    // Verify stream contents.
    const input copy(iostream);
    BC_REQUIRE(iostream);
    BC_REQUIRE(copy.is_valid());
    BC_REQUIRE(copy == expected_input);
}

// properties
// ----------------------------------------------------------------------------

// serialized_size

// methods
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(input__is_final__max_input_sequence__true)
{
    const input instance(point{}, {}, max_input_sequence);
    BC_REQUIRE(instance.is_final());
}

BC_AUTO_TEST_CASE(input__is_final__sequence_zero__false)
{
    const input instance(point{}, {}, 0);
    BC_REQUIRE(!instance.is_final());
}

BC_AUTO_TEST_CASE(input__is_relative_locked__enabled_block_sequence_age_equals_minimum__false)
{
    constexpr auto age = 7u;
    constexpr auto sequence_enabled_block_type_minimum = age;
    const input instance(point{}, {}, sequence_enabled_block_type_minimum);
    BC_REQUIRE(!instance.prevout);
    instance.metadata.prevout_height = 42;
    BC_REQUIRE(!instance.is_relative_locked(instance.metadata.prevout_height + age, 0));
}

BC_AUTO_TEST_CASE(input__is_relative_locked__enabled_block_type_sequence_age_above_minimum__false)
{
    constexpr auto age = 7u;
    constexpr auto sequence_enabled_block_type_minimum = sub1(age);
    const input instance(point{}, {}, sequence_enabled_block_type_minimum);
    BC_REQUIRE(!instance.prevout);
    instance.metadata.prevout_height = 42;
    BC_REQUIRE(!instance.is_relative_locked(instance.metadata.prevout_height + age, 0));
}

BC_AUTO_TEST_CASE(input__is_relative_locked__enabled_block_type_sequence_age_below_minimum__true)
{
    constexpr auto age = 7u;
    constexpr auto sequence_enabled_block_type_minimum = add1(age);
    const input instance(point{}, {}, sequence_enabled_block_type_minimum);
    BC_REQUIRE(!instance.prevout);
    instance.metadata.prevout_height = 42;
    BC_REQUIRE(instance.is_relative_locked(instance.metadata.prevout_height + age, 0));
}

BC_AUTO_TEST_CASE(input__is_relative_locked__disabled_block_type_sequence_age_below_minimum__false)
{
    constexpr auto age = 7u;
    constexpr auto sequence_disabled_block_type_minimum = bit_right<uint32_t>(relative_locktime_disabled_bit) | add1(age);
    const input instance(point{}, {}, sequence_disabled_block_type_minimum);
    BC_REQUIRE(!instance.prevout);
    instance.metadata.prevout_height = 42;
    BC_REQUIRE(!instance.is_relative_locked(instance.metadata.prevout_height + age, 0));
}

BC_AUTO_TEST_CASE(input__is_relative_locked__enabled_time_type_sequence_age_equals_minimum__false)
{
    constexpr auto age = 7u;
    constexpr auto age_seconds = 7u << relative_locktime_seconds_shift_left;
    constexpr auto sequence_enabled_time_type_minimum = bit_right<uint32_t>(relative_locktime_time_locked_bit) | age;
    const input instance(point{}, {}, sequence_enabled_time_type_minimum);
    BC_REQUIRE(!instance.prevout);
    instance.metadata.median_time_past = 42;
    BC_REQUIRE(!instance.is_relative_locked(0, instance.metadata.median_time_past + age_seconds));
}

BC_AUTO_TEST_CASE(input__is_relative_locked__enabled_time_type_sequence_age_above_minimum__false)
{
    constexpr auto age = 7u;
    constexpr auto age_seconds = 7u << relative_locktime_seconds_shift_left;
    constexpr auto sequence_enabled_time_type_minimum = bit_right<uint32_t>(relative_locktime_time_locked_bit) | sub1(age);
    const input instance(point{}, {}, sequence_enabled_time_type_minimum);
    BC_REQUIRE(!instance.prevout);
    instance.metadata.median_time_past = 42;
    BC_REQUIRE(!instance.is_relative_locked(0, instance.metadata.median_time_past + age_seconds));
}

BC_AUTO_TEST_CASE(input__is_relative_locked__enabled_time_type_sequence_age_below_minimum__true)
{
    constexpr auto age = 7u;
    constexpr auto age_seconds = 7u << relative_locktime_seconds_shift_left;
    constexpr auto sequence_enabled_time_type_minimum = bit_right<uint32_t>(relative_locktime_time_locked_bit) | add1(age);
    const input instance(point{}, {}, sequence_enabled_time_type_minimum);
    BC_REQUIRE(!instance.prevout);
    instance.metadata.median_time_past = 42;
    BC_REQUIRE(instance.is_relative_locked(0, instance.metadata.median_time_past + age_seconds));
}

BC_AUTO_TEST_CASE(input__is_relative_locked__disabled_time_type_sequence_age_below_minimum__false)
{
    constexpr auto age = 7u;
    constexpr auto age_seconds = 7u << relative_locktime_seconds_shift_left;
    constexpr auto sequence_disabled_time_type_minimum = bit_right<uint32_t>(relative_locktime_disabled_bit) | bit_right<uint32_t>(relative_locktime_time_locked_bit) | add1(age);
    const input instance(point{}, {}, sequence_disabled_time_type_minimum);
    BC_REQUIRE(!instance.prevout);
    instance.metadata.median_time_past = 42;
    BC_REQUIRE(!instance.is_relative_locked(0, instance.metadata.median_time_past + age_seconds));
}

// These express the sequence as a literal, so they pin the bit layout that
// the parameterized cases above take from the same constants as the code.

// BIP68: only the low 16 bits of the sequence encode the lock.
BC_AUTO_TEST_CASE(input__is_relative_locked__block_type_bit_above_mask__false)
{
    constexpr uint32_t sequence = 0x00010000;
    const input instance(point{}, {}, sequence);
    instance.metadata.prevout_height = 0;
    BC_REQUIRE(!instance.is_relative_locked(0, 0));
}

// BIP68: bit 22 selects seconds, each unit 512 seconds.
BC_AUTO_TEST_CASE(input__is_relative_locked__time_type_age_below_512_seconds__true)
{
    constexpr uint32_t sequence = 0x00400001;
    const input instance(point{}, {}, sequence);
    instance.metadata.median_time_past = 0;
    BC_REQUIRE(instance.is_relative_locked(0, 511));
}

BC_AUTO_TEST_CASE(input__is_relative_locked__time_type_age_at_512_seconds__false)
{
    constexpr uint32_t sequence = 0x00400001;
    const input instance(point{}, {}, sequence);
    instance.metadata.median_time_past = 0;
    BC_REQUIRE(!instance.is_relative_locked(0, 512));
}

// metadata (chain::prevout)
// ----------------------------------------------------------------------------
// The prevout defaults fail closed, so unpopulated metadata rejects.

BC_AUTO_TEST_CASE(input__metadata__default__fails_closed)
{
    const input instance{};
    BC_REQUIRE(instance.metadata.coinbase);
    BC_REQUIRE_EQUAL(instance.metadata.median_time_past, max_uint32);
    BC_REQUIRE_EQUAL(instance.metadata.prevout_height, max_uint32);
    BC_REQUIRE_EQUAL(instance.metadata.spender_height, max_uint32);
    BC_REQUIRE_EQUAL(instance.metadata.parent_tx, max_uint32);
    BC_REQUIRE_EQUAL(instance.metadata.point_link, max_uint32);
}

BC_AUTO_TEST_CASE(input__metadata__mutable_on_const_input__assignable)
{
    const input instance{};
    instance.metadata.coinbase = false;
    instance.metadata.prevout_height = 42;
    BC_REQUIRE(!instance.metadata.coinbase);
    BC_REQUIRE_EQUAL(instance.metadata.prevout_height, 42u);
}

// reserved_hash

BC_AUTO_TEST_CASE(input__reserved_hash__single_thirty_two_byte_element__expected)
{
    const auto reservation = base16_chunk("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20");
    const witness spender{ chunk_cptrs{ to_shared(reservation) } };
    const input instance{ point{}, script{}, spender, max_input_sequence };

    hash_cref out{ null_hash };
    BC_REQUIRE(instance.reserved_hash(out));
    BC_REQUIRE_EQUAL(out.get(), base16_array("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"));
}

BC_AUTO_TEST_CASE(input__reserved_hash__wrong_element_size__false)
{
    const witness spender{ chunk_cptrs{ to_shared(base16_chunk("0102")) } };
    const input instance{ point{}, script{}, spender, max_input_sequence };

    hash_cref out{ null_hash };
    BC_REQUIRE(!instance.reserved_hash(out));
}

BC_AUTO_TEST_CASE(input__reserved_hash__two_elements__false)
{
    const auto reservation = base16_chunk("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20");
    const witness spender{ chunk_cptrs{ to_shared(reservation), to_shared(reservation) } };
    const input instance{ point{}, script{}, spender, max_input_sequence };

    hash_cref out{ null_hash };
    BC_REQUIRE(!instance.reserved_hash(out));
}

BC_AUTO_TEST_CASE(input__reserved_hash__empty_witness__false)
{
    const input instance{ point{}, script{}, max_input_sequence };

    hash_cref out{ null_hash };
    BC_REQUIRE(!instance.reserved_hash(out));
}

BC_AUTO_TEST_CASE(input__signature_operations__no_prevout__expected)
{
    const script script(base16_chunk("02acad"), true);
    BC_REQUIRE(script.is_valid());

    const input instance{ {}, script, chain::max_input_sequence };
    BC_REQUIRE_EQUAL(instance.signature_operations(true, false), 2u);
    BC_REQUIRE_EQUAL(instance.signature_operations(false, false), 2u);
    BC_REQUIRE_EQUAL(instance.signature_operations(true, true), 8u);
    BC_REQUIRE_EQUAL(instance.signature_operations(false, true), 8u);
}

// is_relative_locktime_applied
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(input__is_relative_locktime_applied__zero_sequence__true)
{
    BC_REQUIRE(input::is_relative_locktime_applied(0));
}

// BIP68: bit 31 set carries no consensus meaning.
BC_AUTO_TEST_CASE(input__is_relative_locktime_applied__disable_bit__false)
{
    BC_REQUIRE(!input::is_relative_locktime_applied(0x80000000));
    BC_REQUIRE(!input::is_relative_locktime_applied(max_uint32));
}

BC_AUTO_TEST_CASE(input__is_relative_locktime_applied__maximum_applied__true)
{
    BC_REQUIRE(input::is_relative_locktime_applied(0x7fffffff));
}

// point_ptr/witness_ptr
// ----------------------------------------------------------------------------

BC_AUTO_TEST_CASE(input__point_ptr__default__matches_point)
{
    const input instance{};
    BC_REQUIRE(instance.point_ptr());
    BC_REQUIRE(*instance.point_ptr() == instance.point());
}

BC_AUTO_TEST_CASE(input__witness_ptr__witnessed__matches_witness)
{
    const chain::witness witness{ chunk_cptrs{ to_shared<data_chunk>({ 0x42_u8 }) } };
    const input instance{ point{}, script{}, witness, 42 };
    BC_REQUIRE(instance.witness_ptr());
    BC_REQUIRE(*instance.witness_ptr() == instance.witness());
}

BC_AUTO_TEST_CASE(input__construct__null_pointers__defaults)
{
    const input instance{ point::cptr{}, script::cptr{}, 7 };
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance.point() == point{});
    BC_REQUIRE(instance.script() == script{});
    BC_REQUIRE_EQUAL(instance.sequence(), 7u);
}

BC_AUTO_TEST_CASE(input__construct__pointers__expected)
{
    const auto value = to_shared<point>(point{ one_hash, 42 });
    const auto code = to_shared<script>(script{ operations{ operation{ opcode::dup } } });
    const input instance{ value, code, to_shared<chain::witness>(), 7 };
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance.point() == *value);
    BC_REQUIRE(instance.script() == *code);
}

// Constant reference optimizers.

BC_AUTO_TEST_CASE(input__cref_point_lesser__lesser_index__true)
{
    const auto hash = one_hash;
    const cref_point left{ hash, 1 };
    const cref_point right{ hash, 2 };
    BC_REQUIRE(left < right);
    BC_REQUIRE(!(right < left));
}

BC_AUTO_TEST_CASE(input__cref_point_lesser__same_index__orders_by_hash)
{
    const auto lesser = null_hash;
    const auto greater = one_hash;
    const cref_point left{ lesser, 1 };
    const cref_point right{ greater, 1 };
    BC_REQUIRE(left < right);
    BC_REQUIRE(!(right < left));
}

BC_AUTO_TEST_CASE(input__cref_point_inequality__different__true)
{
    const auto hash = one_hash;
    const cref_point left{ hash, 1 };
    const cref_point right{ hash, 2 };
    BC_REQUIRE(left != right);
    BC_REQUIRE(!(left != left));
}

// signature_operations
// ----------------------------------------------------------------------------

static input sigops_input(const script& input_script,
    const script& prevout_script, const chain::witness& spender={}) NOEXCEPT
{
    input instance{ point{ one_hash, 0 }, input_script, spender, 42 };
    instance.prevout = to_shared(output{ 0, prevout_script });
    return instance;
}

BC_AUTO_TEST_CASE(input__signature_operations__native_key_hash__one)
{
    const data_chunk program(short_hash_size, 0x01_u8);
    const script prevout{ script::to_pay_witness_pattern(0, program) };
    const auto instance = sigops_input(script{}, prevout);
    BC_REQUIRE_EQUAL(instance.signature_operations(true, true), one);
}

BC_AUTO_TEST_CASE(input__signature_operations__native_script_hash__witness_script_count)
{
    const operations ops{ operation{ opcode::checksig }, operation{ opcode::checksig } };
    const auto data = script{ ops }.to_data(false);
    const data_chunk program(hash_size, 0x01_u8);
    const script prevout{ script::to_pay_witness_pattern(0, program) };
    const chunk_cptrs stack{ to_shared<data_chunk>(data) };
    const auto instance = sigops_input(script{}, prevout, chain::witness{ stack });
    BC_REQUIRE_EQUAL(instance.signature_operations(true, true), two);
}

BC_AUTO_TEST_CASE(input__signature_operations__embedded_key_hash__one)
{
    const data_chunk program(short_hash_size, 0x01_u8);
    const auto data = script{ script::to_pay_witness_pattern(0, program) }.to_data(false);
    const script prevout{ script::to_pay_script_hash_pattern(bitcoin_short_hash(data)) };
    const script input_script{ operations{ operation{ data, false } } };
    const auto instance = sigops_input(input_script, prevout);
    BC_REQUIRE_EQUAL(instance.signature_operations(true, true), one);
}

BC_AUTO_TEST_CASE(input__signature_operations__embedded_script__weighted_count)
{
    const auto data = script{ operations{ operation{ opcode::checksig } } }.to_data(false);
    const script prevout{ script::to_pay_script_hash_pattern(bitcoin_short_hash(data)) };
    const script input_script{ operations{ operation{ data, false } } };
    const auto instance = sigops_input(input_script, prevout);
    BC_REQUIRE_EQUAL(instance.signature_operations(true, true), heavy_sigops_factor);
    BC_REQUIRE_EQUAL(instance.signature_operations(true, false), one);
}

BC_AUTO_TEST_CASE(input__signature_operations__taproot__zero)
{
    const data_chunk program(hash_size, 0x01_u8);
    const script prevout{ script::to_pay_witness_pattern(1, program) };
    const auto instance = sigops_input(script{}, prevout);
    BC_REQUIRE_EQUAL(instance.signature_operations(true, true), zero);
}

BC_AUTO_TEST_CASE(input__signature_operations__no_prevout__input_script_only)
{
    const input instance{ point{}, script{ operations{ operation{ opcode::checksig } } }, 42 };
    BC_REQUIRE_EQUAL(instance.signature_operations(true, true), heavy_sigops_factor);
}

// prevout_signature_operations
// ----------------------------------------------------------------------------
// bip54 counts accurately, so a preceding OP_1..OP_16 bounds a multisig.

BC_AUTO_TEST_CASE(input__prevout_signature_operations__no_prevout__input_script_only)
{
    const input instance{ point{}, script{ operations{ operation{ opcode::checksig }, operation{ opcode::checksig } } }, 42 };
    BC_REQUIRE_EQUAL(instance.prevout_signature_operations(), two);
}

BC_AUTO_TEST_CASE(input__prevout_signature_operations__bare_multisig__accurate)
{
    const script prevout{ operations{ operation{ opcode::push_positive_2 }, operation{ opcode::checkmultisig } } };
    const auto instance = sigops_input(script{}, prevout);
    BC_REQUIRE_EQUAL(instance.prevout_signature_operations(), two);
    BC_REQUIRE_EQUAL(prevout.signature_operations(false), 20u);
}

BC_AUTO_TEST_CASE(input__prevout_signature_operations__embedded_multisig__accurate)
{
    const auto data = script{ operations{ operation{ opcode::push_positive_3 }, operation{ opcode::checkmultisig } } }.to_data(false);
    const script prevout{ script::to_pay_script_hash_pattern(bitcoin_short_hash(data)) };
    const script input_script{ operations{ operation{ data, false } } };
    const auto instance = sigops_input(input_script, prevout);
    BC_REQUIRE_EQUAL(instance.prevout_signature_operations(), 3u);
}

BC_AUTO_TEST_CASE(input__prevout_signature_operations__script_hash_not_push_only__input_script_only)
{
    const script prevout{ script::to_pay_script_hash_pattern(short_hash{}) };
    const script input_script{ operations{ operation{ opcode::checksig } } };
    const auto instance = sigops_input(input_script, prevout);
    BC_REQUIRE_EQUAL(instance.prevout_signature_operations(), one);
}

BC_AUTO_TEST_CASE(input__prevout_signature_operations__script_hash_numeric_last_push__input_script_only)
{
    const script prevout{ script::to_pay_script_hash_pattern(short_hash{}) };
    const script input_script{ operations{ operation{ opcode::checksig }, operation{ opcode::push_positive_16 } } };
    const auto instance = sigops_input(input_script, prevout);
    BC_REQUIRE_EQUAL(instance.prevout_signature_operations(), one);
}

BC_AUTO_TEST_CASE(input__prevout_signature_operations__script_hash_empty_input_script__zero)
{
    const script prevout{ script::to_pay_script_hash_pattern(short_hash{}) };
    const auto instance = sigops_input(script{}, prevout);
    BC_REQUIRE_EQUAL(instance.prevout_signature_operations(), zero);
}

// A truncated push consumes the remaining script, so counting stops there.
BC_AUTO_TEST_CASE(input__prevout_signature_operations__truncated_push__counted_to_truncation)
{
    const script input_script(base16_chunk("acac4c05ac"), false);
    const script prevout{ operations{ operation{ opcode::checksig } } };
    const auto instance = sigops_input(input_script, prevout);
    BC_REQUIRE_EQUAL(instance.prevout_signature_operations(), 3u);
}

BC_AUTO_TEST_CASE(input__prevout_signature_operations__witness_script__not_counted)
{
    const auto data = script{ operations{ operation{ opcode::checksig }, operation{ opcode::checksig } } }.to_data(false);
    const data_chunk program(hash_size, 0x01_u8);
    const script prevout{ script::to_pay_witness_pattern(0, program) };
    const chunk_cptrs stack{ to_shared<data_chunk>(data) };
    const auto instance = sigops_input(script{}, prevout, chain::witness{ stack });
    BC_REQUIRE_EQUAL(instance.prevout_signature_operations(), zero);
}

BC_AUTO_TEST_SUITE_END()
