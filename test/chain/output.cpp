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

BOOST_AUTO_TEST_SUITE(output_tests)

namespace json = boost::json;
using namespace system::chain;

static const auto output_data = base16_chunk(
    "20300500000000001976a914905f933de850988603aafeeb2fd7fce61e66fe5d88ac");

static const output expected_output(output_data);

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid

BOOST_AUTO_TEST_CASE(output__constructor__default__invalid)
{
    output instance;
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(output__constructor__move__expected)
{
    auto copy = output_data;
    output instance(std::move(copy));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.value() == expected_output.value());
    BOOST_REQUIRE(instance.script() == expected_output.script());
}

BOOST_AUTO_TEST_CASE(output__constructor__copy__expected)
{
    const output instance(expected_output);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.value() == expected_output.value());
    BOOST_REQUIRE(instance.script() == expected_output.script());
}

BOOST_AUTO_TEST_CASE(output__constructor__move_parameters__expected)
{
    auto script_copy = expected_output.script();
    output instance(expected_output.value(), std::move(script_copy));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.value() == expected_output.value());
    BOOST_REQUIRE(instance.script() == expected_output.script());
}

BOOST_AUTO_TEST_CASE(output__constructor__copy_parameters__expected)
{
    output instance(expected_output.value(), expected_output.script());
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.value() == expected_output.value());
    BOOST_REQUIRE(instance.script() == expected_output.script());
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(output__assign__copy__expected)
{
    const auto& alpha = expected_output;
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(output__assign__move__expected)
{
    const auto& alpha = expected_output;
    const auto beta = alpha;
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(output__equality__same__true)
{
    const auto& alpha = expected_output;
    const output beta(alpha);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(output__equality__different__false)
{
    const auto& alpha = expected_output;
    const output beta;
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(output__inequality__same__false)
{
    const auto& alpha = expected_output;
    const output beta(alpha);
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(output__inequality__different__true)
{
    const auto& alpha = expected_output;
    const output beta;
    BOOST_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(output__to_data__data__expected)
{
    const auto size = expected_output.to_data().size();
    BOOST_REQUIRE_EQUAL(size, expected_output.serialized_size());
}

BOOST_AUTO_TEST_CASE(output__to_data__stream__expected)
{
    // Write output to stream.
    std::stringstream iostream;
    expected_output.to_data(iostream);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const output copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_output);
}

BOOST_AUTO_TEST_CASE(output__to_data__writer__expected)
{
    // Write output to stream.
    std::stringstream iostream;
    write::bytes::ostream out(iostream);
    expected_output.to_data(out);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const output copy(iostream);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_output);
}

// properties
// ----------------------------------------------------------------------------

// serialized_size

// methods
// ----------------------------------------------------------------------------

// committed_hash

BOOST_AUTO_TEST_CASE(output__committed_hash__commitment_pattern__expected)
{
    const script commitment(base16_chunk("6a24aa21a9ed0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"), false);
    const output instance{ 0, commitment };

    hash_cref out{ null_hash };
    BOOST_REQUIRE(instance.committed_hash(out));
    BOOST_REQUIRE_EQUAL(out.get(), base16_array("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"));
}

// Trailing bytes after the commitment have no consensus meaning [bip141].
BOOST_AUTO_TEST_CASE(output__committed_hash__trailing_operations__expected)
{
    const script commitment(base16_chunk("6a24aa21a9ed0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2051"), false);
    const output instance{ 0, commitment };

    hash_cref out{ null_hash };
    BOOST_REQUIRE(instance.committed_hash(out));
    BOOST_REQUIRE_EQUAL(out.get(), base16_array("0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"));
}

BOOST_AUTO_TEST_CASE(output__committed_hash__wrong_head__false)
{
    const script commitment(base16_chunk("6a24aa21a9ee0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"), false);
    const output instance{ 0, commitment };

    hash_cref out{ null_hash };
    BOOST_REQUIRE(!instance.committed_hash(out));
}

BOOST_AUTO_TEST_CASE(output__committed_hash__short_push__false)
{
    const script commitment(base16_chunk("6a23aa21a9ed0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"), false);
    const output instance{ 0, commitment };

    hash_cref out{ null_hash };
    BOOST_REQUIRE(!instance.committed_hash(out));
}

BOOST_AUTO_TEST_CASE(output__committed_hash__not_op_return__false)
{
    const script commitment(base16_chunk("6b24aa21a9ed0102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20"), false);
    const output instance{ 0, commitment };

    hash_cref out{ null_hash };
    BOOST_REQUIRE(!instance.committed_hash(out));
}

BOOST_AUTO_TEST_CASE(output__signature_operations__checksig_checksigverify__expected)
{
    const script script(base16_chunk("02acad"), true);
    BOOST_REQUIRE(script.is_valid());

    const output instance{ 42, script };
    BOOST_REQUIRE_EQUAL(instance.signature_operations(false), 2u);
    BOOST_REQUIRE_EQUAL(instance.signature_operations(true), 8u);
}

// is_dust

BOOST_AUTO_TEST_SUITE_END()
