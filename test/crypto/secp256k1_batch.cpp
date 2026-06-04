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

const ec_secret secret1 = base16_array("8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536");
const ec_secret secret3 = base16_array("33436393f770d9b3f5d11c20be561837300f89515284008965d2fd3f714b8fce");
const ec_secret one = base16_array("0000000000000000000000000000000000000000000000000000000000000001");

// batch schnorr signature verification

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__all_valid__expected)
{
    using namespace system;
    using namespace system::schnorr;
    const auto hash = bitcoin_hash(to_chunk("batch-schnorr"));

    ec_compressed p1{};
    ec_compressed p2{};
    ec_compressed p3{};
    BOOST_REQUIRE(secret_to_public(p1, secret1));
    BOOST_REQUIRE(secret_to_public(p2, secret3));
    BOOST_REQUIRE(secret_to_public(p3, one));

    ec_xonly point1{};
    ec_xonly point2{};
    ec_xonly point3{};
    std::copy_n(std::next(p1.begin()), ec_xonly_size, point1.begin());
    std::copy_n(std::next(p2.begin()), ec_xonly_size, point2.begin());
    std::copy_n(std::next(p3.begin()), ec_xonly_size, point3.begin());

    ec_signature sig1{};
    ec_signature sig2{};
    ec_signature sig3{};
    constexpr hash_digest auxiliary{};
    BOOST_REQUIRE(sign(sig1, secret1, hash, auxiliary));
    BOOST_REQUIRE(sign(sig2, secret3, hash, auxiliary));
    BOOST_REQUIRE(sign(sig3, one, hash, auxiliary));

    const std::array<triple, 3> batch
    {
        triple{ hash, point1, sig1, { 0, 0, 0 } },
        triple{ hash, point2, sig2, { 1, 0, 0 } },
        triple{ hash, point3, sig3, { 2, 0, 0 } }
    };

    const auto tokens = verify_signatures({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__one_invalid__expected)
{
    using namespace system;
    using namespace system::schnorr;
    const auto hash = bitcoin_hash(to_chunk("batch-schnorr-neg"));

    ec_compressed p1{};
    ec_compressed p2{};
    ec_compressed p3{};
    BOOST_REQUIRE(secret_to_public(p1, secret1));
    BOOST_REQUIRE(secret_to_public(p2, secret3));
    BOOST_REQUIRE(secret_to_public(p3, one));

    ec_xonly point1{};
    ec_xonly point2{};
    ec_xonly point3{};
    std::copy_n(std::next(p1.begin()), ec_xonly_size, point1.begin());
    std::copy_n(std::next(p2.begin()), ec_xonly_size, point2.begin());
    std::copy_n(std::next(p3.begin()), ec_xonly_size, point3.begin());

    ec_signature sig1{};
    ec_signature sig2{};
    ec_signature sig3{};
    constexpr hash_digest auxiliary{};
    BOOST_REQUIRE(sign(sig1, secret1, hash, auxiliary));
    BOOST_REQUIRE(sign(sig2, secret3, hash, auxiliary));
    BOOST_REQUIRE(sign(sig3, one, hash, auxiliary));

    // corrupt signature of second row
    sig2[10] ^= 0xff;

    const std::array<triple, 3> batch
    {
        triple{ hash, point1, sig1, { 0, 0, 0 } },
        triple{ hash, point2, sig2, { 1, 0, 0 } },
        triple{ hash, point3, sig3, { 2, 0, 0 } }
    };

    const auto tokens = verify_signatures({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);
    BOOST_REQUIRE_EQUAL(tokens[0], batch[1].identifier);
}

// batch ecdsa signature verification

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__all_valid__expected)
{
    using namespace system;
    using namespace system::ecdsa;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa"));

    ec_compressed point1{};
    ec_compressed point2{};
    ec_compressed point3{};
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, secret3));
    BOOST_REQUIRE(secret_to_public(point3, one));

    ec_signature sig1{};
    ec_signature sig2{};
    ec_signature sig3{};
    BOOST_REQUIRE(sign(sig1, secret1, hash));
    BOOST_REQUIRE(sign(sig2, secret3, hash));
    BOOST_REQUIRE(sign(sig3, one, hash));

    const std::array<triple, 3> batch
    {
        triple{ hash, point1, sig1, { 0, 0, 0 } },
        triple{ hash, point2, sig2, { 1, 0, 0 } },
        triple{ hash, point3, sig3, { 2, 0, 0 } }
    };

    const auto tokens = verify_signatures({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__one_invalid_with_key__expected)
{
    using namespace system;
    using namespace system::ecdsa;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa-key"));

    ec_compressed point1{};
    ec_compressed point2{};
    ec_compressed point3{};
    ec_compressed point4{};
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, secret3));
    BOOST_REQUIRE(secret_to_public(point3, one));
    BOOST_REQUIRE(secret_to_public(point4, secret1));

    ec_signature sig1{};
    ec_signature sig2{};
    ec_signature sig3{};
    ec_signature sig4{};
    BOOST_REQUIRE(sign(sig1, secret1, hash));
    BOOST_REQUIRE(sign(sig2, secret3, hash));
    BOOST_REQUIRE(sign(sig3, one, hash));
    BOOST_REQUIRE(sign(sig4, secret1, hash));

    // corrupt signature of third row
    sig3[10] ^= 0xff;

    const std::array<triple, 4> batch
    {
        triple{ hash, point1, sig1, { 0, 0, 0 } },
        triple{ hash, point2, sig2, { 1, 0, 0 } },
        triple{ hash, point3, sig3, { 2, 0, 0 } },
        triple{ hash, point4, sig4, { 3, 0, 0 } }
    };

    const auto tokens = verify_signatures({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);
    BOOST_REQUIRE_EQUAL(tokens[0], batch[2].identifier);
}

// batch multisig (ecdsa) verification

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_multisig_batch_verify__all_valid__expected)
{
    using namespace system;
    using namespace system::multisig;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa"));

    ec_compressed point1{};
    ec_compressed point2{};
    ec_compressed point3{};
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, secret3));
    BOOST_REQUIRE(secret_to_public(point3, one));

    ec_signature sig1{};
    ec_signature sig2{};
    ec_signature sig3{};
    BOOST_REQUIRE(ecdsa::sign(sig1, secret1, hash));
    BOOST_REQUIRE(ecdsa::sign(sig2, secret3, hash));
    BOOST_REQUIRE(ecdsa::sign(sig3, one, hash));

    const std::array<triple, 3> batch
    {
        triple{ hash, point1, sig1, 3_u8, 6_u16, { 0, 0, 0 } },
        triple{ hash, point2, sig2, 4_u8, 7_u16, { 1, 0, 0 } },
        triple{ hash, point3, sig3, 5_u8, 8_u16, { 2, 0, 0 } }
    };

    const auto tokens = verify_signatures({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_multisig_batch_verify__one_invalid_with_key__expected)
{
    using namespace system;
    using namespace system::multisig;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa-key"));

    ec_compressed point1{};
    ec_compressed point2{};
    ec_compressed point3{};
    ec_compressed point4{};
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, secret3));
    BOOST_REQUIRE(secret_to_public(point3, one));
    BOOST_REQUIRE(secret_to_public(point4, secret1));

    ec_signature sig1{};
    ec_signature sig2{};
    ec_signature sig3{};
    ec_signature sig4{};
    BOOST_REQUIRE(ecdsa::sign(sig1, secret1, hash));
    BOOST_REQUIRE(ecdsa::sign(sig2, secret3, hash));
    BOOST_REQUIRE(ecdsa::sign(sig3, one, hash));
    BOOST_REQUIRE(ecdsa::sign(sig4, secret1, hash));

    // corrupt signature of third row
    sig3[10] ^= 0xff;

    const std::array<triple, 4> batch
    {
        triple{ hash, point1, sig1, 4_u8,  8_u16, { 0, 0, 0 } },
        triple{ hash, point2, sig2, 5_u8,  9_u16, { 1, 0, 0 } },
        triple{ hash, point3, sig3, 6_u8, 10_u16, { 2, 0, 0 } },
        triple{ hash, point4, sig4, 7_u8, 11_u16, { 3, 0, 0 } }
    };

    // TODO: update for multisig correlation when implemented.
    const auto tokens = verify_signatures({ batch.data(), batch.size() }, false);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);
    BOOST_REQUIRE_EQUAL(tokens[0], batch[2].identifier);
}

BOOST_AUTO_TEST_SUITE_END()
