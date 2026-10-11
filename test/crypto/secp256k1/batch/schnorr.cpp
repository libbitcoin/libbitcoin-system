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

BOOST_AUTO_TEST_SUITE(secp256k1_batch_schnorr_tests)

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
        correlate{ { 0, 0, 0 } },
        correlate{ { 1, 0, 0 } },
        correlate{ { 2, 0, 0 } }
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
        correlate{ { 0, 0, 0 } },
        correlate{ { 1, 0, 0 } },
        correlate{ { 2, 0, 0 } }
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

static batched::links_t chunked_schnorr_failures() NOEXCEPT
{
    using namespace system;
    using namespace system::schnorr;
    using correlate = batch::correlate_t;
    const auto hash = bitcoin_hash(to_chunk("batch-schnorr-chunked"));

    ec_compressed compressed{};
    ec_signature signature{}, corrupt{};
    if (!secret_to_public(compressed, secret0) || !sign(signature, secret0, hash, {}))
        return { max_uint32 };

    const auto& point = array_cast<uint8_t, ec_xonly_size, 1>(compressed);
    corrupt = signature;
    corrupt[10] ^= 0xff;

    std::vector<correlate> correlates(chunked_rows);
    std::vector<hash_digest> digests(chunked_rows, hash);
    std::vector<ec_xonly> points(chunked_rows, point);
    std::vector<ec_signature> signatures(chunked_rows, signature);
    for (size_t row{}; row < chunked_rows; ++row)
        correlates[row] = correlate{ chunked_id(row) };

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

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__chunked_two_invalid__expected)
{
    const auto tokens = chunked_schnorr_failures();
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

static batched::links_t distinct_schnorr_failures(bool fail,
    bool canceled) NOEXCEPT
{
    using namespace system;
    using namespace system::schnorr;
    using correlate = batch::correlate_t;

    std::vector<correlate> correlates(distinct_rows);
    std::vector<hash_digest> digests(distinct_rows);
    std::vector<ec_xonly> points(distinct_rows);
    std::vector<ec_signature> signatures(distinct_rows);
    for (size_t row{}; row < distinct_rows; ++row)
    {
        ec_compressed compressed{};
        const auto secret = distinct_secret(row);
        digests[row] = distinct_digest(row);
        correlates[row] = correlate{ chunked_id(row) };
        if (!secret_to_public(compressed, secret) ||
            !sign(signatures[row], secret, digests[row], {}))
            return { max_uint32 };

        points[row] = array_cast<uint8_t, ec_xonly_size, 1>(compressed);
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

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__distinct_all_valid__empty)
{
    BOOST_REQUIRE(distinct_schnorr_failures(false, false).empty());
}

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__distinct_three_invalid__expected)
{
    const auto tokens = distinct_schnorr_failures(true, false);
    BOOST_REQUIRE_EQUAL(tokens.size(), 3u);
    BOOST_REQUIRE_EQUAL(tokens.at(0), distinct_token(distinct_fail1));
    BOOST_REQUIRE_EQUAL(tokens.at(1), distinct_token(distinct_fail2));
    BOOST_REQUIRE_EQUAL(tokens.at(2), distinct_token(distinct_fail3));
}

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__distinct_canceled__empty)
{
    BOOST_REQUIRE(distinct_schnorr_failures(true, true).empty());
}

// empty
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(secp256k1__schnorr_batch_verify__empty__empty)
{
    const stopper cancel{};
    BOOST_REQUIRE(schnorr::batch::verify(cancel, schnorr::batch{}).empty());
}

BOOST_AUTO_TEST_SUITE_END()

BC_POP_WARNING()
