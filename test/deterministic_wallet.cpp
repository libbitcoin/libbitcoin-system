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
    BOOST_REQUIRE(set_public_key(addr, pubkey));
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

