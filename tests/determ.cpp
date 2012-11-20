#include <bitcoin/bitcoin.hpp>
using namespace bc;

int main()
{
    deterministic_wallet wallet;
    bool set_seed_success =
        wallet.set_seed("a219213f9b12422aa206d988e3e49607");
    BITCOIN_ASSERT(set_seed_success);
    // master public key
    data_chunk mpk = wallet.master_public_key();
    log_info() << "mpk: " << mpk;
    BITCOIN_ASSERT(pretty_hex(mpk) == "d996c1a50ca4a57a9dface614338a1d837cb339e08361cfaf66ffd7da8e21786a7142a014056439d579654d7bb58dd5724b93372b5efae62e76783300f2b6cb5");
    // get an address
    payment_address addr;
    data_chunk pubkey = wallet.generate_public_key(2, true);
    bool set_pubkey_success = set_public_key(addr, pubkey);
    BITCOIN_ASSERT(set_pubkey_success);
    log_info() << addr.encoded();
    BITCOIN_ASSERT(addr.encoded() == "1MstsXwLK8pH7jxpG5HYettWVPSUxwdwgY");
    // get privkey
    secret_parameter secret = wallet.generate_secret(2, true);
    log_info() << "secret: " << secret;
    BITCOIN_ASSERT(pretty_hex(secret) == "d5c41f9c49ecbe48eb6fad73f6bc4a795dd1698813892cb0eeaa8362f85c83bc");
    elliptic_curve_key privkey;
    bool set_secret_success = privkey.set_secret(secret);
    BITCOIN_ASSERT(set_secret_success);
    BITCOIN_ASSERT(privkey.public_key() == pubkey);
    // new seed
    wallet.new_seed();
    log_info() << "new seed: " << wallet.seed();

    deterministic_wallet wallet2;
    wallet2.set_master_public_key(mpk);
    BITCOIN_ASSERT(wallet2.generate_public_key(2, true) == pubkey);
    BITCOIN_ASSERT(wallet2.generate_secret(2) == null_hash);
    return 0;
}

