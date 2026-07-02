/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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
    using namespace system::chain;

    // One segregated (witness) transaction and one legacy (non-witness)
    // transaction in a single block, so the per-transaction witness strip is
    // exercised both ways within one to_data(false) call.
    const block mixed
    {
        header
        {
            0x31323334,         // version
            test::block1a.hash(),
            system::hash_digest{ 0x2b },// merkle_root
            0x41424344,         // timestamp
            0x51525354,         // bits
            0x61626364          // nonce
        },
        transactions
        {
            transaction         // segregated (has witness)
            {
                0xb2,           // version
                inputs
                {
                    input
                    {
                        point{ test::block1a.transactions_ptr()->front()->hash(false), 0x00 },
                        script{ { { opcode::checkmultisig }, { opcode::pick } } },
                        witness{ "[242424]" },
                        0xb2    // sequence
                    }
                },
                outputs{ output{ 0x81, script{ { { opcode::pick } } } } },
                0x81            // locktime
            },
            transaction         // legacy (no witness)
            {
                0xb3,           // version
                inputs
                {
                    input
                    {
                        point{ test::block1a.transactions_ptr()->front()->hash(false), 0x01 },
                        script{ { { opcode::checkmultisig }, { opcode::roll } } },
                        witness{},
                        0x83    // sequence
                    }
                },
                outputs{ output{ 0x81, script{ { { opcode::pick } } } } },
                0x81            // locktime
            }
        }
    };

    const chain::block_view view{ mixed.to_data(true), true };
    BOOST_REQUIRE(view.is_segregated());
    BOOST_REQUIRE_EQUAL(view.transactions(), 2u);
    BOOST_CHECK_EQUAL(view.to_data(true), mixed.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), mixed.to_data(false));
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

    ec = view.identify({ 0, 1, 0 });
    BOOST_CHECK_EQUAL(ec, error::block_success);
}

// TODO: add positive test for bip141.
// TODO: add full malleation coverage since it is partially independent of
// block implemention.

BOOST_AUTO_TEST_SUITE_END()
