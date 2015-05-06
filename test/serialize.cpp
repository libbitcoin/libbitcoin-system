/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
#include "genesis_block.hpp"

using namespace bc;

BOOST_AUTO_TEST_SUITE(serialize_tests)

BOOST_AUTO_TEST_CASE(genesis_block_serialize_test)
{
    chain::block genblk = genesis_block();
    BOOST_REQUIRE_EQUAL(genblk.satoshi_size(), 285u);
    BOOST_REQUIRE_EQUAL(genblk.header().satoshi_size(), 80u);
    // Save genesis block.
    data_chunk rawblk = genblk;
    BOOST_REQUIRE_EQUAL(std::distance(rawblk.begin(), rawblk.end()), 285u);
    BOOST_REQUIRE_EQUAL(rawblk.size(), 285u);
    // Reload genesis block.
    data_chunk_buffer rawblkbuf(rawblk.data(), rawblk.size());
    std::istream rawblkis(&rawblkbuf);
    chain::block blk(rawblkis);
    BOOST_REQUIRE(genblk.header().version() == blk.header().version());
    BOOST_REQUIRE(genblk.header().previous_block_hash() == blk.header().previous_block_hash());
    BOOST_REQUIRE(genblk.header().merkle() == blk.header().merkle());
    BOOST_REQUIRE(genblk.header().timestamp() == blk.header().timestamp());
    BOOST_REQUIRE(genblk.header().bits() == blk.header().bits());
    BOOST_REQUIRE(genblk.header().nonce() == blk.header().nonce());
    BOOST_REQUIRE(genblk.header() == blk.header());
    const hash_digest& merkle = chain::block::generate_merkle_root(blk.transactions());
    BOOST_REQUIRE(genblk.header().merkle() == merkle);
}

BOOST_AUTO_TEST_CASE(junk_test)
{
    auto junk = base16_literal(
        "000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0"
        "000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0");

    data_chunk_buffer junkbuf(junk.data(), junk.size());
    std::istream junkis(&junkbuf);
    chain::transaction tx(junkis);
}

BOOST_AUTO_TEST_CASE(tx_test)
{
    hash_digest tx_hash_1 = hash_literal(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");
    data_chunk raw_tx_1 = to_data_chunk(base16_literal(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00"));

    data_chunk_buffer raw_tx_1_buf(raw_tx_1.data(), raw_tx_1.size());
    std::istream raw_tx_1_is(&raw_tx_1_buf);
    BOOST_REQUIRE_EQUAL(raw_tx_1.size(), 225u);
    chain::transaction tx_1(raw_tx_1_is);
    BOOST_REQUIRE_EQUAL(tx_1.satoshi_size(), 225u);
    BOOST_REQUIRE(tx_1.hash() == tx_hash_1);
    // Re-save tx and compare against original.
    BOOST_REQUIRE_EQUAL(tx_1.satoshi_size(), raw_tx_1.size());
    data_chunk resave_1 = tx_1;
    BOOST_REQUIRE(resave_1 == raw_tx_1);

    hash_digest tx_hash_2 = hash_literal(
        "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");
    data_chunk raw_tx_2 = to_data_chunk(base16_literal(
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
    BOOST_REQUIRE_EQUAL(raw_tx_2.size(), 523u);
    data_chunk_buffer raw_tx_2_buf(raw_tx_2.data(), raw_tx_2.size());
    std::istream raw_tx_2_is(&raw_tx_2_buf);
    chain::transaction tx_2(raw_tx_2_is);
    BOOST_REQUIRE(tx_2.hash() == tx_hash_2);
    // Re-save tx and compare against original.
    BOOST_REQUIRE(tx_2.satoshi_size() == raw_tx_2.size());
    data_chunk resave_2 = tx_2;
    BOOST_REQUIRE(resave_2 == raw_tx_2);
}

BOOST_AUTO_TEST_CASE(script_parse_save_test)
{
    BOOST_REQUIRE_THROW(
        chain::script psc(to_data_chunk(base16_literal(
            "3045022100ff1fc58dbd608e5e05846a8e6b45a46ad49878aef6879ad1a7cf4c"
            "5a7f853683022074a6a10f6053ab3cddc5620d169c7374cd42c1416c51b9744d"
            "b2c8d9febfb84d01"))),
        std::ios_base::failure);

    data_chunk normal_output_script = to_data_chunk(base16_literal(
        "76a91406ccef231c2db72526df9338894ccf9355e8f12188ac"));
    chain::script out_scr(normal_output_script);
    data_chunk roundtrip = out_scr;
    BOOST_REQUIRE(roundtrip == normal_output_script);

    data_chunk weird_script = to_data_chunk(base16_literal(
        "0c49206c69656b20636174732e483045022100c7387f64e1f4"
        "cf654cae3b28a15f7572106d6c1319ddcdc878e636ccb83845"
        "e30220050ebf440160a4c0db5623e0cb1562f46401a7ff5b87"
        "7aa03415ae134e8c71c901534d4f0176519c6375522103b124"
        "c48bbff7ebe16e7bd2b2f2b561aa53791da678a73d2777cc1c"
        "a4619ab6f72103ad6bb76e00d124f07a22680e39debd4dc4bd"
        "b1aa4b893720dd05af3c50560fdd52af67529c63552103b124"
        "c48bbff7ebe16e7bd2b2f2b561aa53791da678a73d2777cc1c"
        "a4619ab6f721025098a1d5a338592bf1e015468ec5a8fafc1f"
        "c9217feb5cb33597f3613a2165e9210360cfabc01d52eaaeb3"
        "976a5de05ff0cfa76d0af42d3d7e1b4c233ee8a00655ed2103"
        "f571540c81fd9dbf9622ca00cfe95762143f2eab6b65150365"
        "bb34ac533160432102bc2b4be1bca32b9d97e2d6fb255504f4"
        "bc96e01aaca6e29bfa3f8bea65d8865855af672103ad6bb76e"
        "00d124f07a22680e39debd4dc4bdb1aa4b893720dd05af3c50"
        "560fddada820a4d933888318a23c28fb5fc67aca8530524e20"
        "74b1d185dbf5b4db4ddb0642848868685174519c6351670068"));

    chain::script weird(weird_script);
    data_chunk roundtrip_result = weird;

    BOOST_REQUIRE(roundtrip_result == weird_script);
}

BOOST_AUTO_TEST_CASE(serialize_deserialize)
{
    data_chunk data(1 + 2 + 4 + 8 + 4 + 4 + 3 + 7);
    auto s = make_serializer(data.begin());
    s.write_byte(0x80);
    s.write_2_bytes(0x8040);
    s.write_4_bytes(0x80402010);
    s.write_8_bytes(0x8040201011223344);
    s.write_big_endian<uint32_t>(0x80402010);
    s.write_variable_uint(1234);
    s.write_data(to_data_chunk(to_little_endian<uint32_t>(0xbadf00d)));
    s.write_string("hello");
    auto ds = make_deserializer(data.begin(), s.iterator());
    BOOST_REQUIRE_EQUAL(ds.read_byte(), 0x80u);
    BOOST_REQUIRE_EQUAL(ds.read_2_bytes(), 0x8040u);
    BOOST_REQUIRE_EQUAL(ds.read_4_bytes(), 0x80402010u);
    BOOST_REQUIRE_EQUAL(ds.read_8_bytes(), 0x8040201011223344u);
    BOOST_REQUIRE_EQUAL(ds.read_big_endian<uint32_t>(),  0x80402010u);
    BOOST_REQUIRE_EQUAL(ds.read_variable_uint(), 1234u);
    BOOST_REQUIRE_EQUAL(from_little_endian_unsafe<uint32_t>(
        ds.read_data(4).begin()), 0xbadf00du);
    BOOST_REQUIRE_EQUAL(ds.read_string(), "hello");
    BOOST_REQUIRE_THROW(ds.read_byte(), end_of_stream);
}

BOOST_AUTO_TEST_SUITE_END()
