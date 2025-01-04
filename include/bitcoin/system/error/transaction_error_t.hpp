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
#ifndef LIBBITCOIN_SYSTEM_ERROR_TRANSACTION_ERROR_T_HPP
#define LIBBITCOIN_SYSTEM_ERROR_TRANSACTION_ERROR_T_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>

// Transaction sequences are limited to 0xff for single byte store encoding.
// With the uint8_t domain specified the compiler enforces this guard.

namespace libbitcoin {
namespace system {
namespace error {

enum transaction_error_t
{
    transaction_success = 0,

    // TODO: order these.

    // check transaction
    empty_transaction,
    previous_output_null,
    spend_overflow,
    invalid_coinbase_script_size,
    coinbase_transaction,
    transaction_internal_double_spend,
    transaction_size_limit,
    transaction_legacy_sigop_limit,

    // accept transaction
    unexpected_witness_transaction,
    transaction_non_final,
    premature_validation,
    unspent_duplicate,
    missing_previous_output,
    double_spend,
    coinbase_maturity,
    spend_exceeds_value,
    transaction_sigop_limit,
    relative_time_locked,
    transaction_weight_limit,

    // confirm transaction
    unconfirmed_spend,
    confirmed_double_spend,

    // chained to script_error_t
    tx_error_last
};

DECLARE_ERROR_T_CODE_CATEGORY(transaction_error);

} // namespace error
} // namespace system
} // namespace libbitcoin

DECLARE_STD_ERROR_REGISTRATION(bc::system::error::transaction_error)

#endif
