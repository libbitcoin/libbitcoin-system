#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>
using namespace bc;

BOOST_AUTO_TEST_CASE(detwallet)
{
    deterministic_wallet wallet;
    BOOST_REQUIRE(wallet.set_seed("a219213f9b12422aa206d988e3e49607"));
    // master public key
    data_chunk mpk = wallet.master_public_key();
    BOOST_REQUIRE(encode_hex(mpk) == "d996c1a50ca4a57a9dface614338a1d837cb339e08361cfaf66ffd7da8e21786a7142a014056439d579654d7bb58dd5724b93372b5efae62e76783300f2b6cb5");
    // get an address
    payment_address addr;
    data_chunk pubkey = wallet.generate_public_key(2, true);
    set_public_key(addr, pubkey);
    BOOST_REQUIRE(addr.encoded() == "1MstsXwLK8pH7jxpG5HYettWVPSUxwdwgY");
    // get privkey
    secret_parameter secret = wallet.generate_secret(2, true);
    BOOST_REQUIRE(encode_hex(secret) == "d5c41f9c49ecbe48eb6fad73f6bc4a795dd1698813892cb0eeaa8362f85c83bc");
    elliptic_curve_key privkey;
    BOOST_REQUIRE(privkey.set_secret(secret));
    BOOST_REQUIRE(privkey.public_key() == pubkey);
    // new seed
    wallet.new_seed();

    deterministic_wallet wallet2;
    BOOST_REQUIRE(wallet2.set_master_public_key(mpk));
    BOOST_REQUIRE(wallet2.generate_public_key(2, true) == pubkey);
    BOOST_REQUIRE(wallet2.generate_secret(2) == null_hash);
}

BOOST_AUTO_TEST_CASE(mnemonic)
{
    const std::string seed = "148f0a1d77e20dbaee3ff920ca40240d";
    string_list words{
        "people", "blonde", "admit", "dart", "couple", "different",
        "truth", "common", "alas", "stumble", "time", "cookie"};
    BOOST_REQUIRE(encode_mnemonic(seed) == words);
    BOOST_REQUIRE(decode_mnemonic(words) == seed);
}

BOOST_AUTO_TEST_CASE(nullbyte)
{
     deterministic_wallet wallet;
     wallet.set_seed("a219213f9b12422aa206d988e3e49607");
     secret_parameter secret = wallet.generate_secret(10);
     BOOST_REQUIRE(secret[0] == 0x00);
}

BOOST_AUTO_TEST_CASE(copying)
{
    data_chunk mpk = bc::decode_hex(
        "3315ae236373067ea27d92f10f9475b1ff727eebe45f4ce4dd21cf548a237755397"
        "548d57fdb94610aef20993b4ff4695cae581d3be98743593336b21090c7d2");
    // Setting MPK.
    deterministic_wallet wallet1;
    wallet1.set_master_public_key(mpk);
    BOOST_REQUIRE(wallet1.master_public_key() == mpk);
    // Copying MPK.
    deterministic_wallet wallet2;
    wallet2.set_master_public_key(wallet1.master_public_key());
    BOOST_REQUIRE(wallet1.master_public_key() == wallet2.master_public_key());
    // Copy construction.
    deterministic_wallet wallet3 = wallet1;
    BOOST_REQUIRE(wallet1.master_public_key() == wallet3.master_public_key());
}

