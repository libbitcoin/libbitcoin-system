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
#ifndef LIBBITCOIN_SYSTEM_ERROR_SCRIPT_ERROR_T_HPP
#define LIBBITCOIN_SYSTEM_ERROR_SCRIPT_ERROR_T_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/error/macros.hpp>
#include <bitcoin/system/error/transaction_error_t.hpp>

 // Transaction sequences are limited to 0xff for single byte store encoding.
 // With the uint8_t domain specified the compiler enforces this guard.

namespace libbitcoin {
namespace system {
namespace error {

enum script_error_t : uint8_t
{
    script_success = 0,
    inputs_overflow = add1<int>(transaction_error_t::tx_error_last),
    prefail_script,
    prevalid_script,
    invalid_script,
    invalid_script_size,
    invalid_push_data_size,
    invalid_operation_count,
    invalid_stack_size,
    invalid_stack_scope,
    invalid_script_embed,
    unversioned_script,
    unexpected_witness,
    invalid_witness,
    invalid_witness_stack,
    dirty_witness,
    stack_false,

    // chained to op_error_t
    script_error_last
};

DECLARE_ERROR_T_CODE_CATEGORY(script_error);

} // namespace error
} // namespace system
} // namespace libbitcoin

DECLARE_STD_ERROR_REGISTRATION(bc::system::error::script_error)

#endif
