/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/error/error_category.hpp>

#include <string>
#include <unordered_map>
#include <bitcoin/system/error/error_code.hpp>

namespace libbitcoin {
namespace system {
namespace error {

const error_category error_category::singleton;
const char* error_category::category_name = "bitcoin";
static const std::string invalid = "invalid code";

const std::unordered_map<error_t, std::string> messages
{
    // general
    { error::success, "success" },
    { error::unknown, "unknown error" },
    { error::not_found, "object does not exist" },
    { error::not_implemented, "feature not implemented" },
    { error::oversubscribed, "service oversubscribed" },
    { error::file_system, "file system error" },

    // network
    { error::listen_failed, "incoming connection failed" },
    { error::accept_failed, "connection acceptance failed" },
    { error::resolve_failed, "resolving hostname failed" },
    { error::connect_failed, "unable to reach remote host" },
    { error::channel_timeout, "connection timed out" },
    { error::channel_stopped, "channel stopped" },
    { error::service_stopped, "service stopped" },
    { error::operation_canceled, "operation canceled" },
    { error::operation_failed, "operation failed" },
    { error::address_in_use, "address already in use" },
    { error::bad_stream, "bad data stream" },
    { error::address_blocked, "address blocked by policy" },
    { error::peer_throttling, "unresponsive peer may be throttling" },

    // database
    { error::store_block_duplicate, "block duplicate" },
    { error::store_block_invalid_height, "block out of order" },
    { error::store_block_missing_parent, "block missing parent" },
    { error::store_incorrect_state, "incorrect state" },
    { error::store_lock_failure, "lock failure" },

    // blockchain
    { error::duplicate_block, "duplicate block" },
    { error::orphan_block, "missing block parent" },
    { error::invalid_previous_block, "previous block failed to validate" },
    { error::insufficient_work, "insufficient work to reorganize" },
    { error::duplicate_transaction, "duplicate transaction" },
    { error::orphan_transaction, "missing transaction parent" },
    { error::transaction_version, "transaction version not standard" },
    { error::insufficient_fee, "insufficient transaction fee" },
    { error::stale_chain, "blockchain too far behind" },
    { error::dusty_transaction, "output value too low" },

    // check header
    { error::invalid_proof_of_work, "proof of work invalid" },
    { error::futuristic_timestamp, "timestamp too far in the future" },

    // accept header
    { error::checkpoints_failed, "block hash rejected by checkpoint" },
    { error::invalid_block_version, "block version rejected at current height" },
    { error::incorrect_proof_of_work, "proof of work does not match bits field" },
    { error::timestamp_too_early, "block timestamp is too early" },

    // check block
    { error::block_size_limit, "block size limit exceeded" },
    { error::empty_block, "block has no transactions" },
    { error::first_not_coinbase, "first transaction not a coinbase" },
    { error::extra_coinbases, "more than one coinbase" },
    { error::internal_duplicate, "matching transaction hashes in block" },
    { error::block_internal_double_spend, "double spend internal to block" },
    { error::forward_reference, "transactions out of order" },
    { error::merkle_mismatch, "merkle root mismatch" },
    { error::block_legacy_sigop_limit, "too many block legacy signature operations" },

    // accept block
    { error::block_non_final, "block contains a non-final transaction" },
    { error::coinbase_height_mismatch, "block height mismatch in coinbase" },
    { error::coinbase_value_limit, "coinbase value too high" },
    { error::block_embedded_sigop_limit, "too many block embedded signature operations" },
    { error::invalid_witness_commitment, "invalid witness commitment" },
    { error::block_weight_limit, "block weight limit exceeded" },
    { error::temporary_hash_limit, "block contains too many hashes" },
    { error::unspent_coinbase_collision, "unspent coinbase collision" },

    // check transaction
    { error::empty_transaction, "transaction inputs or outputs empty" },
    { error::previous_output_null, "non-coinbase transaction has input with null previous output" },
    { error::spend_overflow, "spend outside valid range" },
    { error::invalid_coinbase_script_size, "coinbase script too small or large" },
    { error::coinbase_transaction, "coinbase transaction disallowed in memory pool" },
    { error::transaction_internal_double_spend, "double spend internal to transaction" },
    { error::transaction_size_limit, "transaction size limit exceeded" },
    { error::transaction_legacy_sigop_limit, "too many transaction legacy signature operations" },

    // accept transaction
    { error::transaction_non_final, "transaction currently non-final for next block" },
    { error::premature_validation, "transaction validation under checkpoint" },
    { error::unspent_duplicate, "matching transaction with unspent outputs" },
    { error::missing_previous_output, "previous output not found" },
    { error::double_spend, "double spend of input" },
    { error::coinbase_maturity, "immature coinbase spent" },
    { error::spend_exceeds_value, "spend exceeds value of inputs" },
    { error::transaction_embedded_sigop_limit, "too many transaction embedded signature operations" },
    { error::sequence_locked, "transaction currently locked" },
    { error::transaction_weight_limit, "transaction weight limit exceeded" },

    // connect input
    { error::invalid_script, "invalid script" },
    { error::invalid_script_size, "invalid script size" },
    { error::invalid_push_data_size, "invalid push data size" },
    { error::invalid_operation_count, "invalid operation count" },
    { error::invalid_stack_size, "invalid stack size" },
    { error::invalid_stack_scope, "invalid stack scope" },
    { error::invalid_script_embed, "invalid script embed" },
    { error::invalid_signature_encoding, "invalid signature encoding" },
    { error::incorrect_signature, "incorrect signature" },
    { error::unexpected_witness, "unexpected witness" },
    { error::invalid_witness, "invalid witness" },
    { error::dirty_witness, "dirty witness" },
    { error::stack_false, "stack false" },

    // op eval
    { error::op_invalid, "op_invalid" },
    { error::op_reserved, "op_reserved" },
    { error::op_push_size, "op_push_size" },
    { error::op_push_data, "op_push_data" },
    { error::op_if, "op_if" },
    { error::op_notif, "op_notif" },
    { error::op_else, "op_else" },
    { error::op_endif, "op_endif" },
    { error::op_verify1, "op_verify1" },
    { error::op_verify2, "op_verify2" },
    { error::op_return, "op_return" },
    { error::op_to_alt_stack, "op_to_alt_stack" },
    { error::op_from_alt_stack, "op_from_alt_stack" },
    { error::op_drop2, "op_drop2" },
    { error::op_dup2, "op_dup2" },
    { error::op_dup3, "op_dup3" },
    { error::op_over2, "op_over2" },
    { error::op_rot2, "op_rot2" },
    { error::op_swap2, "op_swap2" },
    { error::op_if_dup, "op_if_dup" },
    { error::op_drop, "op_drop" },
    { error::op_dup, "op_dup" },
    { error::op_nip, "op_nip" },
    { error::op_over, "op_over" },
    { error::op_pick, "op_pick" },
    { error::op_roll, "op_roll" },
    { error::op_rot, "op_rot" },
    { error::op_swap, "op_swap" },
    { error::op_tuck, "op_tuck" },
    { error::op_size, "op_size" },
    { error::op_equal, "op_equal" },
    { error::op_equal_verify1, "op_equal_verify1" },
    { error::op_equal_verify2, "op_equal_verify2" },
    { error::op_add1, "op_add1" },
    { error::op_sub1, "op_sub1" },
    { error::op_negate, "op_negate" },
    { error::op_abs, "op_abs" },
    { error::op_not, "op_not" },
    { error::op_nonzero, "op_nonzero" },
    { error::op_add, "op_add" },
    { error::op_sub, "op_sub" },
    { error::op_bool_and, "op_bool_and" },
    { error::op_bool_or, "op_bool_or" },
    { error::op_num_equal, "op_num_equal" },
    { error::op_num_equal_verify1, "op_num_equal_verify1" },
    { error::op_num_equal_verify2, "op_num_equal_verify2" },
    { error::op_num_not_equal, "op_num_not_equal" },
    { error::op_less_than, "op_less_than" },
    { error::op_greater_than, "op_greater_than" },
    { error::op_less_than_or_equal, "op_less_than_or_equal" },
    { error::op_greater_than_or_equal, "op_greater_than_or_equal" },
    { error::op_min, "op_min" },
    { error::op_max, "op_max" },
    { error::op_within, "op_within" },
    { error::op_ripemd160, "op_ripemd160" },
    { error::op_sha1, "op_sha1" },
    { error::op_sha256, "op_sha256" },
    { error::op_hash160, "op_hash160" },
    { error::op_hash256, "op_hash256" },
    { error::op_code_seperator, "op_code_seperator" },
    { error::op_check_sig_verify1, "op_check_sig_verify1" },
    { error::op_check_sig, "op_check_sig" },
    { error::op_check_multisig_verify1, "op_check_multisig_verify1" },
    { error::op_check_multisig_verify2, "op_check_multisig_verify2" },
    { error::op_check_multisig_verify3, "op_check_multisig_verify3" },
    { error::op_check_multisig_verify4, "op_check_multisig_verify4" },
    { error::op_check_multisig_verify5, "op_check_multisig_verify5" },
    { error::op_check_multisig_verify6, "op_check_multisig_verify6" },
    { error::op_check_multisig_verify7, "op_check_multisig_verify7" },
    { error::op_check_multisig, "op_check_multisig" },
    { error::op_check_locktime_verify1, "op_check_locktime_verify1" },
    { error::op_check_locktime_verify2, "op_check_locktime_verify2" },
    { error::op_check_locktime_verify3, "op_check_locktime_verify3" },
    { error::op_check_locktime_verify4, "op_check_locktime_verify4" },
    { error::op_check_locktime_verify5, "op_check_locktime_verify5" },
    { error::op_check_locktime_verify6, "op_check_locktime_verify6" },
    { error::op_check_sequence_verify1, "op_check_sequence_verify1" },
    { error::op_check_sequence_verify2, "op_check_sequence_verify2" },
    { error::op_check_sequence_verify3, "op_check_sequence_verify3" },
    { error::op_check_sequence_verify4, "op_check_sequence_verify4" },
    { error::op_check_sequence_verify5, "op_check_sequence_verify5" },
    { error::op_check_sequence_verify6, "op_check_sequence_verify6" },
    { error::op_check_sequence_verify7, "op_check_sequence_verify7" },
    { error::op_check_multisig_verify8, "op_check_multisig_verify8" },

    // http
    { error::http_invalid_request, "invalid request" },
    { error::http_method_not_found, "method not found" },
    { error::http_internal_error, "internal error" }

    ////// server
    ////{ error::unrecognized_filter_type, "Unrecognized filter type" },
    ////{ error::invalid_response_range, "The range of responses is invalid" },
    ////{ error::configuration_disabled, "Disabled by configuration" },
    ////{ error::prevout_missing, "Missing expected cached prevout" }
};

const char* error_category::name() const noexcept
{
    return category_name;
}

std::string error_category::message(int value) const noexcept
{
    const auto message = messages.find(static_cast<error_t>(value));
    return message == messages.end() ? invalid : message->second;
}

std::error_condition error_category::default_error_condition(
    int value) const noexcept
{
    return std::error_condition(value, *this);
}

} // namespace error
} // namespace system
} // namespace libbitcoin