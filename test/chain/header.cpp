/**
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <boost/test/unit_test.hpp>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin.hpp>
#include <chrono>

using namespace bc;

BOOST_AUTO_TEST_SUITE(header_tests)

BOOST_AUTO_TEST_CASE(header__constructor_1__always__initialized_invalid)
{
    chain::header instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(header__constructor_2__always__equals_params)
{
    uint32_t version = 10u;
    hash_digest previous = hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    hash_digest merkle = hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    uint32_t timestamp = 531234u;
    uint32_t bits = 6523454u;
    uint32_t nonce = 68644u;
    uint64_t tx_count = 0u;

    chain::header instance(version, previous, merkle, timestamp, bits, nonce, tx_count);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(timestamp, instance.timestamp());
    BOOST_REQUIRE_EQUAL(bits, instance.bits());
    BOOST_REQUIRE_EQUAL(nonce, instance.nonce());
    BOOST_REQUIRE_EQUAL(tx_count, instance.transaction_count());
    BOOST_REQUIRE(previous == instance.previous_block_hash());
    BOOST_REQUIRE(merkle == instance.merkle());
}

BOOST_AUTO_TEST_CASE(header__constructor_3__always__equals_params)
{
    uint32_t version = 10u;
    hash_digest previous = hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f");
    hash_digest merkle = hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b");
    uint32_t timestamp = 531234u;
    uint32_t bits = 6523454u;
    uint32_t nonce = 68644u;
    uint64_t tx_count = 0u;

    chain::header instance(version, std::move(previous), std::move(merkle),
        timestamp, bits, nonce, tx_count);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE_EQUAL(version, instance.version());
    BOOST_REQUIRE_EQUAL(timestamp, instance.timestamp());
    BOOST_REQUIRE_EQUAL(bits, instance.bits());
    BOOST_REQUIRE_EQUAL(nonce, instance.nonce());
    BOOST_REQUIRE_EQUAL(tx_count, instance.transaction_count());
    BOOST_REQUIRE(previous == instance.previous_block_hash());
    BOOST_REQUIRE(merkle == instance.merkle());
}

BOOST_AUTO_TEST_CASE(header__constructor_4__always__equals_params)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    chain::header instance(expected);
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(header__constructor_5__always__equals_params)
{
    chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    chain::header instance(std::move(expected));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(header__from_data__insufficient_bytes__failure)
{
    data_chunk data(10);

    chain::header header;

    BOOST_REQUIRE_EQUAL(false, header.from_data(data, false));
    BOOST_REQUIRE_EQUAL(false, header.is_valid());
}

BOOST_AUTO_TEST_CASE(header__factory_from_data_1__valid_input__success)
{
    chain::header expected
    {
        10,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234,
        6523454,
        68644
    };

    data_chunk data = expected.to_data(false);

    auto result = chain::header::factory_from_data(data, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(header__factory_from_data_2__valid_input__success)
{
    chain::header expected
    {
        10,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234,
        6523454,
        68644
    };

    data_chunk data = expected.to_data(false);
    data_source istream(data);

    auto result = chain::header::factory_from_data(istream, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(header__factory_from_data_3__valid_input__success)
{
    chain::header expected
    {
        10,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234,
        6523454,
        68644
    };

    data_chunk data = expected.to_data(false);
    data_source istream(data);
    istream_reader source(istream);

    auto result = chain::header::factory_from_data(source, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(header__factory_from_data_3__without_transaction_count__does_not_match)
{
    chain::header expected
    {
        10,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234,
        6523454,
        68644,
        1234
    };

    data_chunk data = expected.to_data(false);
    data_source istream(data);
    istream_reader source(istream);

    auto result = chain::header::factory_from_data(source, false);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected != result);
}

BOOST_AUTO_TEST_CASE(header__factory_from_data_3__with_transaction_count__matches)
{
    chain::header expected
    {
        10,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234,
        6523454,
        68644,
        123544
    };

    data_chunk data = expected.to_data(true);
    data_source istream(data);
    istream_reader source(istream);

    auto result = chain::header::factory_from_data(source, true);

    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(header__version_accessor__always__returns_initialized_value)
{
    uint32_t value = 11234u;
    chain::header instance(
        value,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234u,
        4356344u,
        34564u);

    BOOST_REQUIRE_EQUAL(value, instance.version());
}

BOOST_AUTO_TEST_CASE(header__version_setter__roundtrip__success)
{
    uint32_t expected = 4521u;
    chain::header instance;
    BOOST_REQUIRE(expected != instance.version());
    instance.set_version(expected);
    BOOST_REQUIRE(expected == instance.version());
}

BOOST_AUTO_TEST_CASE(header__previous_block_hash_accessor_1__always__returns_initialized_value)
{
    hash_digest value = hash_literal("abababababababababababababababababababababababababababababababab");
    chain::header instance(
        11234u,
        value,
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234u,
        4356344u,
        34564u);

    BOOST_REQUIRE(value == instance.previous_block_hash());
}

BOOST_AUTO_TEST_CASE(header__previous_block_hash_accessor_2__always__returns_initialized_value)
{
    hash_digest value = hash_literal("abababababababababababababababababababababababababababababababab");
    const chain::header instance(
        11234u,
        value,
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234u,
        4356344u,
        34564u);

    BOOST_REQUIRE(value == instance.previous_block_hash());
}

BOOST_AUTO_TEST_CASE(header__previous_block_hash_setter_1__roundtrip__success)
{
    hash_digest expected = hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe");
    chain::header instance;
    BOOST_REQUIRE(expected != instance.previous_block_hash());
    instance.set_previous_block_hash(expected);
    BOOST_REQUIRE(expected == instance.previous_block_hash());
}

BOOST_AUTO_TEST_CASE(header__previous_block_hash_setter_2__roundtrip__success)
{
    hash_digest expected = hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe");
    hash_digest duplicate = expected;
    chain::header instance;
    BOOST_REQUIRE(expected != instance.previous_block_hash());
    instance.set_previous_block_hash(std::move(duplicate));
    BOOST_REQUIRE(expected == instance.previous_block_hash());
}

BOOST_AUTO_TEST_CASE(header__merkle_accessor_1__always__returns_initialized_value)
{
    hash_digest value = hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe");
    chain::header instance(
        11234u,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        value,
        753234u,
        4356344u,
        34564u);

    BOOST_REQUIRE(value == instance.merkle());
}

BOOST_AUTO_TEST_CASE(header__merkle_accessor_2__always__returns_initialized_value)
{
    hash_digest value = hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe");
    const chain::header instance(
        11234u,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        value,
        753234u,
        4356344u,
        34564u);

    BOOST_REQUIRE(value == instance.merkle());
}

BOOST_AUTO_TEST_CASE(header__merkle_setter_1__roundtrip__success)
{
    hash_digest expected = hash_literal("abababababababababababababababababababababababababababababababab");
    chain::header instance;
    BOOST_REQUIRE(expected != instance.merkle());
    instance.set_merkle(expected);
    BOOST_REQUIRE(expected == instance.merkle());
}

BOOST_AUTO_TEST_CASE(header__merkle_setter_2__roundtrip__success)
{
    hash_digest expected = hash_literal("abababababababababababababababababababababababababababababababab");
    hash_digest duplicate = expected;
    chain::header instance;
    BOOST_REQUIRE(expected != instance.merkle());
    instance.set_merkle(std::move(duplicate));
    BOOST_REQUIRE(expected == instance.merkle());
}

BOOST_AUTO_TEST_CASE(header__timestamp_accessor__always__returns_initialized_value)
{
    uint32_t value = 753234u;
    chain::header instance(
        11234u,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        value,
        4356344u,
        34564u);

    BOOST_REQUIRE_EQUAL(value, instance.timestamp());
}

BOOST_AUTO_TEST_CASE(header__timestamp_setter__roundtrip__success)
{
    uint32_t expected = 4521u;
    chain::header instance;
    BOOST_REQUIRE(expected != instance.timestamp());
    instance.set_timestamp(expected);
    BOOST_REQUIRE(expected == instance.timestamp());
}

BOOST_AUTO_TEST_CASE(header__bits_accessor__always__returns_initialized_value)
{
    uint32_t value = 4356344u;
    chain::header instance(
        11234u,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234u,
        value,
        34564u);

    BOOST_REQUIRE_EQUAL(value, instance.bits());
}

BOOST_AUTO_TEST_CASE(header__bits_setter__roundtrip__success)
{
    uint32_t expected = 4521u;
    chain::header instance;
    BOOST_REQUIRE(expected != instance.bits());
    instance.set_bits(expected);
    BOOST_REQUIRE(expected == instance.bits());
}

BOOST_AUTO_TEST_CASE(header__nonce_accessor__always__returns_initialized_value)
{
    uint32_t value = 34564u;
    chain::header instance(
        11234u,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234u,
        4356344u,
        value);

    BOOST_REQUIRE_EQUAL(value, instance.nonce());
}

BOOST_AUTO_TEST_CASE(header__nonce_setter__roundtrip__success)
{
    uint32_t expected = 4521u;
    chain::header instance;
    BOOST_REQUIRE(expected != instance.nonce());
    instance.set_nonce(expected);
    BOOST_REQUIRE(expected == instance.nonce());
}

BOOST_AUTO_TEST_CASE(header__transaction_count_accessor__always__returns_initialized_value)
{
    uint64_t value = 684553u;
    chain::header instance(
        11234u,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234u,
        4356344u,
        34564u,
        value);

    BOOST_REQUIRE_EQUAL(value, instance.transaction_count());
}

BOOST_AUTO_TEST_CASE(header__transaction_count_setter__roundtrip__success)
{
    uint64_t expected = 985721u;
    chain::header instance;
    BOOST_REQUIRE(expected != instance.transaction_count());
    instance.set_transaction_count(expected);
    BOOST_REQUIRE(expected == instance.transaction_count());
}

BOOST_AUTO_TEST_CASE(header__is_valid_time_stamp__timestamp_less_than_2_hours_from_now__returns_true)
{
    chain::header instance;
    const auto now = std::chrono::system_clock::now();
    const auto now_time = std::chrono::system_clock::to_time_t(now);
    instance.set_timestamp(static_cast<uint32_t>(now_time));
    BOOST_REQUIRE_EQUAL(true, instance.is_valid_time_stamp());
}

BOOST_AUTO_TEST_CASE(header__is_valid_time_stamp__timestamp_greater_than_2_hours_from_now__returns_false)
{
    chain::header instance;
    const auto now = std::chrono::system_clock::now();
    const auto duration = std::chrono::hours(3);
    const auto future = std::chrono::system_clock::to_time_t(now + duration);
    instance.set_timestamp(static_cast<uint32_t>(future));
    BOOST_REQUIRE_EQUAL(false, instance.is_valid_time_stamp());
}

BOOST_AUTO_TEST_CASE(header__is_valid_proof_of_work__bits_exceeds_maximum__returns_false)
{
    chain::header instance;
    instance.set_bits(max_work_bits + 1);
    BOOST_REQUIRE_EQUAL(false, instance.is_valid_proof_of_work());
}

BOOST_AUTO_TEST_CASE(header__is_valid_proof_of_work__hash_greater_bits__returns_false)
{
    chain::header instance(
        11234u,
        hash_literal("abababababababababababababababababababababababababababababababab"),
        hash_literal("fefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefefe"),
        753234u,
        0u,
        34564u);

    BOOST_REQUIRE_EQUAL(false, instance.is_valid_proof_of_work());
}

BOOST_AUTO_TEST_CASE(header__is_valid_proof_of_work__hash_less_than_bits__returns_true)
{
    chain::header instance(
        4u,
        hash_literal("000000000000000003ddc1e929e2944b8b0039af9aa0d826c480a83d8b39c373"),
        hash_literal("a6cb0b0d6531a71abe2daaa4a991e5498e1b6b0b51549568d0f9d55329b905df"),
        1474388414u,
        402972254u,
        2842832236u);

    BOOST_REQUIRE_EQUAL(true, instance.is_valid_proof_of_work());
}

BOOST_AUTO_TEST_CASE(header__operator_assign_equals__always__matches_equivalent)
{
    chain::header value(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    BOOST_REQUIRE(value.is_valid());

    chain::header instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());

    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_equals__duplicates__returns_true)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    chain::header instance(expected);
    BOOST_REQUIRE_EQUAL(true, instance == expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_equals__differs__returns_false)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    chain::header instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_not_equals__duplicates__returns_false)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    chain::header instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(header__operator_boolean_not_equals__differs__returns_true)
{
    const chain::header expected(
        10u,
        hash_literal("000000000019d6689c085ae165831e934ff763ae46a2a6c172b3f1b60a8ce26f"),
        hash_literal("4a5e1e4baab89f3a32518a88c31bc87f618f76673e2cc77ab2127b7afdeda33b"),
        531234u,
        6523454u,
        68644u);

    chain::header instance;
    BOOST_REQUIRE_EQUAL(true, instance != expected);
}

BOOST_AUTO_TEST_SUITE_END()
