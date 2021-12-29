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

BOOST_AUTO_TEST_SUITE(transaction_tests)

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

static const auto tx1_hash = base16_hash(
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

static const auto tx2_hash = base16_hash(
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

    bool is_internal_double_spend() const
    {
        return transaction::is_internal_double_spend();
    }

    bool is_oversized() const
    {
        return transaction::is_oversized();
    }

    bool is_overweight() const
    {
        return transaction::is_overweight();
    }

    bool is_signature_operations_limit(bool bip16, bool bip141) const
    {
        return transaction::is_signature_operations_limit(bip16, bip141);
    }

    bool is_empty() const
    {
        return transaction::is_empty();
    }

    bool is_null_non_coinbase() const
    {
        return transaction::is_null_non_coinbase();
    }

    bool is_invalid_coinbase_size() const
    {
        return transaction::is_invalid_coinbase_size();
    }

    bool is_non_final(size_t height, uint32_t timestamp,
        uint32_t median_time_past, bool bip113) const
    {
        return transaction::is_non_final(height, timestamp, median_time_past,
            bip113);
    }

    bool is_missing_prevouts() const
    {
        return transaction::is_missing_prevouts();
    }

    bool is_overspent() const
    {
        return transaction::is_overspent();
    }

    bool is_immature(size_t height) const
    {
        return transaction::is_immature(height);
    }

    bool is_locked(size_t height, uint32_t median_time_past) const
    {
        return transaction::is_locked(height, median_time_past);
    }

    bool is_unconfirmed_spend(size_t height) const
    {
        return transaction::is_unconfirmed_spend(height);
    }

    bool is_confirmed_double_spend(size_t height) const
    {
        return transaction::is_confirmed_double_spend(height);
    }

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
    const uint32_t version = 2345;
    const uint32_t locktime = 4568656;

    const input input(tx0_inputs);
    BOOST_REQUIRE(input.is_valid());

    const output output(tx0_last_output);
    BOOST_REQUIRE(output.is_valid());

    const transaction instance(version, locktime, { input }, { output });
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(locktime, instance.locktime());
    BOOST_REQUIRE_EQUAL(instance.inputs()->size(), 1u);
    BOOST_REQUIRE_EQUAL(instance.outputs()->size(), 1u);
    BOOST_REQUIRE(*instance.inputs()->front() == input);
    BOOST_REQUIRE(*instance.outputs()->front() == output);
}

BOOST_AUTO_TEST_CASE(transaction__constructor__copy_parameters__expected)
{
    const uint32_t version = 2345;
    const uint32_t locktime = 4568656;

    const input input(tx0_inputs);
    BOOST_REQUIRE(input.is_valid());

    const output output(tx0_last_output);
    BOOST_REQUIRE(output.is_valid());

    const inputs inputs{ input };
    const outputs outputs{ output };
    const transaction instance(version, locktime, inputs, outputs);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(locktime, instance.locktime());
    BOOST_REQUIRE(inputs[0] == *(*instance.inputs())[0]);
    BOOST_REQUIRE(outputs[0] == *(*instance.outputs())[0]);
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

BOOST_AUTO_TEST_CASE(transaction__inequality__different__false)
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
        0,
        inputs{},
        {}
    };

    // floored_subtract(0, 0)
    BOOST_REQUIRE_EQUAL(instance.fee(), 0u);
}

BOOST_AUTO_TEST_CASE(transaction__fee__default_input__max_uint64)
{
    const transaction instance
    {
        0,
        0,
        inputs{ {} },
        {}
    };

    // floored_subtract(max_uint64, 0)
    BOOST_REQUIRE_EQUAL(instance.fee(), max_uint64);
}

BOOST_AUTO_TEST_CASE(transaction__fee__default_output__zero)
{
    const transaction instance
    {
        0,
        0,
        inputs{},
        { {} }
    };

    // floored_subtract(0, max_uint64)
    BOOST_REQUIRE_EQUAL(instance.fee(), 0u);
}

BOOST_AUTO_TEST_CASE(transaction__fee__nonempty__outputs_minus_inputs)
{
    const uint64_t value0 = 123;
    const uint64_t value1 = 321;
    const uint64_t claim0 = 11;
    const uint64_t claim1 = 11;
    const uint64_t claim2 = 22;

    input input0;
    input input1;
    *input0.prevout = prevout{ value0, script{} };
    *input1.prevout = { value1, script{} };

    const transaction instance
    {
        0,
        0,
        { input0, input1 },
        {
            { claim0, script{} },
            { claim1, script{} },
            { claim2, script{} }
        }
    };

    // floored_subtract(values, claims)
    BOOST_REQUIRE_EQUAL(instance.fee(), value0 + value1 - claim0 - claim1 - claim2);
}

BOOST_AUTO_TEST_CASE(transaction__claim__empty_outputs__zero)
{
    transaction instance;
    BOOST_REQUIRE_EQUAL(instance.claim(), 0u);
}

BOOST_AUTO_TEST_CASE(transaction__claim__two_outputs__sum)
{
    const uint64_t claim0 = 123;
    const uint64_t claim1 = 321;
    const transaction instance
    {
        0,
        0,
        {},
        {
            { claim0, script{} },
            { claim1, script{} }
        }
    };


    // ceilinged_add(claim0, claim1)
    BOOST_REQUIRE_EQUAL(instance.claim(), claim0 + claim1);
}

BOOST_AUTO_TEST_CASE(transaction__value__no_inputs__zero)
{
    const transaction instance;
    BOOST_REQUIRE_EQUAL(instance.value(), 0u);
}

BOOST_AUTO_TEST_CASE(transaction__value__default_input2__max_uint64)
{
    transaction instance
    {
        0,
        0,
        inputs{ {}, {} },
        {}
    };

    // ceilinged_add(max_uint64, max_uint64)
    BOOST_REQUIRE_EQUAL(instance.value(), max_uint64);
}

BOOST_AUTO_TEST_CASE(transaction__value__two_prevouts__sum)
{
    const uint64_t value0 = 123;
    const uint64_t value1 = 321;

    const input input0;
    const input input1;
    *input0.prevout = { value0, script{} };
    *input1.prevout = { value1, script{} };

    transaction instance
    {
        0,
        0,
        { input0, input1 },
        {}
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
        0,
        {
            { {}, script{}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__default_input__true)
{
    const transaction instance
    {
        0,
        0,
        inputs{ {} },
        {}
    };

    BOOST_REQUIRE(instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__null_input__true)
{
    const transaction instance
    {
        0,
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__non_null_input__false)
{
    const transaction instance
    {
        0,
        0,
        {
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__first_null_input__false)
{
    const transaction instance
    {
        0,
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 },
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__null_inputs__false)
{
    const transaction instance
    {
        0,
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 },
            { { hash_digest{}, point::null_index }, {}, 0 }
        },
        {}
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
    const transaction instance{ 0, 0, inputs{}, {} };
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
        0,
        {
            {},
            { { tx1_hash, 42 }, {}, 0 },
            { { tx2_hash, 42 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(transaction__is_internal_double_spend__unique_points__false)
{
    const accessor instance
    {
        0,
        0,
        {
            {},
            { { tx1_hash, 42 }, {}, 0 },
            { { tx1_hash, 43 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(transaction__is_internal_double_spend__nonunique_points__true)
{
    const accessor instance
    {
        0,
        0,
        {
            { { tx1_hash, 42 }, {}, 0 },
            { { tx1_hash, 42 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(instance.is_internal_double_spend());
}

// is_oversized
// is_overweight
// is_signature_operations_limit

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
        0,
        inputs{ {} },
        {}
    };

    BOOST_REQUIRE(instance.is_empty());
}

BOOST_AUTO_TEST_CASE(transaction__is_empty__one_output__true)
{
    const accessor instance
    {
        0,
        0,
        inputs{},
        { {} }
    };

    BOOST_REQUIRE(instance.is_empty());
}

BOOST_AUTO_TEST_CASE(transaction__is_empty__one_input_one_output__false)
{
    const accessor instance
    {
        0,
        0,
        inputs{ {} },
        { {} }
    };

    BOOST_REQUIRE(!instance.is_empty());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__non_null_input__false)
{
    const accessor instance
    {
        0,
        0,
        {
            { { tx1_hash, point::null_index }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__non_null_inputs__false)
{
    const accessor instance
    {
        0,
        0,
        {
            { { hash_digest{}, 42 }, {}, 0 },
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__first_null_input__true)
{
    const accessor instance
    {
        0,
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 },
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__second_null_input__true)
{
    const accessor instance
    {
        0,
        0,
        {
            { { hash_digest{}, 42 }, {}, 0 },
            { { hash_digest{}, point::null_index }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__null_inputs__true)
{
    const accessor instance
    {
        0,
        0,
        {
            { { hash_digest{}, point::null_index }, {}, 0 },
            { { hash_digest{}, point::null_index }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_below_min__true)
{
    const accessor instance
    {
        0,
        0,
        {
            { {}, { data_chunk(sub1(min_coinbase_size), 0x00), false }, 0 },
        },
        {}
    };

    BOOST_REQUIRE_LT(instance.inputs()->back()->script().serialized_size(false), min_coinbase_size);
    BOOST_REQUIRE(instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_above_max__true)
{
    const accessor instance
    {
        0,
        0,
        {
            { { hash_digest{}, point::null_index }, { data_chunk(add1(max_coinbase_size), 0x00), false }, 0 },
        },
        {}
    };

    BOOST_REQUIRE_GT(instance.inputs()->back()->script().serialized_size(false), max_coinbase_size);
    BOOST_REQUIRE(instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_max__false)
{
    const accessor instance
    {
        0,
        0,
        {
            { {}, { data_chunk(max_coinbase_size, 0x00), false }, 0 },
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_min__false)
{
    const accessor instance
    {
        0,
        0,
        {
            { {}, { data_chunk(min_coinbase_size, 0x00), false }, 0 },
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_below_max__false)
{
    const accessor instance
    {
        0,
        0,
        {
            { {}, { data_chunk(sub1(max_coinbase_size), 0x00), false }, 0 },
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_invalid_coinbase_size__script_size_above_min__false)
{
    const accessor instance
    {
        0,
        0,
        {
            { {}, { data_chunk(add1(min_coinbase_size), 0x00), false }, 0 },
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_invalid_coinbase_size());
}

BOOST_AUTO_TEST_CASE(transaction__is_non_final__locktime_zero__false)
{
    const bool bip113 = false;
    const size_t height = 100;
    const uint32_t time = 100;
    const uint32_t past = 0;
    const uint32_t locktime = 0;

    const accessor instance
    {
        0,
        locktime,
        inputs{},
        {}
    };

    BOOST_REQUIRE(!instance.is_non_final(height, time, past, bip113));
}


BOOST_AUTO_TEST_CASE(transaction__is_non_final__locktime_less_block_time_greater_threshold__false)
{
    const bool bip113 = false;
    const size_t height = locktime_threshold + 100;
    const uint32_t time = 100;
    const uint32_t past = 0;
    const uint32_t locktime = locktime_threshold + 50;

    const accessor instance
    {
        0,
        locktime,
        inputs{},
        {}
    };

    BOOST_REQUIRE(!instance.is_non_final(height, time, past, bip113));
}

BOOST_AUTO_TEST_CASE(transaction__is_non_final__locktime_less_block_height_less_threshold_false)
{
    const bool bip113 = false;
    const size_t height = 100;
    const uint32_t time = 100;
    const uint32_t past = 0;
    const uint32_t locktime = 50;

    const accessor instance
    {
        0,
        locktime,
        inputs{},
        {}
    };

    BOOST_REQUIRE(!instance.is_non_final(height, time, past, bip113));
}

BOOST_AUTO_TEST_CASE(transaction__is_non_final__locktime_input_not_final__true)
{
    const bool bip113 = false;
    const size_t height = 100;
    const uint32_t time = 100;
    const uint32_t past = 0;
    const uint32_t locktime = 101;
    const uint32_t sequence = 1;

    const accessor instance
    {
        0,
        locktime,
        { { {}, script{}, sequence } },
        {}
    };

    BOOST_REQUIRE(instance.is_non_final(height, time, past, bip113));
}

BOOST_AUTO_TEST_CASE(transaction__is_non_final__locktime_inputs_final__false)
{
    const bool bip113 = false;
    const size_t height = 100;
    const uint32_t time = 100;
    const uint32_t past = 0;
    const uint32_t locktime = 101;
    const uint32_t sequence = max_input_sequence;

    const accessor instance
    {
        0,
        locktime,
        {
            { {}, script{}, sequence }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_non_final(height, time, past, bip113));
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
        0,
        inputs{ {}, {} },
        {}
    };

    BOOST_REQUIRE(instance.is_missing_prevouts());
}

BOOST_AUTO_TEST_CASE(transaction__is_missing_prevouts__valid_prevout__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    *input.prevout = { 42, script{} };
    const accessor instance
    {
        0,
        0,
        { input },
        {}
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
        0,
        {},
        { { 1, script{} }, { 0, script{} } }
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
    input.prevout->height = 0;
    input.prevout->coinbase = true;
    const accessor instance
    {
        0,
        0,
        { input },
        {}
    };

    BOOST_REQUIRE(instance.is_immature(coinbase_maturity));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__premature_coinbase__true)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.prevout->height = 1;
    input.prevout->coinbase = true;
    const accessor instance
    {
        0,
        0,
        { input },
        {}
    };

    BOOST_REQUIRE(instance.is_immature(coinbase_maturity));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__premature_non_coinbase__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.prevout->height = 1;
    input.prevout->coinbase = false;
    const accessor instance
    {
        0,
        0,
        { input },
        {}
    };

    BOOST_REQUIRE(!instance.is_immature(coinbase_maturity));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__mature_coinbase__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.prevout->height = 1;
    input.prevout->coinbase = true;
    const accessor instance
    {
        0,
        0,
        { input },
        {}
    };

    BOOST_REQUIRE(!instance.is_immature(add1(coinbase_maturity)));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__mature_non_coinbase__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.prevout->height = 1;
    input.prevout->coinbase = false;
    const accessor instance
    {
        0,
        0,
        { input },
        {}
    };

    BOOST_REQUIRE(!instance.is_immature(add1(coinbase_maturity)));
}

BOOST_AUTO_TEST_CASE(transaction__is_locked__version_1_empty__false)
{
    const uint32_t version = 1;
    const accessor instance
    {
        version,
        0,
        {
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_locked(0, 0));
}

BOOST_AUTO_TEST_CASE(transaction__is_locked__version_2_empty__false)
{
    const uint32_t version = 2;
    const accessor instance
    {
        version,
        0,
        {
            { { hash_digest{}, 42 }, {}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_locked(0, 0));
}

BOOST_AUTO_TEST_CASE(transaction__is_locked__version_1_one_of_two_locked_locked__false)
{
    const uint32_t version = 1;
    const accessor instance
    {
        version,
        0,
        {
            { {}, script{}, 1 },
            { {}, script{}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(!instance.is_locked(0, 0));
}

BOOST_AUTO_TEST_CASE(transaction__is_locked__version_4_one_of_two_locked__true)
{
    const uint32_t version = 4;
    const accessor instance
    {
        version,
        0,
        {
            { {}, script{}, 1 },
            { {}, script{}, 0 }
        },
        {}
    };

    BOOST_REQUIRE(instance.is_locked(0, 0));
}

// is_unconfirmed_spend

BOOST_AUTO_TEST_CASE(transaction__is_confirmed_double_spend__empty_inputs__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_confirmed_double_spend(42));
}

BOOST_AUTO_TEST_CASE(transaction__is_confirmed_double_spend__default_inputs__true)
{
    const accessor instance
    {
        0,
        0,
        inputs{ {}, {} },
        {}
    };

    BOOST_REQUIRE(instance.is_confirmed_double_spend(42));
}

BOOST_AUTO_TEST_CASE(transaction__is_confirmed_double_spend__unspent_input__false)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.prevout->spent = false;
    const accessor instance
    {
        0,
        0,
        { input },
        {}
    };

    BOOST_REQUIRE(!instance.is_confirmed_double_spend(42));
}

BOOST_AUTO_TEST_CASE(transaction__is_confirmed_double_spend__spent_input__true)
{
    const input input{ { hash_digest{}, 42 }, {}, 0 };
    input.prevout->spent = true;
    const accessor instance
    {
        0,
        0,
        { input },
        {}
    };

    BOOST_REQUIRE(instance.is_confirmed_double_spend(42));
}

BOOST_AUTO_TEST_SUITE_END()
