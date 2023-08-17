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

BOOST_AUTO_TEST_SUITE(transaction_error_t_tests)

BOOST_AUTO_TEST_CASE(transaction_error_t__code__transaction_success__false_exected_message)
{
    constexpr auto value = error::transaction_success;
    const auto ec = code(value);
    BOOST_REQUIRE(!ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "transaction success");
}

// check transaction

BOOST_AUTO_TEST_CASE(transaction_error_t__code__empty_transaction__true_exected_message)
{
    constexpr auto value = error::empty_transaction;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "transaction inputs or outputs empty");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__previous_output_null__true_exected_message)
{
    constexpr auto value = error::previous_output_null;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "non-coinbase transaction has input with null previous output");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__spend_overflow__true_exected_message)
{
    constexpr auto value = error::spend_overflow;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "spend outside valid range");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__invalid_coinbase_script_size__true_exected_message)
{
    constexpr auto value = error::invalid_coinbase_script_size;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "coinbase script too small or large");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__coinbase_transaction__true_exected_message)
{
    constexpr auto value = error::coinbase_transaction;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "coinbase transaction disallowed in memory pool");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__transaction_internal_double_spend_true_exected_message)
{
    constexpr auto value = error::transaction_internal_double_spend;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "double spend internal to transaction");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__transaction_size_limit__true_exected_message)
{
    constexpr auto value = error::transaction_size_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "transaction size limit exceeded");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__transaction_legacy_sigop_limit__true_exected_message)
{
    constexpr auto value = error::transaction_legacy_sigop_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "too many transaction legacy signature operations");
}

// accept transaction

BOOST_AUTO_TEST_CASE(transaction_error_t__code__unexpected_witness_transaction__true_exected_message)
{
    constexpr auto value = error::unexpected_witness_transaction;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "unexpected witness transaction");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__transaction_non_final__true_exected_message)
{
    constexpr auto value = error::transaction_non_final;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "transaction currently non-final for next block");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__premature_validation__true_exected_message)
{
    constexpr auto value = error::premature_validation;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "transaction validation under checkpoint");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__unspent_duplicate__true_exected_message)
{
    constexpr auto value = error::unspent_duplicate;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "matching transaction with unspent outputs");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__missing_previous_output_true_exected_message)
{
    constexpr auto value = error::missing_previous_output;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "previous output not found");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__double_spend__true_exected_message)
{
    constexpr auto value = error::double_spend;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "double spend of input");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__coinbase_maturity__true_exected_message)
{
    constexpr auto value = error::coinbase_maturity;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "immature coinbase spent");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__spend_exceeds_value__true_exected_message)
{
    constexpr auto value = error::spend_exceeds_value;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "spend exceeds value of inputs");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__transaction_sigop_limit__true_exected_message)
{
    constexpr auto value = error::transaction_sigop_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "too many transaction embedded signature operations");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__relative_time_locked__true_exected_message)
{
    constexpr auto value = error::relative_time_locked;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "transaction currently locked");
}

BOOST_AUTO_TEST_CASE(transaction_error_t__code__transaction_weight_limit__true_exected_message)
{
    constexpr auto value = error::transaction_weight_limit;
    const auto ec = code(value);
    BOOST_REQUIRE(ec);
    BOOST_REQUIRE(ec == value);
    BOOST_REQUIRE_EQUAL(ec.message(), "transaction weight limit exceeded");
}

BOOST_AUTO_TEST_SUITE_END()
