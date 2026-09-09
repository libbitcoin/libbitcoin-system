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

BOOST_AUTO_TEST_SUITE(block_tests)

using namespace system::chain;

constexpr auto hash1 = base16_hash("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
constexpr auto hash2 = base16_hash("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
constexpr auto hash3 = base16_hash("bf7c3f5a69a78edd81f3eff7e93a37fb2d7da394d48db4d85e7e5353b9b8e270");

static const header expected_header
{
    10,
    hash1,
    hash2,
    531234,
    6523454,
    68644
};

struct expected_transactions
{
    static const transactions& get() NOEXCEPT
    {
        static transactions txs_
        {
            { 1, inputs{ {} }, { {} }, 48 },
            { 2, inputs{ {} }, { {} }, 32 },
            { 4, inputs{ {} }, { {} }, 16 }
        };

        return txs_;
    }
};

struct expected_block
{
    static const block& get() NOEXCEPT
    {
        static block block_{ expected_header, expected_transactions::get() };
        return block_;
    }

    static const data_chunk& data() NOEXCEPT
    {
        static data_chunk chunk_{ get().to_data(true) };
        return chunk_;
    }
};

// Access protected validation methods.
class accessor
  : public block
{
public:
    // Use base class constructors.
    using block::block;
    using block::is_empty;
    using block::is_oversized;
    using block::is_first_non_coinbase;
    using block::is_extra_coinbases;
    using block::is_forward_reference;
    using block::is_internal_double_spend;
    using block::is_invalid_merkle_root;
    using block::is_overweight;
    using block::is_invalid_coinbase_script;
    using block::is_hash_limit_exceeded;
    using block::is_invalid_witness_commitment;
    using block::is_overspent;
    using block::is_signature_operations_limited;
};

// constructors
// ----------------------------------------------------------------------------
// tests construction, native properties, is_valid

BOOST_AUTO_TEST_CASE(block__constructor__default__invalid)
{
    const block instance{};
    BOOST_REQUIRE(!instance.is_valid());
}

BOOST_AUTO_TEST_CASE(block__constructor__move__expected)
{
    const block expected(expected_header, expected_transactions::get());
    block copy(expected_header, expected_transactions::get());
    const block instance(std::move(copy));
    BOOST_REQUIRE(instance == instance);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_block::get());
}

BOOST_AUTO_TEST_CASE(block__constructor__copy__expected)
{
    const block expected(expected_header, expected_transactions::get());
    const block instance(expected);
    BOOST_REQUIRE(instance == instance);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_block::get());
}

BOOST_AUTO_TEST_CASE(block__constructor__move_parameters__expected)
{
    auto header = expected_header;
    auto transactions = expected_transactions::get();
    const block instance(std::move(header), std::move(transactions));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_block::get());

}

BOOST_AUTO_TEST_CASE(block__constructor__copy_parameters__expected)
{
    const auto header = expected_header;
    const auto& transactions = expected_transactions::get();
    const block instance(header, transactions);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(instance == expected_block::get());
}

BOOST_AUTO_TEST_CASE(block__constructor__data__expected)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto data = genesis.to_data(true);
    const accessor block(data, true);
    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(!block.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__constructor__fast__success)
{
    const auto data = test::mock_block_b.to_data(true);
    test::reporting_arena<false> arena{};
    stream::in::fast stream(data);
    read::bytes::fast source(stream, &arena);
    const accessor block(source, true);
    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(!block.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__constructor__stream__success)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto data = genesis.to_data(true);
    stream::in::copy stream(data);
    const accessor block(stream, true);
    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(!block.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__constructor__reader__success)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto data = genesis.to_data(true);
    read::bytes::copy source(data);
    const accessor block(source, true);
    BOOST_REQUIRE(block.is_valid());
    BOOST_REQUIRE(!block.is_invalid_merkle_root());
}

// operators
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__assign__move__expected)
{
    const auto& alpha = expected_block::get();
    auto gamma = alpha;
    const auto beta = std::move(gamma);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(block__assign__copy__expected)
{
    const auto& alpha = expected_block::get();
    const auto beta = alpha;
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(block__equality__same__true)
{
    const auto& alpha = expected_block::get();
    const block beta(alpha);
    BOOST_REQUIRE(alpha == beta);
}

BOOST_AUTO_TEST_CASE(block__equality__different__false)
{
    const auto& alpha = expected_block::get();
    const block beta;
    BOOST_REQUIRE(!(alpha == beta));
}

BOOST_AUTO_TEST_CASE(block__inequality__same__false)
{
    const auto& alpha = expected_block::get();
    const block beta(alpha);
    BOOST_REQUIRE(!(alpha != beta));
}

BOOST_AUTO_TEST_CASE(block__inequality__different__true)
{
    const auto& alpha = expected_block::get();
    const block beta;
    BOOST_REQUIRE(alpha != beta);
}

// to_data
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__to_data__data__expected)
{
    const auto size = expected_block::data().size();
    BOOST_REQUIRE_EQUAL(size, expected_block::get().serialized_size(true));
}

BOOST_AUTO_TEST_CASE(block__to_data__stream__expected)
{
    // Write block to stream.
    std::stringstream iostream{};
    expected_block::get().to_data(iostream, true);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const block copy(iostream, true);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_block::get());
}

BOOST_AUTO_TEST_CASE(block__to_data__writer__expected)
{
    // Write block to stream.
    std::stringstream iostream{};
    write::bytes::ostream out(iostream);
    expected_block::get().to_data(out, true);
    BOOST_REQUIRE(iostream);

    // Verify stream contents.
    const block copy(iostream, true);
    BOOST_REQUIRE(iostream);
    BOOST_REQUIRE(copy.is_valid());
    BOOST_REQUIRE(copy == expected_block::get());
}

// properties
// ----------------------------------------------------------------------------

// weight

// The coinbase claim is the spend of the first transaction.
BOOST_AUTO_TEST_CASE(block__claim__coinbase_outputs__sum)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const inputs ins{ input{ point{}, coinbase_script, 0xffffffff } };
    const outputs outs{ output{ 40, script{} }, output{ 2, script{} } };
    const transaction coinbase{ 1, ins, outs, 0 };
    const block instance{ header{}, transactions{ coinbase } };
    BOOST_REQUIRE_EQUAL(instance.claim(), 42u);
}

BOOST_AUTO_TEST_CASE(block__claim__no_transactions__zero)
{
    const block instance{ header{}, transactions{} };
    BOOST_REQUIRE_EQUAL(instance.claim(), 0u);
}

// A transaction without prevouts has no fee, so the coinbase contributes none.
BOOST_AUTO_TEST_CASE(block__fees__unpopulated__zero)
{
    const inputs ins{ input{ point{}, script{}, 0xffffffff } };
    const transaction coinbase{ 1, ins, outputs{ output{ 42, script{} } }, 0 };
    const block instance{ header{}, transactions{ coinbase } };
    BOOST_REQUIRE_EQUAL(instance.fees(), 0u);
}

BOOST_AUTO_TEST_CASE(block__fees__populated_spend__difference)
{
    const inputs coins{ input{ point{}, script{}, 0xffffffff } };
    const transaction coinbase{ 1, coins, outputs{ output{ 0, script{} } }, 0 };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, 0xffffffff } };
    const transaction spend{ 1, ins, outputs{ output{ 30, script{} } }, 0 };
    spend.inputs_ptr()->front()->prevout = to_shared(output{ 42, script{} });

    const block instance{ header{}, transactions{ coinbase, spend } };
    BOOST_REQUIRE_EQUAL(instance.fees(), 12u);
}

// The reward is the subsidy plus fees, bounding the coinbase claim.
BOOST_AUTO_TEST_CASE(block__is_overspent__claim_at_subsidy_plus_fees__false)
{
    const inputs coins{ input{ point{}, script{}, 0xffffffff } };
    const transaction coinbase{ 1, coins, outputs{ output{ 5000000012, script{} } }, 0 };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, 0xffffffff } };
    const transaction spend{ 1, ins, outputs{ output{ 30, script{} } }, 0 };
    spend.inputs_ptr()->front()->prevout = to_shared(output{ 42, script{} });

    const accessor instance{ header{}, transactions{ coinbase, spend } };
    BOOST_REQUIRE_EQUAL(instance.fees(), 12u);
    BOOST_REQUIRE(!instance.is_overspent(0, 210000, 5000000000, false));
}

BOOST_AUTO_TEST_CASE(block__is_overspent__claim_above_subsidy_plus_fees__true)
{
    const inputs coins{ input{ point{}, script{}, 0xffffffff } };
    const transaction coinbase{ 1, coins, outputs{ output{ 5000000013, script{} } }, 0 };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, 0xffffffff } };
    const transaction spend{ 1, ins, outputs{ output{ 30, script{} } }, 0 };
    spend.inputs_ptr()->front()->prevout = to_shared(output{ 42, script{} });

    const accessor instance{ header{}, transactions{ coinbase, spend } };
    BOOST_REQUIRE(instance.is_overspent(0, 210000, 5000000000, false));
}

BOOST_AUTO_TEST_CASE(block__spends__genesis__zero)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    BOOST_REQUIRE(is_zero(genesis.spends()));
}

BOOST_AUTO_TEST_CASE(block__spends__coinbase_only__zero)
{
    BOOST_REQUIRE(is_zero(test::mock_block_b.spends()));
}

BOOST_AUTO_TEST_CASE(block__hash__default__matches_header_hash)
{
    const block instance{};
    BOOST_REQUIRE_EQUAL(instance.hash(), instance.header().hash());
}

// is_malleable
// is_segregated
// serialized_size
// segregated

// validation (public)
// ----------------------------------------------------------------------------

// check
BOOST_AUTO_TEST_CASE(block__check__empty__empty_block)
{
    const block instance{};
    BOOST_REQUIRE_EQUAL(instance.check(), error::empty_block);
}
// accept
// connect

// validation (protected)
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__is_empty__default__true)
{
    const accessor instance;
    BOOST_REQUIRE(instance.is_empty());
}

BOOST_AUTO_TEST_CASE(block__is_empty__single_transaction__false)
{
    const accessor instance
    {
        header{},
        { {} }
    };

    BOOST_REQUIRE(!instance.is_empty());
}

// is_oversized
// is_first_non_coinbase
// is_extra_coinbases

BOOST_AUTO_TEST_CASE(block__is_forward_reference__empty__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_forward_reference__multiple_empty_transactions__false)
{
    const accessor instance
    {
        {},
        {
            { 0, inputs{}, {}, 0 },
            { 0, inputs{}, {}, 0 }
        }
    };

    BOOST_REQUIRE(!instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_forward_reference__duplicate_transactions__false)
{
    const accessor instance
    {
        {},
        {
            { 0, inputs{}, {}, 0 },
            { 0, inputs{}, {}, 0 }
        }
    };

    BOOST_REQUIRE(!instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_forward_reference__backward_reference__false)
{
    const transaction to{ 0, inputs{}, {}, 0 };
    const transaction from{ 0, { { { to.hash(false), 0 }, {}, 0 } }, {}, 0 };
    const accessor instance
    {
        {},
        {
            to,
            from
        }
    };

    BOOST_REQUIRE(!instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_forward_reference__forward_reference__true)
{
    const transaction cb{ 0, inputs{}, {}, 0 };
    const transaction to{ 0, inputs{}, {}, 42 };
    const transaction from{ 0, { { { to.hash(false), 0 }, {}, 0 } }, {}, 0 };
    const accessor instance
    {
        {},
        {
            cb,
            from,
            to
        }
    };

    BOOST_REQUIRE(instance.is_forward_reference());
}

BOOST_AUTO_TEST_CASE(block__is_internal_double_spend__empty__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(block__is_internal_double_spend__distinct_points__false)
{
    const accessor instance
    {
        {},
        {
            {},
            { 0, { { { hash1, 42 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash1, 27 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash2, 42 }, {}, 0 } }, {}, 0 }
        }
    };

    BOOST_REQUIRE(!instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(block__is_internal_double_spend__nondistinct_points__true)
{
    const accessor instance
    {
        {},
        {
            {},
            { 0, { { { hash1, 42 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash2, 27 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash3, 36 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash1, 42 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash2, 27 }, {}, 0 } }, {}, 0 },
            { 0, { { { hash3, 36 }, {}, 0 } }, {}, 0 }
        }
    };

    BOOST_REQUIRE(instance.is_internal_double_spend());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__default__false)
{
    const accessor instance;
    BOOST_REQUIRE(!instance.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__default_header_non_empty__true)
{
    const accessor instance
    {
        header{},
        { {} }
    };

    BOOST_REQUIRE(instance.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__mainnet_genesis__false)
{
    const auto data = settings(selection::mainnet).genesis_block.to_data(true);
    const accessor genesis(data, true);
    BOOST_REQUIRE(genesis.is_valid());
    BOOST_REQUIRE(!genesis.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__testnet_genesis__false)
{
    const auto data = settings(selection::testnet3).genesis_block.to_data(true);
    const accessor genesis(data, true);
    BOOST_REQUIRE(genesis.is_valid());
    BOOST_REQUIRE(!genesis.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__regtest_genesis__false)
{
    const auto data = settings(selection::regtest).genesis_block.to_data(true);
    const accessor genesis(data, true);
    BOOST_REQUIRE(genesis.is_valid());
    BOOST_REQUIRE(!genesis.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__valid__false)
{
    constexpr auto data = base16_array(
        "010000007f110631052deeee06f0754a3629ad7663e56359fd5f3aa7b3e30a0000000"
        "0005f55996827d9712147a8eb6d7bae44175fe0bcfa967e424a25bfe9f4dc118244d6"
        "7fb74c9d8e2f1bea5ee82a03010000000100000000000000000000000000000000000"
        "00000000000000000000000000000ffffffff07049d8e2f1b0114ffffffff0100f205"
        "2a0100000043410437b36a7221bc977dce712728a954e3b5d88643ed5aef46660ddcf"
        "eeec132724cd950c1fdd008ad4a2dfd354d6af0ff155fc17c1ee9ef802062feb07ef1"
        "d065f0ac000000000100000001260fd102fab456d6b169f6af4595965c03c2296ecf2"
        "5bfd8790e7aa29b404eff010000008c493046022100c56ad717e07229eb93ecef2a32"
        "a42ad041832ffe66bd2e1485dc6758073e40af022100e4ba0559a4cebbc7ccb5d14d1"
        "312634664bac46f36ddd35761edaae20cefb16f01410417e418ba79380f462a60d8dd"
        "12dcef8ebfd7ab1741c5c907525a69a8743465f063c1d9182eea27746aeb9f1f52583"
        "040b1bc341b31ca0388139f2f323fd59f8effffffff0200ffb2081d0000001976a914"
        "fc7b44566256621affb1541cc9d59f08336d276b88ac80f0fa02000000001976a9146"
        "17f0609c9fabb545105f7898f36b84ec583350d88ac00000000010000000122cd6da2"
        "6eef232381b1a670aa08f4513e9f91a9fd129d912081a3dd138cb013010000008c493"
        "0460221009339c11b83f234b6c03ebbc4729c2633cbc8cbd0d15774594bfedc45c4f9"
        "9e2f022100ae0135094a7d651801539df110a028d65459d24bc752d7512bc8a9f78b4"
        "ab368014104a2e06c38dc72c4414564f190478e3b0d01260f09b8520b196c2f6ec3d0"
        "6239861e49507f09b7568189efe8d327c3384a4e488f8c534484835f8020b3669e5ae"
        "bffffffff0200ac23fc060000001976a914b9a2c9700ff9519516b21af338d28d53dd"
        "f5349388ac00743ba40b0000001976a914eb675c349c474bec8dea2d79d12cff6f330"
        "ab48788ac00000000");

    const accessor instance(data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(!instance.is_invalid_merkle_root());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_merkle_root__block100k__false)
{
    constexpr auto data = base16_array(
        "010000007f110631052deeee06f0754a3629ad7663e56359fd5f3aa7b3e30a00"
        "000000005f55996827d9712147a8eb6d7bae44175fe0bcfa967e424a25bfe9f4"
        "dc118244d67fb74c9d8e2f1bea5ee82a03010000000100000000000000000000"
        "00000000000000000000000000000000000000000000ffffffff07049d8e2f1b"
        "0114ffffffff0100f2052a0100000043410437b36a7221bc977dce712728a954"
        "e3b5d88643ed5aef46660ddcfeeec132724cd950c1fdd008ad4a2dfd354d6af0"
        "ff155fc17c1ee9ef802062feb07ef1d065f0ac000000000100000001260fd102"
        "fab456d6b169f6af4595965c03c2296ecf25bfd8790e7aa29b404eff01000000"
        "8c493046022100c56ad717e07229eb93ecef2a32a42ad041832ffe66bd2e1485"
        "dc6758073e40af022100e4ba0559a4cebbc7ccb5d14d1312634664bac46f36dd"
        "d35761edaae20cefb16f01410417e418ba79380f462a60d8dd12dcef8ebfd7ab"
        "1741c5c907525a69a8743465f063c1d9182eea27746aeb9f1f52583040b1bc34"
        "1b31ca0388139f2f323fd59f8effffffff0200ffb2081d0000001976a914fc7b"
        "44566256621affb1541cc9d59f08336d276b88ac80f0fa02000000001976a914"
        "617f0609c9fabb545105f7898f36b84ec583350d88ac00000000010000000122"
        "cd6da26eef232381b1a670aa08f4513e9f91a9fd129d912081a3dd138cb01301"
        "0000008c4930460221009339c11b83f234b6c03ebbc4729c2633cbc8cbd0d157"
        "74594bfedc45c4f99e2f022100ae0135094a7d651801539df110a028d65459d2"
        "4bc752d7512bc8a9f78b4ab368014104a2e06c38dc72c4414564f190478e3b0d"
        "01260f09b8520b196c2f6ec3d06239861e49507f09b7568189efe8d327c3384a"
        "4e488f8c534484835f8020b3669e5aebffffffff0200ac23fc060000001976a9"
        "14b9a2c9700ff9519516b21af338d28d53ddf5349388ac00743ba40b00000019"
        "76a914eb675c349c474bec8dea2d79d12cff6f330ab48788ac00000000");

    const accessor instance(data, true);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(!instance.is_invalid_merkle_root());
}

// merkle_branch

BOOST_AUTO_TEST_CASE(block__merkle_branch__single_tx__empty)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    const auto branch = genesis.merkle_branch(0, false);
    BOOST_REQUIRE(branch.empty());
}

BOOST_AUTO_TEST_CASE(block__merkle_branch__position_out_of_range__empty)
{
    const auto genesis = settings(selection::mainnet).genesis_block;
    BOOST_REQUIRE(genesis.merkle_branch(1, false).empty());
    BOOST_REQUIRE(genesis.merkle_branch(999, false).empty());
}

BOOST_AUTO_TEST_CASE(block__merkle_branch__test_block_3_transactions__expected)
{
    const auto& block = expected_block::get();
    const auto branch0 = block.merkle_branch(0, false);
    const auto branch1 = block.merkle_branch(1, false);
    const auto branch2 = block.merkle_branch(2, false);

    const auto expected_size = ceilinged_log2(block.transactions());
    BOOST_CHECK_EQUAL(branch0.size(), expected_size);
    BOOST_CHECK_EQUAL(branch1.size(), expected_size);
    BOOST_CHECK_EQUAL(branch2.size(), expected_size);
}

BOOST_AUTO_TEST_CASE(block__merkle_branch__test_block_3_transactions__correct_siblings)
{
    const auto& block = expected_block::get();
    const auto& txs = expected_transactions::get();
    const auto expected_size = ceilinged_log2(block.transactions());
    const auto tx0_hash = txs[0].hash(false);
    const auto tx1_hash = txs[1].hash(false);
    const auto tx2_hash = txs[2].hash(false);

    const auto branch0 = block.merkle_branch(0, false);
    BOOST_REQUIRE_EQUAL(branch0.size(), expected_size);
    BOOST_CHECK_EQUAL(branch0[0], tx1_hash);
    BOOST_CHECK_EQUAL(branch0[1], bitcoin_hash(tx2_hash, tx2_hash));

    const auto branch1 = block.merkle_branch(1, false);
    BOOST_REQUIRE_EQUAL(branch1.size(), expected_size);
    BOOST_CHECK_EQUAL(branch1[0], tx0_hash);
    BOOST_CHECK_EQUAL(branch1[1], bitcoin_hash(tx2_hash, tx2_hash));

    const auto branch2 = block.merkle_branch(2, false);
    BOOST_REQUIRE_EQUAL(branch2.size(), expected_size);
    BOOST_CHECK_EQUAL(branch2[0], tx2_hash);
    BOOST_CHECK_EQUAL(branch2[1], bitcoin_hash(tx0_hash, tx1_hash));
}

// These aren't actually witness txs, so result is the same as non-witness.
BOOST_AUTO_TEST_CASE(block__merkle_branch__test_block_3_transactions_witness__expected)
{
    const auto& block = expected_block::get();
    const auto non_witness = block.merkle_branch(1, false);
    const auto with_witness = block.merkle_branch(1, true);
    BOOST_CHECK_EQUAL(non_witness, with_witness);
}

hash_digest verify_merkle_proof(hash_digest current, const hashes& branch,
    size_t position) NOEXCEPT
{
    for (const auto& sibling: branch)
    {
        if (is_even(position))
            current = bitcoin_hash(current, sibling);
        else
            current = bitcoin_hash(sibling, current);

        position = to_half(position);
    }

    return current;
}

BOOST_AUTO_TEST_CASE(block__merkle_branch__round_trip__expected)
{
    const auto& block = expected_block::get();
    const auto& txs = expected_transactions::get();

    const auto tx0_hash = txs[0].hash(false);
    const auto tx1_hash = txs[1].hash(false);
    const auto tx2_hash = txs[2].hash(false);


    // Test transaction at position 0
    {
        const auto branch = block.merkle_branch(0, false);
        BOOST_REQUIRE_EQUAL(branch.size(), 2u);
        BOOST_CHECK_EQUAL(branch[0], tx1_hash);
        BOOST_CHECK_EQUAL(branch[1], bitcoin_hash(tx2_hash, tx2_hash));
    }

    // Test transaction at position 1
    {
        const auto branch = block.merkle_branch(1, false);
        BOOST_REQUIRE_EQUAL(branch.size(), 2u);
        BOOST_CHECK_EQUAL(branch[0], tx0_hash);
        BOOST_CHECK_EQUAL(branch[1], bitcoin_hash(tx2_hash, tx2_hash));
    }

    // Test transaction at position 2 (duplicated leaf case)
    {
        const auto branch = block.merkle_branch(2, false);
        BOOST_REQUIRE_EQUAL(branch.size(), 2u);
        BOOST_CHECK_EQUAL(branch[0], tx2_hash);
        BOOST_CHECK_EQUAL(branch[1], bitcoin_hash(tx0_hash, tx1_hash));
    }
}

// validation predicates
// ----------------------------------------------------------------------------

static transaction coinbase_transaction(uint64_t value, const script& coinbase_script)
{
    const inputs ins{ input{ point{}, coinbase_script, 0xffffffff } };
    const outputs outs{ output{ value, script{} } };
    return transaction{ 1, ins, outs, 0 };
}

static transaction spending_transaction()
{
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, 0xffffffff } };
    const outputs outs{ output{ 0, script{} } };
    return transaction{ 1, ins, outs, 0 };
}

BOOST_AUTO_TEST_CASE(block__is_first_non_coinbase__coinbase_first__false)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}) } };
    BOOST_REQUIRE(!instance.is_first_non_coinbase());
}

BOOST_AUTO_TEST_CASE(block__is_first_non_coinbase__spend_first__true)
{
    const accessor instance{ header{}, transactions{ spending_transaction() } };
    BOOST_REQUIRE(instance.is_first_non_coinbase());
}

BOOST_AUTO_TEST_CASE(block__is_extra_coinbases__two_coinbases__true)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}), coinbase_transaction(1, script{}) } };
    BOOST_REQUIRE(instance.is_extra_coinbases());
}

BOOST_AUTO_TEST_CASE(block__is_extra_coinbases__coinbase_and_spend__false)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}), spending_transaction() } };
    BOOST_REQUIRE(!instance.is_extra_coinbases());
}

// bip34 requires the coinbase script begin with a minimal height push.
BOOST_AUTO_TEST_CASE(block__is_invalid_coinbase_script__matching_height__false)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x64 }, true } } };
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    BOOST_REQUIRE(!instance.is_invalid_coinbase_script(100));
}

BOOST_AUTO_TEST_CASE(block__is_invalid_coinbase_script__mismatched_height__true)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x64 }, true } } };
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    BOOST_REQUIRE(instance.is_invalid_coinbase_script(101));
}

BOOST_AUTO_TEST_CASE(block__is_hash_limit_exceeded__small_block__false)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}), spending_transaction() } };
    BOOST_REQUIRE(!instance.is_hash_limit_exceeded());
}

BOOST_AUTO_TEST_CASE(block__is_oversized__megabyte_script__true)
{
    const script big{ operations{ operation{ data_chunk(1'000'000, 0x00), false } } };
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, big) } };
    BOOST_REQUIRE(instance.is_oversized());
    BOOST_REQUIRE(instance.is_overweight());
}

BOOST_AUTO_TEST_CASE(block__is_oversized__small_block__false)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}) } };
    BOOST_REQUIRE(!instance.is_oversized());
    BOOST_REQUIRE(!instance.is_overweight());
}

// Witness data with no coinbase commitment output invalidates the block.
BOOST_AUTO_TEST_CASE(block__is_invalid_witness_commitment__segregated_no_commitment__true)
{
    const witness spender{ chunk_cptrs{ to_shared(base16_chunk("01")) } };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, spender, 0xffffffff } };
    const transaction spend{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}), spend } };
    BOOST_REQUIRE(instance.is_invalid_witness_commitment());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_witness_commitment__unsegregated_no_commitment__false)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}), spending_transaction() } };
    BOOST_REQUIRE(!instance.is_invalid_witness_commitment());
}

// Witness commitment [bip141].
// ----------------------------------------------------------------------------

static script commitment_script(const hash_digest& commitment) NOEXCEPT
{
    constexpr auto head = to_big_endian(witness_head);
    data_chunk data(head.size() + hash_size);
    std::copy(head.begin(), head.end(), data.begin());
    std::copy(commitment.begin(), commitment.end(),
        std::next(data.begin(), head.size()));

    return script{ operations{ operation{ opcode::op_return },
        operation{ data, false } } };
}

static witness reservation_witness(const hash_digest& reservation) NOEXCEPT
{
    const chunk_cptrs stack{ to_shared<data_chunk>(to_chunk(reservation)) };
    return witness{ stack };
}

static transaction commitment_coinbase(const hash_digest& commitment,
    const witness& spender) NOEXCEPT
{
    const inputs ins{ input{ point{}, script{}, spender, 0xffffffff } };
    const outputs outs{ output{ 0, commitment_script(commitment) } };
    return transaction{ 1, ins, outs, 0 };
}

static transaction two_commitment_coinbase(const hash_digest& first,
    const hash_digest& second, const witness& spender) NOEXCEPT
{
    const inputs ins{ input{ point{}, script{}, spender, 0xffffffff } };
    const outputs outs
    {
        output{ 0, commitment_script(first) },
        output{ 0, commitment_script(second) }
    };

    return transaction{ 1, ins, outs, 0 };
}

static hash_digest expected_commitment(const hash_digest& reservation) NOEXCEPT
{
    return sha256::double_hash(null_hash, reservation);
}

BOOST_AUTO_TEST_CASE(block__is_invalid_witness_commitment__commitment_without_reservation__true)
{
    const accessor instance{ header{}, transactions{ commitment_coinbase(one_hash, witness{}) } };
    BOOST_REQUIRE(instance.is_invalid_witness_commitment());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_witness_commitment__wrong_commitment__true)
{
    const accessor instance{ header{}, transactions{ commitment_coinbase(one_hash, reservation_witness(null_hash)) } };
    BOOST_REQUIRE(instance.is_invalid_witness_commitment());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_witness_commitment__valid_commitment__false)
{
    const auto commitment = expected_commitment(one_hash);
    const accessor instance{ header{}, transactions{ commitment_coinbase(commitment, reservation_witness(one_hash)) } };
    BOOST_REQUIRE(!instance.is_invalid_witness_commitment());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_witness_commitment__last_commitment_valid__false)
{
    const auto commitment = expected_commitment(one_hash);
    const accessor instance{ header{}, transactions{ two_commitment_coinbase(null_hash, commitment, reservation_witness(one_hash)) } };
    BOOST_REQUIRE(!instance.is_invalid_witness_commitment());
}

BOOST_AUTO_TEST_CASE(block__is_invalid_witness_commitment__last_commitment_invalid__true)
{
    const auto commitment = expected_commitment(one_hash);
    const accessor instance{ header{}, transactions{ two_commitment_coinbase(commitment, null_hash, reservation_witness(one_hash)) } };
    BOOST_REQUIRE(instance.is_invalid_witness_commitment());
}

// segregated

BOOST_AUTO_TEST_CASE(block__segregated__no_witness__zero)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}), spending_transaction() } };
    BOOST_REQUIRE(!instance.is_segregated());
    BOOST_REQUIRE_EQUAL(instance.segregated(), zero);
}

BOOST_AUTO_TEST_CASE(block__segregated__witness__counted)
{
    const witness spender{ chunk_cptrs{ to_shared(base16_chunk("01")) } };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, spender, 0xffffffff } };
    const transaction spend{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, script{}), spend, spend } };
    BOOST_REQUIRE(instance.is_segregated());
    BOOST_REQUIRE_EQUAL(instance.segregated(), two);
}

// subsidy
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__subsidy__genesis__initial)
{
    BOOST_REQUIRE_EQUAL(block::subsidy(0, 210000, 5000000000, false), 5000000000u);
}

BOOST_AUTO_TEST_CASE(block__subsidy__first_halving__half)
{
    BOOST_REQUIRE_EQUAL(block::subsidy(210000, 210000, 5000000000, false), 2500000000u);
}

BOOST_AUTO_TEST_CASE(block__subsidy__second_halving__quarter)
{
    BOOST_REQUIRE_EQUAL(block::subsidy(420000, 210000, 5000000000, false), 1250000000u);
}

BOOST_AUTO_TEST_CASE(block__subsidy__below_first_halving__initial)
{
    BOOST_REQUIRE_EQUAL(block::subsidy(209999, 210000, 5000000000, false), 5000000000u);
}

// Pre-bip42 a sixty four halving shift is modulo, bip42 shifts to zero.
BOOST_AUTO_TEST_CASE(block__subsidy__sixty_four_halvings_pre_bip42__initial)
{
    BOOST_REQUIRE_EQUAL(block::subsidy(64u * 210000, 210000, 5000000000, false), 5000000000u);
}

BOOST_AUTO_TEST_CASE(block__subsidy__sixty_four_halvings_bip42__zero)
{
    BOOST_REQUIRE_EQUAL(block::subsidy(64u * 210000, 210000, 5000000000, true), 0u);
}

// is_overspent
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__is_overspent__claim_at_subsidy__false)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(5000000000, script{}) } };
    BOOST_REQUIRE(!instance.is_overspent(0, 210000, 5000000000, false));
}

BOOST_AUTO_TEST_CASE(block__is_overspent__claim_above_subsidy__true)
{
    const accessor instance{ header{}, transactions{ coinbase_transaction(5000000001, script{}) } };
    BOOST_REQUIRE(instance.is_overspent(0, 210000, 5000000000, false));
}

// signature_operations
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__signature_operations__coinbase_checksigs__expected)
{
    const script coinbase_script(base16_chunk("02acad"), true);
    const accessor instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    BOOST_REQUIRE_EQUAL(instance.signature_operations(false, false), 2u);
    BOOST_REQUIRE_EQUAL(instance.signature_operations(false, true), 8u);
    BOOST_REQUIRE(!instance.is_signature_operations_limited(false, false));
}

// is_unspent_coinbase_collision

// check
// ----------------------------------------------------------------------------

// Sixty four byte non-coinbase transaction, the malleable64 unit.
static transaction triad_tx64() NOEXCEPT
{
    const script dups{ operations{ operation{ opcode::dup }, operation{ opcode::dup } } };
    const inputs ins{ input{ point{ one_hash, 0 }, dups, 42 } };
    const outputs outs{ output{ 42, dups } };
    return transaction{ 42, ins, outs, 42 };
}

BOOST_AUTO_TEST_CASE(block__check__no_transactions__empty_block)
{
    const block instance{ header{}, transactions{} };
    BOOST_REQUIRE_EQUAL(instance.check(), error::empty_block);
}

BOOST_AUTO_TEST_CASE(block__check__first_not_coinbase__first_not_coinbase)
{
    const block instance{ header{}, transactions{ spending_transaction() } };
    BOOST_REQUIRE_EQUAL(instance.check(false), error::first_not_coinbase);
}

// A malleated block reports the commitment failure, not the structural one.
BOOST_AUTO_TEST_CASE(block__check__malleated64_first_not_coinbase__invalid_transaction_commitment)
{
    const accessor instance{ header{}, transactions{ triad_tx64(), triad_tx64() } };
    BOOST_REQUIRE_EQUAL(triad_tx64().serialized_size(false), 64u);
    BOOST_REQUIRE(instance.is_first_non_coinbase());
    BOOST_REQUIRE_EQUAL(instance.check(false), error::invalid_transaction_commitment);
}

// Duplicate points within one transaction are caught at block scope.
BOOST_AUTO_TEST_CASE(block__check__duplicate_points_within_transaction__block_internal_double_spend)
{
    const inputs ins
    {
        input{ point{ one_hash, 0 }, script{}, 0xffffffff },
        input{ point{ one_hash, 0 }, script{}, 0xffffffff }
    };

    const transaction duplicated{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script), duplicated } };
    BOOST_REQUIRE_EQUAL(instance.check(false), error::block_internal_double_spend);
}

// Duplicate points across transactions are caught at block scope.
BOOST_AUTO_TEST_CASE(block__check__duplicate_points_across_transactions__block_internal_double_spend)
{
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, 0xffffffff } };
    const transaction spend1{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    const transaction spend2{ 2, ins, outputs{ output{ 0, script{} } }, 0 };
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const auto txs = transactions{ coinbase_transaction(0, coinbase_script), spend1, spend2 };
    const block instance{ header{}, txs };
    BOOST_REQUIRE_EQUAL(instance.check(false), error::block_internal_double_spend);
}

BOOST_AUTO_TEST_CASE(block__check__extra_coinbases__extra_coinbases)
{
    const auto txs = transactions{ coinbase_transaction(0, script{}), coinbase_transaction(1, script{}) };
    const block instance{ header{}, txs };
    BOOST_REQUIRE_EQUAL(instance.check(false), error::extra_coinbases);
}

BOOST_AUTO_TEST_CASE(block__check__identity_false__merkle_root_unchecked)
{
    const block instance{ header{}, transactions{ coinbase_transaction(0, script{}) } };
    BOOST_REQUIRE(instance.check(false) != error::invalid_transaction_commitment);
}

BOOST_AUTO_TEST_CASE(block__check__identity_true__invalid_transaction_commitment)
{
    const block instance{ header{}, transactions{ coinbase_transaction(0, script{}) } };
    BOOST_REQUIRE_EQUAL(instance.check(true), error::invalid_transaction_commitment);
}

// The context free check delegates to the transaction check.
BOOST_AUTO_TEST_CASE(block__check__invalid_coinbase_script_size__invalid_coinbase_script_size)
{
    const block instance{ header{}, transactions{ coinbase_transaction(0, script{}) } };
    BOOST_REQUIRE_EQUAL(instance.check(false), error::invalid_coinbase_script_size);
}

// check(context)
// ----------------------------------------------------------------------------

static block triad_block() NOEXCEPT
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    return block{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
}

BOOST_AUTO_TEST_CASE(block__check_context__minimal_block__block_success)
{
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(triad_block().check(ctx, false), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__check_context__coinbase_script_bip34_off__block_success)
{
    const context ctx{ flags::no_rules, 0, 0, 100, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(triad_block().check(ctx, false), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__check_context__coinbase_script_bip34_on__coinbase_height_mismatch)
{
    const context ctx{ flags::bip34_rule, 0, 0, 100, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(triad_block().check(ctx, false), error::coinbase_height_mismatch);
}

BOOST_AUTO_TEST_CASE(block__check_context__matching_coinbase_script_bip34_on__block_success)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x64 }, true } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    const context ctx{ flags::bip34_rule, 0, 0, 100, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::block_success);
}

// Heights above sixteen serialize as a nominal data push in both encodings.
BOOST_AUTO_TEST_CASE(block__check_context__coinbase_script_height_seventeen__block_success)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x11 }, true } } };
    BOOST_REQUIRE_EQUAL(coinbase_script.to_data(false), base16_chunk("0111"));

    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    const context ctx{ flags::bip34_rule, 0, 0, 17, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::block_success);
}

// Heights one through sixteen serialize as a single byte small integer.
BOOST_AUTO_TEST_CASE(block__check_context__coinbase_script_small_integer_height__block_success)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01 }, true } } };
    BOOST_REQUIRE_EQUAL(coinbase_script.to_data(false), base16_chunk("51"));

    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    const context ctx{ flags::bip34_rule, 0, 0, 1, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::block_success);
}

// The sized push of a small integer height is not the minimal encoding.
BOOST_AUTO_TEST_CASE(block__check_context__coinbase_script_pushed_height_one__coinbase_height_mismatch)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01 }, false } } };
    BOOST_REQUIRE_EQUAL(coinbase_script.to_data(false), base16_chunk("0101"));

    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    const context ctx{ flags::bip34_rule, 0, 0, 1, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::coinbase_height_mismatch);
}

BOOST_AUTO_TEST_CASE(block__check_context__coinbase_script_small_integer_height_sixteen__block_success)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x10 }, true } } };
    BOOST_REQUIRE_EQUAL(coinbase_script.to_data(false), base16_chunk("60"));

    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    const context ctx{ flags::bip34_rule, 0, 0, 16, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__check_context__overweight_bip141_off__block_success)
{
    const script big{ operations{ operation{ data_chunk(1'000'000, 0x00), false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, big) } };
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__check_context__overweight_bip141_on__block_weight_limit)
{
    const script big{ operations{ operation{ data_chunk(1'000'000, 0x00), false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, big) } };
    const context ctx{ flags::bip141_rule, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::block_weight_limit);
}

BOOST_AUTO_TEST_CASE(block__check_context__witness_commitment_identity_false__block_success)
{
    const witness spender{ chunk_cptrs{ to_shared(base16_chunk("01")) } };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, spender, 0xffffffff } };
    const transaction spend{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script), spend } };
    const context ctx{ flags::bip141_rule, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__check_context__witness_commitment_identity_true__invalid_witness_commitment)
{
    const witness spender{ chunk_cptrs{ to_shared(base16_chunk("01")) } };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, spender, 0xffffffff } };
    const transaction spend{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script), spend } };
    const context ctx{ flags::bip141_rule, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, true), error::invalid_witness_commitment);
}

// The contextual check delegates to the transaction contextual check.
BOOST_AUTO_TEST_CASE(block__check_context__locked_transaction__absolute_time_locked)
{
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, sub1(0xffffffff) } };
    const transaction locked{ 1, ins, outputs{ output{ 0, script{} } }, 200 };
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script), locked } };
    const context ctx{ flags::no_rules, 0, 0, 100, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.check(ctx, false), error::absolute_time_locked);
}

// identify
// ----------------------------------------------------------------------------

// A block carrying witness data with no commitment, merkle root valid.
static transactions witnessed_transactions() NOEXCEPT
{
    const witness spender{ chunk_cptrs{ to_shared(base16_chunk("01")) } };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, spender, 0xffffffff } };
    const transaction spend{ 1, ins, outputs{ output{ 0, script{} } }, 0 };
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    return transactions{ coinbase_transaction(0, coinbase_script), spend };
}

static block witnessed_block() NOEXCEPT
{
    const auto txs = witnessed_transactions();
    const auto root = bitcoin_hash(txs.front().hash(false), txs.back().hash(false));
    const header head{ 1, hash_digest{}, root, 0, 0, 0 };
    return block{ head, txs };
}

BOOST_AUTO_TEST_CASE(block__identify__witnessed_valid_merkle_root__block_success)
{
    const auto instance = witnessed_block();
    BOOST_REQUIRE(instance.is_segregated());
    BOOST_REQUIRE_EQUAL(instance.identify(), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__identify__witnessed_bip141_off__invalid_witness_commitment)
{
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(witnessed_block().identify(ctx), error::invalid_witness_commitment);
}

BOOST_AUTO_TEST_CASE(block__identify__witnessed_bip141_on__invalid_witness_commitment)
{
    const context ctx{ flags::bip141_rule, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(witnessed_block().identify(ctx), error::invalid_witness_commitment);
}

BOOST_AUTO_TEST_CASE(block__identify__unwitnessed_bip141_off__block_success)
{
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, coinbase_script) } };
    BOOST_REQUIRE(!instance.is_segregated());
    BOOST_REQUIRE_EQUAL(instance.identify(ctx), error::block_success);
}

// A commitment output without witness data is not a malleation before bip141.
BOOST_AUTO_TEST_CASE(block__identify__commitment_without_witness_bip141_off__block_success)
{
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    const script commitment(base16_chunk("6a24aa21a9ed0000000000000000000000000000000000000000000000000000000000000000"), false);
    const inputs ins{ input{ point{}, script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } }, 0xffffffff } };
    const outputs outs{ output{ 0, commitment } };
    const transaction coinbase{ 1, ins, outs, 0 };
    const block instance{ header{}, transactions{ coinbase } };
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(!instance.is_segregated());
    BOOST_REQUIRE_EQUAL(instance.identify(ctx), error::block_success);
}

// accept
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__accept__claim_at_subsidy__block_success)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(5000000000, coinbase_script) } };
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.accept(ctx, 210000, 5000000000), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__accept__claim_above_subsidy__coinbase_value_limit)
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(5000000001, coinbase_script) } };
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(instance.accept(ctx, 210000, 5000000000), error::coinbase_value_limit);
}

// populate
// ----------------------------------------------------------------------------

static transaction populate_coinbase() NOEXCEPT
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const inputs ins{ input{ point{}, coinbase_script, 0xffffffff } };
    return transaction{ 1, ins, outputs{ output{ 42, script{} } }, 0 };
}

BOOST_AUTO_TEST_CASE(block__populate__no_transactions__block_success)
{
    const context ctx{ flags::no_rules, 0, 0, 100, 0, 0, 0 };
    const block instance{ header{}, transactions{} };
    BOOST_REQUIRE_EQUAL(instance.populate(ctx), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__populate__internal_spend__prevout_populated)
{
    const context ctx{ flags::no_rules, 0, 0, 100, 0, 0, 0 };
    const inputs ins1{ input{ point{ one_hash, 0 }, script{}, max_input_sequence } };
    const transaction tx1{ 1, ins1, outputs{ output{ 42, script{} } }, 0 };
    const inputs ins2{ input{ point{ tx1.hash(false), 0 }, script{}, max_input_sequence } };
    const transaction tx2{ 1, ins2, outputs{ output{ 40, script{} } }, 0 };

    const block instance{ header{}, transactions{ populate_coinbase(), tx1, tx2 } };
    BOOST_REQUIRE_EQUAL(instance.populate(ctx), error::block_success);

    const auto& spender = instance.transactions_ptr()->back();
    BOOST_REQUIRE(spender->inputs_ptr()->front()->prevout);
    BOOST_REQUIRE_EQUAL(spender->inputs_ptr()->front()->prevout->value(), 42u);
}

BOOST_AUTO_TEST_CASE(block__populate__external_spend__prevout_unpopulated)
{
    const context ctx{ flags::no_rules, 0, 0, 100, 0, 0, 0 };
    const inputs ins{ input{ point{ one_hash, 0 }, script{}, max_input_sequence } };
    const transaction spend{ 1, ins, outputs{ output{ 40, script{} } }, 0 };

    const block instance{ header{}, transactions{ populate_coinbase(), spend } };
    BOOST_REQUIRE_EQUAL(instance.populate(ctx), error::block_success);
    BOOST_REQUIRE(!instance.transactions_ptr()->back()->inputs_ptr()->front()->prevout);
}

// The coinbase output of a block cannot be spent within that block.
BOOST_AUTO_TEST_CASE(block__populate__spend_of_block_coinbase__coinbase_maturity)
{
    const context ctx{ flags::no_rules, 0, 0, 100, 0, 0, 0 };
    const auto coinbase = populate_coinbase();
    const inputs ins{ input{ point{ coinbase.hash(false), 0 }, script{}, max_input_sequence } };
    const transaction spend{ 1, ins, outputs{ output{ 40, script{} } }, 0 };

    const block instance{ header{}, transactions{ coinbase, spend } };
    BOOST_REQUIRE_EQUAL(instance.populate(ctx), error::coinbase_maturity);
}

BOOST_AUTO_TEST_CASE(block__populate__internally_locked_bip68_on__relative_time_locked)
{
    const context ctx{ flags::bip68_rule, 0, 0, 100, 0, 0, 0 };
    const inputs ins1{ input{ point{ one_hash, 0 }, script{}, max_input_sequence } };
    const transaction tx1{ 1, ins1, outputs{ output{ 42, script{} } }, 0 };
    const inputs ins2{ input{ point{ tx1.hash(false), 0 }, script{}, 1 } };
    const transaction tx2{ 2, ins2, outputs{ output{ 40, script{} } }, 0 };

    const block instance{ header{}, transactions{ populate_coinbase(), tx1, tx2 } };
    BOOST_REQUIRE_EQUAL(instance.populate(ctx), error::relative_time_locked);
}

BOOST_AUTO_TEST_CASE(block__populate__internally_locked_bip68_off__block_success)
{
    const context ctx{ flags::no_rules, 0, 0, 100, 0, 0, 0 };
    const inputs ins1{ input{ point{ one_hash, 0 }, script{}, max_input_sequence } };
    const transaction tx1{ 1, ins1, outputs{ output{ 42, script{} } }, 0 };
    const inputs ins2{ input{ point{ tx1.hash(false), 0 }, script{}, 1 } };
    const transaction tx2{ 2, ins2, outputs{ output{ 40, script{} } }, 0 };

    const block instance{ header{}, transactions{ populate_coinbase(), tx1, tx2 } };
    BOOST_REQUIRE_EQUAL(instance.populate(ctx), error::block_success);
}

// confirm
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__confirm__coinbase_only__block_success)
{
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(triad_block().confirm(ctx), error::block_success);
}

// header_ptr/transaction_hashes
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__header_ptr__genesis__matches_header)
{
    const auto& instance = test::genesis;
    BOOST_REQUIRE(instance.header_ptr());
    BOOST_REQUIRE(*instance.header_ptr() == instance.header());
}

BOOST_AUTO_TEST_CASE(block__transaction_hashes__genesis__coinbase_hash)
{
    const auto& instance = test::genesis;
    const auto& txs = *instance.transactions_ptr();
    const auto out = instance.transaction_hashes(false);
    BOOST_REQUIRE_EQUAL(out.size(), 1u);
    BOOST_REQUIRE_EQUAL(out.front(), txs.front()->hash(false));
}

BOOST_AUTO_TEST_CASE(block__transaction_hashes__block2a__witness_distinguished)
{
    const auto& instance = test::block2a;
    const auto& txs = *instance.transactions_ptr();
    const auto nominal = instance.transaction_hashes(false);
    const auto witnessed = instance.transaction_hashes(true);
    BOOST_REQUIRE_EQUAL(nominal.size(), txs.size());
    BOOST_REQUIRE_EQUAL(witnessed.size(), txs.size());
    BOOST_REQUIRE_EQUAL(nominal.back(), txs.back()->hash(false));
    BOOST_REQUIRE_EQUAL(witnessed.back(), txs.back()->hash(true));
    BOOST_REQUIRE_NE(nominal.back(), witnessed.back());
}

// set_hashes
// ----------------------------------------------------------------------------
// Caches the header hash and each transaction hash from the serialization.

BOOST_AUTO_TEST_CASE(block__set_hashes__genesis__matches_computed)
{
    const auto& expected = test::genesis;
    const auto data = expected.to_data(true);
    block instance{ data, true };
    instance.set_hashes(data);
    BOOST_REQUIRE_EQUAL(instance.hash(), expected.hash());
    BOOST_REQUIRE_EQUAL(instance.transaction_hashes(false), expected.transaction_hashes(false));
}

BOOST_AUTO_TEST_CASE(block__set_hashes__block2a__matches_computed)
{
    const auto& expected = test::block2a;
    const auto data = expected.to_data(true);
    block instance{ data, true };
    instance.set_hashes(data);
    BOOST_REQUIRE_EQUAL(instance.hash(), expected.hash());
    BOOST_REQUIRE_EQUAL(instance.transaction_hashes(false), expected.transaction_hashes(false));
}

// set_state/get_state
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__get_state__default__nullptr)
{
    const block instance{ header{}, transactions{} };
    BOOST_REQUIRE(!instance.get_state());
}

BOOST_AUTO_TEST_CASE(block__set_state__assigned__same_state)
{
    const settings settings(selection::mainnet);
    const auto state = std::make_shared<const chain_state>(chain_state::data{}, settings);
    const block instance{ header{}, transactions{} };
    instance.set_state(state);
    BOOST_REQUIRE(instance.get_state() == state);
}

// check/accept code mappings
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(block__check__oversized__block_size_limit)
{
    const script big{ operations{ operation{ data_chunk(1'000'000, 0x00), false } } };
    const block instance{ header{}, transactions{ coinbase_transaction(0, big) } };
    BOOST_REQUIRE_EQUAL(instance.check(), error::block_size_limit);
}

BOOST_AUTO_TEST_CASE(block__check__forward_reference__forward_reference)
{
    const transaction to{ 0, inputs{}, {}, 42 };
    const transaction from{ 0, { { { to.hash(false), 0 }, {}, 0 } }, {}, 0 };
    const block instance{ header{}, transactions{ coinbase_transaction(0, script{}), from, to } };
    BOOST_REQUIRE_EQUAL(instance.check(), error::forward_reference);
}

static block sigops_block(size_t checksigs) NOEXCEPT
{
    const script coinbase_script{ operations{ operation{ data_chunk{ 0x01, 0x02 }, false } } };
    const inputs ins{ input{ point{}, coinbase_script, max_input_sequence } };
    const outputs outs{ output{ 5000000000, script{ operations(checksigs, operation{ opcode::checksig }) } } };
    return block{ header{}, transactions{ transaction{ 1, ins, outs, 0 } } };
}

BOOST_AUTO_TEST_CASE(block__accept__sigops_at_limit__block_success)
{
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(sigops_block(max_block_sigops).accept(ctx, 210000, 5000000000), error::block_success);
}

BOOST_AUTO_TEST_CASE(block__accept__sigops_above_limit__block_sigop_limit)
{
    const context ctx{ flags::no_rules, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(sigops_block(add1(max_block_sigops)).accept(ctx, 210000, 5000000000), error::block_sigop_limit);
}

// BIP141: legacy sigops are scaled by four against a four times larger limit.
BOOST_AUTO_TEST_CASE(block__accept__sigops_bip141__scaled_limit)
{
    const context ctx{ flags::bip141_rule, 0, 0, 0, 0, 0, 0 };
    BOOST_REQUIRE_EQUAL(sigops_block(max_block_sigops).accept(ctx, 210000, 5000000000), error::block_success);
    BOOST_REQUIRE_EQUAL(sigops_block(add1(max_block_sigops)).accept(ctx, 210000, 5000000000), error::block_sigop_limit);
}

BOOST_AUTO_TEST_SUITE_END()
