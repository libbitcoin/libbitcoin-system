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

    const chain::view::transaction view{ reader, block, false, true };
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

    const chain::view::transaction view{ reader, block, true, true };
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
    const chain::view::transaction view{ reader, transaction, false, witnessed };
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
    const chain::view::transaction view{ reader, transaction, false, stripped };
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
    const chain::view::transaction view{ reader, transaction, false, false };
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

    const chain::view::transaction view{ reader, block, true, true };
    BOOST_CHECK(view.is_valid());

    data_chunk script(expected.size(), 0xff);
    stream::flip::fast ostream{ script };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_inputs_stream();
    read::bytes::fast source{ stream };

    chain::view::transaction::write_input_script(sink, source);
    BOOST_CHECK_EQUAL(script, expected);
}

BOOST_AUTO_TEST_CASE(transaction_view__write_input_script__tx4_witness__expected)
{
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    const auto expected = tx.inputs_ptr()->front()->script().to_data(true);
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::view::transaction view{ reader, transaction, false, true };
    BOOST_CHECK(view.is_valid());

    data_chunk script(expected.size(), 0xff);
    stream::flip::fast ostream{ script };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_inputs_stream();
    read::bytes::fast source{ stream };

    chain::view::transaction::write_input_script(sink, source);
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

    const chain::view::transaction view{ reader, block, true, true };
    BOOST_CHECK(view.is_valid());

    data_chunk witness(expected.size(), 0xff);
    stream::flip::fast ostream{ witness };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_witnesses_stream();
    read::bytes::fast source{ stream };

    chain::view::transaction::write_witness(sink, source);
    BOOST_CHECK_EQUAL(witness, expected);
}

BOOST_AUTO_TEST_CASE(transaction_view__write_witness__tx4_witness__expected)
{
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    const auto expected = tx.inputs_ptr()->front()->witness().to_data(true);
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::view::transaction view{ reader, transaction, false, true };
    BOOST_CHECK(view.is_valid());

    data_chunk witness(expected.size(), 0xff);
    stream::flip::fast ostream{ witness };
    flip::bytes::fast sink{ ostream };
    auto stream = view.get_witnesses_stream();
    read::bytes::fast source{ stream };

    chain::view::transaction::write_witness(sink, source);
    BOOST_CHECK_EQUAL(witness, expected);
}

// witnesses

BOOST_AUTO_TEST_CASE(transaction_view__witnesses__genesis__empty)
{
    const auto block = test::genesis.to_data(true);
    stream::in::fast istream{ block };
    read::bytes::fast reader{ istream };
    reader.skip_bytes(chain::header::serialized_size());
    reader.read_variable();

    const chain::view::transaction view{ reader, block, true, true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(view.witnesses().empty());
}

BOOST_AUTO_TEST_CASE(transaction_view__witnesses__tx4_witness__expected)
{
    const auto transaction = test::tx4.to_data(true);
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const auto expected = base16_chunk("01032525250103353535");
    const chain::view::transaction view{ reader, transaction, false, true };
    BOOST_CHECK(view.is_valid());
    BOOST_CHECK(view.is_segregated());
    BOOST_CHECK_EQUAL(view.witnesses().to_chunk(), expected);
}

// to_data

BOOST_AUTO_TEST_CASE(transaction_view__to_data__tx4_witness__matches_transaction)
{
    // Serialized to buffer WITH witness data, parsed for a witness node.
    const auto transaction = test::tx4.to_data(true);
    const auto& tx = test::tx4;
    stream::in::fast istream{ transaction };
    read::bytes::fast reader{ istream };

    const chain::view::transaction view{ reader, transaction, false, true };
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

    const chain::view::transaction view{ reader, transaction, false, false };
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

    const chain::view::transaction view{ reader, transaction, false, true };
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

    const chain::view::transaction view{ reader, data, false, true };
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

    const chain::view::transaction view{ reader, data, false, true };
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
    BOOST_CHECK_EQUAL(chain::view::transaction::read_witness_size(reader), data.size());
    BOOST_CHECK(reader);
    BOOST_CHECK(reader.is_exhausted());
}

BOOST_AUTO_TEST_CASE(transaction_view__read_witness_size__empty_stack__prefix_only)
{
    const chain::witness instance{ chunk_cptrs{} };
    const auto data = instance.to_data(true);
    stream::in::copy source{ data };
    read::bytes::istream reader{ source };
    BOOST_CHECK_EQUAL(chain::view::transaction::read_witness_size(reader), data.size());
    BOOST_CHECK_EQUAL(data.size(), 1u);
}

// signature_hash (unversioned)

// Uncached legacy sighash, written directly from the tx (reference).
static hash_digest reference_sighash(const chain::transaction& tx,
    uint32_t index, const chain::script& subscript, uint8_t flags) NOEXCEPT
{
    const auto flag = bit_and<uint8_t>(flags, 0x1f);
    const auto anyone = !is_zero(bit_and<uint8_t>(flags, 0x80));
    const auto single = (flag == 0x03);
    const auto none = (flag == 0x02);
    const auto& ins = *tx.inputs_ptr();
    const auto& outs = *tx.outputs_ptr();

    if (single && index >= outs.size())
        return one_hash;

    data_chunk preimage{};
    write::bytes::data sink{ preimage };
    sink.write_4_bytes_little_endian(tx.version());
    sink.write_variable(anyone ? one : ins.size());
    for (uint32_t in{}; in < ins.size(); ++in)
    {
        if (anyone && in != index)
            continue;

        ins.at(in)->point().to_data(sink);
        if (in == index)
        {
            subscript.to_data(sink, true);
            sink.write_4_bytes_little_endian(ins.at(in)->sequence());
        }
        else
        {
            sink.write_byte(0x00);
            sink.write_4_bytes_little_endian((single || none) ? 0u :
                ins.at(in)->sequence());
        }
    }

    if (none)
    {
        sink.write_variable(zero);
    }
    else if (single)
    {
        sink.write_variable(add1(index));
        for (uint32_t out{}; out < index; ++out)
        {
            sink.write_8_bytes_little_endian(max_uint64);
            sink.write_byte(0x00);
        }

        outs.at(index)->to_data(sink);
    }
    else
    {
        sink.write_variable(outs.size());
        for (const auto& out: outs)
            out->to_data(sink);
    }

    sink.write_4_bytes_little_endian(tx.locktime());
    sink.write_4_bytes_little_endian(flags);
    sink.flush();
    return bitcoin_hash(preimage);
}

static uint32_t random_number(std::mt19937& random) NOEXCEPT
{
    return std::uniform_int_distribution<uint32_t>{}(random);
}

static data_chunk random_chunk(std::mt19937& random, size_t limit) NOEXCEPT
{
    data_chunk out(std::uniform_int_distribution<size_t>{ 0, limit }(random));
    for (auto& byte: out)
        byte = narrow_cast<uint8_t>(random_number(random));

    return out;
}

static chain::script random_script(std::mt19937& random,
    size_t limit) NOEXCEPT
{
    return chain::script
    {
        chain::operations
        {
            chain::operation{ random_chunk(random, limit), false }
        }
    };
}

// Witnessed (when witness) inputs ensure that witnesses are serialized between
// outputs and locktime in the block buffer.
static chain::transaction random_transaction(std::mt19937& random,
    size_t inputs, bool witness) NOEXCEPT
{
    chain::inputs ins{};
    for (size_t in{}; in < inputs; ++in)
    {
        hash_digest hash{};
        for (auto& byte: hash)
            byte = narrow_cast<uint8_t>(random_number(random));

        const auto witnessed = witness &&
            (is_zero(in) || is_odd(random_number(random)));

        ins.push_back(chain::input
        {
            chain::point{ hash, random_number(random) },
            random_script(random, 120),
            chain::witness
            {
                witnessed ?
                    chunk_cptrs{ to_shared(random_chunk(random, 80)) } :
                    chunk_cptrs{}
            },
            random_number(random)
        });
    }

    chain::outputs outs{};
    const auto outputs = std::uniform_int_distribution<size_t>{ 1, 5 }(random);
    for (size_t out{}; out < outputs; ++out)
    {
        const auto value = random_number(random);
        outs.push_back(chain::output{ value, random_script(random, 60) });
    }

    return chain::transaction
    {
        random_number(random),
        std::move(ins),
        std::move(outs),
        random_number(random)
    };
}

BOOST_AUTO_TEST_CASE(transaction_view__signature_hash__unversioned_random__expected)
{
    using namespace system::chain;
    constexpr auto unversioned = script_version::unversioned;
    const chain::script coinbase_script
    {
        chain::operations{ chain::operation{ data_chunk{ 0x01, 0x02 }, false } }
    };

    const chain::transaction coinbase
    {
        1,
        chain::inputs{ { chain::point{}, coinbase_script, max_input_sequence } },
        chain::outputs{ { 42, chain::script{} } },
        0
    };

    // All, none, single, unknown (default all), with and without
    // anyone_can_pay, and with unused bits set.
    constexpr std::array<uint8_t, 18> types
    {
        0x00, 0x01, 0x02, 0x03, 0x04, 0x1f, 0x21, 0x41, 0x43,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x9f, 0xc1, 0xc2, 0xff
    };

    // Small, input count varint boundary (253), and random up to 400 inputs.
    std::mt19937 random{ 42 };
    std::vector<size_t> sizes{ 1, 2, 3, 4, 5, 6, 7, 252, 253, 400 };
    std::uniform_int_distribution<size_t> size{ 1, 400 };
    for (size_t count{}; count < 5u; ++count)
        sizes.push_back(size(random));

    for (size_t index{}; index < sizes.size(); ++index)
    {
        const auto witness = is_odd(index);
        const auto tx = random_transaction(random, sizes.at(index), witness);
        BOOST_REQUIRE_EQUAL(tx.is_segregated(), witness);

        const auto ins = tx.inputs_ptr()->size();
        data_chunk prevouts{};
        for (size_t in{}; in < ins; ++in)
        {
            const auto prevout = chain::output{ in, chain::script{} }.to_data();
            prevouts.insert(prevouts.end(), prevout.begin(), prevout.end());
        }

        const chain::block block{ chain::header{}, { coinbase, tx } };
        chain::view::block blocks{ block.to_data(true), true };
        BOOST_REQUIRE(blocks.is_valid());
        BOOST_REQUIRE(blocks.populate(std::move(prevouts), { false, true }));

        const auto& view = blocks.views().at(1);
        BOOST_REQUIRE(view.is_populated());
        BOOST_REQUIRE_EQUAL(view.inputs(), ins);
        BOOST_REQUIRE_EQUAL(view.hash(false), tx.hash(false));

        for (uint32_t in{}; in < ins; ++in)
        {
            const auto subscript = random_script(random, 100);
            const auto view_input = view.input_at(in);
            const auto chain_input = tx.input_at(in);

            std::vector<uint8_t> hash_types(types.begin(), types.end());
            hash_types.push_back(narrow_cast<uint8_t>(random_number(random)));

            for (const auto flag: hash_types)
            {
                hash_digest view_hash{};
                hash_digest chain_hash{};
                const auto expected = reference_sighash(tx, in, subscript, flag);
                BOOST_REQUIRE(view.signature_hash(view_hash, view_input,
                    subscript, {}, {}, unversioned, flag, flags::no_rules));
                BOOST_REQUIRE(tx.signature_hash(chain_hash, chain_input,
                    subscript, {}, {}, unversioned, flag, flags::no_rules));
                BOOST_REQUIRE_MESSAGE(view_hash == expected, "view inputs "
                    << ins << " input " << in << " flag " << unsigned{ flag });
                BOOST_REQUIRE_MESSAGE(chain_hash == expected, "chain inputs "
                    << ins << " input " << in << " flag " << unsigned{ flag });
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
