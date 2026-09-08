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

    const chain::transaction_view view{ reader, block, false, true };
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

    const chain::transaction_view view{ reader, block, true, true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(view.is_coinbase());
    BOOST_CHECK(view.is_null_point());
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
    // Serialized to buffer WITH witness data.
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    // Parse buffer for a witness node (logically unstripped).
    constexpr auto witnessed = true;
    const chain::transaction_view view{ reader, transaction, false, witnessed };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(!view.is_coinbase());
    BOOST_CHECK(!view.is_null_point());
    BOOST_CHECK(view.is_segregated());
    BOOST_CHECK_EQUAL(view.inputs(), 2u);
    BOOST_CHECK_EQUAL(view.outputs(), 1u);
    BOOST_CHECK_EQUAL(view.version(), 0xa5u);
    BOOST_CHECK_EQUAL(view.locktime(), 0x85u);

    // hash and expectations must be witnessed.
    BOOST_CHECK_EQUAL(view.hash(true), tx.hash(witnessed));
    BOOST_CHECK_EQUAL(view.hash(false), tx.hash(false));
    BOOST_CHECK_EQUAL(view.serialized_size(true), tx.serialized_size(witnessed));
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

BOOST_AUTO_TEST_CASE(transaction_view__construct__tx4_stripped__valid)
{
    // Serialized to buffer WITH witness data.
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    // Parse buffer for a non-witness node (logically stripped).
    constexpr auto stripped = false;
    const chain::transaction_view view{ reader, transaction, false, stripped };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(!view.is_coinbase());
    BOOST_CHECK(!view.is_null_point());
    BOOST_CHECK(!view.is_segregated());
    BOOST_CHECK_EQUAL(view.inputs(), 2u);
    BOOST_CHECK_EQUAL(view.outputs(), 1u);
    BOOST_CHECK_EQUAL(view.version(), 0xa5u);
    BOOST_CHECK_EQUAL(view.locktime(), 0x85u);

    // hash and expectations must be non-witnessed (stripped).
    BOOST_CHECK_EQUAL(view.hash(true), tx.hash(stripped));
    BOOST_CHECK_EQUAL(view.hash(false), tx.hash(false));
    BOOST_CHECK_EQUAL(view.serialized_size(true), tx.serialized_size(stripped));
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

BOOST_AUTO_TEST_CASE(transaction_view__construct__tx4_non_witnessed__valid)
{
    // Serialized to buffer WITHOUT witness data.
    const auto transaction = test::tx4.to_data(false);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    // Parse buffer for a witness node (but doesn't exist).
    const chain::transaction_view view{ reader, transaction, false, false };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(!view.is_coinbase());
    BOOST_CHECK(!view.is_null_point());
    BOOST_CHECK(!view.is_segregated());
    BOOST_CHECK_EQUAL(view.inputs(), 2u);
    BOOST_CHECK_EQUAL(view.outputs(), 1u);
    BOOST_CHECK_EQUAL(view.version(), 0xa5u);
    BOOST_CHECK_EQUAL(view.locktime(), 0x85u);

    // hash and expectations must be non-witness (doesn't exist).
    BOOST_CHECK_EQUAL(view.hash(true), tx.hash(false));
    BOOST_CHECK_EQUAL(view.hash(false), tx.hash(false));
    BOOST_CHECK_EQUAL(view.serialized_size(true), tx.serialized_size(false));
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

    const chain::transaction_view view{ reader, block, true, true };
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

    const chain::transaction_view view{ reader, transaction, false, true };
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

    const chain::transaction_view view{ reader, block, true, true };
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

    const chain::transaction_view view{ reader, transaction, false, true };
    BOOST_CHECK(view.is_valid());

    data_chunk witness(expected.size(), 0xff);
    stream::flip::fast ostream{ witness };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_witnesses_stream();
    read::bytes::fast source{ stream };

    chain::transaction_view::write_witness(sink, source);
    BOOST_CHECK_EQUAL(witness, expected);
}

// to_data

BOOST_AUTO_TEST_CASE(transaction_view__to_data__tx4_witness__matches_transaction)
{
    // Serialized to buffer WITH witness data, parsed for a witness node.
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::transaction_view view{ reader, transaction, false, true };
    BOOST_REQUIRE(view.is_valid());
    BOOST_REQUIRE(view.is_segregated());

    // Witnessed form is a copy; stripped drops marker, flag and witness.
    data_chunk witnessed(view.serialized_size(true));
    stream::out::fast witnessed_stream(witnessed);
    write::bytes::fast witnessed_sink(witnessed_stream);
    view.to_data(witnessed_sink, true);
    BOOST_CHECK_EQUAL(witnessed, tx.to_data(true));

    data_chunk stripped(view.serialized_size(false));
    stream::out::fast stripped_stream(stripped);
    write::bytes::fast stripped_sink(stripped_stream);
    view.to_data(stripped_sink, false);
    BOOST_CHECK_EQUAL(stripped, tx.to_data(false));
}

BOOST_AUTO_TEST_CASE(transaction_view__to_data__tx4_stripped_source__stripped)
{
    // Serialized to buffer WITHOUT witness data (nothing to strip).
    const auto transaction = test::tx4.to_data(false);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::transaction_view view{ reader, transaction, false, false };
    BOOST_REQUIRE(view.is_valid());
    BOOST_REQUIRE(!view.is_segregated());

    // Both forms are the stripped bytes; witness is never added.
    data_chunk witnessed(view.serialized_size(true));
    stream::out::fast witnessed_stream(witnessed);
    write::bytes::fast witnessed_sink(witnessed_stream);
    view.to_data(witnessed_sink, true);
    BOOST_CHECK_EQUAL(witnessed, tx.to_data(false));

    data_chunk stripped(view.serialized_size(false));
    stream::out::fast stripped_stream(stripped);
    write::bytes::fast stripped_sink(stripped_stream);
    view.to_data(stripped_sink, false);
    BOOST_CHECK_EQUAL(stripped, tx.to_data(false));
}

BOOST_AUTO_TEST_CASE(transaction_view__to_data__tx4_overloads__match_transaction)
{
    // Serialized to buffer WITH witness data, parsed for a witness node.
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::transaction_view view{ reader, transaction, false, true };
    BOOST_REQUIRE(view.is_valid());
    BOOST_REQUIRE(view.is_segregated());

    // Chunk overload.
    BOOST_CHECK_EQUAL(view.to_data(true), tx.to_data(true));
    BOOST_CHECK_EQUAL(view.to_data(false), tx.to_data(false));

    // Stream overload.
    std::stringstream witnessed_stream{};
    view.to_data(witnessed_stream, true);
    BOOST_REQUIRE(witnessed_stream);
    BOOST_CHECK_EQUAL(to_chunk(witnessed_stream.str()), tx.to_data(true));

    std::stringstream stripped_stream{};
    view.to_data(stripped_stream, false);
    BOOST_REQUIRE(stripped_stream);
    BOOST_CHECK_EQUAL(to_chunk(stripped_stream.str()), tx.to_data(false));
}

// input_table_size/output_table_size
// ----------------------------------------------------------------------------
// Store sizes, computed over the buffer during construction.

// One byte input script and two byte output script, value of 42.
static system::chain::transaction table_tx()
{
    using namespace system::chain;
    const inputs ins{ { point{ one_hash, 0 }, script{ { opcode::dup } }, 42 } };
    const outputs outs{ { 42, script{ { opcode::dup, opcode::dup } } } };
    return { 1, ins, outs, 0 };
}

BOOST_AUTO_TEST_CASE(transaction_view__input_table_size__unwitnessed__scripts_and_input_count)
{
    const chain::block block{ chain::header{}, chain::transactions{ table_tx() } };
    const auto data = block.to_data(true);
    stream::in::fast istream{ data };
    read::bytes::fast reader{ istream };
    reader.skip_bytes(chain::header::serialized_size());
    BOOST_REQUIRE_EQUAL(reader.read_variable(), 1u);

    const chain::transaction_view view{ reader, data, false, true };
    BOOST_REQUIRE(view.is_valid());
    BOOST_REQUIRE(!view.is_segregated());

    // variable_size(1) + 1 script byte, plus one byte for the unwitnessed input.
    BOOST_CHECK_EQUAL(view.input_table_size(false), 3u);

    // variable_size(42) + variable_size(2) + 2 script bytes.
    BOOST_CHECK_EQUAL(view.output_table_size(), 4u);
}

// The pruned form carries an empty script and witness for each input.
BOOST_AUTO_TEST_CASE(transaction_view__input_table_size__pruned__two_per_input)
{
    const chain::block block{ chain::header{}, chain::transactions{ table_tx() } };
    const auto data = block.to_data(true);
    stream::in::fast istream{ data };
    read::bytes::fast reader{ istream };
    reader.skip_bytes(chain::header::serialized_size());
    BOOST_REQUIRE_EQUAL(reader.read_variable(), 1u);

    const chain::transaction_view view{ reader, data, false, true };
    BOOST_REQUIRE(view.is_valid());
    BOOST_CHECK_EQUAL(view.input_table_size(true), 2u * view.inputs());
}

// read_witness_size
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(transaction_view__read_witness_size__populated__serialized_size)
{
    const chain::witness instance{ chunk_cptrs{ to_shared<data_chunk>({ 0x42_u8 }), to_shared<data_chunk>(hash_size, 0x00_u8) } };
    const auto data = instance.to_data(true);
    stream::in::copy source{ data };
    read::bytes::istream reader{ source };
    BOOST_CHECK_EQUAL(chain::transaction_view::read_witness_size(reader), data.size());
    BOOST_CHECK(reader);
    BOOST_CHECK(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(transaction_view__read_witness_size__empty_stack__prefix_only)
{
    const chain::witness instance{ chunk_cptrs{} };
    const auto data = instance.to_data(true);
    stream::in::copy source{ data };
    read::bytes::istream reader{ source };
    BOOST_CHECK_EQUAL(chain::transaction_view::read_witness_size(reader), data.size());
    BOOST_CHECK_EQUAL(data.size(), 1u);
}

BOOST_AUTO_TEST_SUITE_END()
