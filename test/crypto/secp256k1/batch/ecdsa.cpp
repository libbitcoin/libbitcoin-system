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
#include "../../../test.hpp"

BC_PUSH_WARNING(NO_USE_OF_SPAN)

BOOST_AUTO_TEST_SUITE(secp256k1_batch_ecdsa_tests)

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

// chunked
// ----------------------------------------------------------------------------
// Rows beyond a chunk boundary, with failures in and beyond the first chunk.

constexpr size_t chunked_rows = 300;
constexpr size_t chunked_fail1 = 3;
constexpr size_t chunked_fail2 = 257;

static batched::link chunked_id(size_t row) NOEXCEPT
{
    return { narrow_cast<uint8_t>(row), narrow_cast<uint8_t>(row >> 8), 0 };
}

static batched::links_t chunked_ecdsa_failures() NOEXCEPT
{
    using namespace system;
    using namespace system::ecdsa;
    using correlate = batch::correlate_t;
    const auto hash = bitcoin_hash(to_chunk("batch-ecdsa-chunked"));

    ec_compressed point{};
    ec_signature signature{}, corrupt{};
    if (!secret_to_public(point, secret0) || !sign(signature, secret0, hash))
        return { max_uint32 };

    corrupt = signature;
    corrupt[10] ^= 0xff;

    std::vector<correlate> correlates(chunked_rows);
    std::vector<hash_digest> digests(chunked_rows, hash);
    std::vector<ec_compressed> points(chunked_rows, point);
    std::vector<ec_signature> signatures(chunked_rows, signature);
    for (size_t row{}; row < chunked_rows; ++row)
        correlates[row] = correlate{ chunked_id(row), 0, 0 };

    signatures[chunked_fail1] = corrupt;
    signatures[chunked_fail2] = corrupt;

    const batch in
    {
        { correlates.data(), correlates.size() },
        { digests.data(), digests.size() },
        { points.data(), points.size() },
        { signatures.data(), signatures.size() }
    };

    const stopper cancel{};
    return batch::verify(cancel, in);
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__chunked_two_invalid__expected)
{
    const auto tokens = chunked_ecdsa_failures();
    BOOST_REQUIRE_EQUAL(tokens.size(), 2u);
    BOOST_REQUIRE_EQUAL(tokens.at(0), from_little_array<batched::link_t>(chunked_id(chunked_fail1)));
    BOOST_REQUIRE_EQUAL(tokens.at(1), from_little_array<batched::link_t>(chunked_id(chunked_fail2)));
}

// distinct rows
// ----------------------------------------------------------------------------
// Rows of distinct keys and digests, verified on the device where available,
// with failures at the first, a middle and the last row.

constexpr size_t distinct_rows = 512;
constexpr size_t distinct_fail1 = 0;
constexpr size_t distinct_fail2 = 257;
constexpr size_t distinct_fail3 = sub1(distinct_rows);

static ec_secret distinct_secret(size_t row) NOEXCEPT
{
    return sha256_hash(to_little_endian(narrow_cast<uint32_t>(row)));
}

static hash_digest distinct_digest(size_t row) NOEXCEPT
{
    return bitcoin_hash(to_little_endian(narrow_cast<uint32_t>(row)));
}

static bool distinct_failed(size_t row) NOEXCEPT
{
    return row == distinct_fail1 || row == distinct_fail2 ||
        row == distinct_fail3;
}

static batched::links_t distinct_ecdsa_failures(bool fail,
    bool canceled) NOEXCEPT
{
    using namespace system;
    using namespace system::ecdsa;
    using correlate = batch::correlate_t;

    std::vector<correlate> correlates(distinct_rows);
    std::vector<hash_digest> digests(distinct_rows);
    std::vector<ec_compressed> points(distinct_rows);
    std::vector<ec_signature> signatures(distinct_rows);
    for (size_t row{}; row < distinct_rows; ++row)
    {
        const auto secret = distinct_secret(row);
        digests[row] = distinct_digest(row);
        correlates[row] = correlate{ chunked_id(row), 0, 0 };
        if (!secret_to_public(points[row], secret) ||
            !sign(signatures[row], secret, digests[row]))
            return { max_uint32 };

        if (fail && distinct_failed(row))
            digests[row] = distinct_digest(add1(row));
    }

    const batch in
    {
        { correlates.data(), correlates.size() },
        { digests.data(), digests.size() },
        { points.data(), points.size() },
        { signatures.data(), signatures.size() }
    };

    const stopper cancel{ canceled };
    return batch::verify(cancel, in);
}

static batched::link_t distinct_token(size_t row) NOEXCEPT
{
    return from_little_array<batched::link_t>(chunked_id(row));
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__distinct_all_valid__empty)
{
    BOOST_REQUIRE(distinct_ecdsa_failures(false, false).empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__distinct_three_invalid__expected)
{
    const auto tokens = distinct_ecdsa_failures(true, false);
    BOOST_REQUIRE_EQUAL(tokens.size(), 3u);
    BOOST_REQUIRE_EQUAL(tokens.at(0), distinct_token(distinct_fail1));
    BOOST_REQUIRE_EQUAL(tokens.at(1), distinct_token(distinct_fail2));
    BOOST_REQUIRE_EQUAL(tokens.at(2), distinct_token(distinct_fail3));
}

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__distinct_canceled__empty)
{
    BOOST_REQUIRE(distinct_ecdsa_failures(true, true).empty());
}

// empty
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1__ecdsa_batch_verify__empty__empty)
{
    const stopper cancel{};
    BOOST_REQUIRE(ecdsa::batch::verify(cancel, ecdsa::batch{}).empty());
}

BOOST_AUTO_TEST_SUITE_END()

BC_POP_WARNING()
