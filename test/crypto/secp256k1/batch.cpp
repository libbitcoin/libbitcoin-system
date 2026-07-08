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
#include "../../test.hpp"

BC_PUSH_WARNING(NO_USE_OF_SPAN)

BOOST_AUTO_TEST_SUITE(secp256k1_tests)

const ec_secret one = base16_array
(
    "0000000000000000000000000000000000000000000000000000000000000001"
);
const ec_secret secret0 = base16_array
(
    "8010b1bb119ad37d4b65a1022a314897b1b3614b345974332cb1b9582cf03536"
);
const ec_secret secret1 = base16_array
(
    "33436393f770d9b3f5d11c20be561837300f89515284008965d2fd3f714b8fce"
);

// batch ecdsa
// ----------------------------------------------------------------------------
// SoA batch: four corresponding columns (correlates, digests, points,
// signatures). correlate_t is id-first: { id, pair, group }.

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__singles_all_valid__expected)
{
    using namespace system;
    using namespace system::ecdsa;
    using correlate = batch::correlate_t;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa"));

    ec_compressed point0{}, point1{}, point2{};
    BOOST_REQUIRE(secret_to_public(point0, secret0));
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, one));

    ec_signature sig0{}, sig1{}, sig2{};
    BOOST_REQUIRE(sign(sig0, secret0, hash));
    BOOST_REQUIRE(sign(sig1, secret1, hash));
    BOOST_REQUIRE(sign(sig2, one, hash));

    const std::array<correlate, 3> correlates
    {
        correlate{ { 0, 0, 0 }, 0, 0 },
        correlate{ { 1, 0, 0 }, 0, 0 },
        correlate{ { 2, 0, 0 }, 0, 0 }
    };
    const std::array<hash_digest, 3> digests{ hash, hash, hash };
    const std::array<ec_compressed, 3> points{ point0, point1, point2 };
    const std::array<ec_signature, 3> signatures{ sig0, sig1, sig2 };

    const batch in
    {
        { correlates.data(), correlates.size() },
        { digests.data(), digests.size() },
        { points.data(), points.size() },
        { signatures.data(), signatures.size() }
    };

    const stopper cancel{};
    const auto tokens = batch::verify(cancel, in);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__singles_one_invalid__expected)
{
    using namespace system;
    using namespace system::ecdsa;
    using correlate = batch::correlate_t;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa-invalid"));

    ec_compressed point0{}, point1{}, point2{};
    BOOST_REQUIRE(secret_to_public(point0, secret0));
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, one));

    ec_signature sig0{}, sig1{}, sig2{};
    BOOST_REQUIRE(sign(sig0, secret0, hash));
    BOOST_REQUIRE(sign(sig1, secret1, hash));
    BOOST_REQUIRE(sign(sig2, one, hash));

    // corrupt third signature
    sig2[10] ^= 0xff;

    const std::array<correlate, 3> correlates
    {
        correlate{ { 0, 0, 0 }, 0, 0 },
        correlate{ { 1, 0, 0 }, 0, 0 },
        correlate{ { 2, 0, 0 }, 0, 0 }
    };
    const std::array<hash_digest, 3> digests{ hash, hash, hash };
    const std::array<ec_compressed, 3> points{ point0, point1, point2 };
    const std::array<ec_signature, 3> signatures{ sig0, sig1, sig2 };

    const batch in
    {
        { correlates.data(), correlates.size() },
        { digests.data(), digests.size() },
        { points.data(), points.size() },
        { signatures.data(), signatures.size() }
    };

    const stopper cancel{};
    const auto tokens = batch::verify(cancel, in);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);

    const auto value = from_little_array<batched::link_t>(correlates.at(2).id);
    BOOST_REQUIRE_EQUAL(tokens.front(), value);
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__multisig_all_valid__expected)
{
    using namespace system;
    using namespace system::ecdsa;
    using correlate = batch::correlate_t;
    const auto hash = bitcoin_hash(to_chunk("batch-multisig"));

    ec_compressed point0{}, point1{}, point2{};
    BOOST_REQUIRE(secret_to_public(point0, secret0));
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, one));

    ec_signature sig0{}, sig1{};
    BOOST_REQUIRE(sign(sig0, secret0, hash));
    BOOST_REQUIRE(sign(sig1, secret1, hash));

    // 2-of-3 group (same id + group); pair packs (sig,key).
    const std::array<correlate, 4> correlates
    {
        correlate{ { 0, 0, 0 }, 0b0000'0000, 5 }, // invalid
        correlate{ { 0, 0, 0 }, 0b0000'0001, 5 }, // valid
        correlate{ { 0, 0, 0 }, 0b0001'0001, 5 }, // invalid
        correlate{ { 0, 0, 0 }, 0b0001'0010, 5 }  // valid
    };
    const std::array<hash_digest, 4> digests{ hash, hash, hash, hash };
    const std::array<ec_compressed, 4> points{ point0, point1, point1, point2 };
    const std::array<ec_signature, 4> signatures{ sig0, sig0, sig1, sig1 };

    const batch in
    {
        { correlates.data(), correlates.size() },
        { digests.data(), digests.size() },
        { points.data(), points.size() },
        { signatures.data(), signatures.size() }
    };

    const stopper cancel{};
    const auto tokens = batch::verify(cancel, in);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__multisig_one_invalid__expected)
{
    using namespace system;
    using namespace system::ecdsa;
    using correlate = batch::correlate_t;
    const auto hash = bitcoin_hash(to_chunk("batch-multisig-invalid"));

    ec_compressed point0{}, point1{}, point2{};
    BOOST_REQUIRE(secret_to_public(point0, secret0));
    BOOST_REQUIRE(secret_to_public(point1, secret1));
    BOOST_REQUIRE(secret_to_public(point2, one));

    ec_signature sig0{}, sig1{}, sig2{};
    BOOST_REQUIRE(sign(sig0, secret0, hash));
    BOOST_REQUIRE(sign(sig1, secret1, hash));
    BOOST_REQUIRE(sign(sig2, one, hash));

    // corrupt second signature
    sig1[10] ^= 0xff;

    // 2-of-3 group (same id + group)
    const std::array<correlate, 3> correlates
    {
        correlate{ { 0, 0, 0 }, 0b0000'0000, 7 }, // valid
        correlate{ { 0, 0, 0 }, 0b0000'0001, 7 }, // invalid
        correlate{ { 0, 0, 0 }, 0b0001'0000, 7 }  // valid
    };
    const std::array<hash_digest, 3> digests{ hash, hash, hash };
    const std::array<ec_compressed, 3> points{ point0, point1, point2 };
    const std::array<ec_signature, 3> signatures{ sig0, sig1, sig2 };

    const batch in
    {
        { correlates.data(), correlates.size() },
        { digests.data(), digests.size() },
        { points.data(), points.size() },
        { signatures.data(), signatures.size() }
    };

    const stopper cancel{};
    const auto tokens = batch::verify(cancel, in);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);

    const auto value = from_little_array<batched::link_t>(correlates.at(0).id);
    BOOST_REQUIRE_EQUAL(tokens.front(), value);
}

// meets_threshold

struct ecdsa_accessor
    : public ecdsa::batch
{
    using multisig_matrix = multisig_matrix;
    using ecdsa::batch::meets_threshold;
};

using multisig_matrix = ecdsa_accessor::multisig_matrix;

BOOST_AUTO_TEST_CASE(ecdsa_batch__meets_threshold__1_of_1__success)
{
    multisig_matrix matrix{};
    set_right_into(matrix.at(0), 0);
    BOOST_REQUIRE(ecdsa_accessor::meets_threshold(1, 1, matrix));
}

BOOST_AUTO_TEST_CASE(ecdsa_batch__meets_threshold__1_of_1__failure)
{
    multisig_matrix matrix{};
    BOOST_REQUIRE(!ecdsa_accessor::meets_threshold(1, 1, matrix));
}

BOOST_AUTO_TEST_CASE(ecdsa_batch__meets_threshold__2_of_3__success)
{
    multisig_matrix matrix{};
    set_right_into(matrix.at(0), 0);
    set_right_into(matrix.at(1), 1);
    BOOST_REQUIRE(ecdsa_accessor::meets_threshold(2, 3, matrix));
}

BOOST_AUTO_TEST_CASE(ecdsa_batch__meets_threshold__2_of_3__failure)
{
    multisig_matrix matrix{};
    set_right_into(matrix.at(0), 0);
    set_right_into(matrix.at(0), 1); // both successes on same key
    BOOST_REQUIRE(!ecdsa_accessor::meets_threshold(2, 3, matrix));
}

BOOST_AUTO_TEST_CASE(ecdsa_batch__meets_threshold__3_of_3__success)
{
    multisig_matrix matrix{};
    set_right_into(matrix.at(0), 0);
    set_right_into(matrix.at(1), 1);
    set_right_into(matrix.at(2), 2);
    BOOST_REQUIRE(ecdsa_accessor::meets_threshold(3, 3, matrix));
}

BOOST_AUTO_TEST_CASE(ecdsa_batch__meets_threshold__3_of_3__failure)
{
    multisig_matrix matrix{};
    set_right_into(matrix.at(0), 0);
    set_right_into(matrix.at(1), 0);
    set_right_into(matrix.at(2), 0);
    BOOST_REQUIRE(!ecdsa_accessor::meets_threshold(3, 3, matrix));
}

BOOST_AUTO_TEST_CASE(ecdsa_batch__meets_threshold__2_of_2__success)
{
    multisig_matrix matrix{};
    set_right_into(matrix.at(0), 0);
    set_right_into(matrix.at(1), 1);
    BOOST_REQUIRE(ecdsa_accessor::meets_threshold(2, 2, matrix));
}

BOOST_AUTO_TEST_CASE(ecdsa_batch__meets_threshold__2_of_2__failure)
{
    multisig_matrix matrix{};
    set_right_into(matrix.at(0), 0);
    set_right_into(matrix.at(0), 1);
    BOOST_REQUIRE(!ecdsa_accessor::meets_threshold(2, 2, matrix));
}

// batch schnorr
// ----------------------------------------------------------------------------
// SoA batch: correlate_t is id-first: { id, category, pair, group }.

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__single_all_valid__expected)
{
    using namespace system;
    using namespace system::schnorr;
    using correlate = batch::correlate_t;
    const auto hash = bitcoin_hash(to_chunk("batch-schnorr"));

    ec_compressed pub0{}, pub1{}, pub2{};
    BOOST_REQUIRE(secret_to_public(pub0, secret0));
    BOOST_REQUIRE(secret_to_public(pub1, secret1));
    BOOST_REQUIRE(secret_to_public(pub2, one));

    const auto& point0 = array_cast<uint8_t, ec_xonly_size, 1>(pub0);
    const auto& point1 = array_cast<uint8_t, ec_xonly_size, 1>(pub1);
    const auto& point2 = array_cast<uint8_t, ec_xonly_size, 1>(pub2);

    ec_signature sig0{}, sig1{}, sig2{};
    constexpr hash_digest auxiliary{};
    BOOST_REQUIRE(sign(sig0, secret0, hash, auxiliary));
    BOOST_REQUIRE(sign(sig1, secret1, hash, auxiliary));
    BOOST_REQUIRE(sign(sig2, one, hash, auxiliary));

    const std::array<correlate, 3> correlates
    {
        correlate{ { 0, 0, 0 }, 0, 0, 0 },
        correlate{ { 1, 0, 0 }, 0, 0, 0 },
        correlate{ { 2, 0, 0 }, 0, 0, 0 }
    };
    const std::array<hash_digest, 3> digests{ hash, hash, hash };
    const std::array<ec_xonly, 3> points{ point0, point1, point2 };
    const std::array<ec_signature, 3> signatures{ sig0, sig1, sig2 };

    const batch in
    {
        { correlates.data(), correlates.size() },
        { digests.data(), digests.size() },
        { points.data(), points.size() },
        { signatures.data(), signatures.size() }
    };

    const stopper cancel{};
    const auto tokens = batch::verify(cancel, in);
    BOOST_REQUIRE(tokens.empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__single_one_invalid__expected)
{
    using namespace system;
    using namespace system::schnorr;
    using correlate = batch::correlate_t;
    const auto hash = bitcoin_hash(to_chunk("batch-schnorr-neg"));

    ec_compressed pub0{}, pub1{}, pub2{};
    BOOST_REQUIRE(secret_to_public(pub0, secret0));
    BOOST_REQUIRE(secret_to_public(pub1, secret1));
    BOOST_REQUIRE(secret_to_public(pub2, one));

    const auto& point0 = array_cast<uint8_t, ec_xonly_size, 1>(pub0);
    const auto& point1 = array_cast<uint8_t, ec_xonly_size, 1>(pub1);
    const auto& point2 = array_cast<uint8_t, ec_xonly_size, 1>(pub2);

    ec_signature sig0{}, sig1{}, sig2{};
    constexpr hash_digest auxiliary{};
    BOOST_REQUIRE(sign(sig0, secret0, hash, auxiliary));
    BOOST_REQUIRE(sign(sig1, secret1, hash, auxiliary));
    BOOST_REQUIRE(sign(sig2, one, hash, auxiliary));

    // corrupt second signature
    sig1[10] ^= 0xff;

    const std::array<correlate, 3> correlates
    {
        correlate{ { 0, 0, 0 }, 0, 0, 0 },
        correlate{ { 1, 0, 0 }, 0, 0, 0 },
        correlate{ { 2, 0, 0 }, 0, 0, 0 }
    };
    const std::array<hash_digest, 3> digests{ hash, hash, hash };
    const std::array<ec_xonly, 3> points{ point0, point1, point2 };
    const std::array<ec_signature, 3> signatures{ sig0, sig1, sig2 };

    const batch in
    {
        { correlates.data(), correlates.size() },
        { digests.data(), digests.size() },
        { points.data(), points.size() },
        { signatures.data(), signatures.size() }
    };

    const stopper cancel{};
    const auto tokens = batch::verify(cancel, in);
    BOOST_REQUIRE_EQUAL(tokens.size(), 1u);
    BOOST_REQUIRE_EQUAL(tokens.front(), from_little_array<batched::link_t>(correlates.at(1).id));
}

// meets_threshold

struct schnorr_accessor
    : public schnorr::batch
{
    using schnorr::batch::meets_threshold;
};

using category_t = system::chain::threshold::category_t;

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__equal__success)
{
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::equal), 5, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__equal__failure)
{
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::equal), 4, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__inequal__success)
{
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::inequal), 4, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__inequal__failure)
{
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::inequal), 5, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__lesser__success)
{
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::lesser), 3, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__lesser__failure)
{
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::lesser), 5, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__greater__success)
{
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::greater), 7, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__greater__failure)
{
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::greater), 5, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__not_lesser__success)
{
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::not_lesser), 5, 5, 0));
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::not_lesser), 7, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__not_lesser__failure)
{
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::not_lesser), 3, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__not_greater__success)
{
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::not_greater), 5, 5, 0));
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::not_greater), 3, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__not_greater__failure)
{
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::not_greater), 7, 5, 0));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__between__interior__success)
{
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::between), 5, 3, 7));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__between__at_minimum__success)
{
    // Lower bound is inclusive [op_within].
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::between), 3, 3, 7));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__between__below_maximum__success)
{
    // Last passing value of the half-open range.
    BOOST_REQUIRE(schnorr_accessor::meets_threshold(to_value(category_t::between), 6, 3, 7));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__between__at_maximum__failure)
{
    // Upper bound is exclusive [op_within]: min <= x < max.
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::between), 7, 3, 7));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__between__below_minimum__failure)
{
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::between), 2, 3, 7));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__between__above_maximum__failure)
{
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::between), 8, 3, 7));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__between__empty_range__failure)
{
    // min == max is an empty half-open range, unsatisfiable [op_within].
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::between), 5, 5, 5));
}

BOOST_AUTO_TEST_CASE(schnorr_batch__meets_threshold__between__inverted_range__failure)
{
    // max < min is unsatisfiable [op_within].
    BOOST_REQUIRE(!schnorr_accessor::meets_threshold(to_value(category_t::between), 5, 7, 3));
}

BOOST_AUTO_TEST_SUITE_END()

BC_POP_WARNING()
