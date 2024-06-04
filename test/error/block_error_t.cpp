/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(block_error_t_tests)

// These test std::error_code equality operator overrides.

BOOST_AUTO_TEST_CASE(block_error_t__code__block_success__false_exected_message)
{
    constexpr auto value = error::block_success;
    const auto ec = code(value);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block success");
}

// check header

BOOST_AUTO_TEST_CASE(block_error_t__code__invalid_proof_of_work__true_exected_message)
{
    constexpr auto value = error::invalid_proof_of_work;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "proof of work invalid");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__futuristic_timestamp__true_exected_message)
{
    constexpr auto value = error::futuristic_timestamp;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "timestamp too far in the future");
}

// accept header

BOOST_AUTO_TEST_CASE(block_error_t__code__checkpoint_conflict__true_exected_message)
{
    constexpr auto value = error::checkpoint_conflict;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block hash rejected by checkpoint");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__invalid_block_version__true_exected_message)
{
    constexpr auto value = error::invalid_block_version;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block version rejected at current height");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__timestamp_too_early__true_exected_message)
{
    constexpr auto value = error::timestamp_too_early;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block timestamp is too early");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__incorrect_proof_of_work__true_exected_message)
{
    constexpr auto value = error::incorrect_proof_of_work;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "proof of work does not match bits field");
}

// check block

BOOST_AUTO_TEST_CASE(block_error_t__code__block_size_limit__true_exected_message)
{
    constexpr auto value = error::block_size_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block size limit exceeded");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__empty_block__true_exected_message)
{
    constexpr auto value = error::empty_block;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block has no transactions");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__first_not_coinbase__true_exected_message)
{
    constexpr auto value = error::first_not_coinbase;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "first transaction not a coinbase");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__extra_coinbases__true_exected_message)
{
    constexpr auto value = error::extra_coinbases;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "more than one coinbase");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__internal_duplicate__true_exected_message)
{
    constexpr auto value = error::internal_duplicate;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "matching transaction hashes in block");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__block_internal_double_spend__true_exected_message)
{
    constexpr auto value = error::block_internal_double_spend;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "double spend internal to block");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__forward_reference__true_exected_message)
{
    constexpr auto value = error::forward_reference;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "transactions out of order");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__invalid_transaction_commitment__true_exected_message)
{
    constexpr auto value = error::invalid_transaction_commitment;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "invalid transaction commitment");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__block_legacy_sigop_limit__true_exected_message)
{
    constexpr auto value = error::block_legacy_sigop_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "too many block legacy signature operations");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__type32_malleated__true_exected_message)
{
    constexpr auto value = error::type32_malleated;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block is type32 malleated");
}

// accept block

BOOST_AUTO_TEST_CASE(block_error_t__code__block_non_final__true_exected_message)
{
    constexpr auto value = error::block_non_final;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block contains a non-final transaction");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__coinbase_height_mismatch__true_exected_message)
{
    constexpr auto value = error::coinbase_height_mismatch;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block height mismatch in coinbase");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__coinbase_value_limit__true_exected_message)
{
    constexpr auto value = error::coinbase_value_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "coinbase value too high");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__block_sigop_limit__true_exected_message)
{
    constexpr auto value = error::block_sigop_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "too many block signature operations");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__invalid_witness_commitment__true_exected_message)
{
    constexpr auto value = error::invalid_witness_commitment;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "invalid witness commitment");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__block_weight_limit__true_exected_message)
{
    constexpr auto value = error::block_weight_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block weight limit exceeded");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__temporary_hash_limit__true_exected_message)
{
    constexpr auto value = error::temporary_hash_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "block contains too many hashes");
}

BOOST_AUTO_TEST_CASE(block_error_t__code__unspent_coinbase_collision__true_exected_message)
{
    constexpr auto value = error::unspent_coinbase_collision;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "unspent coinbase collision");
}

BOOST_AUTO_TEST_SUITE_END()
