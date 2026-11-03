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

BC_AUTO_TEST_SUITE(block_error_t_tests)

// These test std::error_code equality operator overrides.

BC_AUTO_TEST_CASE(block_error_t__code__block_success__false_expected_message)
{
    constexpr auto value = error::block_success;
    const auto ec = code(value);
    BC_REQUIRE(!ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block success");
}

// check header

BC_AUTO_TEST_CASE(block_error_t__code__invalid_proof_of_work__true_expected_message)
{
    constexpr auto value = error::invalid_proof_of_work;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "proof of work invalid");
}

BC_AUTO_TEST_CASE(block_error_t__code__futuristic_timestamp__true_expected_message)
{
    constexpr auto value = error::futuristic_timestamp;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "timestamp too far in the future");
}

// accept header

BC_AUTO_TEST_CASE(block_error_t__code__checkpoint_conflict__true_expected_message)
{
    constexpr auto value = error::checkpoint_conflict;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block hash rejected by checkpoint");
}

BC_AUTO_TEST_CASE(block_error_t__code__insufficient_block_version__true_expected_message)
{
    constexpr auto value = error::insufficient_block_version;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block version rejected at current height");
}

BC_AUTO_TEST_CASE(block_error_t__code__anachronistic_timestamp__true_expected_message)
{
    constexpr auto value = error::anachronistic_timestamp;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block timestamp is too early");
}

BC_AUTO_TEST_CASE(block_error_t__code__incorrect_proof_of_work__true_expected_message)
{
    constexpr auto value = error::incorrect_proof_of_work;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "proof of work does not match bits field");
}

BC_AUTO_TEST_CASE(block_error_t__code__early_timestamp__true_expected_message)
{
    constexpr auto value = error::early_timestamp;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "retarget block timestamp is too early");
}

// confirm header

BC_AUTO_TEST_CASE(block_error_t__code__orphan_block__true_expected_message)
{
    constexpr auto value = error::orphan_block;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block parent is missing");
}

// check block

BC_AUTO_TEST_CASE(block_error_t__code__block_size_limit__true_expected_message)
{
    constexpr auto value = error::block_size_limit;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block size limit exceeded");
}

BC_AUTO_TEST_CASE(block_error_t__code__empty_block__true_expected_message)
{
    constexpr auto value = error::empty_block;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block has no transactions");
}

BC_AUTO_TEST_CASE(block_error_t__code__first_not_coinbase__true_expected_message)
{
    constexpr auto value = error::first_not_coinbase;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "first transaction not a coinbase");
}

BC_AUTO_TEST_CASE(block_error_t__code__extra_coinbases__true_expected_message)
{
    constexpr auto value = error::extra_coinbases;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "more than one coinbase");
}

BC_AUTO_TEST_CASE(block_error_t__code__internal_duplicate__true_expected_message)
{
    constexpr auto value = error::internal_duplicate;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "matching transaction hashes in block");
}

BC_AUTO_TEST_CASE(block_error_t__code__block_internal_double_spend__true_expected_message)
{
    constexpr auto value = error::block_internal_double_spend;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "double spend internal to block");
}

BC_AUTO_TEST_CASE(block_error_t__code__forward_reference__true_expected_message)
{
    constexpr auto value = error::forward_reference;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "transactions out of order");
}

BC_AUTO_TEST_CASE(block_error_t__code__invalid_transaction_commitment__true_expected_message)
{
    constexpr auto value = error::invalid_transaction_commitment;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid transaction commitment");
}

BC_AUTO_TEST_CASE(block_error_t__code__block_legacy_sigop_limit__true_expected_message)
{
    constexpr auto value = error::block_legacy_sigop_limit;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "too many block legacy signature operations");
}

// accept block

BC_AUTO_TEST_CASE(block_error_t__code__block_non_final__true_expected_message)
{
    constexpr auto value = error::block_non_final;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block contains a non-final transaction");
}

BC_AUTO_TEST_CASE(block_error_t__code__coinbase_height_mismatch__true_expected_message)
{
    constexpr auto value = error::coinbase_height_mismatch;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block height mismatch in coinbase");
}

BC_AUTO_TEST_CASE(block_error_t__code__coinbase_value_limit__true_expected_message)
{
    constexpr auto value = error::coinbase_value_limit;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "coinbase value too high");
}

BC_AUTO_TEST_CASE(block_error_t__code__block_sigop_limit__true_expected_message)
{
    constexpr auto value = error::block_sigop_limit;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "too many block signature operations");
}

BC_AUTO_TEST_CASE(block_error_t__code__invalid_witness_commitment__true_expected_message)
{
    constexpr auto value = error::invalid_witness_commitment;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid witness commitment");
}

BC_AUTO_TEST_CASE(block_error_t__code__block_weight_limit__true_expected_message)
{
    constexpr auto value = error::block_weight_limit;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "block weight limit exceeded");
}

BC_AUTO_TEST_CASE(block_error_t__code__invalid_signet_solution__true_expected_message)
{
    constexpr auto value = error::invalid_signet_solution;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid signet block solution");
}

BC_AUTO_TEST_CASE(block_error_t__code__invalid_signature__true_expected_message)
{
    constexpr auto value = error::invalid_signature;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "invalid batched signature");
}

BC_AUTO_TEST_CASE(block_error_t__code__unspent_coinbase_collision__true_expected_message)
{
    constexpr auto value = error::unspent_coinbase_collision;
    const auto ec = code(value);
    BC_REQUIRE(ec);
    BC_REQUIRE(ec == value);
    BC_REQUIRE_EQUAL(ec.message(), "unspent coinbase collision");
}

BC_AUTO_TEST_SUITE_END()
