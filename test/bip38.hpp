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
#ifndef LIBBITCOIN_TEST_BIP38_HPP
#define LIBBITCOIN_TEST_BIP38_HPP

#include <string>
#include <vector>
#include <bitcoin/bitcoin.hpp>

struct bip38_vector
{
    std::string locked;
    std::string unlocked;
    std::string passphrase;
    std::string intermediate;
    std::string confirmation_code;
    std::string random_seed_data;
    std::string address;
    bool compressed;
    bool ec_multiplied;
};

typedef std::vector<bip38_vector> bip38_vector_list;

// Vectors 0 through 7 taken from:
// github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#Test_vectors
// Above test vectors cannot be locked since they do not include the specific 
// random data used to generate them, which is needed to recreate the keys.
const bip38_vector_list bip38_test_vector
{
    {
        // #1 and #2 from: github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
        {
            "6PRVWUbkzzsbcVac2qwfssoUJAN1Xhrg6bNk8J7Nzm5H7kxEbn2Nh2ZoGg",
            "cbf4b9f70470856bb4f40f80b87edb90865997ffee6df315ab166d713af433a5",
            "TestingOneTwoThree",
            "",
            "",
            "",
            "",
            false,
            false
        },
        {
            "6PRNFFkZc2NZ6dJqFfhRoFNMR9Lnyj7dYGrzdgXXVMXcxoKTePPX1dWByq",
            "09c2686880095b1a4c249ee3ac4eea8a014f11e6f986d0b5025ac1f39afbd9ae",
            "Satoshi",
            "",
            "",
            "",
            "",
            false,
            false
        },
        // github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
        {
            "6PYNKZ1EAgYgmQfmNVamxyXVWHzK5s6DGhwP4J5o44cvXdoY7sRzhtpUeo",
            "cbf4b9f70470856bb4f40f80b87edb90865997ffee6df315ab166d713af433a5",
            "TestingOneTwoThree",
            "",
            "",
            "",
            "",
            true,
            false
        },
        {
            "6PYLtMnXvfG3oJde97zRyLYFZCYizPU5T3LwgdYJz1fRhh16bU7u6PPmY7",
            "09c2686880095b1a4c249ee3ac4eea8a014f11e6f986d0b5025ac1f39afbd9ae",
            "Satoshi",
            "",
            "",
            "",
            "",
            true,
            false
        },
        // github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#no-compression-no-ec-multiply
        {
            "6PfQu77ygVyJLZjfvMLyhLMQbYnu5uguoJJ4kMCLqWwPEdfpwANVS76gTX",
            "a43a940577f4e97f5c4d39eb14ff083a98187c64ea7c99ef7ce460833959a519",
            "TestingOneTwoThree",
            "passphrasepxFy57B9v8HtUsszJYKReoNDV6VHjUSGt8EVJmux9n1J3Ltf1gRxyDGXqnf9qm",
            "",
            "",
            "1PE6TQi6HTVNz5DLwB1LcpMBALubfuN2z2",
            false,
            true
        },
        {
            "6PfLGnQs6VZnrNpmVKfjotbnQuaJK4KZoPFrAjx1JMJUa1Ft8gnf5WxfKd",
            "c2c8036df268f498099350718c4a3ef3984d2be84618c2650f5171dcc5eb660a",
            "Satoshi",
            "passphraseoRDGAXTWzbp72eVbtUDdn1rwpgPUGjNZEc6CGBo8i5EC1FPW8wcnLdq4ThKzAS",
            "",
            "",
            "1CqzrtZC6mXSAhoxtFwVjz8LtwLJjDYU3V",
            false,
            true
        },
        // github.com/bitcoin/bips/blob/master/bip-0038.mediawiki#ec-multiply-no-compression-lotsequence-numbers
        {
            "6PgNBNNzDkKdhkT6uJntUXwwzQV8Rr2tZcbkDcuC9DZRsS6AtHts4Ypo1j",
            "44ea95afbf138356a05ea32110dfd627232d0f2991ad221187be356f19fa8190",
            "MOLON LABE",
            "passphraseaB8feaLQDENqCgr4gKZpmf4VoaT6qdjJNJiv7fsKvjqavcJxvuR1hy25aTu5sX",
            "cfrm38V8aXBn7JWA1ESmFMUn6erxeBGZGAxJPY4e36S9QWkzZKtaVqLNMgnifETYw7BPwWC9aPD",
            "",
            "1Jscj8ALrYu2y9TD8NrpvDBugPedmbj4Yh",
            false,
            true
        },
        {
            "6PgGWtx25kUg8QWvwuJAgorN6k9FbE25rv5dMRwu5SKMnfpfVe5mar2ngH",
            "ca2759aa4adb0f96c414f36abeb8db59342985be9fa50faac228c8e7d90e3006",
            "ΜΟΛΩΝ ΛΑΒΕ",
            "passphrased3z9rQJHSyBkNBwTRPkUGNVEVrUAcfAXDyRU1V28ie6hNFbqDwbFBvsTK7yWVK",
            "cfrm38V8G4qq2ywYEFfWLD5Cc6msj9UwsG2Mj4Z6QdGJAFQpdatZLavkgRd1i4iBMdRngDqDs51",
            "",
            "1Lurmih3KruL4xDB5FmHof38yawNtP9oGf",
            false,
            true
        },
        // generated and verified using bit2factor.com
        {
            "6PfPAw5HErFdzMyBvGMwSfSWjKmzgm3jDg7RxQyVCSSBJFZLAZ6hVupmpn",
            "fb4bfb0bfe151d524b0b11983b9f826d6a0bc7f7bdc480864a1b557ff0c59eb4",
            "libbitcoin test",
            "passphraseo59BauW85etaRsKpbbTrEa5RRYw6bq5K9yrDf4r4N5fcirPdtDKmfJw9oYNoGM",
            "cfrm38V5Nm1mn7GxPBAGTXawqXRwE1EbR19GqsvJ9JmF5VKLqi8nETmULpELkQvExCGkTNCH2An",
            "d36d8e703d8bd5445044178f69087657fba73d9f3ff211f7",
            "1NQgLnFz1ZzF6KkCJ4SM3xz3Jy1q2hEEax",
            false,
            true
        },
        {
            "6PfU2yS6DUHjgH8wmsJRT1rHWXRofmDV5UJ3dypocew56BDcw5TQJXFYfm",
            "97c745cc980e5a070e12d0bff3f539b70748aadb406045fc1b42d4ded559a564",
            "Libbitcoin BIP38 Test Vector",
            "passphraseouGLY8yjTZQ5Q2bTo8rtKfdbHz4tme7QuPheRgES8KnT6pX5yxFauYhv3SVPDD",
            "cfrm38V5ec4E5RKwBu46Jf5zfaE54nuB1NWHpHSpgX4GQqfzx7fvqm43mBHvr89pPgykDHts9VC",
            "bbeac8b9bb39381520b6873553544b387bcaa19112602230",
            "1NjjvGqXDrx1DvrhjYJxzrpyopk1ygHTSJ",
            false,
            true
        }
    }
};

#endif
