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

BOOST_AUTO_TEST_SUITE(transaction_tests)

namespace json = boost::json;
using namespace system::chain;

static const auto tx0_inputs = base16_chunk(
    "f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc4de65310fc"
    "010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294789eb17601"
    "39e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e2830b11547"
    "2fb31de67d16972867f13945012103e589480b2f746381fca01a9b12c517b7a4"
    "82a203c8b2742985da0ac72cc078f2ffffffff");

static const auto tx0_last_output = base16_chunk(
    "f0c9c467000000001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac");

static const auto tx1_data = base16_chunk(
    "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
    "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
    "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
    "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
    "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
    "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
    "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
    "00");

constexpr auto tx1_hash = base16_hash(
    "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");

static const auto tx2_data = base16_chunk(
    "010000000364e62ad837f29617bafeae951776e7a6b3019b2da37827921548d1"
    "a5efcf9e5c010000006b48304502204df0dc9b7f61fbb2e4c8b0e09f3426d625"
    "a0191e56c48c338df3214555180eaf022100f21ac1f632201154f3c69e1eadb5"
    "9901a34c40f1127e96adc31fac6ae6b11fb4012103893d5a06201d5cf61400e9"
    "6fa4a7514fc12ab45166ace618d68b8066c9c585f9ffffffff54b755c39207d4"
    "43fd96a8d12c94446a1c6f66e39c95e894c23418d7501f681b010000006b4830"
    "4502203267910f55f2297360198fff57a3631be850965344370f732950b47795"
    "737875022100f7da90b82d24e6e957264b17d3e5042bab8946ee5fc676d15d91"
    "5da450151d36012103893d5a06201d5cf61400e96fa4a7514fc12ab45166ace6"
    "18d68b8066c9c585f9ffffffff0aa14d394a1f0eaf0c4496537f8ab9246d9663"
    "e26acb5f308fccc734b748cc9c010000006c493046022100d64ace8ec2d5feeb"
    "3e868e82b894202db8cb683c414d806b343d02b7ac679de7022100a2dcd39940"
    "dd28d4e22cce417a0829c1b516c471a3d64d11f2c5d754108bdc0b012103893d"
    "5a06201d5cf61400e96fa4a7514fc12ab45166ace618d68b8066c9c585f9ffff"
    "ffff02c0e1e400000000001976a914884c09d7e1f6420976c40e040c30b2b622"
    "10c3d488ac20300500000000001976a914905f933de850988603aafeeb2fd7fc"
    "e61e66fe5d88ac00000000");

constexpr auto tx2_hash = base16_hash(
    "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");

static const auto tx3_data = base16_chunk(
    "010000000100000000000000000000000000000000000000000000000000000000000"
    "00000ffffffff23039992060481e1e157082800def50009dfdc102f42697446757279"
    "2f5345475749542f00000000015b382d4b000000001976a9148cf4f6175b2651dcdff"
    "0051970a917ea10189c2d88ac00000000");

static const auto tx4_data = base16_chunk(
    "0100000001b63634c25f23018c18cbb24ad503672fe7c5edc3fef193ec0f581dd"
    "b27d4e401490000006a47304402203b361bfb7e189c77379d6ffc90babe1b9658"
    "39d0b9b60966ade0c4b8de28385f022057432fe6f8f530c54d3513e41da6fb138"
    "fba2440c877cd2bfb0c94cdb5610fbe0121020d2d76d6db0d1c0bda17950f6468"
    "6e4bf42481337707e9a81bbe48458cfc8389ffffffff010000000000000000566"
    "a54e38193e381aee4b896e7958ce381afe4bb96e4babae381abe38288e381a3e3"
    "81a6e7ac91e9a194e38292e5a5aae3828fe3828ce3828be7bea9e58b99e38292e"
    "8a8ade38191e381a6e381afe38184e381aae3818400000000");

static const auto tx4_hash = base16_hash(
    "cb1e303db604f066225eb14d59d3f8d2231200817bc9d4610d2802586bd93f8a");

// Access protected validation methods.
class accessor
  : public transaction
{
public:
    // Use base class constructors.
    using transaction::transaction;
    using transaction::is_internal_double_spend;
    using transaction::is_oversized;
    using transaction::is_overweight;
    using transaction::is_signature_operations_limited;
    using transaction::is_empty;
    using transaction::is_absolute_locked;
    using transaction::is_null_non_coinbase;
    using transaction::is_invalid_coinbase_size;
    using transaction::is_missing_prevouts;
    using transaction::is_overspent;
    using transaction::is_immature;
    using transaction::is_relative_locked;
    using transaction::is_unconfirmed_spend;
    using transaction::is_confirmed_double_spend;
};

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid, serialized_size

BOOST_AUTO_TEST_CASE(transaction__constructor__default__invalid)
{
    const transaction instance;
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(transaction__constructor__move__expected)
{
    const transaction expected(tx1_data, true);
    transaction copy(tx1_data, true);
    const transaction instance(std::move(copy));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(transaction__constructor__copy__expected)
{
    const transaction expected(tx1_data, true);
    const transaction instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(transaction__constructor__move_parameters__expected)
{
    constexpr uint32_t version = 2345;
    constexpr uint32_t locktime = 4568656;

    const input input(tx0_inputs);
    BOOST_REQUIRE(input.is_valid());

    const output output(tx0_last_output);
    BOOST_REQUIRE(output.is_valid());

    const transaction instance(version, { input }, { output }, locktime);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(locktime, instance.locktime());
    BOOST_REQUIRE_EQUAL(instance.inputs(), 1u);
    BOOST_REQUIRE_EQUAL(instance.outputs(), 1u);
    BOOST_REQUIRE(*instance.inputs_ptr()->front() == input);
    BOOST_REQUIRE(*instance.outputs_ptr()->front() == output);
}

BOOST_AUTO_TEST_CASE(transaction__constructor__copy_parameters__expected)
{
    constexpr uint32_t version = 2345;
    constexpr uint32_t locktime = 4568656;

    const input input(tx0_inputs);
    BOOST_REQUIRE(input.is_valid());

    const output output(tx0_last_output);
    BOOST_REQUIRE(output.is_valid());

    const inputs inputs{ input };
    const outputs outputs{ output };
    const transaction instance(version, inputs, outputs, locktime);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(locktime, instance.locktime());
    BOOST_REQUIRE(inputs[0] == *(*instance.inputs_ptr())[0]);
    BOOST_REQUIRE(outputs[0] == *(*instance.outputs_ptr())[0]);
}

BOOST_AUTO_TEST_CASE(transaction__constructor__data_1__expected)
{
    const transaction tx(tx1_data, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.to_data(true), tx1_data);
    BOOST_REQUIRE_EQUAL(tx.hash(false), tx1_hash);
    BOOST_REQUIRE_EQUAL(tx.serialized_size(true), tx1_data.size());
}

BOOST_AUTO_TEST_CASE(transaction__constructor__data_2__expected)
{
    const transaction tx(tx2_data, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.to_data(true), tx2_data);
    BOOST_REQUIRE_EQUAL(tx.hash(false), tx2_hash);
    BOOST_REQUIRE_EQUAL(tx.serialized_size(true), tx2_data.size());
}

BOOST_AUTO_TEST_CASE(transaction__constructor__fast_1__success)
{
    stream::in::fast stream(tx1_data);
    const transaction tx(stream, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.to_data(true), tx1_data);
    BOOST_REQUIRE_EQUAL(tx.hash(false), tx1_hash);
    BOOST_REQUIRE_EQUAL(tx.serialized_size(true), tx1_data.size());
}

BOOST_AUTO_TEST_CASE(transaction__constructor___fast_2__success)
{
    stream::in::fast stream(tx2_data);
    const transaction tx(stream, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.hash(false), tx2_hash);
    BOOST_REQUIRE_EQUAL(tx.to_data(true), tx2_data);
    BOOST_REQUIRE_EQUAL(tx.serialized_size(true), tx2_data.size());
}

BOOST_AUTO_TEST_CASE(transaction__constructor__stream_1__success)
{
    stream::in::copy stream(tx1_data);
    const transaction tx(stream, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.to_data(true), tx1_data);
    BOOST_REQUIRE_EQUAL(tx.hash(false), tx1_hash);
    BOOST_REQUIRE_EQUAL(tx.serialized_size(true), tx1_data.size());
}

BOOST_AUTO_TEST_CASE(transaction__constructor__stream_2__success)
{
    stream::in::copy stream(tx2_data);
    const transaction tx(stream, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.hash(false), tx2_hash);
    BOOST_REQUIRE_EQUAL(tx.to_data(true), tx2_data);
    BOOST_REQUIRE_EQUAL(tx.serialized_size(true), tx2_data.size());
}

BOOST_AUTO_TEST_CASE(transaction__constructor__reader_1__success)
{
    read::bytes::copy source(tx1_data);
    const transaction tx(source, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.hash(false), tx1_hash);
    BOOST_REQUIRE_EQUAL(tx.to_data(true), tx1_data);
    BOOST_REQUIRE_EQUAL(tx.serialized_size(true), tx1_data.size());
}

BOOST_AUTO_TEST_CASE(transaction__constructor__reader_2__success)
{
    read::bytes::copy source(tx2_data);
    const transaction tx(source, true);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.hash(false), tx2_hash);
    BOOST_REQUIRE_EQUAL(tx.to_data(true), tx2_data);
    BOOST_REQUIRE_EQUAL(tx.serialized_size(true), tx2_data.size());
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(transaction__assign__move__expected)
{
    const transaction alpha(tx2_data, true);
    transaction gamma(tx2_data, true);
    const transaction beta = std::move(gamma);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(transaction__assign__copy__expected)
{
    const transaction alpha(tx2_data, true);
    const transaction beta = alpha;
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(transaction__equality__same__true)
{
    const transaction alpha(tx2_data, true);
    const transaction beta(tx2_data, true);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(transaction__equality__different_false)
{
    const transaction alpha;
    const transaction beta(tx2_data, true);
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(transaction__inequality__same__false)
{
    const transaction alpha(tx2_data, true);
    const transaction beta(tx2_data, true);
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(transaction__inequality__different__true)
{
    const transaction alpha;
    const transaction beta(tx2_data, true);
    BOOST_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(transaction__to_data__data__expected)
{
    const transaction tx(tx1_data, true);
    const auto size = tx.to_data(true).size();
    BOOST_REQUIRE_EQUAL(size, tx.serialized_size(true));
}

BOOST_AUTO_TEST_CASE(transaction__to_data__stream__expected)
{
    const transaction tx(tx1_data, true);
    BOOST_REQUIRE(tx.is_valid());

    // Write transaction to stream.
    std::stringstream iostream;
    tx.to_data(iostream, true);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const transaction copy(iostream, true);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == tx);
}

BOOST_AUTO_TEST_CASE(transaction__to_data__writer__expected)
{
    const transaction tx(tx1_data, true);
    BOOST_REQUIRE(tx.is_valid());

    // Write transaction to stream via writer.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    tx.to_data(out, true);
    BOOST_REQUIRE(out);

    // Verify stream contents.
    const transaction copy(iostream, true);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == tx);
}

// properties
// ----------------------------------------------------------------------------

// weight

BOOST_AUTO_TEST_CASE(transaction__fee__empty__zero)
{
    const transaction instance
    {
        0,
        inputs{},
        {},
        0
    };

    // floored_subtract(0, 0)
    BOOST_REQUIRE_EQUAL(instance.fee(), 0u);
}

BOOST_AUTO_TEST_CASE(transaction__fee__default_input__zero)
{
    const transaction instance
    {
        0,
        inputs{ {} },
        {},
        0
    };

    BOOST_REQUIRE_EQUAL(instance.fee(), zero);
}

BOOST_AUTO_TEST_CASE(transaction__fee__default_output__zero)
{
    const transaction instance
    {
        0,
        inputs{},
        { {} },
        0
    };

    BOOST_REQUIRE_EQUAL(instance.fee(), 0u);
}

BOOST_AUTO_TEST_CASE(transaction__fee__nonempty__prevouts_minus_outputs)
{
    constexpr uint64_t value0 = 123;
    constexpr uint64_t value1 = 321;
    constexpr uint64_t spend0 = 11;
    constexpr uint64_t spend1 = 11;
    constexpr uint64_t spend2 = 22;

    input input0;
    input input1;
    input0.prevout = to_shared<output>(value0, script{});
    input1.prevout = to_shared<output>(value1, script{});

    const transaction instance
    {
        0,
        { input0, input1 },
        {
            { spend0, script{} },
            { spend1, script{} },
            { spend2, script{} }
        },
        0
    };

    // floored_subtract(values, spend)
    BOOST_REQUIRE_EQUAL(instance.fee(), value0 + value1 - spend0 - spend1 - spend2);
}

BOOST_AUTO_TEST_CASE(transaction__spend__empty_outputs__zero)
{
    transaction instance;
    BOOST_REQUIRE_EQUAL(instance.spend(), 0u);
}

BOOST_AUTO_TEST_CASE(transaction__spend__two_outputs__sum)
{
    constexpr uint64_t spend0{ 123 };
    constexpr uint64_t spend1{ 321 };
    const transaction instance
    {
        0,
        {},
        {
            { spend0, script{} },
            { spend1, script{} }
        },
        0
    };


    // ceilinged_add(spend0, spend1)
    BOOST_REQUIRE_EQUAL(instance.spend(), spend0 + spend1);
}

BOOST_AUTO_TEST_CASE(transaction__value__no_inputs__zero)
{
    const transaction instance;
    BOOST_REQUIRE_EQUAL(instance.value(), 0u);
}

BOOST_AUTO_TEST_CASE(transaction__value__default_input2__zero)
{
    transaction instance
    {
        0,
        inputs{ {}, {} },
        {},
        0
    };

    BOOST_REQUIRE_EQUAL(instance.value(), zero);
}

BOOST_AUTO_TEST_CASE(transaction__value__two_prevouts__sum)
{
    constexpr uint64_t value0{ 123 };
    constexpr uint64_t value1{ 321 };

    const input input0;
    const input input1;
    input0.prevout = to_shared<output>(value0, script{});
    input1.prevout = to_shared<output>(value1, script{});

    transaction instance
    {
        0,
        { input0, input1 },
        {},
        0
    };

    // ceilinged_add(value0, value1)
    BOOST_REQUIRE_EQUAL(instance.value(), value0 + value1);
}

// This is a garbage script that collides with the former opcode::raw_data sentinel.
BOOST_AUTO_TEST_CASE(transaction__hash__block320670__success)
{
    const transaction instance(tx4_data, true);
    BOOST_REQUIRE_EQUAL(instance.hash(false), tx4_hash);
    BOOST_REQUIRE_EQUAL(instance.to_data(true), tx4_data);
}

BOOST_AUTO_TEST_CASE(transaction__set_hash__get_hash__expected)
{
    const transaction instance(tx1_data, true);

    BOOST_REQUIRE_EQUAL(instance.hash(false), tx1_hash);
    BOOST_REQUIRE_EQUAL(instance.hash(true), tx1_hash);
    BOOST_REQUIRE_EQUAL(instance.get_hash(false), tx1_hash);
    BOOST_REQUIRE_EQUAL(instance.get_hash(true), tx1_hash);

    instance.set_nominal_hash(test::move_copy(tx2_hash));
    BOOST_REQUIRE_EQUAL(instance.hash(false), tx2_hash);
    BOOST_REQUIRE_EQUAL(instance.hash(true), tx2_hash);
    BOOST_REQUIRE_EQUAL(instance.get_hash(false), tx2_hash);
    BOOST_REQUIRE_EQUAL(instance.get_hash(true), tx1_hash);

    instance.set_witness_hash(test::move_copy(tx4_hash));
    BOOST_REQUIRE_EQUAL(instance.hash(false), tx2_hash);
    BOOST_REQUIRE_EQUAL(instance.hash(true), tx2_hash);
    BOOST_REQUIRE_EQUAL(instance.get_hash(false), tx2_hash);
    BOOST_REQUIRE_EQUAL(instance.get_hash(true), tx4_hash);
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__empty__false)
{
    transaction instance;
    BOOST_REQUIRE(!instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__default_point__true)
{
    const transaction instance
    {
        0,
        {
            { {}, script{}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__default_input__true)
{
    const transaction instance
    {
        0,
        inputs{ {} },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__null_input__true)
{
    const transaction instance
    {
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__non_null_input__false)
{
    const transaction instance
    {
        0,
        {
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__first_null_input__false)
{
    const transaction instance
    {
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 },
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__null_inputs__false)
{
    const transaction instance
    {
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 },
            { { hash_digest{}, point::null_index }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_coinbase());
}

// is_segregated

// methods
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(transaction__is_dusty__no_outputs_zero__false)
{
    const transaction instance;
    BOOST_REQUIRE(!instance.is_valid());
    BOOST_REQUIRE(!instance.is_dusty(0));
}

BOOST_AUTO_TEST_CASE(transaction__is_dusty__two_outputs_limit_above_both__true)
{
    const transaction instance(tx1_data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.is_dusty(1740950001));
}

BOOST_AUTO_TEST_CASE(transaction__is_dusty__two_outputs_limit_below_both__false)
{
    const transaction instance(tx1_data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(!instance.is_dusty(257999999));
}

BOOST_AUTO_TEST_CASE(transaction__is_dusty__two_outputs_limit_at_upper__true)
{
    const transaction instance(tx1_data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.is_dusty(1740950000));
}

BOOST_AUTO_TEST_CASE(transaction__is_dusty__two_outputs_limit_at_lower__false)
{
    const transaction instance(tx1_data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(!instance.is_dusty(258000000));
}

BOOST_AUTO_TEST_CASE(transaction__is_dusty__two_outputs_limit_between_both__true)
{
    const transaction instance(tx1_data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.is_dusty(258000001));
}

// TODO: tests with initialized data
BOOST_AUTO_TEST_CASE(transaction__signature_operations__empty_input_output__zero)
{
    const transaction instance{ 0, inputs{}, {}, 0 };
    BOOST_REQUIRE_EQUAL(instance.signature_operations(false, false), 0u);
}

// points
// outputs_hash
// points_hash
// sequences_hash

// guards
// ----------------------------------------------------------------------------

// guard 1/2

// validation (public)
// ----------------------------------------------------------------------------

// check
// accept
// connect

// validation (protected)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(transaction__is_internal_double_spend__empty__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(transaction__is_internal_double_spend__unique_point_hashes__false)
{
    const accessor instance
    {
        0,
        {
            {},
            { { tx1_hash, 42 }, {}, 0 },
            { { tx2_hash, 42 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(transaction__is_internal_double_spend__unique_points__false)
{
    const accessor instance
    {
        0,
        {
            {},
            { { tx1_hash, 42 }, {}, 0 },
            { { tx1_hash, 43 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(transaction__is_internal_double_spend__nonunique_points__true)
{
    const accessor instance
    {
        0,
        {
            { { tx1_hash, 42 }, {}, 0 },
            { { tx1_hash, 42 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_internal_double_spend());
}

// is_oversized
// is_overweight
// is_signature_operations_limited

BOOST_AUTO_TEST_CASE(transaction__is_empty__default__true)
{
    const accessor instance;
    BOOST_REQUIRE(instance.is_empty());
}

BOOST_AUTO_TEST_CASE(transaction__is_empty__one_input__true)
{
    const accessor instance
    {
        0,
        inputs{ {} },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_empty());
}

BOOST_AUTO_TEST_CASE(transaction__is_empty__one_output__true)
{
    const accessor instance
    {
        0,
        inputs{},
        { {} },
        0
    };

    BOOST_REQUIRE(instance.is_empty());
}

BOOST_AUTO_TEST_CASE(transaction__is_empty__one_input_one_output__false)
{
    const accessor instance
    {
        0,
        inputs{ {} },
        { {} },
        0
    };

    BOOST_REQUIRE(!instance.is_empty());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__non_null_input__false)
{
    const accessor instance
    {
        0,
        {
            { { tx1_hash, point::null_index }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__non_null_inputs__false)
{
    const accessor instance
    {
        0,
        {
            { { hash_digest{}, 42 }, {}, 0 },
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__first_null_input__true)
{
    const accessor instance
    {
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 },
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__second_null_input__true)
{
    const accessor instance
    {
        0,
        {
            { { hash_digest{}, 42 }, {}, 0 },
            { { hash_digest{}, point::null_index }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__null_inputs__true)
{
    const accessor instance
    {
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 },
            { { hash_digest{}, point::null_index }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_below_min__true)
{
    const accessor instance
    {
        0,
        {
            { {}, { data_chunk(sub1(min_coinbase_size), 0x00), false }, 0 },
        },
        {},
        0
    };

    BOOST_REQUIRE_LT(instance.inputs_ptr()->back()->script().serialized_size(false), min_coinbase_size);
    BOOST_REQUIRE(instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_above_max__true)
{
    const accessor instance
    {
        0,
        {
            { { hash_digest{}, point::null_index }, { data_chunk(add1(max_coinbase_size), 0x00), false }, 0 },
        },
        {},
        0
    };

    BOOST_REQUIRE_GT(instance.inputs_ptr()->back()->script().serialized_size(false), max_coinbase_size);
    BOOST_REQUIRE(instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_max__false)
{
    const accessor instance
    {
        0,
        {
            { {}, { data_chunk(max_coinbase_size, 0x00), false }, 0 },
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_min__false)
{
    const accessor instance
    {
        0,
        {
            { {}, { data_chunk(min_coinbase_size, 0x00), false }, 0 },
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_below_max__false)
{
    const accessor instance
    {
        0,
        {
            { {}, { data_chunk(sub1(max_coinbase_size), 0x00), false }, 0 },
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_above_min__false)
{
    const accessor instance
    {
        0,
        {
            { {}, { data_chunk(add1(min_coinbase_size), 0x00), false }, 0 },
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_absolute_locked__locktime_zero__false)
{
    constexpr bool bip113 = false;
    constexpr size_t height = 100;
    constexpr uint32_t time = 100;
    constexpr uint32_t past = 0;
    constexpr uint32_t locktime = 0;

    const accessor instance
    {
        0,
        inputs{},
        {},
        locktime
    };

    BOOST_REQUIRE(!instance.is_absolute_locked(height, time, past, bip113));
}

BOOST_AUTO_TEST_CASE(transaction__is_absolute_locked__locktime_less_block_time_greater_threshold__false)
{
    constexpr bool bip113 = false;
    constexpr size_t height = locktime_threshold + 100;
    constexpr uint32_t time = 100;
    constexpr uint32_t past = 0;
    constexpr uint32_t locktime = locktime_threshold + 50;

    const accessor instance
    {
        0,
        inputs{},
        {},
        locktime
    };

    BOOST_REQUIRE(!instance.is_absolute_locked(height, time, past, bip113));
}

BOOST_AUTO_TEST_CASE(transaction__is_absolute_locked__locktime_less_block_height_less_threshold_false)
{
    constexpr bool bip113 = false;
    constexpr size_t height = 100;
    constexpr uint32_t time = 100;
    constexpr uint32_t past = 0;
    constexpr uint32_t locktime = 50;

    const accessor instance
    {
        0,
        inputs{},
        {},
        locktime
    };

    BOOST_REQUIRE(!instance.is_absolute_locked(height, time, past, bip113));
}

BOOST_AUTO_TEST_CASE(transaction__is_absolute_locked__locktime_input_not_final__true)
{
    constexpr bool bip113 = false;
    constexpr size_t height = 100;
    constexpr uint32_t time = 100;
    constexpr uint32_t past = 0;
    constexpr uint32_t locktime = 101;
    constexpr uint32_t sequence = 1;

    const accessor instance
    {
        0,
        { { {}, script{}, sequence } },
        {},
        locktime,
    };

    BOOST_REQUIRE(instance.is_absolute_locked(height, time, past, bip113));
}

BOOST_AUTO_TEST_CASE(transaction__is_absolute_locked__locktime_inputs_final__false)
{
    constexpr bool bip113 = false;
    constexpr size_t height = 100;
    constexpr uint32_t time = 100;
    constexpr uint32_t past = 0;
    constexpr uint32_t locktime = 101;
    constexpr uint32_t sequence = max_input_sequence;

    const accessor instance
    {
        0,
        {
            { {}, script{}, sequence }
        },
        {},
        locktime
    };

    BOOST_REQUIRE(!instance.is_absolute_locked(height, time, past, bip113));
}

BOOST_AUTO_TEST_CASE(transaction__is_missing_prevouts__empty_inputs__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_missing_prevouts());
}

BOOST_AUTO_TEST_CASE(transaction__is_missing_prevouts__default_inputs__true)
{
    const accessor instance
    {
        0,
        inputs{ {}, {} },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_missing_prevouts());
}

BOOST_AUTO_TEST_CASE(transaction__is_missing_prevouts__valid_prevout__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.prevout = to_shared<output>(42_u64, script{});
    const accessor instance
    {
        0,
        { input },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_missing_prevouts());
}

BOOST_AUTO_TEST_CASE(transaction__is_overspent__output_does_not_exceed_input__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_overspent());
}

BOOST_AUTO_TEST_CASE(transaction__is_overspent__output_exceeds_input__true)
{
    const accessor instance
    {
        0,
        {},
        { { 1, script{} }, { 0, script{} } },
        0
    };

    BOOST_REQUIRE(instance.is_overspent());
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__no_inputs__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_immature(coinbase_maturity));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__mature_genesis__true)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.metadata.prevout_height = 0;
    input.metadata.coinbase = true;
    const accessor instance
    {
        0,
        { input },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_immature(coinbase_maturity));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__premature_coinbase__true)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.metadata.prevout_height = 1;
    input.metadata.coinbase = true;
    const accessor instance
    {
        0,
        { input },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_immature(coinbase_maturity));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__premature_non_coinbase__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.metadata.prevout_height = 1;
    input.metadata.coinbase = false;
    const accessor instance
    {
        0,
        { input },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_immature(coinbase_maturity));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__mature_coinbase__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.metadata.prevout_height = 1;
    input.metadata.coinbase = true;
    const accessor instance
    {
        0,
        { input },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_immature(add1(coinbase_maturity)));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__mature_non_coinbase__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.metadata.prevout_height = 1;
    input.metadata.coinbase = false;
    const accessor instance
    {
        0,
        { input },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_immature(add1(coinbase_maturity)));
}

BOOST_AUTO_TEST_CASE(transaction__is_relative_locked__version1_empty__false)
{
    constexpr uint32_t version = 1;
    const accessor instance
    {
        version,
        {
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_relative_locked(0, 0));
}

BOOST_AUTO_TEST_CASE(transaction__is_relative_locked__version_2_empty__false)
{
    constexpr uint32_t version = 2;
    const accessor instance
    {
        version,
        {
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_relative_locked(0, 0));
}

BOOST_AUTO_TEST_CASE(transaction__is_relative_locked__version1_one_of_two_locked_locked__false)
{
    constexpr uint32_t version = 1;
    const accessor instance
    {
        version,
        {
            { {}, script{}, 1 },
            { {}, script{}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_relative_locked(0, 0));
}

BOOST_AUTO_TEST_CASE(transaction__is_relative_locked__version_4_one_of_two_locked__true)
{
    constexpr uint32_t version = 4;
    const accessor instance
    {
        version,
        {
            { {}, script{}, 1 },
            { {}, script{}, 0 }
        },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_relative_locked(0, 0));
}

// is_unconfirmed_spend

BOOST_AUTO_TEST_CASE(transaction__is_confirmed_double_spend__empty_inputs__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_confirmed_double_spend(42));
}

BOOST_AUTO_TEST_CASE(transaction__is_confirmed_double_spend__default_inputs__false)
{
    // input.metadata.spender_height defaults to max (spent above height).
    const accessor instance
    {
        0,
        inputs{ {}, {} },
        {},
        0
    };

    BOOST_REQUIRE(!instance.is_confirmed_double_spend(42));
}

BOOST_AUTO_TEST_CASE(transaction__is_confirmed_double_spend__spent_input__true)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.metadata.spender_height = 41;
    const accessor instance
    {
        0,
        { input },
        {},
        0
    };

    BOOST_REQUIRE(instance.is_confirmed_double_spend(42));
}

// check_signature

BOOST_AUTO_TEST_CASE(transaction__check_signature__single__uses_one_hash)
{
    // input 315ac7d4c26d69668129cc352851d9389b4a6868f1509c6c8b66bead11e2619f:1
    const auto tx_data = base16_chunk("0100000002dc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169000000006a47304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27032102100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2feffffffffdc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169010000006b4830450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03210275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cbffffffff0140899500000000001976a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac00000000");
    const transaction parent_tx(tx_data, true);
    BOOST_REQUIRE(parent_tx.is_valid());

    const auto distinguished = base16_chunk("30450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df");
    const auto pubkey = base16_chunk("0275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cb");
    const auto script_data = base16_chunk("76a91433cef61749d11ba2adf091a5e045678177fe3a6d88ac");
    const script subscript(script_data, false);
    BOOST_REQUIRE(parent_tx.is_valid());

    constexpr auto index = 1u;
    constexpr auto value = 0u;
    constexpr auto bip66 = true;
    constexpr auto flags = flags::bip66_rule;
    ec_signature signature;
    BOOST_REQUIRE(ecdsa::decode_signature(signature, distinguished, bip66));
    BOOST_REQUIRE(parent_tx.check_signature(signature, pubkey, subscript, index, value, coverage::hash_single, script_version::unversioned, flags));
}

BOOST_AUTO_TEST_CASE(transaction__check_signature__normal__success)
{
    // input 315ac7d4c26d69668129cc352851d9389b4a6868f1509c6c8b66bead11e2619f:0
    const auto tx_data = base16_chunk("0100000002dc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169000000006a47304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27032102100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2feffffffffdc38e9359bd7da3b58386204e186d9408685f427f5e513666db735aa8a6b2169010000006b4830450220087ede38729e6d35e4f515505018e659222031273b7366920f393ee3ab17bc1e022100ca43164b757d1a6d1235f13200d4b5f76dd8fda4ec9fc28546b2df5b1211e8df03210275983913e60093b767e85597ca9397fb2f418e57f998d6afbbc536116085b1cbffffffff0140899500000000001976a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac00000000");
    const transaction parent_tx(tx_data, true);
    BOOST_REQUIRE(parent_tx.is_valid());

    const auto distinguished = base16_chunk("304402205d8feeb312478e468d0b514e63e113958d7214fa572acd87079a7f0cc026fc5c02200fa76ea05bf243af6d0f9177f241caf606d01fcfd5e62d6befbca24e569e5c27");
    const auto pubkey = base16_chunk("02100a1a9ca2c18932d6577c58f225580184d0e08226d41959874ac963e3c1b2fe");
    const auto script_data = base16_chunk("76a914fcc9b36d38cf55d7d5b4ee4dddb6b2c17612f48c88ac");
    const script subscript(script_data, false);
    BOOST_REQUIRE(parent_tx.is_valid());

    constexpr auto index = 0u;
    constexpr auto value = 0u;
    constexpr auto bip66 = true;
    constexpr auto flags = flags::bip66_rule;
    ec_signature signature;
    BOOST_REQUIRE(ecdsa::decode_signature(signature, distinguished, bip66));
    BOOST_REQUIRE(parent_tx.check_signature(signature, pubkey, subscript, index, value, coverage::hash_single, script_version::unversioned, flags));
}

// create_endorsement

BOOST_AUTO_TEST_CASE(transaction__create_endorsement__single_input_single_output__expected)
{
    const auto tx_data = base16_chunk("0100000001b3807042c92f449bbf79b33ca59d7dfec7f4cc71096704a9c526dddf496ee0970100000000ffffffff01905f0100000000001976a91418c0bd8d1818f1bf99cb1df2269c645318ef7b7388ac00000000");
    const transaction test_tx(tx_data, true);
    BOOST_REQUIRE(test_tx.is_valid());

    const script prevout_script(std::string{ "dup hash160 [88350574280395ad2c3e2ee20e322073d94e5e40] equalverify checksig" });
    BOOST_REQUIRE(prevout_script.is_valid());

    const ec_secret secret = base16_hash("ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333");
    constexpr auto index = 0u;
    constexpr auto value = 0u;
    constexpr auto flags = flags::no_rules;
    endorsement out;
    BOOST_REQUIRE(test_tx.create_endorsement(out, secret, prevout_script, index, value, coverage::hash_all, script_version::unversioned, flags));

    const auto expected = base16_chunk("3045022100e428d3cc67a724cb6cfe8634aa299e58f189d9c46c02641e936c40cc16c7e8ed0220083949910fe999c21734a1f33e42fca15fb463ea2e08f0a1bccd952aacaadbb801");
    BOOST_REQUIRE_EQUAL(out, expected);
}

BOOST_AUTO_TEST_CASE(transaction__create_endorsement__single_input_no_output__expected)
{
    const auto tx_data = base16_chunk("0100000001b3807042c92f449bbf79b33ca59d7dfec7f4cc71096704a9c526dddf496ee0970000000000ffffffff0000000000");
    const transaction test_tx(tx_data, true);
    BOOST_REQUIRE(test_tx.is_valid());

    const script prevout_script(std::string{ "dup hash160 [88350574280395ad2c3e2ee20e322073d94e5e40] equalverify checksig" });
    BOOST_REQUIRE(prevout_script.is_valid());

    const ec_secret secret = base16_hash("ce8f4b713ffdd2658900845251890f30371856be201cd1f5b3d970f793634333");
    constexpr auto index = 0u;
    constexpr auto value = 0u;
    constexpr auto flags = flags::no_rules;
    endorsement out;
    BOOST_REQUIRE(test_tx.create_endorsement(out, secret, prevout_script, index, value, coverage::hash_all, script_version::unversioned, flags));

    const auto expected = base16_chunk("3045022100ba57820be5f0b93a0d5b880fbf2a86f819d959ecc24dc31b6b2d4f6ed286f253022071ccd021d540868ee10ca7634f4d270dfac7aea0d5912cf2b104111ac9bc756b01");
    BOOST_REQUIRE_EQUAL(out, expected);
}

// signature_hash

BOOST_AUTO_TEST_CASE(transaction__signature_hash__all__expected)
{
    const auto tx_data = base16_chunk("0100000001b3807042c92f449bbf79b33ca59d7dfec7f4cc71096704a9c526dddf496ee0970000000000ffffffff0000000000");
    transaction test_tx(tx_data, true);
    BOOST_REQUIRE(test_tx.is_valid());

    const script prevout_script(std::string{ "dup hash160 [88350574280395ad2c3e2ee20e322073d94e5e40] equalverify checksig" });
    BOOST_REQUIRE(prevout_script.is_valid());

    constexpr auto value = 0u;
    constexpr auto flags = flags::no_rules;
    const auto& input = test_tx.inputs_ptr()->begin();

    hash_digest sighash{};
    const hash_cptr tapleaf{};
    BOOST_REQUIRE(test_tx.signature_hash(sighash, input, prevout_script, value, tapleaf, script_version::unversioned, coverage::hash_all, flags));

    const auto expected = base16_array("f89572635651b2e4f89778350616989183c98d1a721c911324bf9f17a0cf5bf0");
    BOOST_REQUIRE_EQUAL(sighash, expected);
}

// signature_hash (unversioned coverage matrix)
// ----------------------------------------------------------------------------
// Expected digests computed by an independent implementation of the Satoshi
// signature hash algorithm (not derived from this library).

constexpr char sighash_subscript[] = "76a91488350574280395ad2c3e2ee20e322073d94e5e4088ac";

static transaction unversioned_sighash_transaction() NOEXCEPT
{
    const inputs ins
    {
        input{ point{ base16_hash("1111111111111111111111111111111111111111111111111111111111111111"), 0 }, script{}, 0xffffffff },
        input{ point{ base16_hash("2222222222222222222222222222222222222222222222222222222222222222"), 1 }, script{}, 0xfffffffe }
    };

    const outputs outs
    {
        output{ 1000, script(base16_chunk("51"), false) },
        output{ 2000, script(base16_chunk("52"), false) }
    };

    return transaction{ 1, ins, outs, 0 };
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__unversioned_all__expected)
{
    const auto tx = unversioned_sighash_transaction();
    const script subscript(base16_chunk(sighash_subscript), false);
    const auto input = std::next(tx.inputs_ptr()->begin());
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 0, tapleaf, script_version::unversioned, coverage::hash_all, flags::no_rules));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("6e8be8581d56213cea29b15b05763e925246c09e35ae9942fbf1bae8730f849a"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__unversioned_none__expected)
{
    const auto tx = unversioned_sighash_transaction();
    const script subscript(base16_chunk(sighash_subscript), false);
    const auto input = std::next(tx.inputs_ptr()->begin());
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 0, tapleaf, script_version::unversioned, coverage::hash_none, flags::no_rules));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("07af6b16850616dbc378c34ef2b8cdfcc7d63e2a7dd85c84bab41e8dedca8d19"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__unversioned_single__expected)
{
    const auto tx = unversioned_sighash_transaction();
    const script subscript(base16_chunk(sighash_subscript), false);
    const auto input = std::next(tx.inputs_ptr()->begin());
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 0, tapleaf, script_version::unversioned, coverage::hash_single, flags::no_rules));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("d2bcdf411cae692cff594654c7069d6bdc48378ddaa8e5e01d5a6f92a1843fb1"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__unversioned_all_anyone_can_pay__expected)
{
    const auto tx = unversioned_sighash_transaction();
    const script subscript(base16_chunk(sighash_subscript), false);
    const auto input = std::next(tx.inputs_ptr()->begin());
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 0, tapleaf, script_version::unversioned, coverage::all_anyone_can_pay, flags::no_rules));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("ebda8aa0db2288a47d097108211e555aa7319ad53e573a604ad7c04f99143b66"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__unversioned_none_anyone_can_pay__expected)
{
    const auto tx = unversioned_sighash_transaction();
    const script subscript(base16_chunk(sighash_subscript), false);
    const auto input = std::next(tx.inputs_ptr()->begin());
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 0, tapleaf, script_version::unversioned, coverage::none_anyone_can_pay, flags::no_rules));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("8cba56695f43d3185bbd74742a6ae82d0415bb0bb5a566d6d9ada1441ee9cb4f"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__unversioned_single_anyone_can_pay__expected)
{
    const auto tx = unversioned_sighash_transaction();
    const script subscript(base16_chunk(sighash_subscript), false);
    const auto input = std::next(tx.inputs_ptr()->begin());
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 0, tapleaf, script_version::unversioned, coverage::single_anyone_can_pay, flags::no_rules));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("ae19e548c5bab135dafb27cf22a0117caf8ba3df728198527c469dd3fbac98b1"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__unversioned_all_first_input__expected)
{
    const auto tx = unversioned_sighash_transaction();
    const script subscript(base16_chunk(sighash_subscript), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 0, tapleaf, script_version::unversioned, coverage::hash_all, flags::no_rules));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("f44ea6f29147d263dcff34dc0d1440d869039e1d159079e22c2b031dfda19521"));
}

// CONSENSUS: the Satoshi hash_single bug, input beyond outputs signs one_hash.
BOOST_AUTO_TEST_CASE(transaction__signature_hash__unversioned_single_output_overflow__one_hash)
{
    const inputs ins
    {
        input{ point{ base16_hash("1111111111111111111111111111111111111111111111111111111111111111"), 0 }, script{}, 0xffffffff },
        input{ point{ base16_hash("2222222222222222222222222222222222222222222222222222222222222222"), 1 }, script{}, 0xffffffff }
    };

    const outputs outs{ output{ 1000, script(base16_chunk("51"), false) } };
    const transaction tx{ 1, ins, outs, 0 };
    const script subscript(base16_chunk(sighash_subscript), false);
    const auto input = std::next(tx.inputs_ptr()->begin());
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 0, tapleaf, script_version::unversioned, coverage::hash_single, flags::no_rules));
    BOOST_REQUIRE_EQUAL(sighash, one_hash);
}

// signature_hash (version 0, bip143 vectors)
// ----------------------------------------------------------------------------
// Transactions, scripts, values and digests from the bip143 specification.

constexpr auto bip143_flags = flags::bip141_rule | flags::bip143_rule;

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_p2wpkh__expected)
{
    const transaction tx(base16_chunk("0100000002fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f0000000000eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a0100000000ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac11000000"), true);
    BOOST_REQUIRE(tx.is_valid());

    const script subscript(base16_chunk("76a9141d0f172a0ecb48aee1be1f2687d2963ae33f71a188ac"), false);
    const auto input = std::next(tx.inputs_ptr()->begin());
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 600000000, tapleaf, script_version::segwit, coverage::hash_all, bip143_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("c37af31116d1b27caf68aae9e3ac82f1477929014d5b917657d0eb49478cb670"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_p2sh_p2wpkh__expected)
{
    const transaction tx(base16_chunk("0100000001db6b1b20aa0fd7b23880be2ecbd4a98130974cf4748fb66092ac4d3ceb1a54770100000000feffffff02b8b4eb0b000000001976a914a457b684d7f0d539a46a45bbc043f35b59d0d96388ac0008af2f000000001976a914fd270b1ee6abcaea97fea7ad0402e8bd8ad6d77c88ac92040000"), true);
    BOOST_REQUIRE(tx.is_valid());

    const script subscript(base16_chunk("76a91479091972186c449eb1ded22b78e40d009bdf008988ac"), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, 1000000000, tapleaf, script_version::segwit, coverage::hash_all, bip143_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("64f3b0f4dd2bb3aa1ce8566d220cc74dda9df97d8490cc81d89d735c92e59fb6"));
}

// The bip143 p2sh-p2wsh example, signed with each of the six sighash types.

constexpr char bip143_p2wsh_tx[] = "010000000136641869ca081e70f394c6948e8af409e18b619df2ed74aa106c1ca29787b96e0100000000ffffffff0200e9a435000000001976a914389ffce9cd9ae88dcc0631e88a821ffdbe9bfe2688acc0832f05000000001976a9147480a33f950689af511e6e84c138dbbd3c3ee41588ac00000000";
constexpr char bip143_p2wsh_script[] = "56210307b8ae49ac90a048e9b53357a2354b3334e9c8bee813ecb98e99a7e07e8c3ba32103b28f0c28bfab54554ae8c658ac5c3e0ce6e79ad336331f78c428dd43eea8449b21034b8113d703413d57761b8b9781957b8c0ac1dfe69f492580ca4195f50376ba4a21033400f6afecb833092a9a21cfdf1ed1376e58c5d1f47de74683123987e967a8f42103a6d48b1131e94ba04d9737d61acdaa1322008af9602b3b14862c07a1789aac162102d8b661b0b3302ee2f162b09e07a55ad5dfbe673a9f01d9f0c19617681024306b56ae";
constexpr uint64_t bip143_p2wsh_value = 987654321;

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_p2wsh_all__expected)
{
    const transaction tx(base16_chunk(bip143_p2wsh_tx), true);
    const script subscript(base16_chunk(bip143_p2wsh_script), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::hash_all, bip143_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("185c0be5263dce5b4bb50a047973c1b6272bfbd0103a89444597dc40b248ee7c"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_p2wsh_none__expected)
{
    const transaction tx(base16_chunk(bip143_p2wsh_tx), true);
    const script subscript(base16_chunk(bip143_p2wsh_script), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::hash_none, bip143_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("e9733bc60ea13c95c6527066bb975a2ff29a925e80aa14c213f686cbae5d2f36"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_p2wsh_single__expected)
{
    const transaction tx(base16_chunk(bip143_p2wsh_tx), true);
    const script subscript(base16_chunk(bip143_p2wsh_script), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::hash_single, bip143_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("1e1f1c303dc025bd664acb72e583e933fae4cff9148bf78c157d1e8f78530aea"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_p2wsh_all_anyone_can_pay__expected)
{
    const transaction tx(base16_chunk(bip143_p2wsh_tx), true);
    const script subscript(base16_chunk(bip143_p2wsh_script), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::all_anyone_can_pay, bip143_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("2a67f03e63a6a422125878b40b82da593be8d4efaafe88ee528af6e5a9955c6e"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_p2wsh_none_anyone_can_pay__expected)
{
    const transaction tx(base16_chunk(bip143_p2wsh_tx), true);
    const script subscript(base16_chunk(bip143_p2wsh_script), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::none_anyone_can_pay, bip143_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("781ba15f3779d5542ce8ecb5c18716733a5ee42a6f51488ec96154934e2c890a"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_p2wsh_single_anyone_can_pay__expected)
{
    const transaction tx(base16_chunk(bip143_p2wsh_tx), true);
    const script subscript(base16_chunk(bip143_p2wsh_script), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::single_anyone_can_pay, bip143_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("511e8e52ed574121fc1b654970395502128263f62662e076dc6baf05c2e6a99b"));
}

// signature_hash (version 1, bip341 key path vectors)
// ----------------------------------------------------------------------------
// Transaction, prevouts and digests from the bip341 wallet test vectors.

constexpr auto bip341_flags = flags::bip141_rule | flags::bip143_rule | flags::bip341_rule | flags::bip342_rule;

static transaction bip341_spending_transaction() NOEXCEPT
{
    transaction tx(base16_chunk("02000000097de20cbff686da83a54981d2b9bab3586f4ca7e48f57f5b55963115f3b334e9c010000000000000000d7b7cab57b1393ace2d064f4d4a2cb8af6def61273e127517d44759b6dafdd990000000000fffffffff8e1f583384333689228c5d28eac13366be082dc57441760d957275419a418420000000000fffffffff0689180aa63b30cb162a73c6d2a38b7eeda2a83ece74310fda0843ad604853b0100000000feffffffaa5202bdf6d8ccd2ee0f0202afbbb7461d9264a25e5bfd3c5a52ee1239e0ba6c0000000000feffffff956149bdc66faa968eb2be2d2faa29718acbfe3941215893a2a3446d32acd050000000000000000000e664b9773b88c09c32cb70a2a3e4da0ced63b7ba3b22f848531bbb1d5d5f4c94010000000000000000e9aa6b8e6c9de67619e6a3924ae25696bb7b694bb677a632a74ef7eadfd4eabf0000000000ffffffffa778eb6a263dc090464cd125c466b5a99667720b1c110468831d058aa1b82af10100000000ffffffff0200ca9a3b000000001976a91406afd46bcdfd22ef94ac122aa11f241244a37ecc88ac807840cb0000000020ac9a87f5594be208f8532db38cff670c450ed2fea8fcdefcc9a663f78bab962b0065cd1d"), true);
    const auto& ins = *tx.inputs_ptr();
    ins[0]->prevout = to_shared(output{ 420000000, { base16_chunk("512053a1f6e454df1aa2776a2814a721372d6258050de330b3c6d10ee8f4e0dda343"), false } });
    ins[1]->prevout = to_shared(output{ 462000000, { base16_chunk("5120147c9c57132f6e7ecddba9800bb0c4449251c92a1e60371ee77557b6620f3ea3"), false } });
    ins[2]->prevout = to_shared(output{ 294000000, { base16_chunk("76a914751e76e8199196d454941c45d1b3a323f1433bd688ac"), false } });
    ins[3]->prevout = to_shared(output{ 504000000, { base16_chunk("5120e4d810fd50586274face62b8a807eb9719cef49c04177cc6b76a9a4251d5450e"), false } });
    ins[4]->prevout = to_shared(output{ 630000000, { base16_chunk("512091b64d5324723a985170e4dc5a0f84c041804f2cd12660fa5dec09fc21783605"), false } });
    ins[5]->prevout = to_shared(output{ 378000000, { base16_chunk("00147dd65592d0ab2fe0d0257d571abf032cd9db93dc"), false } });
    ins[6]->prevout = to_shared(output{ 672000000, { base16_chunk("512075169f4001aa68f15bbed28b218df1d0a62cbbcf1188c6665110c293c907b831"), false } });
    ins[7]->prevout = to_shared(output{ 546000000, { base16_chunk("5120712447206d7a5238acc7ff53fbe94a3b64539ad291c7cdbc490b7577e4b17df5"), false } });
    ins[8]->prevout = to_shared(output{ 588000000, { base16_chunk("512077e30a5522dd9f894c3f8b8bd4c4b2cf82ca7da8a3ea6a239655c39c050ab220"), false } });
    return tx;
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip341_key_path_single__expected)
{
    const auto tx = bip341_spending_transaction();
    const auto input = std::next(tx.inputs_ptr()->begin(), 0);
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, {}, 420000000, tapleaf, script_version::taproot, coverage::hash_single, bip341_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("2514a6272f85cfa0f45eb907fcb0d121b808ed37c6ea160a5a9046ed5526d555"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip341_key_path_single_anyone_can_pay__expected)
{
    const auto tx = bip341_spending_transaction();
    const auto input = std::next(tx.inputs_ptr()->begin(), 1);
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, {}, 462000000, tapleaf, script_version::taproot, coverage::single_anyone_can_pay, bip341_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("325a644af47e8a5a2591cda0ab0723978537318f10e6a63d4eed783b96a71a4d"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip341_key_path_all__expected)
{
    const auto tx = bip341_spending_transaction();
    const auto input = std::next(tx.inputs_ptr()->begin(), 3);
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, {}, 504000000, tapleaf, script_version::taproot, coverage::hash_all, bip341_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("bf013ea93474aa67815b1b6cc441d23b64fa310911d991e713cd34c7f5d46669"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip341_key_path_default__expected)
{
    const auto tx = bip341_spending_transaction();
    const auto input = std::next(tx.inputs_ptr()->begin(), 4);
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, {}, 630000000, tapleaf, script_version::taproot, coverage::hash_default, bip341_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("4f900a0bae3f1446fd48490c2958b5a023228f01661cda3496a11da502a7f7ef"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip341_key_path_none__expected)
{
    const auto tx = bip341_spending_transaction();
    const auto input = std::next(tx.inputs_ptr()->begin(), 6);
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, {}, 672000000, tapleaf, script_version::taproot, coverage::hash_none, bip341_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("15f25c298eb5cdc7eb1d638dd2d45c97c4c59dcaec6679cfc16ad84f30876b85"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip341_key_path_none_anyone_can_pay__expected)
{
    const auto tx = bip341_spending_transaction();
    const auto input = std::next(tx.inputs_ptr()->begin(), 7);
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, {}, 546000000, tapleaf, script_version::taproot, coverage::none_anyone_can_pay, bip341_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("cd292de50313804dabe4685e83f923d2969577191a3e1d2882220dca88cbeb10"));
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip341_key_path_all_anyone_can_pay__expected)
{
    const auto tx = bip341_spending_transaction();
    const auto input = std::next(tx.inputs_ptr()->begin(), 8);
    const hash_cptr tapleaf{};
    hash_digest sighash{};
    BOOST_REQUIRE(tx.signature_hash(sighash, input, {}, 588000000, tapleaf, script_version::taproot, coverage::all_anyone_can_pay, bip341_flags));
    BOOST_REQUIRE_EQUAL(sighash, base16_array("cccb739eca6c13a8a89e6e5cd317ffe55669bbda23f2fd37b0f18755e008edd2"));
}

// signature_hash (midstate cache invariance)
// ----------------------------------------------------------------------------
// Interleaved coverage modes on one instance must each match the digest of a
// fresh instance, or a stale midstate cache is leaking across modes.

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip143_interleaved_modes__cache_invariant)
{
    const transaction tx(base16_chunk(bip143_p2wsh_tx), true);
    const script subscript(base16_chunk(bip143_p2wsh_script), false);
    const auto input = tx.inputs_ptr()->begin();
    const hash_cptr tapleaf{};

    hash_digest all1{};
    BOOST_REQUIRE(tx.signature_hash(all1, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::hash_all, bip143_flags));
    BOOST_REQUIRE_EQUAL(all1, base16_array("185c0be5263dce5b4bb50a047973c1b6272bfbd0103a89444597dc40b248ee7c"));

    hash_digest single1{};
    BOOST_REQUIRE(tx.signature_hash(single1, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::hash_single, bip143_flags));
    BOOST_REQUIRE_EQUAL(single1, base16_array("1e1f1c303dc025bd664acb72e583e933fae4cff9148bf78c157d1e8f78530aea"));

    hash_digest all2{};
    BOOST_REQUIRE(tx.signature_hash(all2, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::hash_all, bip143_flags));
    BOOST_REQUIRE_EQUAL(all2, all1);

    hash_digest none1{};
    BOOST_REQUIRE(tx.signature_hash(none1, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::hash_none, bip143_flags));
    BOOST_REQUIRE_EQUAL(none1, base16_array("e9733bc60ea13c95c6527066bb975a2ff29a925e80aa14c213f686cbae5d2f36"));

    hash_digest anyone1{};
    BOOST_REQUIRE(tx.signature_hash(anyone1, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::all_anyone_can_pay, bip143_flags));
    BOOST_REQUIRE_EQUAL(anyone1, base16_array("2a67f03e63a6a422125878b40b82da593be8d4efaafe88ee528af6e5a9955c6e"));

    hash_digest all3{};
    BOOST_REQUIRE(tx.signature_hash(all3, input, subscript, bip143_p2wsh_value, tapleaf, script_version::segwit, coverage::hash_all, bip143_flags));
    BOOST_REQUIRE_EQUAL(all3, all1);
}

BOOST_AUTO_TEST_CASE(transaction__signature_hash__bip341_interleaved_modes__cache_invariant)
{
    const auto tx = bip341_spending_transaction();
    const auto input = std::next(tx.inputs_ptr()->begin(), 4);
    const hash_cptr tapleaf{};

    hash_digest default1{};
    BOOST_REQUIRE(tx.signature_hash(default1, input, {}, 630000000, tapleaf, script_version::taproot, coverage::hash_default, bip341_flags));
    BOOST_REQUIRE_EQUAL(default1, base16_array("4f900a0bae3f1446fd48490c2958b5a023228f01661cda3496a11da502a7f7ef"));

    const auto single_input = std::next(tx.inputs_ptr()->begin(), 0);
    hash_digest single1{};
    BOOST_REQUIRE(tx.signature_hash(single1, single_input, {}, 420000000, tapleaf, script_version::taproot, coverage::hash_single, bip341_flags));
    BOOST_REQUIRE_EQUAL(single1, base16_array("2514a6272f85cfa0f45eb907fcb0d121b808ed37c6ea160a5a9046ed5526d555"));

    hash_digest default2{};
    BOOST_REQUIRE(tx.signature_hash(default2, input, {}, 630000000, tapleaf, script_version::taproot, coverage::hash_default, bip341_flags));
    BOOST_REQUIRE_EQUAL(default2, default1);
}

BOOST_AUTO_TEST_SUITE_END()
