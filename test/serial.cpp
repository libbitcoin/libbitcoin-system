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

