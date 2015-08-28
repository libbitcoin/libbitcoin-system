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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
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

BOOST_AUTO_TEST_CASE(is_coinbase_returns_false)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(is_coinbase_returns_true)
{
    chain::transaction instance;
    instance.inputs.emplace_back();
    instance.inputs.back().previous_output.index = max_input_sequence;
    instance.inputs.back().previous_output.hash = null_hash;
    BOOST_REQUIRE_EQUAL(true, instance.is_coinbase());
}

BOOST_AUTO_TEST_CASE(is_final_locktime_zero_returns_true)
{
    uint64_t height = 100;
    uint32_t time = 100;
    chain::transaction instance;
    instance.locktime = 0;
    BOOST_REQUIRE_EQUAL(true, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(is_final_locktime_less_block_time_greater_threshold_returns_true)
{
    uint64_t height = locktime_threshold + 100;
    uint32_t time = 100;
    chain::transaction instance;
    instance.locktime = locktime_threshold + 50;
    BOOST_REQUIRE_EQUAL(true, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(is_final_locktime_less_block_height_less_threshold_returns_true)
{
    uint64_t height = 100;
    uint32_t time = 100;
    chain::transaction instance;
    instance.locktime = 50;
    BOOST_REQUIRE_EQUAL(true, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(is_final_locktime_input_not_final_returns_false)
{
    uint64_t height = 100;
    uint32_t time = 100;
    chain::transaction instance;
    instance.locktime = 101;
    instance.inputs.emplace_back();
    instance.inputs.back().sequence = 1;
    BOOST_REQUIRE_EQUAL(false, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(is_final_locktime_inputs_final_returns_true)
{
    uint64_t height = 100;
    uint32_t time = 100;
    chain::transaction instance;
    instance.locktime = 101;
    instance.inputs.emplace_back();
    instance.inputs.back().sequence = max_sequence;
    BOOST_REQUIRE_EQUAL(true, instance.is_final(height, time));
}

BOOST_AUTO_TEST_CASE(is_locktime_conflict_locktime_zero_returns_false)
{
    chain::transaction instance;
    instance.locktime = 0;
    BOOST_REQUIRE_EQUAL(false, instance.is_locktime_conflict());
}

BOOST_AUTO_TEST_CASE(is_locktime_conflict_input_sequence_not_maximum_returns_false)
{
    chain::transaction instance;
    instance.locktime = 2143;
    instance.inputs.emplace_back();
    instance.inputs.back().sequence = 1;
    BOOST_REQUIRE_EQUAL(false, instance.is_locktime_conflict());
}

BOOST_AUTO_TEST_CASE(is_locktime_conflict_no_inputs_returns_true)
{
    chain::transaction instance;
    instance.locktime = 2143;
    BOOST_REQUIRE_EQUAL(true, instance.is_locktime_conflict());
}

BOOST_AUTO_TEST_CASE(is_locktime_conflict_input_max_sequence_returns_true)
{
    chain::transaction instance;
    instance.locktime = 2143;
    instance.inputs.emplace_back();
    instance.inputs.back().sequence = max_sequence;
    BOOST_REQUIRE_EQUAL(true, instance.is_locktime_conflict());
}

BOOST_AUTO_TEST_CASE(total_output_value_returns_zero)
{
    chain::transaction instance;
    BOOST_REQUIRE_EQUAL(0u, instance.total_output_value());
}

BOOST_AUTO_TEST_CASE(total_output_value_returns_positive)
{
    uint64_t expected = 1234;
    chain::transaction instance;
    instance.outputs.emplace_back();
    instance.outputs.back().value = 1200;
    instance.outputs.emplace_back();
    instance.outputs.back().value = 34;
    BOOST_REQUIRE_EQUAL(expected, instance.total_output_value());
}

BOOST_AUTO_TEST_CASE(from_data_fails)
{
    data_chunk data(2);

    chain::transaction instance;

    BOOST_REQUIRE_EQUAL(false, instance.from_data(data));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(from_data_valid_junk)
{
    auto junk = base16_literal(
        "000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0"
        "000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0");
    // data_chunk_stream_host host(junk);
    byte_source<std::array<uint8_t, 64>> source(junk);
    boost::iostreams::stream<byte_source<std::array<uint8_t, 64>>> stream(source);

    chain::transaction tx;
    BOOST_REQUIRE(tx.from_data(stream));
}

BOOST_AUTO_TEST_CASE(case_1_factory_data_chunk)
{
    hash_digest tx_hash = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");
    data_chunk raw_tx = to_data_chunk(base16_literal(
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

BOOST_AUTO_TEST_CASE(case_2_factory_data_chunk)
{
    hash_digest tx_hash = hash_literal(
        "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");
    data_chunk raw_tx = to_data_chunk(base16_literal(
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

BOOST_AUTO_TEST_CASE(case_1_factory_stream)
{
    hash_digest tx_hash = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");
    data_chunk raw_tx = to_data_chunk(base16_literal(
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

BOOST_AUTO_TEST_CASE(case_2_factory_stream)
{
    hash_digest tx_hash = hash_literal(
        "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");
    data_chunk raw_tx = to_data_chunk(base16_literal(
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

BOOST_AUTO_TEST_CASE(case_1_factory_reader)
{
    hash_digest tx_hash = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");
    data_chunk raw_tx = to_data_chunk(base16_literal(
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

BOOST_AUTO_TEST_CASE(case_2_factory_reader)
{
    hash_digest tx_hash = hash_literal(
        "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");
    data_chunk raw_tx = to_data_chunk(base16_literal(
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

BOOST_AUTO_TEST_SUITE_END()
