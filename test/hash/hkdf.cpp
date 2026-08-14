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
#include "../test.hpp"

BOOST_AUTO_TEST_SUITE(hkdf_tests)

// rfc5869 appendix A test case 1.
BOOST_AUTO_TEST_CASE(hkdf__sha256__rfc5869_case_1__expected)
{
    const auto ikm = base16_chunk("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b");
    const auto salt = base16_chunk("000102030405060708090a0b0c");
    const auto info = base16_chunk("f0f1f2f3f4f5f6f7f8f9");
    const auto prk_expected = base16_array(
        "077709362c2e32df0ddc3f0dc47bba6390b6c73bb50f9c3122ec844ad7c2b3e5");
    const auto okm_expected = base16_array(
        "3cb25f25faacd57a90434f64d0362f2a2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
        "34007208d5b887185865");

    const auto prk = hkdf<sha256>::extract(ikm, salt);
    BOOST_REQUIRE_EQUAL(prk, prk_expected);

    data_array<42> okm{};
    hkdf<sha256>::expand(okm, prk, info);
    BOOST_REQUIRE_EQUAL(okm, okm_expected);

    // Combined extract-and-expand.
    const auto combined = hkdf<sha256>::key<42>(ikm, salt, info);
    BOOST_REQUIRE_EQUAL(combined, okm_expected);
}

// rfc5869 appendix A test case 2 (longer inputs/outputs).
BOOST_AUTO_TEST_CASE(hkdf__sha256__rfc5869_case_2__expected)
{
    const auto ikm = base16_chunk(
        "000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"
        "202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f"
        "404142434445464748494a4b4c4d4e4f");
    const auto salt = base16_chunk(
        "606162636465666768696a6b6c6d6e6f707172737475767778797a7b7c7d7e7f"
        "808182838485868788898a8b8c8d8e8f909192939495969798999a9b9c9d9e9f"
        "a0a1a2a3a4a5a6a7a8a9aaabacadaeaf");
    const auto info = base16_chunk(
        "b0b1b2b3b4b5b6b7b8b9babbbcbdbebfc0c1c2c3c4c5c6c7c8c9cacbcccdcecf"
        "d0d1d2d3d4d5d6d7d8d9dadbdcdddedfe0e1e2e3e4e5e6e7e8e9eaebecedeeef"
        "f0f1f2f3f4f5f6f7f8f9fafbfcfdfeff");
    const auto okm_expected = base16_array(
        "b11e398dc80327a1c8e7f78c596a49344f012eda2d4efad8a050cc4c19afa97c"
        "59045a99cac7827271cb41c65e590e09da3275600c2f09b8367793a9aca3db71"
        "cc30c58179ec3e87c14c01d5c1f3434f1d87");

    data_array<82> okm{};
    hkdf<sha256>::key(okm, ikm, salt, info);
    BOOST_REQUIRE_EQUAL(okm, okm_expected);
}

// rfc5869 appendix A test case 3 (empty salt and info).
BOOST_AUTO_TEST_CASE(hkdf__sha256__rfc5869_case_3__expected)
{
    const auto ikm = base16_chunk("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b");
    const auto okm_expected = base16_array(
        "8da4e775a563c18f715f802a063c5a31b8a11f5c5ee1879ec3454e5f3c738d2d"
        "9d201395faa4b61a96c8");

    data_array<42> okm{};
    hkdf<sha256>::key(okm, ikm, {}, {});
    BOOST_REQUIRE_EQUAL(okm, okm_expected);
}

// bip324 key derivation (from bips.dev/324 packet encoding test vector 1).
BOOST_AUTO_TEST_CASE(hkdf__sha256__bip324_session_id__expected)
{
    // v2_ecdh shared secret for vector 1 (see secp256k1 ellswift tests).
    const auto secret = base16_chunk(
        "c6992a117f5edbea70c3f511d32d26b9798be4b81a62eaee1a5acaa8459a3592");
    const auto session_id = base16_array(
        "ce72dffb015da62b0d0f5474cab8bc72605225b0cee3f62312ec680ec5f41ba5");

    // Salt is "bitcoin_v2_shared_secret" + mainnet magic (f9beb4d9).
    const auto salt = build_chunk(
    {
        to_chunk("bitcoin_v2_shared_secret"),
        base16_chunk("f9beb4d9")
    });

    const auto prk = hkdf<sha256>::extract(secret, salt);

    data_array<32> okm{};
    hkdf<sha256>::expand(okm, prk, to_chunk("session_id"));
    BOOST_REQUIRE_EQUAL(okm, session_id);
}

BOOST_AUTO_TEST_SUITE_END()
