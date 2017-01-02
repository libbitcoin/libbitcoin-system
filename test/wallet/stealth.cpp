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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <bitcoin/bitcoin.hpp>

#include <iostream>

using namespace bc;
using namespace bc::wallet;

BOOST_AUTO_TEST_SUITE(wallet_stealth_tests)

#define MAIN_KEY "tprv8ctN3HAF9dCgX9ggdCwiZHa7c3UHuG2Ev4jgYWDhTHDUVWKKsg7znbr3vYtmCzVqcMQsjd9cSKsyKGaDvTAUMkw1UphETe1j8LcT21eWPkH"
#define STEALTH_ADDRESS "vJmudwspxzmEoz1AP5tTrRMcuop6XjNWa1SnjHFmLeSc9DAkro6J6oYnD7MubLHx9wT3rm7D6xgA8U9Lr9zjzijhVSuUbYdMNYUN27"
#define EPHEMERAL_PRIVATE "f91e673103863bbeb0ef1852cd8eade6b73ea55afc9b1873be62bf628eac072a"
#define RECEIVER_PRIVATE "fc696c9f7143916f24977210c806101866c7fa13cc06982978d80518c91af2fb"

#define DERIVED_ADDRESS "mtKffkQLTw2D6f6mTkrWfi8qxLv4jL1LrK"

ec_compressed extract_ephemeral_public(const chain::script& meta_script)
{
    ec_compressed ephemeral_public;
    BOOST_REQUIRE(is_stealth_script(meta_script));
    ephemeral_public[0] = ephemeral_public_key_sign;
    const data_chunk& data = meta_script.operations()[1].data();
    std::copy(data.begin(), data.end(), ephemeral_public.begin() + 1);
    return ephemeral_public;
}

BOOST_AUTO_TEST_CASE(stealth__exchange_between_sender_and_receiver)
{
    hd_private main_key(MAIN_KEY, hd_private::testnet);
    const auto scan_key = main_key.derive_private(
        0 + hd_first_hardened_key);
    const auto spend_key = main_key.derive_private(
        1 + hd_first_hardened_key);
    const auto scan_private = scan_key.secret();
    const auto spend_private = spend_key.secret();

    stealth_receiver receiver(scan_private, spend_private,
        payment_address::testnet_p2kh);
    stealth_sender sender(payment_address::testnet_p2kh);

    const stealth_address stealth_addr = receiver.generate_stealth_address();
    BOOST_REQUIRE_EQUAL(stealth_addr.encoded(), STEALTH_ADDRESS);

    // Instead of generating a random ephemeral_private, we are going to
    // use this one instead.
    ec_secret ephemeral_private;
    BOOST_REQUIRE(decode_base16(ephemeral_private, EPHEMERAL_PRIVATE));
    sender.send_to_stealth_address(stealth_addr, ephemeral_private);

    /////////////////////////////////////////
    // Send sends BTC to send_address
    // Output before is ephemeral_public
    // Padded to 40 bytes
    /////////////////////////////////////////

    BOOST_REQUIRE_EQUAL(sender.send_address().encoded(), DERIVED_ADDRESS);

    // Rows:
    //   [ephemkey:32] [address:20] [tx_hash:32]

    // client.fetch_stealth()

    /////////////////////////////////////////
    // Receiver scans the blockchain
    // And gets a list of keys and addresses for scanning
    /////////////////////////////////////////

    // Normally this is returned by the blockchain server.
    ec_compressed ephemeral_public = extract_ephemeral_public(
        sender.meta_script());

    // Now the receiver should be able to regenerate the send_address
    // using just the ephemeral_public.
    const auto derived_address = receiver.derive_address(ephemeral_public);
    BOOST_REQUIRE_EQUAL(derived_address, sender.send_address());

    // Only reciever can derive stealth private, as it requires both scan
    // and spend private.
    const auto& receiver_private = receiver.derive_private(ephemeral_public);
    ec_compressed receiver_public;
    secret_to_public(receiver_public, receiver_private);

    payment_address receiver_address(receiver_public,
        payment_address::testnet_p2kh);

    // Now we've re-derived the same key again.
    // And we also have the private key.
    BOOST_REQUIRE_EQUAL(receiver_address, sender.send_address());
    BOOST_REQUIRE_EQUAL(encode_base16(receiver_private), RECEIVER_PRIVATE);
}

BOOST_AUTO_TEST_SUITE_END()

