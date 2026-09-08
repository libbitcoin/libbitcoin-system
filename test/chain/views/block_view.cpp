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
#include "../../mocks/blocks.hpp"

BOOST_AUTO_TEST_SUITE(block_view_tests)

// construct and properties

BOOST_AUTO_TEST_CASE(block_view__construct__empty__invalid)
{
    const chain::block_view view{ {}, true };
    BOOST_CHECK(!view.is_valid());
}

BOOST_AUTO_TEST_CASE(block_view__construct__genesis__valid)
{
    const auto& block = test::genesis;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(!view.is_segregated());
    BOOST_CHECK_EQUAL(view.transactions(), 1u);
    BOOST_CHECK_EQUAL(view.views().size(), 1u);
    BOOST_CHECK_EQUAL(view.hash(), block.hash());
    BOOST_CHECK_EQUAL(view.serialized_size(true), block.serialized_size(true));
    BOOST_CHECK_EQUAL(view.serialized_size(false), block.serialized_size(false));
}

BOOST_AUTO_TEST_CASE(block_view__construct__block1a_witness__valid)
{
    const auto& block = test::block1a;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(view.is_segregated());
    BOOST_CHECK_EQUAL(view.transactions(), 1u);
    BOOST_CHECK_EQUAL(view.views().size(), 1u);
    BOOST_CHECK_EQUAL(view.hash(), block.hash());
    BOOST_CHECK_EQUAL(view.serialized_size(true), block.serialized_size(true));
    BOOST_CHECK_EQUAL(view.serialized_size(false), block.serialized_size(false));
}

BOOST_AUTO_TEST_CASE(block_view__construct__block1a_non_witness__valid)
{
    const auto& block = test::block1a;
    const chain::block_view view{ block.to_data(true), false };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(!view.is_segregated());
    BOOST_CHECK_EQUAL(view.transactions(), 1u);
    BOOST_CHECK_EQUAL(view.views().size(), 1u);
    BOOST_CHECK_EQUAL(view.hash(), block.hash());
    BOOST_CHECK_EQUAL(view.serialized_size(true), block.serialized_size(true));
    BOOST_CHECK_EQUAL(view.serialized_size(false), block.serialized_size(false));
}

// to_data

BOOST_AUTO_TEST_CASE(block_view__to_data__genesis__matches_block)
{
    const auto& block = test::genesis;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

BOOST_AUTO_TEST_CASE(block_view__to_data__block1a_witness__matches_block)
{
    const auto& block = test::block1a;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

BOOST_AUTO_TEST_CASE(block_view__to_data__block1a_stripped_source__stripped)
{
    const auto& block = test::block1a;
    const chain::block_view view{ block.to_data(false), false };
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(false));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

BOOST_AUTO_TEST_CASE(block_view__to_data__block2a_witness__matches_block)
{
    // block2a is a multi-transaction block bearing witness data.
    const auto& block = test::block2a;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_REQUIRE(view.is_segregated());
    BOOST_CHECK_EQUAL(view.transactions(), block.transactions_ptr()->size());
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

BOOST_AUTO_TEST_CASE(block_view__to_data__mixed_witness_and_legacy__matches_block)
{
    const auto& block = test::block2c;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_REQUIRE(view.is_segregated());
    BOOST_REQUIRE_EQUAL(view.transactions(), 2u);
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

// identify1 and identify2

BOOST_AUTO_TEST_CASE(block_view__identify__genesis__expected)
{
    using namespace system;
    constexpr auto bip141 = chain::flags::bip141_rule;

    const auto& block = test::genesis;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_CHECK(view.is_valid());

    auto ec = view.identify();
    BOOST_CHECK_EQUAL(ec, error::block_success);

    ec = view.identify({ bip141, 1, 0 });
    BOOST_CHECK_EQUAL(ec, error::block_success);

    ec = view.identify({ 0, 1, 0 });
    BOOST_CHECK_EQUAL(ec, error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__identify__block1a_witness__expected)
{
    using namespace system;
    constexpr auto bip141 = chain::flags::bip141_rule;

    const auto& block = test::block1a;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_CHECK(view.is_valid());

    // block1a has a bogus merkle root.
    auto ec = view.identify();
    BOOST_CHECK_EQUAL(ec, error::invalid_transaction_commitment);

    // block1a has a bogus merkle root.
    ec = view.identify({ bip141, 1, 0 });
    BOOST_CHECK_EQUAL(ec, error::invalid_witness_commitment);

    // Witness is uncommitted before bip141.
    ec = view.identify({ 0, 1, 0 });
    BOOST_CHECK_EQUAL(ec, error::invalid_witness_commitment);
}

BOOST_AUTO_TEST_CASE(block_view__identify__unwitnessed_bip141_off__block_success)
{
    using namespace system;
    const auto& block = test::genesis;
    const chain::block_view view{ block.to_data(true), true };
    BOOST_CHECK(!view.is_segregated());
    BOOST_CHECK_EQUAL(view.identify({ 0, 1, 0 }), error::block_success);
}

// malleation
// ----------------------------------------------------------------------------

// block_view is final, so malleation is asserted through identify.
// Each fixture carries its computed merkle root.

// Sixty four byte transaction, the malleable64 unit.
static system::chain::transaction view_tx64(uint32_t index) NOEXCEPT
{
    using namespace system::chain;
    const script dups{ operations{ operation{ opcode::dup }, operation{ opcode::dup } } };
    const inputs ins{ input{ point{ one_hash, index }, dups, 42 } };
    const outputs outs{ output{ 42, dups } };
    return transaction{ 42, ins, outs, 42 };
}

static system::chain::transaction view_coinbase64() NOEXCEPT
{
    using namespace system::chain;
    const script dups{ operations{ operation{ opcode::dup }, operation{ opcode::dup } } };
    const inputs ins{ input{ point{}, dups, 42 } };
    const outputs outs{ output{ 42, dups } };
    return transaction{ 42, ins, outs, 42 };
}

static system::chain::transaction view_tx(uint32_t index) NOEXCEPT
{
    using namespace system::chain;
    const inputs ins{ input{ point{ one_hash, index }, script{}, 42 } };
    const outputs outs{ output{ 42, script{} } };
    return transaction{ 1, ins, outs, 0 };
}

static system::hash_digest view_root(const system::chain::transactions& txs) NOEXCEPT
{
    using namespace system;
    const auto left = bitcoin_hash(txs[0].hash(false), txs[1].hash(false));
    if (txs.size() == two) return left;
    const auto right = bitcoin_hash(txs[2].hash(false), txs[3].hash(false));
    return bitcoin_hash(left, right);
}

static system::chain::block view_block(const system::chain::transactions& txs) NOEXCEPT
{
    using namespace system;
    const chain::header head{ 1, hash_digest{}, view_root(txs), 0, 0, 0 };
    return chain::block{ head, txs };
}

BOOST_AUTO_TEST_CASE(block_view__identify__all_sixty_four_byte__invalid_transaction_commitment)
{
    using namespace system;
    const chain::transactions txs{ view_tx64(0), view_tx64(1) };
    BOOST_REQUIRE_EQUAL(txs.front().serialized_size(false), 64u);

    const chain::block_view view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

// A null point in the first transaction precludes the malleated64 shape.
BOOST_AUTO_TEST_CASE(block_view__identify__sixty_four_byte_coinbase_first__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_coinbase64(), view_tx64(1) };
    const chain::block_view view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__identify__mixed_transaction_sizes__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx64(0), view_tx(1) };
    BOOST_REQUIRE_NE(txs.back().serialized_size(false), 64u);

    const chain::block_view view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__identify__tail_clone_of_four__invalid_transaction_commitment)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(1), view_tx(2), view_tx(2) };
    const chain::block_view view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

BOOST_AUTO_TEST_CASE(block_view__identify__four_distinct__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(1), view_tx(2), view_tx(3) };
    const chain::block_view view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

// A duplicate that is not the merkle clone is not a malleation.
BOOST_AUTO_TEST_CASE(block_view__identify__leading_duplicate_of_four__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(0), view_tx(2), view_tx(3) };
    const chain::block_view view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

// An even set at width depth is not cloned by merkle.
BOOST_AUTO_TEST_CASE(block_view__identify__tail_clone_of_two__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(0) };
    const chain::block_view view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

// merkle root
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block_view__identify__computed_root__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(1) };
    const chain::block_view view{ view_block(txs).to_data(true), true };
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__identify__wrong_root__invalid_transaction_commitment)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(1) };
    const chain::header head{ 1, hash_digest{}, one_hash, 0, 0, 0 };
    const chain::block block{ head, txs };
    const chain::block_view view{ block.to_data(true), true };
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

BOOST_AUTO_TEST_SUITE_END()
