/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(secp256k1_tests)

const ec_secret one = base16_array("0000000000000000000000000000000000000000000000000000000000000001");
const ec_secret secret1 = base16_array("8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536");
const ec_secret secret3 = base16_array("33436393f770d9b3f5d11c20be561837300f89515284008965d2fd3f714b8fce");

// batch schnorr signature verification

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__all_valid__expected)
{
    using namespace system;
    using namespace system::schnorr;
    const auto hash = bitcoin_hash(to_chunk("batch-schnorr"));

    ec_compressed pub1{}, pub2{}, pub3{};
    BOOST_REQUIRE(secret_to_public(pub1, secret1));
    BOOST_REQUIRE(secret_to_public(pub2, secret3));
    BOOST_REQUIRE(secret_to_public(pub3, one));

    // TODO: add system utils: ec_secret -> ec_compressed -> ec_xonly.
    const auto& point1 = array_cast<uint8_t, ec_xonly_size, 1>(pub1);
    const auto& point2 = array_cast<uint8_t, ec_xonly_size, 1>(pub2);
    const auto& point3 = array_cast<uint8_t, ec_xonly_size, 1>(pub3);

    ec_signature sig1{}, sig2{}, sig3{};
    constexpr hash_digest auxiliary{};
    BOOST_REQUIRE(sign(sig1, secret1, hash, auxiliary));
    BOOST_REQUIRE(sign(sig2, secret3, hash, auxiliary));
    BOOST_REQUIRE(sign(sig3, one, hash, auxiliary));

    const std::array<signatures, 3> batch
    {
        signatures{ hash, point1, sig1, { 0, 0, 0 } },
        signatures{ hash, point2, sig2, { 1, 0, 0 } },
        signatures{ hash, point3, sig3, { 2, 0, 0 } }
    };

    const auto tokens = signatures::verify({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__one_invalid__expected)
{
    using namespace system;
    using namespace system::schnorr;
    const auto hash = bitcoin_hash(to_chunk("batch-schnorr-neg"));

    ec_compressed pub1{}, pub2{}, pub3{};
    BOOST_REQUIRE(secret_to_public(pub1, secret1));
    BOOST_REQUIRE(secret_to_public(pub2, secret3));
    BOOST_REQUIRE(secret_to_public(pub3, one));

    // TODO: add system utils: ec_secret -> ec_compressed -> ec_xonly.
    const auto& point1 = array_cast<uint8_t, ec_xonly_size, 1>(pub1);
    const auto& point2 = array_cast<uint8_t, ec_xonly_size, 1>(pub2);
    const auto& point3 = array_cast<uint8_t, ec_xonly_size, 1>(pub3);

    ec_signature sig1{}, sig2{}, sig3{};
    constexpr hash_digest auxiliary{};
    BOOST_REQUIRE(sign(sig1, secret1, hash, auxiliary));
    BOOST_REQUIRE(sign(sig2, secret3, hash, auxiliary));
    BOOST_REQUIRE(sign(sig3, one, hash, auxiliary));

    // corrupt second signature
    sig2[10] ^= 0xff;
    const std::array<signatures, 3> batch
    {
        signatures{ hash, point1, sig1, { 0, 0, 0 } },
        signatures{ hash, point2, sig2, { 1, 0, 0 } },
        signatures{ hash, point3, sig3, { 2, 0, 0 } }
    };

    const auto tokens = signatures::verify({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);
    BOOST_REQUIRE_EQUAL(tokens.front(), from_little_array<signatures::link_t>(batch.at(1).id));
}

// batch ecdsa signature verification

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__all_valid__expected)
{
    using namespace system;
    using namespace system::ecdsa;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa"));

    ec_compressed point1{}, point2{}, point3{};
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, secret3));
    BOOST_REQUIRE(secret_to_public(point3, one));

    ec_signature sig1{}, sig2{}, sig3{};
    BOOST_REQUIRE(sign(sig1, secret1, hash));
    BOOST_REQUIRE(sign(sig2, secret3, hash));
    BOOST_REQUIRE(sign(sig3, one, hash));

    const std::array<signatures, 3> batch
    {
        signatures{ hash, point1, sig1, { 0, 0, 0 } },
        signatures{ hash, point2, sig2, { 1, 0, 0 } },
        signatures{ hash, point3, sig3, { 2, 0, 0 } }
    };

    const auto tokens = signatures::verify({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__one_invalid__expected)
{
    using namespace system;
    using namespace system::ecdsa;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa-invalid"));

    ec_compressed point1{}, point2{}, point3{};
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, secret3));
    BOOST_REQUIRE(secret_to_public(point3, one));

    ec_signature sig1{}, sig2{}, sig3{};
    BOOST_REQUIRE(sign(sig1, secret1, hash));
    BOOST_REQUIRE(sign(sig2, secret3, hash));
    BOOST_REQUIRE(sign(sig3, one, hash));

    // corrupt third signature
    sig3[10] ^= 0xff;
    const std::array<signatures, 3> batch
    {
        signatures{ hash, point1, sig1, { 0, 0, 0 } },
        signatures{ hash, point2, sig2, { 1, 0, 0 } },
        signatures{ hash, point3, sig3, { 2, 0, 0 } }
    };

    const auto tokens = signatures::verify({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);
    BOOST_REQUIRE_EQUAL(tokens.front(), from_little_array<signatures::link_t>(batch.at(2).id));
}

// batch multisig (ecdsa) verification

BOOST_AUTO_TEST_CASE(secp256k1__multisig_batch_verify__all_valid__expected)
{
    using namespace system;
    using namespace system::multisig;

    const auto hash = bitcoin_hash(to_chunk("batch-multisig"));

    ec_compressed point1{}, point2{}, point3{};
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, secret3));
    BOOST_REQUIRE(secret_to_public(point3, one));

    ec_signature sig1{}, sig2{}, sig3{};
    BOOST_REQUIRE(ecdsa::sign(sig1, secret1, hash));
    BOOST_REQUIRE(ecdsa::sign(sig2, secret3, hash));
    BOOST_REQUIRE(ecdsa::sign(sig3, one, hash));

    // One 2-of-3 multisig group (same id + set)
    const std::array<signatures, 3> batch
    {
        signatures{ hash, point1, sig1, 0x12, 5, { 0, 0, 0 } }, // (sig 1, key 2)
        signatures{ hash, point2, sig2, 0x01, 5, { 0, 0, 0 } }, // (sig 0, key 1)
        signatures{ hash, point3, sig3, 0x20, 5, { 0, 0, 0 } }  // (sig 2, key 0)
    };

    const auto tokens = signatures::verify({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__multisig_batch_verify__one_invalid__expected)
{
    using namespace system;
    using namespace system::multisig;
    const auto hash = bitcoin_hash(to_chunk("batch-multisig-invalid"));

    ec_compressed point1{}, point2{}, point3{};
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, secret3));
    BOOST_REQUIRE(secret_to_public(point3, one));

    ec_signature sig1{}, sig2{}, sig3{};
    BOOST_REQUIRE(ecdsa::sign(sig1, secret1, hash));
    BOOST_REQUIRE(ecdsa::sign(sig2, secret3, hash));
    BOOST_REQUIRE(ecdsa::sign(sig3, one, hash));

    // corrupt second signature
    sig2[10] ^= 0xff;

    // Same group (id + set)
    const std::array<signatures, 3> batch
    {
        signatures{ hash, point1, sig1, 0x12, 7, { 0, 0, 0 } },
        signatures{ hash, point2, sig2, 0x01, 7, { 0, 0, 0 } },
        signatures{ hash, point3, sig3, 0x20, 7, { 0, 0, 0 } }
    };

    const auto tokens = signatures::verify({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);
    BOOST_REQUIRE_EQUAL(tokens.front(), from_little_array<signatures::link_t>(batch.at(0).id));
}

BOOST_AUTO_TEST_SUITE_END()
