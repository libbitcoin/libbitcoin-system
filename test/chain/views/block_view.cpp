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
    const chain::view::block view{ {}, true };
    BOOST_CHECK(!view.is_valid());
}

BOOST_AUTO_TEST_CASE(block_view__construct__genesis__valid)
{
    const auto& block = test::genesis;
    const chain::view::block view{ block.to_data(true), true };
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
    const chain::view::block view{ block.to_data(true), true };
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
    const chain::view::block view{ block.to_data(true), false };
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
    const chain::view::block view{ block.to_data(true), true };
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

BOOST_AUTO_TEST_CASE(block_view__to_data__block1a_witness__matches_block)
{
    const auto& block = test::block1a;
    const chain::view::block view{ block.to_data(true), true };
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

BOOST_AUTO_TEST_CASE(block_view__to_data__block1a_stripped_source__stripped)
{
    const auto& block = test::block1a;
    const chain::view::block view{ block.to_data(false), false };
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(false));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

BOOST_AUTO_TEST_CASE(block_view__to_data__block2a_witness__matches_block)
{
    // block2a is a multi-transaction block bearing witness data.
    const auto& block = test::block2a;
    const chain::view::block view{ block.to_data(true), true };
    BOOST_REQUIRE(view.is_segregated());
    BOOST_CHECK_EQUAL(view.transactions(), block.transactions_ptr()->size());
    BOOST_CHECK_EQUAL(view.to_data(true), block.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), block.to_data(false));
}

BOOST_AUTO_TEST_CASE(block_view__to_data__mixed_witness_and_legacy__matches_block)
{
    const auto& block = test::block2c;
    const chain::view::block view{ block.to_data(true), true };
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
    const chain::view::block view{ block.to_data(true), true };
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
    const chain::view::block view{ block.to_data(true), true };
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
    const chain::view::block view{ block.to_data(true), true };
    BOOST_CHECK(!view.is_segregated());
    BOOST_CHECK_EQUAL(view.identify({ 0, 1, 0 }), error::block_success);
}

// malleation
// ----------------------------------------------------------------------------

// Malleation methods are private, so asserted through identify.
// Each fixture carries its computed merkle root.

// Sixty four byte transaction, the malleable64 unit.
static system::chain::transaction view_tx64(uint32_t index) NOEXCEPT
{
    using namespace system::chain;
    const operations ops{ operation{ opcode::dup }, operation{ opcode::dup } };
    const script dups{ ops };
    const inputs ins{ input{ point{ one_hash, index }, dups, 42 } };
    const outputs outs{ output{ 42, dups } };
    return transaction{ 42, ins, outs, 42 };
}

static system::chain::transaction view_coinbase64() NOEXCEPT
{
    using namespace system::chain;
    const operations ops{ operation{ opcode::dup }, operation{ opcode::dup } };
    const script dups{ ops };
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

static system::hash_digest view_root(
    const system::chain::transactions& txs) NOEXCEPT
{
    system::hashes leaves{};
    for (const auto& tx: txs)
        leaves.push_back(tx.hash(false));

    return system::merkle_root(std::move(leaves));
}

static system::chain::transactions view_txs(
    const std::vector<uint32_t>& indexes) NOEXCEPT
{
    system::chain::transactions txs{};
    for (const auto index: indexes)
        txs.push_back(view_tx(index));

    return txs;
}

static system::chain::block view_block(
    const system::chain::transactions& txs) NOEXCEPT
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

    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

// A null point in the first transaction precludes the malleated64 shape.
BOOST_AUTO_TEST_CASE(block_view__identify__sixty_four_byte_coinbase_first__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_coinbase64(), view_tx64(1) };
    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__identify__mixed_transaction_sizes__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx64(0), view_tx(1) };
    BOOST_REQUIRE_NE(txs.back().serialized_size(false), 64u);

    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__identify__tail_clone_of_four__invalid_transaction_commitment)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(1), view_tx(2), view_tx(2) };
    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

BOOST_AUTO_TEST_CASE(block_view__identify__four_distinct__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(1), view_tx(2), view_tx(3) };
    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

// A duplicate that is not the merkle clone is not a malleation.
BOOST_AUTO_TEST_CASE(block_view__identify__leading_duplicate_of_four__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(0), view_tx(2), view_tx(3) };
    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

// An even set at width depth is not cloned by merkle.
BOOST_AUTO_TEST_CASE(block_view__identify__tail_clone_of_two__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(0) };
    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

// Prefixes of a padded set share its root.
BOOST_AUTO_TEST_CASE(block_view__identify__two_trailing_clones_of_seven__invalid_transaction_commitment)
{
    using namespace system;
    const auto txs = view_txs({ 0, 1, 2, 3, 4, 4, 4 });
    BOOST_REQUIRE_EQUAL(view_root(txs), view_root(view_txs({ 0, 1, 2, 3, 4 })));

    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

BOOST_AUTO_TEST_CASE(block_view__identify__four_trailing_clones_of_thirteen__invalid_transaction_commitment)
{
    using namespace system;
    const auto txs = view_txs({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 8, 8, 8, 8 });
    BOOST_REQUIRE_EQUAL(view_root(txs), view_root(view_txs({ 0, 1, 2, 3, 4, 5, 6, 7, 8 })));

    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

BOOST_AUTO_TEST_CASE(block_view__identify__two_trailing_pair_clones_of_fourteen__invalid_transaction_commitment)
{
    using namespace system;
    const auto txs = view_txs({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 8, 9, 8, 9 });
    BOOST_REQUIRE_EQUAL(view_root(txs), view_root(view_txs({ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 })));

    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

// Not a prefix of any padded set.
BOOST_AUTO_TEST_CASE(block_view__identify__two_distinct_trailing_clones_of_seven__block_success)
{
    using namespace system;
    const auto txs = view_txs({ 0, 1, 2, 3, 3, 4, 4 });
    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

// Only the last node at a depth is cloned.
BOOST_AUTO_TEST_CASE(block_view__identify__leading_pair_clone_of_eight__block_success)
{
    using namespace system;
    const auto txs = view_txs({ 0, 1, 0, 1, 2, 3, 4, 5 });
    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

// merkle root
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block_view__identify__computed_root__block_success)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(1) };
    const chain::view::block view{ view_block(txs).to_data(true), true };
    BOOST_CHECK_EQUAL(view.identify(), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__identify__wrong_root__invalid_transaction_commitment)
{
    using namespace system;
    const chain::transactions txs{ view_tx(0), view_tx(1) };
    const chain::header head{ 1, hash_digest{}, one_hash, 0, 0, 0 };
    const chain::block block{ head, txs };
    const chain::view::block view{ block.to_data(true), true };
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

// Witness commitment [bip141].

static chain::script commitment_script(const hash_digest& commitment) NOEXCEPT
{
    constexpr auto head = to_big_endian(chain::witness_head);
    data_chunk data(head.size() + hash_size);
    const auto start = std::next(data.begin(), head.size());
    std::copy(head.begin(), head.end(), data.begin());
    std::copy(commitment.begin(), commitment.end(), start);

    return chain::script
    {
        chain::operations
        {
            chain::operation{ chain::opcode::op_return },
            chain::operation{ data, false }
        }
    };
}

static chain::witness reservation_witness(
    const hash_digest& reservation) NOEXCEPT
{
    const chunk_cptrs stack{ to_shared<data_chunk>(to_chunk(reservation)) };
    return chain::witness{ stack };
}

static chain::block commitment_block(const hash_digest& commitment,
    const chain::witness& spender) NOEXCEPT
{
    const chain::inputs ins
    {
        chain::input
        {
            chain::point{},
            chain::script{},
            spender,
            0xffffffff
        }
    };

    const chain::outputs outs
    {
        chain::output
        {
            0,
            commitment_script(commitment)
        }
    };

    return chain::block
    {
        chain::header{},
        chain::transactions
        {
            chain::transaction{ 1, ins, outs, 0 }
        }
    };
}

static hash_digest expected_commitment(const hash_digest& reservation) NOEXCEPT
{
    return sha256::double_hash(null_hash, reservation);
}

BOOST_AUTO_TEST_CASE(block_view__construct__truncated__invalid)
{
    auto data = test::block1a.to_data(true);
    data.resize(sub1(data.size()));
    const chain::view::block view{ std::move(data), true };
    BOOST_CHECK(!view.is_valid());
}

BOOST_AUTO_TEST_CASE(block_view__to_data__ostream__round_trips)
{
    const auto& block = test::block1a;
    const chain::view::block view{ block.to_data(true), true };
    std::ostringstream stream{};
    view.to_data(stream, true);
    const auto text = stream.str();
    const data_chunk data(text.begin(), text.end());
    BOOST_CHECK_EQUAL(data, block.to_data(true));
}

BOOST_AUTO_TEST_CASE(block_view__identify__empty__empty_block)
{
    const chain::view::block view{ {}, true };
    BOOST_CHECK_EQUAL(view.identify(), error::empty_block);
}

BOOST_AUTO_TEST_CASE(block_view__identify__bad_merkle__transaction_commitment)
{
    const auto block = commitment_block(one_hash, chain::witness{});
    const chain::view::block view{ block.to_data(true), true };
    BOOST_CHECK_EQUAL(view.identify(), error::invalid_transaction_commitment);
}

BOOST_AUTO_TEST_CASE(block_view__identify__empty_context__block_success)
{
    const chain::context ctx{ chain::flags::bip141_rule, 0, 0, 0, 0, 0, 0 };
    const chain::view::block view{ {}, true };
    BOOST_CHECK_EQUAL(view.identify(ctx), error::block_success);
}

// Witness commitment [bip141].

// Access protected validation methods.
class accessor
  : public chain::view::block
{
public:
    using chain::view::block::block;
    using chain::view::block::is_invalid_witness_commitment;
};

BOOST_AUTO_TEST_CASE(block_view__is_invalid_witness_commitment__empty__false)
{
    const accessor view{ {}, true };
    BOOST_CHECK(!view.is_invalid_witness_commitment());
}

BOOST_AUTO_TEST_CASE(block_view__is_invalid_witness_commitment__no_reserve__true)
{
    const auto block = commitment_block(one_hash, chain::witness{});
    const accessor view{ block.to_data(true), true };
    BOOST_CHECK(view.is_invalid_witness_commitment());
}

BOOST_AUTO_TEST_CASE(block_view__is_invalid_witness_commitment__wrong__true)
{
    const auto reserved = reservation_witness(null_hash);
    const auto block = commitment_block(one_hash, reserved);
    const accessor view{ block.to_data(true), true };
    BOOST_CHECK(view.is_invalid_witness_commitment());
}

BOOST_AUTO_TEST_CASE(block_view__is_invalid_witness_commitment__valid__false)
{
    const auto commitment = expected_commitment(one_hash);
    const auto reserved = reservation_witness(one_hash);
    const auto block = commitment_block(commitment, reserved);
    const accessor view{ block.to_data(true), true };
    BOOST_CHECK(!view.is_invalid_witness_commitment());
}

// A segregated serialization whose witnesses are all empty is superfluous,
// and is treated as an invalid serialization [bip144].
BOOST_AUTO_TEST_CASE(block_view__construct__superfluous_witness__invalid)
{
    auto data = base16_chunk("0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010100000000010100000000000000000000000000000000000000000000000000000000000000000000000000ffffffff010000000000000000000000000000");
    const chain::view::block view{ std::move(data), true };
    BOOST_CHECK(!view.is_valid());
}

// populate/check/accept/connect
// ----------------------------------------------------------------------------

using namespace system::chain;

constexpr uint64_t subsidy_interval = 210'000;
constexpr uint64_t initial_subsidy = 5'000'000'000;
constexpr auto bip143_flags = flags::bip141_rule | flags::bip143_rule;
const context no_rules_context{ flags::no_rules, 0, 0, 100, 0, 0, 0 };

static transaction view_coinbase(uint64_t value=initial_subsidy) NOEXCEPT
{
    const operation push{ data_chunk{ 0x01, 0x02 }, false };
    const script coinbase_script{ operations{ push } };
    const inputs ins{ input{ point{}, coinbase_script, max_input_sequence } };
    return { 1, ins, outputs{ output{ value, script{} } }, 0 };
}

static transaction view_spend(const point& point, uint64_t value,
    uint32_t sequence=max_input_sequence, uint32_t version=1) NOEXCEPT
{
    const inputs ins{ input{ point, script{}, sequence } };
    return { version, ins, outputs{ output{ value, script{} } }, 0 };
}

static data_chunk view_block_data(const transactions& txs) NOEXCEPT
{
    return block{ header{}, txs }.to_data(true);
}

static data_chunk view_prevouts_data(const outputs& prevouts) NOEXCEPT
{
    data_chunk out{};
    for (const auto& prevout: prevouts)
    {
        const auto data = prevout.to_data();
        out.insert(out.end(), data.begin(), data.end());
    }

    return out;
}

BOOST_AUTO_TEST_CASE(block_view__populate__coinbase_only__success)
{
    chain::view::block view{ view_block_data({ view_coinbase() }), true };
    BOOST_REQUIRE(view.is_valid());
    BOOST_REQUIRE(!view.is_populated());
    BOOST_REQUIRE_EQUAL(view.spends(), 0u);
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, {}), error::block_success);
    BOOST_REQUIRE(view.is_populated());
    BOOST_REQUIRE_EQUAL(view.inputs().size(), 1u);
    BOOST_REQUIRE(view.prevouts().empty());
    BOOST_REQUIRE(view.views().front().is_populated());
    BOOST_REQUIRE(view.views().front().inputs_begin()->is_null_point());
    BOOST_REQUIRE_EQUAL(view.check(), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(no_rules_context), error::block_success);
    BOOST_REQUIRE_EQUAL(view.accept(no_rules_context, subsidy_interval, initial_subsidy), error::block_success);
    BOOST_REQUIRE_EQUAL(view.connect(no_rules_context, {}), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__populate__external_and_internal_spends__prevouts_assigned)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    const auto tx2 = view_spend({ tx1.hash(false), 0 }, 40);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1, tx2 }), true };
    BOOST_REQUIRE_EQUAL(view.spends(), 2u);
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} }, { 42, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.inputs().size(), 3u);
    BOOST_REQUIRE_EQUAL(view.prevouts().size(), 2u);

    const auto& spender = view.views().back();
    BOOST_REQUIRE(spender.is_populated());
    BOOST_REQUIRE(!is_null(spender.inputs_begin()->prevout));
    BOOST_REQUIRE_EQUAL(spender.inputs_begin()->prevout->value(), 42u);
    BOOST_REQUIRE_EQUAL(spender.value(), 42u);
    BOOST_REQUIRE_EQUAL(spender.spend(), 40u);
    BOOST_REQUIRE_EQUAL(spender.fee(), 2u);
    BOOST_REQUIRE_EQUAL(view.check(), error::block_success);
    BOOST_REQUIRE_EQUAL(view.accept(no_rules_context, subsidy_interval, initial_subsidy), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__populate__selected__selected_prevouts_assigned)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    const auto tx2 = view_spend({ one_hash, 1 }, 40);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1, tx2 }), true };
    BOOST_REQUIRE(view.populate(view_prevouts_data({ { 50, script{} } }), { false, false, true }));
    BOOST_REQUIRE(view.is_populated());
    BOOST_REQUIRE_EQUAL(view.inputs().size(), 3u);
    BOOST_REQUIRE_EQUAL(view.prevouts().size(), 1u);
    BOOST_REQUIRE(is_null(view.views().at(1).inputs_begin()->prevout));
    BOOST_REQUIRE(!is_null(view.views().back().inputs_begin()->prevout));
    BOOST_REQUIRE_EQUAL(view.views().back().inputs_begin()->prevout->value(), 50u);
}

BOOST_AUTO_TEST_CASE(block_view__populate__none_selected__no_prevouts)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1 }), true };
    BOOST_REQUIRE(view.populate(data_chunk{}, { false, false }));
    BOOST_REQUIRE(view.is_populated());
    BOOST_REQUIRE(view.prevouts().empty());
    BOOST_REQUIRE(is_null(view.views().back().inputs_begin()->prevout));
}

BOOST_AUTO_TEST_CASE(block_view__populate__selected_short_prevouts__false)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1 }), true };
    BOOST_REQUIRE(!view.populate(data_chunk{}, { false, true }));
}

BOOST_AUTO_TEST_CASE(block_view__populate__selection_size_mismatch__false)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1 }), true };
    BOOST_REQUIRE(!view.populate(view_prevouts_data({ { 50, script{} } }), { true }));
}

BOOST_AUTO_TEST_CASE(block_view__populate__short_prevouts__missing_previous_output)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, {}), error::missing_previous_output);
}

BOOST_AUTO_TEST_CASE(block_view__populate__excess_prevouts__missing_previous_output)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} }, { 42, script{} } })), error::missing_previous_output);
}

BOOST_AUTO_TEST_CASE(block_view__populate__coinbase_spend__coinbase_maturity)
{
    const auto coinbase = view_coinbase();
    const auto tx1 = view_spend({ coinbase.hash(false), 0 }, 42);
    chain::view::block view{ view_block_data({ coinbase, tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { initial_subsidy, script{} } })), error::coinbase_maturity);
}

BOOST_AUTO_TEST_CASE(block_view__populate__internal_relative_lock_bip68__relative_time_locked)
{
    const context ctx{ flags::bip68_rule, 0, 0, 100, 0, 0, 0 };
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    const auto tx2 = view_spend({ tx1.hash(false), 0 }, 40, 1, 2);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1, tx2 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(ctx, view_prevouts_data({ { 50, script{} }, { 42, script{} } })), error::relative_time_locked);
}

BOOST_AUTO_TEST_CASE(block_view__populate__internal_relative_lock_bip68_off__success)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    const auto tx2 = view_spend({ tx1.hash(false), 0 }, 40, 1, 2);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1, tx2 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} }, { 42, script{} } })), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__check__first_not_coinbase__first_not_coinbase)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    chain::view::block view{ view_block_data({ tx1, view_coinbase() }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { initial_subsidy, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(), error::first_not_coinbase);
}

BOOST_AUTO_TEST_CASE(block_view__check__extra_coinbases__extra_coinbases)
{
    chain::view::block view{ view_block_data({ view_coinbase(), view_coinbase(1) }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 1, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(), error::extra_coinbases);
}

BOOST_AUTO_TEST_CASE(block_view__check__forward_reference__forward_reference)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    const auto tx2 = view_spend({ tx1.hash(false), 0 }, 40);
    chain::view::block view{ view_block_data({ view_coinbase(), tx2, tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 42, script{} }, { 50, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(), error::forward_reference);
}

BOOST_AUTO_TEST_CASE(block_view__check__internal_double_spend__block_internal_double_spend)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    const auto tx2 = view_spend({ one_hash, 0 }, 40);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1, tx2 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} }, { 50, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(), error::block_internal_double_spend);
}

BOOST_AUTO_TEST_CASE(block_view__check__null_non_coinbase__previous_output_null)
{
    const auto tx1 = transaction{ 1, inputs{ input{ point{ one_hash, 0 }, script{}, max_input_sequence }, input{ point{}, script{}, max_input_sequence } }, outputs{ output{ 42, script{} } }, 0 };
    chain::view::block view{ view_block_data({ view_coinbase(), tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} }, { 50, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(), error::previous_output_null);
}

BOOST_AUTO_TEST_CASE(block_view__check_context__coinbase_height_bip34__coinbase_height_mismatch)
{
    const context ctx{ flags::bip34_rule, 0, 0, 100, 0, 0, 0 };
    chain::view::block view{ view_block_data({ view_coinbase() }), true };
    BOOST_REQUIRE_EQUAL(view.populate(ctx, {}), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(ctx), error::coinbase_height_mismatch);
}

BOOST_AUTO_TEST_CASE(block_view__check_context__absolute_locked__absolute_time_locked)
{
    const auto tx1 = transaction{ 1, inputs{ input{ point{ one_hash, 0 }, script{}, 0 } }, outputs{ output{ 42, script{} } }, 200 };
    chain::view::block view{ view_block_data({ view_coinbase(), tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(no_rules_context), error::absolute_time_locked);
}

BOOST_AUTO_TEST_CASE(block_view__accept__claim_above_reward__coinbase_value_limit)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    chain::view::block view{ view_block_data({ view_coinbase(initial_subsidy + 9), tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.accept(no_rules_context, subsidy_interval, initial_subsidy), error::coinbase_value_limit);
}

BOOST_AUTO_TEST_CASE(block_view__accept__claim_at_reward__block_success)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 42);
    chain::view::block view{ view_block_data({ view_coinbase(initial_subsidy + 8), tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.accept(no_rules_context, subsidy_interval, initial_subsidy), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__accept__overspent__spend_exceeds_value)
{
    const auto tx1 = view_spend({ one_hash, 0 }, 51);
    chain::view::block view{ view_block_data({ view_coinbase(), tx1 }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data({ { 50, script{} } })), error::block_success);
    BOOST_REQUIRE_EQUAL(view.accept(no_rules_context, subsidy_interval, initial_subsidy), error::spend_exceeds_value);
}

// bip143 native p2wpkh (inputs: p2pk, p2wpkh).
static const auto bip143_tx_data = base16_chunk("01000000000102fff7f7881a8099afa6940d42d1e7f6362bec38171ea3edf433541db4e4ad969f00000000494830450221008b9d1dc26ba6a9cb62127b02742fa9d754cd3bebf337f7a55d114c8e5cdd30be022040529b194ba3f9281a99f2b1c0a19c0489bc22ede944ccf4ecbab4cc618ef3ed01eeffffffef51e1b804cc89d182d279655c3aa89e815b1b309fe287d9b2b55d57b90ec68a0100000000ffffffff02202cb206000000001976a9148280b37df378db99f66f85c95a783a76ac7a6d5988ac9093510d000000001976a9143bde42dbee7e4dbe6a21b2d50ce2f0167faa815988ac000247304402203609e17b84f6a7d30c80bfa610b5b4542f32a8a0d5447a12fb1366d7f01cc44a0220573a954c4518331561406f90300e8f3358f51928d43c212a8caed02de67eebee0121025476c2e83188368da1ff3e292e7acafcdb3566bb0ad253f62fc70f07aeee635711000000");
static const script bip143_p2pk_script(base16_chunk("2103c9f4836b9a4f77fc0d81f7bcb01b7f1b35916864b9476c241ce9fc198bd25432ac"), false);
static const script bip143_p2wpkh_script(base16_chunk("00141d0f172a0ecb48aee1be1f2687d2963ae33f71a1"), false);
static const outputs bip143_prevouts
{
    { 625000000, bip143_p2pk_script },
    { 600000000, bip143_p2wpkh_script }
};

static const std::vector<uint8_t> bip143_coverages
{
    coverage::hash_all,
    coverage::hash_none,
    coverage::hash_single,
    coverage::all_anyone_can_pay,
    coverage::none_anyone_can_pay,
    coverage::single_anyone_can_pay
};

static block bip143_block() NOEXCEPT
{
    const transaction tx{ bip143_tx_data, true };
    const block block{ header{}, transactions{ view_coinbase(), tx } };
    const auto& spend = *block.transactions_ptr()->back();
    (*spend.inputs_ptr())[0]->prevout = to_shared(bip143_prevouts[0]);
    (*spend.inputs_ptr())[1]->prevout = to_shared(bip143_prevouts[1]);
    return block;
}

BOOST_AUTO_TEST_CASE(block_view__connect__bip143_p2wpkh__script_success)
{
    const context ctx{ bip143_flags, 0, 0, 100, 0, 0, 0 };
    chain::view::block view{ view_block_data({ view_coinbase(), transaction{ bip143_tx_data, true } }), true };
    BOOST_REQUIRE(view.is_valid());
    BOOST_REQUIRE_EQUAL(view.populate(ctx, view_prevouts_data(bip143_prevouts)), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(), error::block_success);
    BOOST_REQUIRE_EQUAL(view.check(ctx), error::block_success);
    BOOST_REQUIRE_EQUAL(view.accept(ctx, subsidy_interval, initial_subsidy), error::block_success);
    BOOST_REQUIRE_EQUAL(view.connect(ctx, {}), error::block_success);
}

BOOST_AUTO_TEST_CASE(block_view__connect__bip143_p2wpkh_without_bip143__stack_false)
{
    const context ctx{ flags::bip141_rule, 0, 0, 100, 0, 0, 0 };
    chain::view::block view{ view_block_data({ view_coinbase(), transaction{ bip143_tx_data, true } }), true };
    BOOST_REQUIRE_EQUAL(view.populate(ctx, view_prevouts_data(bip143_prevouts)), error::block_success);
    BOOST_REQUIRE_EQUAL(view.connect(ctx, {}), error::stack_false);
}

BOOST_AUTO_TEST_CASE(block_view__connect__bip143_p2wpkh_without_bip141__unexpected_witness)
{
    chain::view::block view{ view_block_data({ view_coinbase(), transaction{ bip143_tx_data, true } }), true };
    BOOST_REQUIRE_EQUAL(view.populate(no_rules_context, view_prevouts_data(bip143_prevouts)), error::block_success);
    BOOST_REQUIRE_EQUAL(view.connect(no_rules_context, {}), error::unexpected_witness);
}

BOOST_AUTO_TEST_CASE(block_view__connect__bip143_p2wpkh_stripped__invalid_witness)
{
    const context ctx{ bip143_flags, 0, 0, 100, 0, 0, 0 };
    chain::view::block view{ view_block_data({ view_coinbase(), transaction{ bip143_tx_data, true } }), false };
    BOOST_REQUIRE(!view.is_segregated());
    BOOST_REQUIRE_EQUAL(view.populate(ctx, view_prevouts_data(bip143_prevouts)), error::block_success);
    BOOST_REQUIRE_EQUAL(view.connect(ctx, {}), error::invalid_witness);
}

BOOST_AUTO_TEST_CASE(block_view__signature_operations__bip143__matches_block)
{
    const context ctx{ bip143_flags, 0, 0, 100, 0, 0, 0 };
    const auto block = bip143_block();
    chain::view::block view{ view_block_data({ view_coinbase(), transaction{ bip143_tx_data, true } }), true };
    BOOST_REQUIRE_EQUAL(view.populate(ctx, view_prevouts_data(bip143_prevouts)), error::block_success);

    const auto& tx = *block.transactions_ptr()->back();
    const auto& tx_view = view.views().back();
    BOOST_REQUIRE_EQUAL(tx_view.signature_operations(true, true), tx.signature_operations(true, true));
    BOOST_REQUIRE_EQUAL(tx_view.signature_operations(false, false), tx.signature_operations(false, false));
    BOOST_REQUIRE_EQUAL(tx_view.fee(), tx.fee());
    BOOST_REQUIRE_EQUAL(tx_view.value(), tx.value());
    BOOST_REQUIRE_EQUAL(tx_view.spend(), tx.spend());
}

BOOST_AUTO_TEST_CASE(block_view__compute_filter__bip143__matches_block)
{
    const context ctx{ bip143_flags, 0, 0, 100, 0, 0, 0 };
    const auto block = bip143_block();
    chain::view::block view{ view_block_data({ view_coinbase(), transaction{ bip143_tx_data, true } }), true };
    BOOST_REQUIRE_EQUAL(view.populate(ctx, view_prevouts_data(bip143_prevouts)), error::block_success);

    data_chunk expected{};
    data_chunk actual{};
    BOOST_REQUIRE(neutrino::compute_filter(expected, block));
    BOOST_REQUIRE(neutrino::compute_filter(actual, view));
    BOOST_REQUIRE_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_CASE(block_view__signature_hash__bip143_p2pk__matches_transaction)
{
    const context ctx{ bip143_flags, 0, 0, 100, 0, 0, 0 };
    const auto block = bip143_block();
    chain::view::block view{ view_block_data({ view_coinbase(), transaction{ bip143_tx_data, true } }), true };
    BOOST_REQUIRE_EQUAL(view.populate(ctx, view_prevouts_data(bip143_prevouts)), error::block_success);

    const auto& tx = *block.transactions_ptr()->back();
    const auto& tx_view = view.views().back();
    const auto& prevout = bip143_prevouts[0];
    const auto it = tx.input_at(0);
    const auto it_view = tx_view.input_at(0);
    const hash_cptr tapleaf{};
    hash_digest expected{};
    hash_digest actual{};

    for (const auto sighash_flags: bip143_coverages)
    {
        BOOST_REQUIRE(tx.signature_hash(expected, it, prevout.script(), prevout.value(), tapleaf, script_version::unversioned, sighash_flags, flags::no_rules));
        BOOST_REQUIRE(tx_view.signature_hash(actual, it_view, prevout.script(), prevout.value(), tapleaf, script_version::unversioned, sighash_flags, flags::no_rules));
        BOOST_REQUIRE_EQUAL(actual, expected);
        BOOST_REQUIRE(tx.signature_hash(expected, it, prevout.script(), prevout.value(), tapleaf, script_version::segwit, sighash_flags, bip143_flags));
        BOOST_REQUIRE(tx_view.signature_hash(actual, it_view, prevout.script(), prevout.value(), tapleaf, script_version::segwit, sighash_flags, bip143_flags));
        BOOST_REQUIRE_EQUAL(actual, expected);
    }
}

BOOST_AUTO_TEST_CASE(block_view__signature_hash__bip143_p2wpkh__matches_transaction)
{
    const context ctx{ bip143_flags, 0, 0, 100, 0, 0, 0 };
    const auto block = bip143_block();
    chain::view::block view{ view_block_data({ view_coinbase(), transaction{ bip143_tx_data, true } }), true };
    BOOST_REQUIRE_EQUAL(view.populate(ctx, view_prevouts_data(bip143_prevouts)), error::block_success);

    const auto& tx = *block.transactions_ptr()->back();
    const auto& tx_view = view.views().back();
    const auto& prevout = bip143_prevouts[1];
    const auto it = tx.input_at(1);
    const auto it_view = tx_view.input_at(1);
    const hash_cptr tapleaf{};
    hash_digest expected{};
    hash_digest actual{};

    for (const auto sighash_flags: bip143_coverages)
    {
        BOOST_REQUIRE(tx.signature_hash(expected, it, prevout.script(), prevout.value(), tapleaf, script_version::unversioned, sighash_flags, flags::no_rules));
        BOOST_REQUIRE(tx_view.signature_hash(actual, it_view, prevout.script(), prevout.value(), tapleaf, script_version::unversioned, sighash_flags, flags::no_rules));
        BOOST_REQUIRE_EQUAL(actual, expected);
        BOOST_REQUIRE(tx.signature_hash(expected, it, prevout.script(), prevout.value(), tapleaf, script_version::segwit, sighash_flags, bip143_flags));
        BOOST_REQUIRE(tx_view.signature_hash(actual, it_view, prevout.script(), prevout.value(), tapleaf, script_version::segwit, sighash_flags, bip143_flags));
        BOOST_REQUIRE_EQUAL(actual, expected);
    }
}

BOOST_AUTO_TEST_SUITE_END()
