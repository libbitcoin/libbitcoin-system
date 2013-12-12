/*
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
    set_public_key(addr, pubkey);
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

