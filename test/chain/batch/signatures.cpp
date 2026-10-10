/**
 * Copyright (c) 2011-2026 libbitcoin developers
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(signatures_tests)

using namespace system::chain;

const hash_digest ecdsa_sighash = base16_array
(
    "504d68beac187dd0b259ddd6ed6d5d6348150b9b23ee6dfdb43e87f74dd3c547"
);
const ec_compressed ecdsa_key = base16_array
(
    "039cfcfe4a5d0efad27382e5d2b478eb398a8b691a66e01c878b600b5042b33166"
);
const ec_signature ecdsa_sig = base16_array
(
    "b434c7c720d63d71e1136d740df7ff636770ce59cb0389ae8cd24c0d4441f143"
    "01f4e1dbc36f32b4683faeecc8e4b2c6810da69e98fd783f1aad105636c3da08"
);
const hash_digest schnorr_sighash = base16_array
(
    "0000000000000000000000000000000000000000000000000000000000000000"
);
const ec_xonly schnorr_key = base16_array
(
    "f9308a019258c31049344f85f89d5229b531c845836f99b08601f113bce036f9"
);
const ec_signature schnorr_sig = base16_array
(
    "e907831f80848d1069a5371b402410364bdf1c5f8307b0084c55f1ce2dca8215"
    "25f66a4a85ea8b71e482a74f382d2ce5ebeee8fdb2172f477df4900d310536c0"
);

// multisig

BOOST_AUTO_TEST_CASE(signatures__multisig_rows__two_of_three__four)
{
    BOOST_REQUIRE_EQUAL(multisig::rows(2, 3), 4u);
}

BOOST_AUTO_TEST_CASE(signatures__multisig_rows__sigs_above_keys__zero)
{
    BOOST_REQUIRE_EQUAL(multisig::rows(2, 1), zero);
}

BOOST_AUTO_TEST_CASE(signatures__multisig_rows__maximum_gap__zero)
{
    BOOST_REQUIRE_EQUAL(multisig::rows(0, max_size_t), zero);
}

BOOST_AUTO_TEST_CASE(signatures__multisig_rows__product_overflow__zero)
{
    constexpr auto half = power2<size_t>(to_half(bits<size_t>));
    BOOST_REQUIRE_EQUAL(multisig::rows(half, half + half), zero);
}

// signatures (thread statics)

BOOST_AUTO_TEST_CASE(signatures__signatures__thread_statics__stable_and_purgeable)
{
    auto& ecdsa = signatures::ecdsa_rows();
    auto& schnorr = signatures::schnorr_rows();
    BOOST_REQUIRE_EQUAL(&signatures::ecdsa_rows(), &ecdsa);
    BOOST_REQUIRE_EQUAL(&signatures::schnorr_rows(), &schnorr);

    const signatures capture{ .enabled = true };
    BOOST_REQUIRE(capture.ecdsa(ecdsa_sighash, ecdsa_key, ecdsa_sig));
    BOOST_REQUIRE(capture.schnorr(schnorr_sighash, schnorr_key, schnorr_sig));
    BOOST_REQUIRE_EQUAL(ecdsa.groups(), 1u);
    BOOST_REQUIRE_EQUAL(schnorr.rows().size(), 1u);

    signatures::purge();
    BOOST_REQUIRE(ecdsa.empty());
    BOOST_REQUIRE(schnorr.empty());
}

BOOST_AUTO_TEST_CASE(signatures__signatures__threshold_cursor__appends_rows)
{
    signatures::purge();
    auto& schnorr = signatures::schnorr_rows();

    const signatures capture{ .enabled = true };
    auto cursor = capture.threshold(two);
    BOOST_REQUIRE(cursor.is_open());
    BOOST_REQUIRE_EQUAL(schnorr.thresholds(), 2u);

    cursor.write(schnorr_sighash, schnorr_key, schnorr_sig);
    BOOST_REQUIRE(cursor.is_open());
    cursor.write(schnorr_sighash, schnorr_key, schnorr_sig);
    BOOST_REQUIRE(!cursor.is_open());
    BOOST_REQUIRE_EQUAL(schnorr.rows().size(), 2u);

    signatures::purge();
}

BOOST_AUTO_TEST_SUITE_END()
