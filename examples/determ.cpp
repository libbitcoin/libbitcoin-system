/*
  Demonstration of deterministic wallet.
*/
#include <bitcoin/bitcoin.hpp>
using namespace bc;

int main()
{
    deterministic_wallet wallet;
    // Set seed.
    if (!wallet.set_seed("a219213f9b12422aa206d988e3e49607"))
        log_error() << "Error setting seed.";

    // Get an address from wallet...
    data_chunk pubkey = wallet.generate_public_key(2);
    payment_address addr;
    if (!set_public_key(addr, pubkey))
        log_error() << "Error setting public key.";
    assert(addr.encoded() == "1E4vM9q25xsyDwWwdqHUWnwshdWC9PykmL");

    // ... Get the corresponding private key.
    // Extract the secret parameter.
    secret_parameter secret = wallet.generate_secret(2);
    assert(encode_hex(secret) == "33cc7e35fbb78d17d207e53d0fe950d1db571be889b3ff87aec653e501759264");
    // The secret parameter is used to compute the private key
    // by the elliptic curve formula.
    elliptic_curve_key privkey;
    if (!privkey.set_secret(secret))
        log_error() << "Error set private key.";
    // Wallet generated public key should match corresponding public key
    // in the private key.
    assert(privkey.public_key() == pubkey);

    // Master public key
    data_chunk mpk = wallet.master_public_key();
    assert(encode_hex(mpk) == "d996c1a50ca4a57a9dface614338a1d837cb339e08361cfaf66ffd7da8e21786a7142a014056439d579654d7bb58dd5724b93372b5efae62e76783300f2b6cb5");

    // A master key can only generate public keys and not the private keys.
    deterministic_wallet wallet2;
    wallet2.set_master_public_key(mpk);
    assert(wallet2.generate_public_key(2) == pubkey);
    // Trying to generate the secret parameter will always return null_hash.
    assert(wallet2.generate_secret(2) == null_hash);
    return 0;
}

