/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/error/transaction_error_t.hpp>

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

namespace libbitcoin {
namespace system {
namespace error {

DEFINE_ERROR_T_MESSAGE_MAP(transaction_error)
{
    { transaction_success, "transaction success" },

    // check transaction
    { empty_transaction, "transaction inputs or outputs empty" },
    { previous_output_null, "non-coinbase transaction has input with null previous output" },
    { spend_overflow, "spend outside valid range" },
    { invalid_coinbase_script_size, "coinbase script too small or large" },
    { coinbase_transaction, "coinbase transaction disallowed in memory pool" },
    { transaction_internal_double_spend, "double spend internal to transaction" },
    { transaction_size_limit, "transaction size limit exceeded" },
    { transaction_legacy_sigop_limit, "too many transaction legacy signature operations" },

    // accept transaction
    { unexpected_witness_transaction, "unexpected witness transaction" },
    { transaction_non_final, "transaction currently non-final" },
    { premature_validation, "transaction validation under checkpoint" },
    { unspent_duplicate, "matching transaction with unspent outputs" },
    { missing_previous_output, "previous output not found" },
    { double_spend, "double spend of input" },
    { coinbase_maturity, "immature coinbase spent" },
    { spend_exceeds_value, "spend exceeds value of inputs" },
    { transaction_sigop_limit, "too many transaction embedded signature operations" },
    { relative_time_locked, "transaction currently locked" },
    { transaction_weight_limit, "transaction weight limit exceeded" },

    // dconfirm transaction
    { unconfirmed_spend, "spend of unconfirmed previous output" },
    { confirmed_double_spend, "spend of confirmed spent previous output" }
};

DEFINE_ERROR_T_CATEGORY(transaction_error, "transaction", "transaction code")

} // namespace error
} // namespace system
} // namespace libbitcoin
