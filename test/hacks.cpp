/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include "test.hpp"
#include "./performance/performance.hpp"
#include <utility>

#if defined(HAVE_PERFORMANCE_TESTS) && defined(HAVE_EXECUTION)

BOOST_AUTO_TEST_SUITE(skip_parser_tests)

// [skip + max(parse, hash)] vs. [parse + hash]
//
// 1 + max(1, 10) = 11
// 1 + 10 = 11
// 11/11 of sequential cost
// skip is inherently cheaper than parse, so this is a limit.
// however there is always concurrency overhead, so net cost
// is increased where [parse - skip < overhead].
//
// 1 + max(5, 10) = 11
// 5 + 10 = 15
// 11/15 of sequential cost
//
// 1 + max(10, 5) = 11
// 10 + 5 = 15
// 11/15 of sequential cost
//
// 1 + max(10, 10) = 11
// 10 + 10 = 20
// 11/20 of sequential cost
//
// 0 + max(10, 10) = 10
// 10 + 10 = 20
// 10/20 of sequential cost
// skip is non-zero, so this is a limit.
// 50% reduction is a limit, where parse = hash and negligable skip.

#if !defined(VISIBLE)

#endif

using namespace chain;
using namespace perf;

// No read-size limits are required, as nothing is allocated.

void skip_witness(reader& source) NOEXCEPT
{
    const auto count = source.read_size();
    for (size_t i = 0; i < count; ++i)
        source.skip_bytes(source.read_size());
}

void skip_witnesses(reader& source, size_t inputs) NOEXCEPT
{
    for (size_t i = 0; i < inputs; ++i)
        skip_witness(source);
}

void skip_script(reader& source) NOEXCEPT
{
    source.skip_bytes(source.read_size());
}

void skip_point(reader& source) NOEXCEPT
{
    constexpr auto index = 4u;
    constexpr auto hash = hash_size;
    source.skip_bytes(hash + index);
}

void skip_input(reader& source) NOEXCEPT
{
    constexpr auto sequence = 4u;
    skip_point(source);
    skip_script(source);
    source.skip_bytes(sequence);
}

void skip_output(reader& source) NOEXCEPT
{
    constexpr auto value = 8u;
    source.skip_bytes(value);
    skip_script(source);
}

void skip_inputs(reader& source, size_t inputs) NOEXCEPT
{
    for (size_t i = 0; i < inputs; ++i)
        skip_input(source);
}

void skip_outputs(reader& source) NOEXCEPT
{
    const auto count = source.read_size();
    for (size_t i = 0; i < count; ++i)
        skip_output(source);
}

// TODO: Add static skip parsing to each chain class as inline.
// TODO: Rolls up into block and transaction (verify inlining).
// TODO: Create txs pointers vector from block skip parser static.
// TODO: Emit merkle root from skip-parsed static block (chunk/stream/reader).
// TODO: Initially parallel over txs, but can later also vectorize.
// TODO: parallel over sha-ni/neon may be more optimal than vectorization.
size_t skip_parse(reader& source) NOEXCEPT
{
    constexpr auto version = 4u;
    constexpr auto locktime = 4u;

    const auto start = source.get_position();
    source.skip_bytes(version);
    auto inputs = source.read_size();
    skip_inputs(source, inputs);

    if ((inputs == witness_marker) && (source.peek_byte() == witness_enabled))
    {
        source.skip_byte();
        inputs = source.read_size();
        skip_inputs(source, inputs);
        skip_outputs(source);
        skip_witnesses(source, inputs);
    }
    else
    {
        skip_outputs(source);
    }

    source.skip_bytes(locktime);
    const auto size = (source.get_position() - start);
    return size;
}

BOOST_AUTO_TEST_CASE(skip_parser__parse__always__correct)
{
    constexpr auto witness = false;
    transaction tx2(tx_2, witness);
    const auto expected0 = tx_2.size();
    const auto expected1 = tx2.to_data(witness).size();
    const auto expected2 = tx2.serialized_size(witness);
    BOOST_REQUIRE_EQUAL(expected0, expected1);
    BOOST_REQUIRE_EQUAL(expected1, expected2);

    stream::in::copy input(tx_2);
    read::bytes::istream source(input);
    BOOST_REQUIRE_EQUAL(skip_parse(source), expected0);
}

BOOST_AUTO_TEST_CASE(skip_parser__hash__always__correct)
{
    constexpr auto witness = false;
    transaction tx2(tx_2, witness);
    const auto expected = tx2.hash(witness);
    BOOST_REQUIRE_EQUAL(bitcoin_hash(tx_2), expected);
}

constexpr auto factor = 1'000'000_u64;

BOOST_AUTO_TEST_CASE(skip_parser__parse_hash__factor__baseline)
{
    constexpr auto witness = true;
    const auto expected = bitcoin_hash(tx_2);

    for (size_t i = 0; i < factor; ++i)
    {
        transaction tx2(tx_2, witness);
        BOOST_REQUIRE_EQUAL(bitcoin_hash(tx_2), expected);
    }
}

BOOST_AUTO_TEST_CASE(skip_parser__parse_serialize_hash__factor__baseline)
{
    constexpr auto witness = true;
    const auto expected = bitcoin_hash(tx_2);

    for (size_t i = 0; i < factor; ++i)
    {
        transaction tx2(tx_2, witness);
        BOOST_REQUIRE_EQUAL(tx2.hash(witness), expected);
    }
}

BOOST_AUTO_TEST_CASE(skip_parser__skip_parse__factor__baseline)
{
    const auto expected = tx_2.size();

    for (size_t i = 0; i < factor; ++i)
    {
        stream::in::copy input(tx_2);
        read::bytes::istream source(input);
        BOOST_REQUIRE_EQUAL(skip_parse(source), expected);
    }
}

BOOST_AUTO_TEST_CASE(skip_parser__skip_parse_hash__factor__expected)
{
    const auto expected0 = tx_2.size();
    const auto expected1 = bitcoin_hash(tx_2);

    for (size_t i = 0; i < factor; ++i)
    {
        stream::in::copy input(tx_2);
        read::bytes::istream source(input);
        BOOST_REQUIRE_EQUAL(skip_parse(source), expected0);
        BOOST_REQUIRE_EQUAL(bitcoin_hash(tx_2), expected1);
    }
}

// boost test spits out a bunch of "info" messages to the console on these.
#if defined (DISABLED)

// This is a test of singleton tx concurrent parse/hash.
BOOST_AUTO_TEST_CASE(skip_parser__skip_parse_concurrent_tx__factor__expected)
{
    const auto expected0 = tx_2.size();
    const auto expected1 = bitcoin_hash(tx_2);
    constexpr std_array<bool, 2> jobs{ true, false };

    for (size_t i = 0; i < factor; ++i)
    {
        std::for_each(std::execution::par_unseq, jobs.begin(), jobs.end(),
            [&](const auto job)
            {
                if (job)
                {
                    BOOST_REQUIRE_EQUAL(bitcoin_hash(tx_2), expected1);
                }
                else
                {
                    stream::in::copy input(tx_2);
                    read::bytes::istream source(input);
                    BOOST_REQUIRE_EQUAL(skip_parse(source), expected0);
                }
            });
    }
}

// This is a test of concurrent tx parse/hash across a block.
// Approximates concurrency striped across txs (without vectorization).
BOOST_AUTO_TEST_CASE(skip_parser__skip_parse_concurrent_block__factor_txs__expected)
{
    const auto expected0 = tx_2.size();
    const auto expected1 = bitcoin_hash(tx_2);
    constexpr std_array<bool, factor> jobs{};

    std::for_each(std::execution::par_unseq, jobs.begin(), jobs.end(),
        [&](const auto)
        {
            stream::in::copy input(tx_2);
            read::bytes::istream source(input);
            BOOST_REQUIRE_EQUAL(skip_parse(source), expected0);
            BOOST_REQUIRE_EQUAL(bitcoin_hash(tx_2), expected1);
        });
}

#endif // HAVE_PERFORMANCE_TESTS

BOOST_AUTO_TEST_SUITE_END()

#endif
