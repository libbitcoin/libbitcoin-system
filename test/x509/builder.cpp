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

BC_AUTO_TEST_SUITE(builder_tests)

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

BC_AUTO_TEST_CASE(builder__build_self_signed__subject__expected_fields)
{
    data_chunk der{};
    BC_REQUIRE(build_self_signed(der, fixture::key_secret, bs()));

    certificate out{};
    BC_REQUIRE(parse(out, der));
    BC_REQUIRE_EQUAL(out.serial, base16_chunk("008123456789abcdef0123456789abcdef"));
    BC_REQUIRE_EQUAL(out.issuer, out.subject);
    BC_REQUIRE_EQUAL(out.not_before, 1735689600u);
    BC_REQUIRE_EQUAL(out.not_after, 2524608000u);
    BC_REQUIRE(out.curve == curve::secp256r1);
    BC_REQUIRE(out.algorithm == signature_algorithm::ecdsa_sha256);
    BC_REQUIRE(!out.authority);
    BC_REQUIRE_EQUAL(out.key_usage, digital_signature);
    BC_REQUIRE(out.server_authentication);
    BC_REQUIRE(out.client_authentication);
    BC_REQUIRE_EQUAL(out.dns_names.size(), 2u);
    BC_REQUIRE_EQUAL(out.dns_names.back(), "bs.local");
    BC_REQUIRE_EQUAL(out.ip_addresses.size(), 1u);
    BC_REQUIRE_EQUAL(out.ip_addresses.front(), base16_chunk("7f000001"));
}

BC_AUTO_TEST_CASE(builder__build_self_signed__subject__signed_and_pinned)
{
    data_chunk der{};
    BC_REQUIRE(build_self_signed(der, fixture::key_secret, bs()));

    certificate out{};
    BC_REQUIRE(parse(out, der));
    BC_REQUIRE(is_signed_by(out, out));
    BC_REQUIRE_EQUAL(verify({ out }, { out }, fixture::now, purpose::server), error::x509_success);
    BC_REQUIRE_EQUAL(verify({ out }, { out }, fixture::now, purpose::client), error::x509_success);
}

BC_AUTO_TEST_CASE(builder__build_self_signed__fixture_key__public_key_matches)
{
    data_chunk der{};
    BC_REQUIRE(build_self_signed(der, fixture::key_secret, bs()));

    secp256r1::point_t point{};
    BC_REQUIRE(secp256r1::public_key(point, fixture::key_secret));

    certificate out{};
    BC_REQUIRE(parse(out, der));
    BC_REQUIRE_EQUAL(out.public_key, to_chunk(point));
}

BC_AUTO_TEST_CASE(builder__build_self_signed__no_names__empty_subject)
{
    subject name{};
    name.not_before = 1735689600;
    name.not_after = 1735689600;

    data_chunk der{};
    BC_REQUIRE(build_self_signed(der, fixture::key_secret, name));

    certificate out{};
    BC_REQUIRE(parse(out, der));
    BC_REQUIRE_EQUAL(out.subject, base16_chunk("3000"));
    BC_REQUIRE(out.dns_names.empty());
}

BC_AUTO_TEST_CASE(builder__build_self_signed__names_without_common_name__critical_names)
{
    subject name{};
    name.dns_names = { "localhost" };
    name.not_after = 1735689600;

    data_chunk der{};
    BC_REQUIRE(build_self_signed(der, fixture::key_secret, name));

    certificate out{};
    BC_REQUIRE(parse(out, der));
    BC_REQUIRE_EQUAL(out.dns_names.front(), "localhost");
}

BC_AUTO_TEST_CASE(builder__build_self_signed__zero_key__false)
{
    data_chunk der{};
    BC_REQUIRE(!build_self_signed(der, secret{}, bs()));
}

BC_AUTO_TEST_CASE(builder__build_self_signed__reversed_validity__false)
{
    auto name = bs();
    name.not_before = add1(name.not_after);

    data_chunk der{};
    BC_REQUIRE(!build_self_signed(der, fixture::key_secret, name));
}

BC_AUTO_TEST_CASE(builder__encode_certificate__built__round_trip)
{
    data_chunk der{};
    BC_REQUIRE(build_self_signed(der, fixture::key_secret, bs()));

    const auto text = encode_certificate(der);
    BC_REQUIRE(text.starts_with("-----BEGIN CERTIFICATE-----\n"));

    certificates out{};
    BC_REQUIRE(parse(out, text));
    BC_REQUIRE_EQUAL(out.size(), 1u);
    BC_REQUIRE_EQUAL(out.front().encoding, der);
}

BC_AUTO_TEST_SUITE_END()
