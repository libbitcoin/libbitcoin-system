/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_ERROR_HPP
#define LIBBITCOIN_ERROR_HPP

#include <string>
#include <system_error>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {

/// Console result codes, positive values are domain-specific.
enum console_result : int
{
    failure = -1,
    okay = 0,
    invalid = 1
};

/// Alias for error code declarations.
typedef std::error_code code;

/// Alias for boost error code declarations.
typedef boost::system::error_code boost_code;

namespace error {

// The numeric values of these codes may change without notice.
enum error_code_t
{
    // general codes
    success = 0,
    deprecated = 6,
    unknown = 43,
    not_found = 3,
    file_system = 42,
    non_standard = 17,

    // network
    service_stopped = 1,
    operation_failed = 2,
    resolve_failed = 7,
    network_unreachable = 8,
    address_in_use = 9,
    listen_failed = 10,
    accept_failed = 11,
    bad_stream = 12,
    channel_timeout = 13,
    address_blocked = 44,
    channel_stopped = 45,

    // block pool
    duplicate_block = 51,
    orphan_block = 5,
    invalid_previous_block = 24,
    //// TODO: block_pool_filled,

    // transaction pool
    blockchain_reorganized = 14,
    transaction_pool_filled = 15,
    duplicate_pool_transaction = 4,

    // check header
    invalid_proof_of_work = 26,
    futuristic_timestamp = 27,

    // accept header
    checkpoints_failed = 35,
    old_version_block = 36,
    incorrect_proof_of_work = 32,
    timestamp_too_early = 33,

    // check block
    block_size_limit = 50,
    empty_block = 47,
    first_not_coinbase = 28,
    extra_coinbases = 29,
    internal_duplicate = 49,
    merkle_mismatch = 31,
    insufficient_work = 48,
    too_many_sigops = 30,

    // accept block
    non_final_transaction = 34,
    coinbase_height_mismatch = 37,
    coinbase_too_large = 41,
    ////too_many_sigops

    // check transaction
    empty_transaction = 20,
    previous_output_null = 23,
    spend_overflow = 21,
    invalid_coinbase_script_size = 22,
    coinbase_transaction = 16,
    ////block_size_limit
    ////too_many_sigops

    // accept transaction
    unspent_duplicate = 38,
    missing_input = 19,
    double_spend = 18,
    coinbase_maturity = 46,
    spend_exceeds_value = 40,
    ////too_many_sigops

    // connect input
    validate_inputs_failed = 39
};

enum error_condition_t
{
    //// validate
    //validate_failed = 1,
    //forced_removal
};

BC_API code make_error_code(error_code_t e);
BC_API std::error_condition make_error_condition(error_condition_t e);
BC_API error_code_t boost_to_error_code(const boost_code& ec);

} // namespace error
} // namespace libbitcoin

namespace std {

template <>
struct is_error_code_enum<bc::error::error_code_t>
  : public true_type
{
};

template <>
struct is_error_condition_enum<bc::error::error_condition_t>
  : public true_type
{
};

} // namespace std

#endif
