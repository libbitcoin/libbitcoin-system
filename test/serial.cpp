#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
using namespace bc;

BOOST_AUTO_TEST_CASE(serialize)
{
    const std::string rawdat_repr =
        "46682488f0a721124a3905a1bb72445bf13493e2cd46c5c0c8db1c15afa0d58e00000000";
    data_chunk rawdat(decode_hex(rawdat_repr));
    BOOST_REQUIRE(rawdat == (data_chunk{0x46, 0x68, 0x24, 0x88, 0xf0, 0xa7,
                0x21, 0x12, 0x4a, 0x39, 0x05, 0xa1, 0xbb, 0x72, 0x44, 0x5b,
                0xf1, 0x34, 0x93, 0xe2, 0xcd, 0x46, 0xc5, 0xc0, 0xc8, 0xdb,
                0x1c, 0x15, 0xaf, 0xa0, 0xd5, 0x8e, 0x00, 0x00, 0x00, 0x00}));
    auto deserial = make_deserializer(rawdat.begin(), rawdat.end());
    output_point outpoint;
    outpoint.hash = deserial.read_hash();
    outpoint.index = deserial.read_4_bytes();
    BOOST_REQUIRE(outpoint.hash == (hash_digest{0x8e, 0xd5, 0xa0, 0xaf, 0x15, 0x1c, 0xdb, 0xc8, 0xc0, 0xc5, 0x46, 0xcd, 0xe2, 0x93, 0x34, 0xf1, 0x5b, 0x44, 0x72, 0xbb, 0xa1, 0x05, 0x39, 0x4a, 0x12, 0x21, 0xa7, 0xf0, 0x88, 0x24, 0x68, 0x46}));
    BOOST_REQUIRE(outpoint.index == 0);
    data_chunk buff(36);
    auto serial = make_serializer(buff.begin());
    serial.write_hash(outpoint.hash);
    serial.write_4_bytes(outpoint.index);
    BOOST_REQUIRE(buff == rawdat);
}

BOOST_AUTO_TEST_CASE(genesis_block_serialize)
{
    block_type genblk = genesis_block();
    BOOST_REQUIRE_EQUAL(satoshi_raw_size(genblk), 285);
    BOOST_REQUIRE_EQUAL(satoshi_raw_size(genblk.header), 80);
    data_chunk rawblk(285);
    BOOST_REQUIRE_EQUAL(std::distance(rawblk.begin(), rawblk.end()), 285);
    // Save genesis block.
    auto end_iter = satoshi_save(genblk, rawblk.begin());
    BOOST_REQUIRE_EQUAL(std::distance(rawblk.begin(), end_iter), 285);
    BOOST_REQUIRE(end_iter == rawblk.end());
    // Reload genesis block.
    block_type blk;
    satoshi_load(rawblk.begin(), rawblk.end(), blk);
    BOOST_REQUIRE(genblk.header == blk.header);
    const hash_digest& merkle = generate_merkle_root(blk.transactions);
    BOOST_REQUIRE(genblk.header.merkle == merkle);
}

BOOST_AUTO_TEST_CASE(junk)
{
    data_chunk junk = decode_hex(
        "000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0"
        "000000000000005739943a9c29a1955dfae2b3f37de547005bfb9535192e5fb0");
    transaction_type tx;
    satoshi_load(junk.begin(), junk.end(), tx);
}

BOOST_AUTO_TEST_CASE(tx)
{
    hash_digest tx_hash_1 = decode_hex_digest<hash_digest>(
        "bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");
    data_chunk raw_tx_1 = decode_hex(
        "0100000001f08e44a96bfb5ae63eda1a6620adae37ee37ee4777fb0336e1bbbc"
        "4de65310fc010000006a473044022050d8368cacf9bf1b8fb1f7cfd9aff63294"
        "789eb1760139e7ef41f083726dadc4022067796354aba8f2e02363c5e510aa7e"
        "2830b115472fb31de67d16972867f13945012103e589480b2f746381fca01a9b"
        "12c517b7a482a203c8b2742985da0ac72cc078f2ffffffff02f0c9c467000000"
        "001976a914d9d78e26df4e4601cf9b26d09c7b280ee764469f88ac80c4600f00"
        "0000001976a9141ee32412020a324b93b1a1acfdfff6ab9ca8fac288ac000000"
        "00");
    BOOST_REQUIRE(raw_tx_1.size() == 225);
    transaction_type tx_1;
    satoshi_load(raw_tx_1.begin(), raw_tx_1.end(), tx_1);
    BOOST_REQUIRE(hash_transaction(tx_1) == tx_hash_1);
    // Re-save tx and compare against original.
    BOOST_REQUIRE(satoshi_raw_size(tx_1) == raw_tx_1.size());
    data_chunk resave_1(satoshi_raw_size(tx_1));
    satoshi_save(tx_1, resave_1.begin());
    BOOST_REQUIRE(resave_1 == raw_tx_1);

    hash_digest tx_hash_2 = decode_hex_digest<hash_digest>(
        "8a6d9302fbe24f0ec756a94ecfc837eaffe16c43d1e68c62dfe980d99eea556f");
    data_chunk raw_tx_2 = decode_hex(
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
    BOOST_REQUIRE(raw_tx_2.size() == 523);
    transaction_type tx_2;
    satoshi_load(raw_tx_2.begin(), raw_tx_2.end(), tx_2);
    BOOST_REQUIRE(hash_transaction(tx_2) == tx_hash_2);
    // Re-save tx and compare against original.
    BOOST_REQUIRE(satoshi_raw_size(tx_2) == raw_tx_2.size());
    data_chunk resave_2(satoshi_raw_size(tx_2));
    satoshi_save(tx_2, resave_2.begin());
    BOOST_REQUIRE(resave_2 == raw_tx_2);
}

BOOST_AUTO_TEST_CASE(script_parse_save)
{
    BOOST_REQUIRE_THROW(
    script psc = parse_script(decode_hex(
        "3045022100ff1fc58dbd608e5e05846a8e6b45a46ad49878aef6879ad1a7cf4c"
        "5a7f853683022074a6a10f6053ab3cddc5620d169c7374cd42c1416c51b9744d"
        "b2c8d9febfb84d01")),
    end_of_stream);

    data_chunk normal_output_script = decode_hex(
        "76a91406ccef231c2db72526df9338894ccf9355e8f12188ac");
    script out_scr = parse_script(normal_output_script);
    BOOST_REQUIRE(save_script(out_scr) == normal_output_script);
}

