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
#include "../mocks/blocks.hpp"

BOOST_AUTO_TEST_SUITE(witness_tests)

namespace json = boost::json;
using namespace system::chain;

BOOST_AUTO_TEST_CASE(witness__block__with_witness__parse_round_trips)
{
    constexpr auto witness = true;
    const auto& instance1 = test::mock_block_c;
    const auto data = test::mock_block_c.to_data(witness);
    const block instance2(data, witness);
    BOOST_REQUIRE(instance1.is_valid());
    BOOST_REQUIRE(instance2.is_valid());
    BOOST_REQUIRE(instance1.hash() == instance2.hash());
    BOOST_REQUIRE(instance1 == instance2);
    BOOST_REQUIRE(instance1.is_segregated());
    BOOST_REQUIRE(instance2.is_segregated());
}

BOOST_AUTO_TEST_CASE(witness__block__genesis_block__arena_parse_round_trips)
{
    constexpr auto witness = true;
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto data = genesis.to_data(true);

    stream::in::fast stream(data);
    read::bytes::fast source(stream);
    const block instance(source, witness);
    BOOST_REQUIRE(!instance.check());
}

BOOST_AUTO_TEST_CASE(witness__block__with_witness__checks)
{
    constexpr auto witness = true;
    const auto data = test::mock_block_c.to_data(witness);
    stream::in::fast stream(data);
    read::bytes::fast source(stream);
    const block instance(source, witness);
    BOOST_REQUIRE(!instance.check());
}

BOOST_AUTO_TEST_CASE(witness__block__without_witness__checks)
{
    constexpr auto witness = false;
    const auto data = test::mock_block_c.to_data(witness);
    stream::in::fast stream(data);
    read::bytes::fast source(stream);
    const block instance(source, witness);
    BOOST_REQUIRE(!instance.check());
}

// annex

// The moved-from stack is empty, so the annex must be taken from the member.
BOOST_AUTO_TEST_CASE(witness__annex__moved_annex_pattern__expected)
{
    const data_chunk expected{ taproot_annex_prefix, 0x42_u8 };
    chunk_cptrs stack
    {
        to_shared<data_chunk>({ 0x24_u8 }),
        to_shared<data_chunk>(expected)
    };

    const chain::witness instance{ std::move(stack) };
    BOOST_REQUIRE(instance.annex());
    BOOST_REQUIRE_EQUAL(instance.annex().data(), expected);
}

BOOST_AUTO_TEST_CASE(witness__annex__copied_annex_pattern__expected)
{
    const data_chunk expected{ taproot_annex_prefix, 0x42_u8 };
    const chunk_cptrs stack
    {
        to_shared<data_chunk>({ 0x24_u8 }),
        to_shared<data_chunk>(expected)
    };

    const chain::witness instance{ stack };
    BOOST_REQUIRE(instance.annex());
    BOOST_REQUIRE_EQUAL(instance.annex().data(), expected);
}

BOOST_AUTO_TEST_CASE(witness__annex__moved_not_annex_pattern__empty)
{
    chunk_cptrs stack
    {
        to_shared<data_chunk>({ 0x24_u8 }),
        to_shared<data_chunk>({ 0x24_u8 })
    };

    const chain::witness instance{ std::move(stack) };
    BOOST_REQUIRE(!instance.annex());
}

// extract_taproot

// Commitments below are derived from bip340/341 reference math, cross-checked
// against the bip341 wallet test vectors. Each commits the same internal key
// (d6889cb081036e0faefa3a35157ad71086b123b2b144b649798b494c300a961d) to the
// same single op leaf script (op_1), varying leaf version and merkle path.
static const data_chunk tapleaf_script{ 0x51_u8 };

// Leaf version 0xc0 (tapscript), no merkle path, even output key parity.
static const auto c0_program = base16_chunk("81d6ccecd0da56aafd816eb5548c4aec7342287fe7cdd2781140d2d162f0f88a");
static const auto c0_control = base16_chunk("c0d6889cb081036e0faefa3a35157ad71086b123b2b144b649798b494c300a961d");
static const auto c0_tapleaf = base16_array("a85b2107f791b26a84e7586c28cec7cb61202ed3d01944d832500f363782d675");

// Leaf version 0xc2 (undefined), no merkle path, even output key parity.
static const auto c2_program = base16_chunk("f608ee80bd1b9cf910a33d91a31f2d67ebe8994e2c8ce62dad6697be5d36b334");
static const auto c2_control = base16_chunk("c2d6889cb081036e0faefa3a35157ad71086b123b2b144b649798b494c300a961d");

// Leaf version 0xc2 (undefined), one merkle node, odd output key parity.
static const auto c2m_program = base16_chunk("ce5bbc28aaeab2c6285290ab3d172ee0f8923426f126c482592273aece547405");
static const auto c2m_control = base16_chunk("c3d6889cb081036e0faefa3a35157ad71086b123b2b144b649798b494c300a961daaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");

static script taproot_prevout(const data_chunk& program) NOEXCEPT
{
    return script{ script::to_pay_witness_pattern(1_u8, program) };
}

static chain::witness script_path(const data_chunk& control) NOEXCEPT
{
    const auto script_element = to_shared<data_chunk>(tapleaf_script);
    const auto control_element = to_shared<data_chunk>(control);
    return chain::witness{ chunk_cptrs{ script_element, control_element } };
}

BOOST_AUTO_TEST_CASE(witness__extract_taproot__tapscript_committed__success_with_tapleaf)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c0_program);
    const auto instance = script_path(c0_control);

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::script_success);
    BOOST_REQUIRE(leaf);
    BOOST_REQUIRE_EQUAL(*leaf, c0_tapleaf);
    BOOST_REQUIRE(*out == script(tapleaf_script, false));
    BOOST_REQUIRE(!out->is_prevalid());
    BOOST_REQUIRE(stack->empty());
}

BOOST_AUTO_TEST_CASE(witness__extract_taproot__tapscript_uncommitted__invalid_commitment)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c2_program);
    const auto instance = script_path(c0_control);

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::invalid_commitment);
}

// The commitment binds all leaf versions, so an undefined version is not
// unencumbered until its control block commits to the output key [bip341].
BOOST_AUTO_TEST_CASE(witness__extract_taproot__undefined_leaf_version_committed__success_unencumbered)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c2_program);
    const auto instance = script_path(c2_control);

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::script_success);
    BOOST_REQUIRE(!leaf);
    BOOST_REQUIRE(out->is_prevalid());
    BOOST_REQUIRE(stack->empty());
}

BOOST_AUTO_TEST_CASE(witness__extract_taproot__undefined_leaf_version_uncommitted__invalid_commitment)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c0_program);
    const auto instance = script_path(c2_control);

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::invalid_commitment);
}

BOOST_AUTO_TEST_CASE(witness__extract_taproot__undefined_leaf_version_merkle_path_committed__success_unencumbered)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c2m_program);
    const auto instance = script_path(c2m_control);

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::script_success);
    BOOST_REQUIRE(!leaf);
    BOOST_REQUIRE(out->is_prevalid());
    BOOST_REQUIRE(stack->empty());
}

BOOST_AUTO_TEST_CASE(witness__extract_taproot__undefined_leaf_version_merkle_path_uncommitted__invalid_commitment)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c2_program);
    const auto instance = script_path(c2m_control);

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::invalid_commitment);
}

BOOST_AUTO_TEST_CASE(witness__extract_taproot__control_size_underflow__invalid_witness)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c2_program);
    const auto instance = script_path(data_chunk(ec_xonly_size, 0xc2_u8));

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::invalid_witness);
}

BOOST_AUTO_TEST_CASE(witness__extract_taproot__control_size_not_multiple__invalid_witness)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c2_program);
    const auto instance = script_path(data_chunk(ec_xonly_size + 2_size, 0xc2_u8));

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::invalid_witness);
}

BOOST_AUTO_TEST_CASE(witness__extract_taproot__key_path__success_with_checksig)
{
    hash_cptr leaf{};
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = taproot_prevout(c0_program);
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>({ 0x24_u8 }) } };

    BOOST_REQUIRE_EQUAL(instance.extract_taproot(leaf, out, stack, prevout), error::script_success);
    BOOST_REQUIRE(!leaf);
    BOOST_REQUIRE(*out == script{ { { opcode::checksig } } });
    BOOST_REQUIRE_EQUAL(stack->size(), two);
    BOOST_REQUIRE_EQUAL(*stack->back(), c0_program);
}

BOOST_AUTO_TEST_SUITE_END()
