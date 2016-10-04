/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/iostreams/stream.hpp>
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

using namespace bc;

BOOST_AUTO_TEST_SUITE(transaction_tests)

BOOST_AUTO_TEST_CASE(transaction__constructor_1__always__returns_default_initialized)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(transaction__constructor_2__valid_input__returns_input_initialized)
{
    uint32_t version = 2345u;
    uint32_t locktime = 4568656u;
    chain::input::list inputs;
    inputs.emplace_back();
    BOOST_REQUIRE(inputs.back().from_data(to_chunk(base16_literal(
        "f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc4de65310fc010000006a4730"
        "44022050d8368cacf9bf1b8fb1f7cfd9aff63294789eb1760139e7ef41f083726dadc402206779"
        "6354aba8f2e02363c5e510aa7e2830b115472fb31de67d16972867f13945012103e589480b2f74"
        "6381fca01a9b12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff"))));
    chain::output::list outputs;
    outputs.emplace_back();
    BOOST_REQUIRE(outputs.back().from_data(to_chunk(base16_literal(
        "f0c9c467000000001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac"))));

    chain::transaction instance(version, locktime, inputs, outputs);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(locktime, instance.locktime());
    BOOST_REQUIRE(inputs == instance.inputs());
    BOOST_REQUIRE(outputs == instance.outputs());
}

BOOST_AUTO_TEST_CASE(transaction__constructor_3__valid_input__returns_input_initialized)
{
    uint32_t version = 2345u;
    uint32_t locktime = 4568656u;
    chain::input::list inputs;
    inputs.emplace_back();
    BOOST_REQUIRE(inputs.back().from_data(to_chunk(base16_literal(
        "f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc4de65310fc010000006a4730"
        "44022050d8368cacf9bf1b8fb1f7cfd9aff63294789eb1760139e7ef41f083726dadc402206779"
        "6354aba8f2e02363c5e510aa7e2830b115472fb31de67d16972867f13945012103e589480b2f74"
        "6381fca01a9b12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff"))));
    chain::output::list outputs;
    outputs.emplace_back();
    BOOST_REQUIRE(outputs.back().from_data(to_chunk(base16_literal(
        "f0c9c467000000001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac"))));

    // These must be non-const.
    auto dup_inputs = inputs;
    auto dup_outputs = outputs;

    chain::transaction instance(version, locktime, std::move(dup_inputs), std::move(dup_outputs));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(locktime, instance.locktime());
    BOOST_REQUIRE(inputs == instance.inputs());
    BOOST_REQUIRE(outputs == instance.outputs());
}

BOOST_AUTO_TEST_CASE(transaction__constructor_4__valid_input__returns_input_initialized)
{
    data_chunk raw_tx = to_chunk(base16_literal(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00"));

    chain::transaction expected;
    BOOST_REQUIRE(expected.from_data(raw_tx));

    chain::transaction instance(expected);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(transaction__constructor_5__valid_input__returns_input_initialized)
{
    data_chunk raw_tx = to_chunk(base16_literal(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00"));

    // This must be non-const.
    chain::transaction expected;
    BOOST_REQUIRE(expected.from_data(raw_tx));

    chain::transaction instance(std::move(expected));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(transaction__constructor_6__valid_input__returns_input_initialized)
{
    data_chunk raw_tx = to_chunk(base16_literal(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00"));

    chain::transaction expected;
    BOOST_REQUIRE(expected.from_data(raw_tx));
    const hash_digest expected_hash = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");

    chain::transaction instance(expected, expected_hash);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(expected == instance);
    BOOST_REQUIRE(expected_hash == instance.hash());
}

BOOST_AUTO_TEST_CASE(transaction__constructor_7__valid_input__returns_input_initialized)
{
    data_chunk raw_tx = to_chunk(base16_literal(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00"));

    // This must be non-const.
    chain::transaction expected;

    BOOST_REQUIRE(expected.from_data(raw_tx));
    const hash_digest expected_hash = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");

    chain::transaction instance(std::move(expected), expected_hash);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(expected_hash == instance.hash());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__empty_inputs__returns_false)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_coinbase__with_coinbase_input__returns_true)
{
    chain::input::list inputs;
    inputs.emplace_back();
    inputs.back().set_previous_output(chain::point{ null_hash, max_input_sequence });
    chain::transaction instance;
    instance.set_inputs(inputs);
    BOOST_REQUIRE_EQUAL(true, instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_final__locktime_zero__returns_true)
{
    static const size_t height = 100;
    static const uint32_t time = 100;
    chain::transaction instance;
    instance.set_locktime(0);
    BOOST_REQUIRE_EQUAL(true, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(transaction__is_final__locktime_less_block_time_greater_threshold__returns_true)
{
    static const size_t height = locktime_threshold + 100;
    static const uint32_t time = 100;
    chain::transaction instance;
    instance.set_locktime(locktime_threshold + 50);
    BOOST_REQUIRE_EQUAL(true, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(transaction__is_final__locktime_less_block_height_less_threshold_returns_true)
{
    static const size_t height = 100;
    static const uint32_t time = 100;
    chain::transaction instance;
    instance.set_locktime(50);
    BOOST_REQUIRE_EQUAL(true, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(transaction__is_final__locktime_input_not_final__returns_false)
{
    static const size_t height = 100;
    static const uint32_t time = 100;
    chain::input input;
    input.set_sequence(1);
    chain::transaction instance(0, 101, { input }, {});
    BOOST_REQUIRE_EQUAL(false, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(transaction__is_final__locktime_inputs_final__returns_true)
{
    static const size_t height = 100;
    static const uint32_t time = 100;
    chain::input input;
    input.set_sequence(max_input_sequence);
    chain::transaction instance(0u, 101u, { input }, {});
    BOOST_REQUIRE_EQUAL(true, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(transaction__is_locktime_conflict__locktime_zero__returns_false)
{
    chain::transaction instance;
    instance.set_locktime(0);
    BOOST_REQUIRE_EQUAL(false, instance.is_locktime_conflict());
}

BOOST_AUTO_TEST_CASE(transaction__is_locktime_conflict__input_sequence_not_maximum__returns_false)
{
    chain::input input;
    input.set_sequence(1);
    chain::transaction instance(0, 2143u, { input }, {});
    BOOST_REQUIRE_EQUAL(false, instance.is_locktime_conflict());
}

BOOST_AUTO_TEST_CASE(transaction__is_locktime_conflict__no_inputs__returns_true)
{
    chain::transaction instance;
    instance.set_locktime(2143);
    BOOST_REQUIRE_EQUAL(true, instance.is_locktime_conflict());
}

BOOST_AUTO_TEST_CASE(transaction__is_locktime_conflict__input_max_sequence__returns_true)
{
    // This must be non-const.
    chain::input::list inputs;

    inputs.emplace_back();
    inputs.back().set_sequence(max_input_sequence);
    chain::transaction instance(0, 2143u, std::move(inputs), {});
    BOOST_REQUIRE_EQUAL(true, instance.is_locktime_conflict());
}

BOOST_AUTO_TEST_CASE(transaction__from_data__insufficient_version_bytes__failure)
{
    data_chunk data(2);

    chain::transaction instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(transaction__from_data__insufficient_input_bytes__failure)
{
    data_chunk data = to_chunk(base16_literal("0000000103"));
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(transaction__from_data__insufficient_output_bytes__failure)
{
    data_chunk data = to_chunk(base16_literal("000000010003"));
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(transaction__from_data__valid_input__roundtrip_success)
{
    auto data_satoshi = to_chunk(base16_literal(
        "010000000209e300a61db28e4fd3562aec52647646fc55aa3e3f7d824f20f451"
        "a45db8c958010000006a4730440220364484206d2d3977373a82135cbdb78f20"
        "0e2160ec2636c9f080424a61748d15022056c9729b9fbd5c04170a7bb63b1d1b"
        "02da183fa3605864666dba6e216c3ce9270121027d4b693a2851541b1e393732"
        "0c5e4173ea8ab3f152f7a7fa96dbb936d2cff73dffffffff1cbb3eb855334221"
        "0c67e27dab3c2e72a9c0937b20dc6fe4d08d209fc4c2f163000000006a473044"
        "02207bc1940e12ec94544b7080518f73840f9bd191bd5fcb6b00f69a57a58658"
        "33bc02201bd759d978305e4346b39a9ee8b38043888621748dd1f8ab822df542"
        "427e49d6012102a17da2659b6149fb281a675519b5fd64dd80699dccd509f76e"
        "655699f2f625efffffffff021dc05c00000000001976a914e785da41a84114af"
        "0762c5a6f9e5b78ff730581988ac70e0cf02000000001976a914607a10e5b5f5"
        "3610341db013e77ba7c317a10c9088ac00000000"));

    auto data_other = to_chunk(base16_literal(
        "0100000000000000021dc05c00000000001976a914e785da41a84114af0762c5"
        "a6f9e5b78ff730581988ac70e0cf02000000001976a914607a10e5b5f5361034"
        "1db013e77ba7c317a10c9088ac0209e300a61db28e4fd3562aec52647646fc55"
        "aa3e3f7d824f20f451a45db8c958010000006a4730440220364484206d2d3977"
        "373a82135cbdb78f200e2160ec2636c9f080424a61748d15022056c9729b9fbd"
        "5c04170a7bb63b1d1b02da183fa3605864666dba6e216c3ce9270121027d4b69"
        "3a2851541b1e3937320c5e4173ea8ab3f152f7a7fa96dbb936d2cff73dffffff"
        "ff1cbb3eb8553342210c67e27dab3c2e72a9c0937b20dc6fe4d08d209fc4c2f1"
        "63000000006a47304402207bc1940e12ec94544b7080518f73840f9bd191bd5f"
        "cb6b00f69a57a5865833bc02201bd759d978305e4346b39a9ee8b38043888621"
        "748dd1f8ab822df542427e49d6012102a17da2659b6149fb281a675519b5fd64"
        "dd80699dccd509f76e655699f2f625efffffffff"));

    boost::iostreams::stream<byte_source<data_chunk>> satoshi_stream(data_satoshi);
    chain::transaction satoshi_tx;
    BOOST_REQUIRE(satoshi_tx.from_data(satoshi_stream, true));
    BOOST_REQUIRE(data_satoshi == satoshi_tx.to_data(true));
    boost::iostreams::stream<byte_source<data_chunk>> other_stream(data_other);
    chain::transaction other_tx;
    BOOST_REQUIRE(other_tx.from_data(other_stream, false));
    BOOST_REQUIRE(data_other == other_tx.to_data(false));
    BOOST_REQUIRE(satoshi_tx == other_tx);
}

BOOST_AUTO_TEST_CASE(transaction__factory_data_1__case_1__success)
{
    static const hash_digest tx_hash = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");
    static const data_chunk raw_tx = to_chunk(base16_literal(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00"));
    BOOST_REQUIRE_EQUAL(raw_tx.size(), 225u);

    chain::transaction tx = chain::transaction::factory_from_data(raw_tx);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.serialized_size(), 225u);
    BOOST_REQUIRE(tx.hash() == tx_hash);

    // Re-save tx and compare against original.
    BOOST_REQUIRE_EQUAL(tx.serialized_size(), raw_tx.size());
    data_chunk resave = tx.to_data();
    BOOST_REQUIRE(resave == raw_tx);
}

BOOST_AUTO_TEST_CASE(transaction__factory_data_1__case_2__success)
{
    static const hash_digest tx_hash = hash_literal(
        "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");
    static const data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));
    BOOST_REQUIRE_EQUAL(raw_tx.size(), 523u);

    chain::transaction tx = chain::transaction::factory_from_data(raw_tx);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE(tx.hash() == tx_hash);

    // Re-save tx and compare against original.
    BOOST_REQUIRE(tx.serialized_size() == raw_tx.size());
    data_chunk resave = tx.to_data();
    BOOST_REQUIRE(resave == raw_tx);
}

BOOST_AUTO_TEST_CASE(transaction__factory_data_2__case_1__success)
{
    static const hash_digest tx_hash = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");
    static const data_chunk raw_tx = to_chunk(base16_literal(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00"));
    BOOST_REQUIRE_EQUAL(raw_tx.size(), 225u);

    data_source stream(raw_tx);
    chain::transaction tx = chain::transaction::factory_from_data(stream);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.serialized_size(), 225u);
    BOOST_REQUIRE(tx.hash() == tx_hash);

    // Re-save tx and compare against original.
    BOOST_REQUIRE_EQUAL(tx.serialized_size(), raw_tx.size());
    data_chunk resave = tx.to_data();
    BOOST_REQUIRE(resave == raw_tx);
}

BOOST_AUTO_TEST_CASE(transaction__factory_data_2__case_2__success)
{
    static const hash_digest tx_hash = hash_literal(
        "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");
    static const data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));
    BOOST_REQUIRE_EQUAL(raw_tx.size(), 523u);

    data_source stream(raw_tx);
    chain::transaction tx = chain::transaction::factory_from_data(stream);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE(tx.hash() == tx_hash);

    // Re-save tx and compare against original.
    BOOST_REQUIRE(tx.serialized_size() == raw_tx.size());
    data_chunk resave = tx.to_data();
    BOOST_REQUIRE(resave == raw_tx);
}

BOOST_AUTO_TEST_CASE(transaction__factory_data_3__case_1__success)
{
    static const hash_digest tx_hash = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");
    static const data_chunk raw_tx = to_chunk(base16_literal(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00"));
    BOOST_REQUIRE_EQUAL(raw_tx.size(), 225u);

    data_source stream(raw_tx);
    istream_reader source(stream);
    chain::transaction tx = chain::transaction::factory_from_data(source);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE_EQUAL(tx.serialized_size(), 225u);
    BOOST_REQUIRE(tx.hash() == tx_hash);

    // Re-save tx and compare against original.
    BOOST_REQUIRE_EQUAL(tx.serialized_size(), raw_tx.size());
    data_chunk resave = tx.to_data();
    BOOST_REQUIRE(resave == raw_tx);
}

BOOST_AUTO_TEST_CASE(transaction__factory_data_3__case_2__success)
{
    static const hash_digest tx_hash = hash_literal(
        "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");
    static const data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));
    BOOST_REQUIRE_EQUAL(raw_tx.size(), 523u);

    data_source stream(raw_tx);
    istream_reader source(stream);
    chain::transaction tx = chain::transaction::factory_from_data(source);
    BOOST_REQUIRE(tx.is_valid());
    BOOST_REQUIRE(tx.hash() == tx_hash);

    // Re-save tx and compare against original.
    BOOST_REQUIRE(tx.serialized_size() == raw_tx.size());
    data_chunk resave = tx.to_data();
    BOOST_REQUIRE(resave == raw_tx);
}

BOOST_AUTO_TEST_CASE(transaction__to_string__valid_data__success)
{
    uint32_t flags = 0;
    std::string expected =
        "Transaction:\n"
        "\tversion = 1\n"
        "\tlocktime = 0\n"
        "Inputs:\n"
        "\thash = 5c9ecfefa5d14815922778a32d9b01b3a6e7761795aefeba1796f237d82ae664\n"
        "\tindex = 1\n"
        "\t[ 304502204df0dc9b7f61fbb2e4c8b0e09f3426d625a0191e56c48c338df3214555180eaf022100f21ac1f632201154f3c69e1eadb59901a34c40f1127e96adc31fac6ae6b11fb401 ] [ 03893d5a06201d5cf61400e96fa4a7514fc12ab45166ace618d68b8066c9c585f9 ]\n"
        "\tsequence = 4294967295\n"
        "\thash = 1b681f50d71834c294e8959ce3666f1c6a44942cd1a896fd43d40792c355b754\n"
        "\tindex = 1\n"
        "\t[ 304502203267910f55f2297360198fff57a3631be850965344370f732950b47795737875022100f7da90b82d24e6e957264b17d3e5042bab8946ee5fc676d15d915da450151d3601 ] [ 03893d5a06201d5cf61400e96fa4a7514fc12ab45166ace618d68b8066c9c585f9 ]\n"
        "\tsequence = 4294967295\n"
        "\thash = 9ccc48b734c7cc8f305fcb6ae263966d24b98a7f5396440caf0e1f4a394da10a\n"
        "\tindex = 1\n"
        "\t[ 3046022100d64ace8ec2d5feeb3e868e82b894202db8cb683c414d806b343d02b7ac679de7022100a2dcd39940dd28d4e22cce417a0829c1b516c471a3d64d11f2c5d754108bdc0b01 ] [ 03893d5a06201d5cf61400e96fa4a7514fc12ab45166ace618d68b8066c9c585f9 ]\n"
        "\tsequence = 4294967295\n"
        "Outputs:\n"
        "\tvalue = 15000000\n"
        "\tdup hash160 [ 884c09d7e1f6420976c40e040c30b2b62210c3d4 ] equalverify checksig\n"
        "\tvalue = 340000\n"
        "\tdup hash160 [ 905f933de850988603aafeeb2fd7fce61e66fe5d ] equalverify checksig\n\n";

    data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));
    BOOST_REQUIRE_EQUAL(raw_tx.size(), 523u);

    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(true, instance.from_data(raw_tx));
    BOOST_REQUIRE_EQUAL(expected, instance.to_string(flags));
}

BOOST_AUTO_TEST_CASE(transaction__version__roundtrip__success)
{
    uint32_t version = 1254u;
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, version == instance.version());
    instance.set_version(version);
    BOOST_REQUIRE_EQUAL(version, instance.version());
}

BOOST_AUTO_TEST_CASE(transaction__locktime__roundtrip__success)
{
    uint32_t locktime = 1254u;
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, locktime == instance.locktime());
    instance.set_locktime(locktime);
    BOOST_REQUIRE_EQUAL(locktime, instance.locktime());
}

BOOST_AUTO_TEST_CASE(transaction__inputs_setter_1__roundtrip__success)
{
    chain::input::list inputs;
    inputs.emplace_back();
    BOOST_REQUIRE(inputs.back().from_data(to_chunk(base16_literal(
        "f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc4de65310fc010000006a4730"
        "44022050d8368cacf9bf1b8fb1f7cfd9aff63294789eb1760139e7ef41f083726dadc402206779"
        "6354aba8f2e02363c5e510aa7e2830b115472fb31de67d16972867f13945012103e589480b2f74"
        "6381fca01a9b12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff"))));

    chain::transaction instance;
    BOOST_REQUIRE(inputs != instance.inputs());
    instance.set_inputs(inputs);
    BOOST_REQUIRE(inputs == instance.inputs());
}

BOOST_AUTO_TEST_CASE(transaction__inputs_setter_2__roundtrip__success)
{
    chain::input::list inputs;
    inputs.emplace_back();
    BOOST_REQUIRE(inputs.back().from_data(to_chunk(base16_literal(
        "f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc4de65310fc010000006a4730"
        "44022050d8368cacf9bf1b8fb1f7cfd9aff63294789eb1760139e7ef41f083726dadc402206779"
        "6354aba8f2e02363c5e510aa7e2830b115472fb31de67d16972867f13945012103e589480b2f74"
        "6381fca01a9b12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff"))));

    // This must be non-const.
    auto dup_inputs = inputs;

    chain::transaction instance;
    BOOST_REQUIRE(inputs != instance.inputs());
    instance.set_inputs(std::move(dup_inputs));
    BOOST_REQUIRE(inputs == instance.inputs());
}

BOOST_AUTO_TEST_CASE(transaction__outputs_setter_1__roundtrip__success)
{
    chain::output::list outputs;
    outputs.emplace_back();
    BOOST_REQUIRE(outputs.back().from_data(to_chunk(base16_literal(
        "f0c9c467000000001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac"))));

    chain::transaction instance;
    BOOST_REQUIRE(outputs != instance.outputs());
    instance.set_outputs(outputs);
    BOOST_REQUIRE(outputs == instance.outputs());
}

BOOST_AUTO_TEST_CASE(transaction__outputs_setter_2__roundtrip__success)
{
    chain::output::list outputs;
    outputs.emplace_back();
    BOOST_REQUIRE(outputs.back().from_data(to_chunk(base16_literal(
        "f0c9c467000000001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac"))));

    // This must be non-const.
    auto dup_outputs = outputs;

    chain::transaction instance;
    BOOST_REQUIRE(outputs != instance.outputs());
    instance.set_outputs(std::move(dup_outputs));
    BOOST_REQUIRE(outputs == instance.outputs());
}

BOOST_AUTO_TEST_CASE(transaction__is_oversized_coinbase__non_coinbase_tx__returns_false)
{
    auto data = to_chunk(base16_literal(
        "01000000023562c207a2a505820324aa03b769ee9c04a221eff59fdab6d52c312544a"
        "c4b21020000006a473044022075d3dd4cd26137f50d1b8c18b5ecbd13b7309b801f62"
        "83ebb951b137972d6e5b02206776f5e3acb2d996a9553f2438a4d2566c1fd786d9075"
        "5a5bca023bd9ae3945b0121029caef1b63490b7deabc9547e3e5d8b13c004b4bfd04d"
        "fae270874d569e5b89a8ffffffff8593568e460593c3dd30a470977a14928be6a29c6"
        "14a644c531471a773a63601020000006a47304402201fd9ea7dc62628ea82ff7b38cc"
        "90b3f2aa8c9ae25aa575600de38c79eafc925602202ca57bcd29d38a3e6aebd6809f7"
        "be4379d86f173b2ad2d42892dcb1dccca14b60121029caef1b63490b7deabc9547e3e"
        "5d8b13c004b4bfd04dfae270874d569e5b89a8ffffffff01763d0300000000001976a"
        "914e0d40d609d0282cc97314e454d194f65c16c257888ac00000000"));

    chain::transaction instance;
    BOOST_REQUIRE(instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_coinbase());
    BOOST_REQUIRE_EQUAL(false, instance.is_oversized_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_oversized_coinbase__script_size_below_min__returns_true)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().set_index(chain::point::null_index);
    instance.inputs().back().previous_output().set_hash(null_hash);
    BOOST_REQUIRE_EQUAL(true, instance.is_coinbase());
    BOOST_REQUIRE(instance.inputs().back().script().serialized_size(false) < min_coinbase_size);
    BOOST_REQUIRE_EQUAL(true, instance.is_oversized_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_oversized_coinbase__script_size_above_max__returns_true)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().set_index(chain::point::null_index);
    instance.inputs().back().previous_output().set_hash(null_hash);
    BOOST_REQUIRE(instance.inputs().back().script().from_data(
        data_chunk(max_coinbase_size + 10), false,
        chain::script::parse_mode::raw_data));

    BOOST_REQUIRE_EQUAL(true, instance.is_coinbase());
    BOOST_REQUIRE(instance.inputs().back().script().serialized_size(false) > max_coinbase_size);
    BOOST_REQUIRE_EQUAL(true, instance.is_oversized_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_oversized_coinbase__script_size_within_bounds__returns_false)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().set_index(chain::point::null_index);
    instance.inputs().back().previous_output().set_hash(null_hash);
    BOOST_REQUIRE(instance.inputs().back().script().from_data(
        data_chunk(50), false,
        chain::script::parse_mode::raw_data));

    BOOST_REQUIRE_EQUAL(true, instance.is_coinbase());
    BOOST_REQUIRE(instance.inputs().back().script().serialized_size(false) >= min_coinbase_size);
    BOOST_REQUIRE(instance.inputs().back().script().serialized_size(false) <= max_coinbase_size);
    BOOST_REQUIRE_EQUAL(false, instance.is_oversized_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__coinbase_tx__returns_false)
{
    auto data = to_chunk(base16_literal(
        "010000000100000000000000000000000000000000000000000000000000000000000"
        "00000ffffffff23039992060481e1e157082800def50009dfdc102f42697446757279"
        "2f5345475749542f00000000015b382d4b000000001976a9148cf4f6175b2651dcdff"
        "0051970a917ea10189c2d88ac00000000"));

    chain::transaction instance;
    BOOST_REQUIRE(instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__no_null_input_prevout__returns_false)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_coinbase());
    BOOST_REQUIRE_EQUAL(false, instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__is_null_non_coinbase__null_input_prevout__returns_true)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().set_index(chain::point::null_index);
    instance.inputs().back().previous_output().set_hash(null_hash);
    BOOST_REQUIRE_EQUAL(false, instance.is_coinbase());
    BOOST_REQUIRE_EQUAL(true, instance.inputs().back().previous_output().is_null());
    BOOST_REQUIRE_EQUAL(true, instance.is_null_non_coinbase());
}

BOOST_AUTO_TEST_CASE(transaction__total_input_value__no_cache__returns_zero)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().emplace_back();
    BOOST_REQUIRE_EQUAL(0u, instance.total_input_value());
}

BOOST_AUTO_TEST_CASE(transaction__total_input_value__cache__returns_cache_value_sum)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.cache.set_value(123u);
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.cache.set_value(321u);
    BOOST_REQUIRE_EQUAL(444u, instance.total_input_value());
}

BOOST_AUTO_TEST_CASE(transaction__total_output_value__empty_outputs__returns_zero)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(0u, instance.total_output_value());
}

BOOST_AUTO_TEST_CASE(transaction__total_output_value__non_empty_outputs__returns_sum)
{
    uint64_t expected = 1234;
    chain::transaction instance;

    // This must be non-const.
    chain::output::list outputs;

    outputs.emplace_back();
    outputs.back().set_value(1200);
    outputs.emplace_back();
    outputs.back().set_value(34);
    instance.set_outputs(std::move(outputs));
    BOOST_REQUIRE_EQUAL(expected, instance.total_output_value());
}

BOOST_AUTO_TEST_CASE(transaction__fees__nonempty__returns_outputs_minus_inputs)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.cache.set_value(123u);
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.cache.set_value(321u);
    instance.outputs().emplace_back();
    instance.outputs().back().set_value(44u);
    BOOST_REQUIRE_EQUAL(400u, instance.fees());
}

BOOST_AUTO_TEST_CASE(transaction__is_overspent__output_does_not_exceed_input__returns_false)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_overspent());
}

BOOST_AUTO_TEST_CASE(transaction__is_overspent__output_exceeds_input__returns_true)
{
    chain::transaction instance;
    instance.outputs().emplace_back();
    instance.outputs().back().set_value(1200);
    instance.outputs().emplace_back();
    instance.outputs().back().set_value(34);
    BOOST_REQUIRE_EQUAL(true, instance.is_overspent());
}

// TODO: tests with initialized data
BOOST_AUTO_TEST_CASE(transaction__signature_operations_single_input_output_uninitialized__returns_zero)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.outputs().emplace_back();
    BOOST_REQUIRE_EQUAL(0u, instance.signature_operations(false));
}

BOOST_AUTO_TEST_CASE(transaction__is_missing_inputs__empty_inputs__returns_false)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_missing_inputs());
}

BOOST_AUTO_TEST_CASE(transaction__is_missing_inputs__inputs_without_cache_value__returns_true)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    BOOST_REQUIRE_EQUAL(true, instance.is_missing_inputs());
}

BOOST_AUTO_TEST_CASE(transaction__is_missing_inputs__inputs_with_cache_value__returns_false)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.cache.set_value(123u);
    BOOST_REQUIRE_EQUAL(false, instance.is_missing_inputs());
}

BOOST_AUTO_TEST_CASE(transaction__missing_inputs__empty_inputs__returns_empty)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(0u, instance.missing_inputs().size());
}

BOOST_AUTO_TEST_CASE(transaction__missing_inputs__inputs_without_cache_value__returns_single_index)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    auto result = instance.missing_inputs();
    BOOST_REQUIRE_EQUAL(1u, result.size());
    BOOST_REQUIRE_EQUAL(0u, result.back());
}

BOOST_AUTO_TEST_CASE(transaction__missing_inputs__inputs_with_cache_value__returns_empty)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.cache.set_value(123u);
    BOOST_REQUIRE_EQUAL(0u, instance.missing_inputs().size());
}

BOOST_AUTO_TEST_CASE(transaction__is_double_spend__empty_inputs__returns_false)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_double_spend(false));
    BOOST_REQUIRE_EQUAL(false, instance.is_double_spend(true));
}

BOOST_AUTO_TEST_CASE(transaction__is_double_spend__unspent_inputs__returns_false)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    BOOST_REQUIRE_EQUAL(false, instance.is_double_spend(false));
    BOOST_REQUIRE_EQUAL(false, instance.is_double_spend(true));
}

BOOST_AUTO_TEST_CASE(transaction__is_double_spend__include_unconfirmed_false_with_unconfirmed__returns_false)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.spent = true;
    BOOST_REQUIRE_EQUAL(false, instance.is_double_spend(false));
}

BOOST_AUTO_TEST_CASE(transaction__is_double_spend__include_unconfirmed_false_with_confirmed__returns_true)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.spent = true;
    instance.inputs().back().previous_output().validation.confirmed = true;
    BOOST_REQUIRE_EQUAL(true, instance.is_double_spend(false));
}

BOOST_AUTO_TEST_CASE(transaction__is_double_spend__include_unconfirmed_true_with_unconfirmed__returns_true)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.spent = true;
    BOOST_REQUIRE_EQUAL(true, instance.is_double_spend(true));
}

BOOST_AUTO_TEST_CASE(transaction__double_spends__empty_inputs__returns_empty)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(true, instance.double_spends(false).empty());
    BOOST_REQUIRE_EQUAL(true, instance.double_spends(true).empty());
}

BOOST_AUTO_TEST_CASE(transaction__double_spends__unspent_inputs__returns_empty)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    BOOST_REQUIRE_EQUAL(true, instance.double_spends(false).empty());
    BOOST_REQUIRE_EQUAL(true, instance.double_spends(true).empty());
}

BOOST_AUTO_TEST_CASE(transaction__double_spends__include_unconfirmed_false_with_unconfirmed__returns_empty)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.spent = true;
    BOOST_REQUIRE_EQUAL(false, instance.is_double_spend(false));
}

BOOST_AUTO_TEST_CASE(transaction__double_spends__include_unconfirmed_false_with_confirmed__returns_expected)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.spent = true;
    instance.inputs().back().previous_output().validation.confirmed = true;
    auto result = instance.double_spends(false);
    BOOST_REQUIRE_EQUAL(1u, result.size());
    BOOST_REQUIRE_EQUAL(0u, result.back());
}

BOOST_AUTO_TEST_CASE(transaction__double_spends__include_unconfirmed_true_with_unconfirmed__returns_expected)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.spent = true;
    auto result = instance.double_spends(true);
    BOOST_REQUIRE_EQUAL(1u, result.size());
    BOOST_REQUIRE_EQUAL(0u, result.back());
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__empty_inputs__returns_false)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_immature(453u));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__mature_inputs__returns_false)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().set_index(123u);
    BOOST_REQUIRE_EQUAL(false, instance.is_immature(453u));
}

BOOST_AUTO_TEST_CASE(transaction__is_immature__immature_inputs__returns_true)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.height = 20u;
    BOOST_REQUIRE_EQUAL(true, instance.is_immature(50u));
}

BOOST_AUTO_TEST_CASE(transaction__immature_inputs__empty_inputs__returns_empty)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(true, instance.immature_inputs(453u).empty());
}

BOOST_AUTO_TEST_CASE(transaction__immature_inputs__mature_inputs__returns_empty)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().set_index(123u);
    BOOST_REQUIRE_EQUAL(true, instance.immature_inputs(453u).empty());
}

BOOST_AUTO_TEST_CASE(transaction__immature_inputs__immature_inputs__returns_input_indexes)
{
    chain::transaction instance;
    instance.inputs().emplace_back();
    instance.inputs().back().previous_output().validation.height = 20u;
    auto result = instance.immature_inputs(50u);
    BOOST_REQUIRE_EQUAL(1u, result.size());
    BOOST_REQUIRE_EQUAL(0u, result.back());
}

BOOST_AUTO_TEST_CASE(transaction__operator_assign_equals_1__always__matches_equivalent)
{
    data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));

    chain::transaction expected;
    BOOST_REQUIRE(expected.from_data(raw_tx));
    chain::transaction instance;
    instance = chain::transaction::factory_from_data(raw_tx);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(transaction__operator_assign_equals_2__always__matches_equivalent)
{
    data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));

    chain::transaction expected;
    BOOST_REQUIRE(expected.from_data(raw_tx));
    chain::transaction instance;
    instance = expected;
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(transaction__operator_boolean_equals__duplicates__returns_true)
{
    data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));

    chain::transaction alpha;
    chain::transaction beta;
    BOOST_REQUIRE(alpha.from_data(raw_tx));
    BOOST_REQUIRE(beta.from_data(raw_tx));
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(transaction__operator_boolean_equals__differs__returns_false)
{
    data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));

    chain::transaction alpha;
    chain::transaction beta;
    BOOST_REQUIRE(alpha.from_data(raw_tx));
    BOOST_REQUIRE_EQUAL(false, alpha == beta);
}

BOOST_AUTO_TEST_CASE(transaction__operator_boolean_not_equals__duplicates__returns_false)
{
    data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));

    chain::transaction alpha;
    chain::transaction beta;
    BOOST_REQUIRE(alpha.from_data(raw_tx));
    BOOST_REQUIRE(beta.from_data(raw_tx));
    BOOST_REQUIRE_EQUAL(false, alpha != beta);
}

BOOST_AUTO_TEST_CASE(transaction__operator_boolean_not_equals__differs__returns_true)
{
    data_chunk raw_tx = to_chunk(base16_literal(
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
        "e61e66fe5d88ac00000000"));

    chain::transaction alpha;
    chain::transaction beta;
    BOOST_REQUIRE(alpha.from_data(raw_tx));
    BOOST_REQUIRE(alpha != beta);
}

BOOST_AUTO_TEST_CASE(transaction__hash__block320670__success)
{
    // This is a garbage output script containing a collision with the
    // opcode::raw_data extended opcode (nonstandard, should be removed).
    const auto expected = hash_literal(
        "cb1e303db604f066225eb14d59d3f8d2231200817bc9d4610d2802586bd93f8a");

    const auto data = to_chunk(base16_literal(
        "0100000001b63634c25f23018c18cbb24ad503672fe7c5edc3fef193ec0f581dd"
        "b27d4e401490000006a47304402203b361bfb7e189c77379d6ffc90babe1b9658"
        "39d0b9b60966ade0c4b8de28385f022057432fe6f8f530c54d3513e41da6fb138"
        "fba2440c877cd2bfb0c94cdb5610fbe0121020d2d76d6db0d1c0bda17950f6468"
        "6e4bf42481337707e9a81bbe48458cfc8389ffffffff010000000000000000566"
        "a54e38193e381aee4b896e7958ce381afe4bb96e4babae381abe38288e381a3e3"
        "81a6e7ac91e9a194e38292e5a5aae3828fe3828ce3828be7bea9e58b99e38292e"
        "8a8ade38191e381a6e381afe38184e381aae3818400000000"));

    chain::transaction instance;
    BOOST_REQUIRE(instance.from_data(data));
    BOOST_REQUIRE(expected == instance.hash());
    BOOST_REQUIRE(data == instance.to_data());
}

BOOST_AUTO_TEST_SUITE_END()
