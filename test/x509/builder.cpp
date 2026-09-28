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

BOOST_AUTO_TEST_SUITE(builder_tests)

using namespace x509;

static subject bs()
{
    subject out{};
    out.common_name = "bs";
    out.dns_names = { "localhost", "bs.local" };
    out.ip_addresses = { base16_chunk("7f000001") };
    out.serial = base16_array("8123456789abcdef0123456789abcdef");
    out.not_before = 1735689600;
    out.not_after = 2524608000;
    return out;
}

BOOST_AUTO_TEST_CASE(builder__build_self_signed__subject__expected_fields)
{
    data_chunk der{};
    BOOST_REQUIRE(build_self_signed(der, fixture::key_secret, bs()));

    certificate out{};
    BOOST_REQUIRE(parse(out, der));
    BOOST_REQUIRE_EQUAL(out.serial, base16_chunk("008123456789abcdef0123456789abcdef"));
    BOOST_REQUIRE_EQUAL(out.issuer, out.subject);
    BOOST_REQUIRE_EQUAL(out.not_before, 1735689600u);
    BOOST_REQUIRE_EQUAL(out.not_after, 2524608000u);
    BOOST_REQUIRE(out.curve == curve::secp256r1);
    BOOST_REQUIRE(out.algorithm == signature_algorithm::ecdsa_sha256);
    BOOST_REQUIRE(!out.authority);
    BOOST_REQUIRE_EQUAL(out.key_usage, digital_signature);
    BOOST_REQUIRE(out.server_authentication);
    BOOST_REQUIRE(out.client_authentication);
    BOOST_REQUIRE_EQUAL(out.dns_names.size(), 2u);
    BOOST_REQUIRE_EQUAL(out.dns_names.back(), "bs.local");
    BOOST_REQUIRE_EQUAL(out.ip_addresses.size(), 1u);
    BOOST_REQUIRE_EQUAL(out.ip_addresses.front(), base16_chunk("7f000001"));
}

BOOST_AUTO_TEST_CASE(builder__build_self_signed__subject__signed_and_pinned)
{
    data_chunk der{};
    BOOST_REQUIRE(build_self_signed(der, fixture::key_secret, bs()));

    certificate out{};
    BOOST_REQUIRE(parse(out, der));
    BOOST_REQUIRE(is_signed_by(out, out));
    BOOST_REQUIRE_EQUAL(verify({ out }, { out }, fixture::now, purpose::server), error::x509_success);
    BOOST_REQUIRE_EQUAL(verify({ out }, { out }, fixture::now, purpose::client), error::x509_success);
}

BOOST_AUTO_TEST_CASE(builder__build_self_signed__fixture_key__public_key_matches)
{
    data_chunk der{};
    BOOST_REQUIRE(build_self_signed(der, fixture::key_secret, bs()));

    secp256r1::point_t point{};
    BOOST_REQUIRE(secp256r1::public_key(point, fixture::key_secret));

    certificate out{};
    BOOST_REQUIRE(parse(out, der));
    BOOST_REQUIRE_EQUAL(out.public_key, to_chunk(point));
}

BOOST_AUTO_TEST_CASE(builder__build_self_signed__no_names__empty_subject)
{
    subject name{};
    name.not_before = 1735689600;
    name.not_after = 1735689600;

    data_chunk der{};
    BOOST_REQUIRE(build_self_signed(der, fixture::key_secret, name));

    certificate out{};
    BOOST_REQUIRE(parse(out, der));
    BOOST_REQUIRE_EQUAL(out.subject, base16_chunk("3000"));
    BOOST_REQUIRE(out.dns_names.empty());
}

BOOST_AUTO_TEST_CASE(builder__build_self_signed__names_without_common_name__critical_names)
{
    subject name{};
    name.dns_names = { "localhost" };
    name.not_after = 1735689600;

    data_chunk der{};
    BOOST_REQUIRE(build_self_signed(der, fixture::key_secret, name));

    certificate out{};
    BOOST_REQUIRE(parse(out, der));
    BOOST_REQUIRE_EQUAL(out.dns_names.front(), "localhost");
}

BOOST_AUTO_TEST_CASE(builder__build_self_signed__zero_key__false)
{
    data_chunk der{};
    BOOST_REQUIRE(!build_self_signed(der, secret{}, bs()));
}

BOOST_AUTO_TEST_CASE(builder__build_self_signed__reversed_validity__false)
{
    auto name = bs();
    name.not_before = add1(name.not_after);

    data_chunk der{};
    BOOST_REQUIRE(!build_self_signed(der, fixture::key_secret, name));
}

BOOST_AUTO_TEST_CASE(builder__encode_certificate__built__round_trip)
{
    data_chunk der{};
    BOOST_REQUIRE(build_self_signed(der, fixture::key_secret, bs()));

    const auto text = encode_certificate(der);
    BOOST_REQUIRE(text.starts_with("-----BEGIN CERTIFICATE-----\n"));

    certificates out{};
    BOOST_REQUIRE(parse(out, text));
    BOOST_REQUIRE_EQUAL(out.size(), 1u);
    BOOST_REQUIRE_EQUAL(out.front().encoding, der);
}

BOOST_AUTO_TEST_SUITE_END()
