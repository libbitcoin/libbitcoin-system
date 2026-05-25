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

BOOST_AUTO_TEST_SUITE(transaction_view_tests)

// construct and properties

BOOST_AUTO_TEST_CASE(transaction_view__construct__empty__invalid)
{
    auto block = to_chunk(test::header0_data);
    block.push_back(0x42);
    stream::in::fast istream{ block };
    read::bytes::fast reader{ istream };
    reader.skip_bytes(chain::header::serialized_size());
    BOOST_CHECK_EQUAL(reader.read_variable(), 0x42u);

    const chain::transaction_view view{ reader, block, true };
    BOOST_CHECK(!view.is_valid());
}

BOOST_AUTO_TEST_CASE(transaction_view__construct__genesis__valid)
{
    const auto block = test::genesis.to_data(true);
    const auto& tx = *test::genesis.transactions_ptr()->front();
    stream::in::fast istream{ block };
    read::bytes::fast reader{ istream };
    reader.skip_bytes(chain::header::serialized_size());
    BOOST_CHECK_EQUAL(reader.read_variable(), 1u);

    const chain::transaction_view view{ reader, block, true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(view.is_coinbase());
    BOOST_CHECK(!view.is_segregated());
    BOOST_CHECK_EQUAL(view.inputs(), 1u);
    BOOST_CHECK_EQUAL(view.outputs(), 1u);
    BOOST_CHECK_EQUAL(view.version(), 1u);
    BOOST_CHECK_EQUAL(view.locktime(), 0u);
    BOOST_CHECK_EQUAL(view.hash(true), tx.hash(true));
    BOOST_CHECK_EQUAL(view.hash(false), tx.hash(false));
    BOOST_CHECK_EQUAL(view.serialized_size(true), tx.serialized_size(true));
    BOOST_CHECK_EQUAL(view.serialized_size(false), tx.serialized_size(false));

    BOOST_CHECK_EQUAL(view.get_inputs_stream().rdstate(), 0);
    BOOST_CHECK_EQUAL(view.get_outputs_stream().rdstate(), 0);
    BOOST_CHECK_EQUAL(view.get_witnesses_stream().rdstate(), 0);

    // Coinbase but not witness.
    hash_cref commitment{ null_hash };
    hash_cref reservation{ null_hash };
    BOOST_CHECK(!view.get_witness_commitment(commitment));
    BOOST_CHECK(!view.get_witness_reservation(reservation));
}

BOOST_AUTO_TEST_CASE(transaction_view__construct__tx4_witness__valid)
{
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::transaction_view view{ reader, transaction, true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(!view.is_coinbase());
    BOOST_CHECK(view.is_segregated());
    BOOST_CHECK_EQUAL(view.inputs(), 2u);
    BOOST_CHECK_EQUAL(view.outputs(), 1u);
    BOOST_CHECK_EQUAL(view.version(), 0xa5u);
    BOOST_CHECK_EQUAL(view.locktime(), 0x85u);
    BOOST_CHECK_EQUAL(view.hash(true), tx.hash(true));
    BOOST_CHECK_EQUAL(view.hash(false), tx.hash(false));
    BOOST_CHECK_EQUAL(view.serialized_size(true), tx.serialized_size(true));
    BOOST_CHECK_EQUAL(view.serialized_size(false), tx.serialized_size(false));

    BOOST_CHECK_EQUAL(view.get_inputs_stream().rdstate(), 0);
    BOOST_CHECK_EQUAL(view.get_outputs_stream().rdstate(), 0);
    BOOST_CHECK_EQUAL(view.get_witnesses_stream().rdstate(), 0);

    // Witness but not coinbase.
    hash_cref commitment{ null_hash };
    hash_cref reservation{ null_hash };
    BOOST_CHECK(!view.get_witness_commitment(commitment));
    BOOST_CHECK(!view.get_witness_reservation(reservation));
}

BOOST_AUTO_TEST_CASE(transaction_view__construct__tx4_non_witness__valid)
{
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    // Parse for a non-witness node.
    const chain::transaction_view view{ reader, transaction, false };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(!view.is_coinbase());
    BOOST_CHECK(!view.is_segregated());
    BOOST_CHECK_EQUAL(view.inputs(), 2u);
    BOOST_CHECK_EQUAL(view.outputs(), 1u);
    BOOST_CHECK_EQUAL(view.version(), 0xa5u);
    BOOST_CHECK_EQUAL(view.locktime(), 0x85u);

    // hash expectation must be non-witness.
    BOOST_CHECK_EQUAL(view.hash(true), tx.hash(false));
    BOOST_CHECK_EQUAL(view.hash(false), tx.hash(false));
    BOOST_CHECK_EQUAL(view.serialized_size(true), tx.serialized_size(true));
    BOOST_CHECK_EQUAL(view.serialized_size(false), tx.serialized_size(false));

    BOOST_CHECK_EQUAL(view.get_inputs_stream().rdstate(), 0);
    BOOST_CHECK_EQUAL(view.get_outputs_stream().rdstate(), 0);
    BOOST_CHECK_EQUAL(view.get_witnesses_stream().rdstate(), 0);

    // Witness but not coinbase.
    hash_cref commitment{ null_hash };
    hash_cref reservation{ null_hash };
    BOOST_CHECK(!view.get_witness_commitment(commitment));
    BOOST_CHECK(!view.get_witness_reservation(reservation));
}

// get_inputs_stream and write_input_script

BOOST_AUTO_TEST_CASE(transaction_view__write_input_script__genesis__expected)
{
    const auto block = test::genesis.to_data(true);
    const auto& tx = *test::genesis.transactions_ptr()->front();
    const auto expected = tx.inputs_ptr()->front()->script().to_data(true);
    stream::in::fast istream{ block };
    read::bytes::fast reader{ istream };
    reader.skip_bytes(chain::header::serialized_size());
    reader.read_variable();

    const chain::transaction_view view{ reader, block, true };
    BOOST_CHECK(view.is_valid());

    data_chunk script(expected.size(), 0xff);
    stream::flip::fast ostream{ script };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_inputs_stream();
    read::bytes::fast source{ stream };

    chain::transaction_view::write_input_script(sink, source);
    BOOST_CHECK_EQUAL(script, expected);
}

BOOST_AUTO_TEST_CASE(transaction_view__write_input_script__tx4_witness__expected)
{
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    const auto expected = tx.inputs_ptr()->front()->script().to_data(true);
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::transaction_view view{ reader, transaction, true };
    BOOST_CHECK(view.is_valid());

    data_chunk script(expected.size(), 0xff);
    stream::flip::fast ostream{ script };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_inputs_stream();
    read::bytes::fast source{ stream };

    chain::transaction_view::write_input_script(sink, source);
    BOOST_CHECK_EQUAL(script, expected);
}

// get_witnesses_stream and write_witness

BOOST_AUTO_TEST_CASE(transaction_view__write_witness__genesis__expected)
{
    const auto block = test::genesis.to_data(true);
    const auto& tx = *test::genesis.transactions_ptr()->front();
    const auto expected = tx.inputs_ptr()->front()->witness().to_data(true);

    stream::in::fast istream{ block };
    read::bytes::fast reader{ istream };
    reader.skip_bytes(chain::header::serialized_size());
    reader.read_variable();

    const chain::transaction_view view{ reader, block, true };
    BOOST_CHECK(view.is_valid());

    data_chunk witness(expected.size(), 0xff);
    stream::flip::fast ostream{ witness };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_witnesses_stream();
    read::bytes::fast source{ stream };

    chain::transaction_view::write_witness(sink, source);
    BOOST_CHECK_EQUAL(witness, expected);
}

BOOST_AUTO_TEST_CASE(transaction_view__write_witness__tx4_witness__expected)
{
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    const auto expected = tx.inputs_ptr()->front()->witness().to_data(true);
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::transaction_view view{ reader, transaction, true };
    BOOST_CHECK(view.is_valid());

    data_chunk witness(expected.size(), 0xff);
    stream::flip::fast ostream{ witness };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_witnesses_stream();
    read::bytes::fast source{ stream };

    chain::transaction_view::write_witness(sink, source);
    BOOST_CHECK_EQUAL(witness, expected);
}

BOOST_AUTO_TEST_SUITE_END()
