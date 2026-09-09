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

// extract_segwit

static script segwit_prevout(const data_chunk& program) NOEXCEPT
{
    return script{ script::to_pay_witness_pattern(0_u8, program) };
}

static const data_chunk key_hash_program = base16_chunk("1d0f172a0ecb48aee1be1f2687d2963ae33f71a1");
static const data_chunk embedded_script{ 0x51_u8 };

BOOST_AUTO_TEST_CASE(witness__extract_segwit__key_hash_two_elements__success)
{
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = segwit_prevout(key_hash_program);
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>({ 0x01_u8 }), to_shared<data_chunk>({ 0x02_u8 }) } };

    BOOST_REQUIRE_EQUAL(instance.extract_segwit(out, stack, prevout), error::script_success);
    BOOST_REQUIRE(*out == script{ script::to_pay_key_hash_pattern(to_shared(key_hash_program)) });
    BOOST_REQUIRE_EQUAL(stack->size(), two);
}

BOOST_AUTO_TEST_CASE(witness__extract_segwit__key_hash_one_element__invalid_witness)
{
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = segwit_prevout(key_hash_program);
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>({ 0x01_u8 }) } };

    BOOST_REQUIRE_EQUAL(instance.extract_segwit(out, stack, prevout), error::invalid_witness);
}

BOOST_AUTO_TEST_CASE(witness__extract_segwit__key_hash_three_elements__invalid_witness)
{
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = segwit_prevout(key_hash_program);
    const chunk_cptrs elements{ to_shared<data_chunk>({ 0x01_u8 }), to_shared<data_chunk>({ 0x02_u8 }), to_shared<data_chunk>({ 0x03_u8 }) };
    const chain::witness instance{ elements };

    BOOST_REQUIRE_EQUAL(instance.extract_segwit(out, stack, prevout), error::invalid_witness);
}

BOOST_AUTO_TEST_CASE(witness__extract_segwit__script_hash_committed__success)
{
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto program = to_chunk(sha256_hash(embedded_script));
    const auto prevout = segwit_prevout(program);
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>({ 0x02_u8 }), to_shared<data_chunk>(embedded_script) } };

    BOOST_REQUIRE_EQUAL(instance.extract_segwit(out, stack, prevout), error::script_success);
    BOOST_REQUIRE(*out == script(embedded_script, false));
    BOOST_REQUIRE_EQUAL(stack->size(), one);
}

BOOST_AUTO_TEST_CASE(witness__extract_segwit__script_hash_uncommitted__invalid_witness)
{
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto program = to_chunk(sha256_hash(data_chunk{ 0x52_u8 }));
    const auto prevout = segwit_prevout(program);
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>(embedded_script) } };

    BOOST_REQUIRE_EQUAL(instance.extract_segwit(out, stack, prevout), error::invalid_witness);
}

BOOST_AUTO_TEST_CASE(witness__extract_segwit__script_hash_empty_stack__invalid_witness)
{
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto program = to_chunk(sha256_hash(embedded_script));
    const auto prevout = segwit_prevout(program);
    const chain::witness instance{ chunk_cptrs{} };

    BOOST_REQUIRE_EQUAL(instance.extract_segwit(out, stack, prevout), error::invalid_witness);
}

// A version zero program of neither twenty nor thirty two bytes must fail.
BOOST_AUTO_TEST_CASE(witness__extract_segwit__undefined_program_size__invalid_witness)
{
    script::cptr out{};
    chunk_cptrs_ptr stack{};
    const auto prevout = segwit_prevout(data_chunk(21, 0x00));
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>({ 0x01_u8 }), to_shared<data_chunk>({ 0x02_u8 }) } };

    BOOST_REQUIRE_EQUAL(instance.extract_segwit(out, stack, prevout), error::invalid_witness);
}

// is_push_size
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(witness__is_push_size__empty__true)
{
    BOOST_REQUIRE(chain::witness::is_push_size(chunk_cptrs{}));
}

BOOST_AUTO_TEST_CASE(witness__is_push_size__maximum__true)
{
    const auto element = to_shared<data_chunk>(max_push_data_size, 0x42_u8);
    BOOST_REQUIRE(chain::witness::is_push_size(chunk_cptrs{ element }));
}

BOOST_AUTO_TEST_CASE(witness__is_push_size__oversized__false)
{
    const auto element = to_shared<data_chunk>(add1(max_push_data_size), 0x42_u8);
    BOOST_REQUIRE(!chain::witness::is_push_size(chunk_cptrs{ element }));
}

// Any oversized element fails the whole stack.
BOOST_AUTO_TEST_CASE(witness__is_push_size__trailing_oversized__false)
{
    const auto small = to_shared<data_chunk>(1, 0x42_u8);
    const auto large = to_shared<data_chunk>(add1(max_push_data_size), 0x42_u8);
    BOOST_REQUIRE(!chain::witness::is_push_size(chunk_cptrs{ small, large }));
}

// is_reserved_pattern
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(witness__is_reserved_pattern__single_hash__true)
{
    const auto element = to_shared<data_chunk>(hash_size, 0x42_u8);
    BOOST_REQUIRE(chain::witness::is_reserved_pattern(chunk_cptrs{ element }));
}

BOOST_AUTO_TEST_CASE(witness__is_reserved_pattern__empty__false)
{
    BOOST_REQUIRE(!chain::witness::is_reserved_pattern(chunk_cptrs{}));
}

BOOST_AUTO_TEST_CASE(witness__is_reserved_pattern__wrong_size__false)
{
    const auto element = to_shared<data_chunk>(short_hash_size, 0x42_u8);
    BOOST_REQUIRE(!chain::witness::is_reserved_pattern(chunk_cptrs{ element }));
}

BOOST_AUTO_TEST_CASE(witness__is_reserved_pattern__two_hashes__false)
{
    const auto element = to_shared<data_chunk>(hash_size, 0x42_u8);
    const chunk_cptrs stack{ element, element };
    BOOST_REQUIRE(!chain::witness::is_reserved_pattern(stack));
}

// skip
// ----------------------------------------------------------------------------
// The return is superfluity, meaning the stack carries no non-empty element.

BOOST_AUTO_TEST_CASE(witness__skip__prefixed_non_empty__true)
{
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>({ 0x42_u8 }) } };
    const auto data = instance.to_data(true);
    stream::in::copy source{ data };
    read::bytes::istream reader{ source };
    BOOST_REQUIRE(chain::witness::skip(reader, true));
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(witness__skip__prefixed_empty_element__false)
{
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>() } };
    const auto data = instance.to_data(true);
    stream::in::copy source{ data };
    read::bytes::istream reader{ source };
    BOOST_REQUIRE(!chain::witness::skip(reader, true));
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(witness__skip__unprefixed_non_empty__true)
{
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>({ 0x42_u8 }) } };
    const auto data = instance.to_data(false);
    stream::in::copy source{ data };
    read::bytes::istream reader{ source };
    BOOST_REQUIRE(chain::witness::skip(reader, false));
    BOOST_REQUIRE(reader);
    BOOST_REQUIRE(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(witness__skip__prefixed_empty_stack__false)
{
    const chain::witness instance{ chunk_cptrs{} };
    const auto data = instance.to_data(true);
    stream::in::copy source{ data };
    read::bytes::istream reader{ source };
    BOOST_REQUIRE(!chain::witness::skip(reader, true));
    BOOST_REQUIRE(reader);
}

// construct
// ----------------------------------------------------------------------------

static data_stack witness_stack() NOEXCEPT
{
    return data_stack{ { 0x01_u8 }, { 0x02_u8, 0x03_u8 } };
}

BOOST_AUTO_TEST_CASE(witness__construct__copy_data_stack__expected)
{
    const auto stack = witness_stack();
    const chain::witness instance{ stack };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(instance.stack().size(), two);
}

BOOST_AUTO_TEST_CASE(witness__construct__data_slice_prefixed__round_trips)
{
    const chain::witness expected{ witness_stack() };
    const auto data = expected.to_data(true);
    const chain::witness instance{ data, true };
    BOOST_REQUIRE(instance == expected);
}

// Unprefixed deserialization reads elements until the source is exhausted.
BOOST_AUTO_TEST_CASE(witness__construct__data_slice_unprefixed__round_trips)
{
    const chain::witness expected{ witness_stack() };
    const auto data = expected.to_data(false);
    const chain::witness instance{ data, false };
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(witness__construct__fast_stream__round_trips)
{
    const chain::witness expected{ witness_stack() };
    const auto data = expected.to_data(true);
    stream::in::fast source{ data };
    const chain::witness instance{ source, true };
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(witness__construct__istream__round_trips)
{
    const chain::witness expected{ witness_stack() };
    const auto data = expected.to_data(true);
    const std::string text(data.begin(), data.end());
    std::istringstream source{ text };
    const chain::witness instance{ source, true };
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(witness__inequality__different__true)
{
    const chain::witness instance1{ witness_stack() };
    const chain::witness instance2{ chunk_cptrs{} };
    BOOST_REQUIRE(instance1 != instance2);
}

// serialization
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(witness__to_data__ostream__expected)
{
    const chain::witness instance{ witness_stack() };
    const auto expected = instance.to_data(true);
    std::ostringstream output{};
    instance.to_data(output, true);
    const std::string text(expected.begin(), expected.end());
    BOOST_REQUIRE_EQUAL(output.str(), text);
}

// text
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(witness__construct__whitespace_mnemonic__empty)
{
    const chain::witness instance{ "  " };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance.stack().empty());
}

BOOST_AUTO_TEST_CASE(witness__construct__undelimited_mnemonic__invalid)
{
    const chain::witness instance{ "42" };
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(witness__to_string__invalid__expected)
{
    const chain::witness instance{ "42" };
    BOOST_REQUIRE_EQUAL(instance.to_string(), "(?)");
}

BOOST_AUTO_TEST_SUITE_END()
