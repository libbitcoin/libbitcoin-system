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
#include "../../test.hpp"

BOOST_AUTO_TEST_SUITE(input_view_tests)

using namespace system::chain;

static const script p2sh_script(base16_chunk("a914d8dacdadb7462ae15cd906f1878706d0da8660e687"), false);
static const script p2wpkh_script(base16_chunk("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"), false);
static const script sign_script(base16_chunk("4830450221008b9d1dc26ba6a9cb62127b02742fa9d754cd3bebf337f7a55d114c8e5cdd30be022040529b194ba3f9281a99f2b1c0a19c0489bc22ede944ccf4ecbab4cc618ef3ed01"), false);
static const witness p2wpkh_witness(base16_chunk("02304402203609e17b84f6a7d30c80bfa610b5b4542f32a8a0d5447a12fb1366d7f01cc44a0220573a954c4518331561406f90300e8f3358f51928d43c212a8caed02de67eebee0121025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee6357"), true);

BOOST_AUTO_TEST_CASE(input_view__construct__default__invalid)
{
    const chain::view::input view{};
    BOOST_CHECK(!view.is_valid());
    BOOST_CHECK(is_null(view.prevout));
}

BOOST_AUTO_TEST_CASE(input_view__construct__null_point__expected)
{
    const input expected{ point{}, sign_script, max_input_sequence };
    const auto data = expected.to_data();
    const chain::view::input view{ data.data(), nullptr, 0 };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(view.is_null_point());
    BOOST_CHECK_EQUAL(view.point_hash(), null_hash);
    BOOST_CHECK_EQUAL(view.point_index(), point::null_index);
    BOOST_CHECK_EQUAL(view.sequence(), max_input_sequence);
    BOOST_CHECK(view.is_final());
    BOOST_CHECK_EQUAL(view.script_size(), sign_script.serialized_size(false));
    BOOST_CHECK_EQUAL(view.script_data().to_chunk(), sign_script.to_data(false));
    BOOST_CHECK(view.script() == sign_script);
    BOOST_CHECK(*view.script_ptr() == sign_script);
    BOOST_CHECK(view.witness().stack().empty());
    BOOST_CHECK(view.witness_data().empty());
}

BOOST_AUTO_TEST_CASE(input_view__construct__witnessed__expected)
{
    const input expected{ point{ one_hash, 7 }, script{}, p2wpkh_witness, 42 };
    const auto data = expected.to_data();
    const auto witness_data = p2wpkh_witness.to_data(true);
    const chain::view::input view{ data.data(), witness_data.data(), witness_data.size() };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(!view.is_null_point());
    BOOST_CHECK_EQUAL(view.point_hash(), one_hash);
    BOOST_CHECK_EQUAL(view.point_index(), 7u);
    BOOST_CHECK_EQUAL(view.sequence(), 42u);
    BOOST_CHECK(!view.is_final());
    BOOST_CHECK_EQUAL(view.script_size(), 0u);
    BOOST_CHECK(view.script().ops().empty());
    BOOST_CHECK(view.witness() == p2wpkh_witness);
    BOOST_CHECK_EQUAL(view.witness_data().to_chunk(), witness_data);
}

BOOST_AUTO_TEST_CASE(input_view__signature_operations__no_prevout__input_script_only)
{
    const input expected{ point{ one_hash, 0 }, sign_script, max_input_sequence };
    const auto data = expected.to_data();
    const chain::view::input view{ data.data(), nullptr, 0 };
    BOOST_CHECK_EQUAL(view.signature_operations(true, true), expected.signature_operations(true, true));
    BOOST_CHECK_EQUAL(view.signature_operations(true, true), 0u);
}

BOOST_AUTO_TEST_CASE(input_view__signature_operations__p2wpkh__matches_input)
{
    const input expected{ point{ one_hash, 0 }, script{}, p2wpkh_witness, max_input_sequence };
    const output prevout{ 1, p2wpkh_script };
    expected.prevout = to_shared(prevout);

    const auto data = expected.to_data();
    const auto witness_data = p2wpkh_witness.to_data(true);
    const auto prevout_data = prevout.to_data();
    const chain::view::output prevout_view{ prevout_data.data() };
    const chain::view::input view{ data.data(), witness_data.data(), witness_data.size() };
    view.prevout = &prevout_view;

    BOOST_CHECK_EQUAL(view.signature_operations(true, true), expected.signature_operations(true, true));
    BOOST_CHECK_EQUAL(view.signature_operations(true, false), expected.signature_operations(true, false));
    BOOST_CHECK_EQUAL(view.signature_operations(true, true), 1u);
    BOOST_CHECK(!view.is_roller());
}

BOOST_AUTO_TEST_CASE(input_view__signature_operations__p2sh_multisig__matches_input)
{
    const script embedded(base16_chunk("52483045022015bd0139bcccf990a6af6ec5c1c52ed8222e03a0d51c334df139968525d2fcd20221009f9efe325476eb64c3958e4713e9eefe49bf1d820ed58d2112721b134e2a1a5303210378d430274f8c5ec1321338151e9f27f4c676a008bdf8638d07c0b6be9ab35c71210378d430274f8c5ec1321338151e9f27f4c676a008bdf8638d07c0b6be9ab35c7153ae"), false);
    const script spend{ operations{ operation{ opcode::push_size_0 }, operation{ embedded.to_data(false), false } } };
    const input expected{ point{ one_hash, 0 }, spend, max_input_sequence };
    const output prevout{ 1, p2sh_script };
    expected.prevout = to_shared(prevout);

    const auto data = expected.to_data();
    const auto prevout_data = prevout.to_data();
    const chain::view::output prevout_view{ prevout_data.data() };
    const chain::view::input view{ data.data(), nullptr, 0 };
    view.prevout = &prevout_view;

    BOOST_CHECK_EQUAL(view.signature_operations(true, true), expected.signature_operations(true, true));
    BOOST_CHECK_EQUAL(view.signature_operations(false, true), expected.signature_operations(false, true));
    BOOST_CHECK_EQUAL(view.signature_operations(true, false), 3u);
}

BOOST_AUTO_TEST_SUITE_END()
