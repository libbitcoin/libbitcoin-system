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
    BITCOIN_ASSERT(encode_hex(mpk) == "d996c1a50ca4a57a9dface614338a1d837cb339e08361cfaf66ffd7da8e21786a7142a014056439d579654d7bb58dd5724b93372b5efae62e76783300f2b6cb5");
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
    BITCOIN_ASSERT(encode_hex(secret) == "d5c41f9c49ecbe48eb6fad73f6bc4a795dd1698813892cb0eeaa8362f85c83bc");
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

