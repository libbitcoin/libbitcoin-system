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
#include "fixtures.hpp"

BOOST_AUTO_TEST_SUITE(verify_tests)

using namespace x509;

static certificates chain(const std::string& text)
{
    certificates out{};
    parse(out, text);
    return out;
}

// A self-signed certificate with the subject name of the intermediate.
static certificates impostor()
{
    subject name{};
    name.common_name = "intermediate";
    name.not_before = 1735689600;
    name.not_after = 2051222400;

    data_chunk der{};
    build_self_signed(der, base16_array("0000000000000000000000000000000000000000000000000000000000000002"), name);

    certificate out{};
    parse(out, der);
    return { out };
}

BOOST_AUTO_TEST_CASE(verify__empty_chain__chain_empty)
{
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify({}, anchors, fixture::now, purpose::server), error::chain_empty);
}

BOOST_AUTO_TEST_CASE(verify__server_chain__success)
{
    const auto leaves = chain(fixture::server_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::server), error::x509_success);
}

BOOST_AUTO_TEST_CASE(verify__client_chain__success)
{
    const auto leaves = chain(fixture::client_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::client), error::x509_success);
}

BOOST_AUTO_TEST_CASE(verify__client_chain_for_server__extended_key_usage_invalid)
{
    const auto leaves = chain(fixture::client_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::server), error::extended_key_usage_invalid);
}

BOOST_AUTO_TEST_CASE(verify__leaf_issued_by_anchor__success)
{
    const auto leaves = chain(fixture::direct_pem);
    const auto anchors = chain(fixture::self_pem + fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::client), error::x509_success);
}

BOOST_AUTO_TEST_CASE(verify__pinned_self_signed__success)
{
    const auto leaves = chain(fixture::self_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, leaves, fixture::now, purpose::server), error::x509_success);
}

BOOST_AUTO_TEST_CASE(verify__chain_with_anchor__success)
{
    const auto leaves = chain(fixture::server_pem + fixture::intermediate_pem + fixture::root_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::server), error::x509_success);
}

BOOST_AUTO_TEST_CASE(verify__before_validity__not_yet_valid)
{
    const auto leaves = chain(fixture::server_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, 0, purpose::server), error::certificate_not_yet_valid);
}

BOOST_AUTO_TEST_CASE(verify__expired_leaf__expired)
{
    const auto leaves = chain(fixture::expired_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::client), error::certificate_expired);
}

BOOST_AUTO_TEST_CASE(verify__validity_end__inclusive)
{
    const auto leaves = chain(fixture::direct_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, 2051222400 - 1, purpose::client), error::x509_success);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, 2051222400 + 1, purpose::client), error::certificate_expired);
}

BOOST_AUTO_TEST_CASE(verify__leaf_without_digital_signature__key_usage_invalid)
{
    const auto leaves = chain(fixture::encipher_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::client), error::key_usage_invalid);
}

BOOST_AUTO_TEST_CASE(verify__unanchored__chain_untrusted)
{
    const auto leaves = chain(fixture::server_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::self_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::server), error::chain_untrusted);
}

BOOST_AUTO_TEST_CASE(verify__no_anchors__chain_untrusted)
{
    const auto leaves = chain(fixture::server_pem + fixture::intermediate_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, {}, fixture::now, purpose::server), error::chain_untrusted);
}

BOOST_AUTO_TEST_CASE(verify__out_of_order__issuer_mismatch)
{
    const auto leaves = chain(fixture::server_pem + fixture::root_pem);
    const auto anchors = chain(fixture::self_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::server), error::issuer_mismatch);
}

BOOST_AUTO_TEST_CASE(verify__impostor_issuer__certificate_signature)
{
    auto leaves = chain(fixture::server_pem);
    leaves.push_back(impostor().front());
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::server), error::certificate_signature);
}

BOOST_AUTO_TEST_CASE(verify__issued_by_leaf__issuer_not_authority)
{
    const auto leaves = chain(fixture::child_pem + fixture::client_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::client), error::issuer_not_authority);
}

BOOST_AUTO_TEST_CASE(verify__anchor_without_certificate_sign__key_usage_invalid)
{
    const auto leaves = chain(fixture::nosign_leaf_pem);
    const auto anchors = chain(fixture::nosign_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::client), error::key_usage_invalid);
}

BOOST_AUTO_TEST_CASE(verify__intermediate_below_constrained_anchor__path_length_exceeded)
{
    const auto leaves = chain(fixture::deep_pem + fixture::constrained_intermediate_pem);
    const auto anchors = chain(fixture::constrained_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::client), error::path_length_exceeded);
}

BOOST_AUTO_TEST_CASE(verify__intermediate_below_unconstrained_anchor__success)
{
    const auto leaves = chain(fixture::deep_pem + fixture::intermediate_pem);
    const auto anchors = chain(fixture::root_pem);
    BOOST_REQUIRE_EQUAL(verify(leaves, anchors, fixture::now, purpose::client), error::x509_success);
}

BOOST_AUTO_TEST_SUITE_END()
