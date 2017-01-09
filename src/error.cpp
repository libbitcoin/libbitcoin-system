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
#include <bitcoin/bitcoin/error.hpp>

#include <boost/asio.hpp>
#include <boost/system/error_code.hpp>
#include <bitcoin/bitcoin/compat.hpp>

using namespace bc;

class error_category_impl
  : public std::error_category
{
public:
    virtual const char* name() const BC_NOEXCEPT;
    virtual std::string message(int ev) const BC_NOEXCEPT;
    virtual std::error_condition default_error_condition(int ev) 
        const BC_NOEXCEPT;
};

static const error_category_impl& get_error_category_instance()
{
    static error_category_impl instance;
    return instance;
}

const char* error_category_impl::name() const BC_NOEXCEPT
{
    return "bitcoin";
}

std::string error_category_impl::message(int ev) const BC_NOEXCEPT
{
    // TODO: use a static map (hash table)
    // This text mapping may change without notice.
    switch (ev)
    {
        // general codes
        case error::success:
            return "success";
        case error::deprecated:
            return "deprecated";
        case error::unknown:
            return "unknown error";
        case error::not_found:
            return "object does not exist";
        case error::file_system:
            return "file system error";
        case error::non_standard:
            return "transaction not standard";
        case error::not_implemented:
            return "featre not implemented";

        // database
        case error::store_block_invalid_height:
            return "block out of order";
        case error::store_block_missing_parent:
            return "block missing parent";
        case error::store_block_duplicate:
            return "block duplicate";

        // network
        case error::service_stopped:
            return "service stopped";
        case error::operation_failed:
            return "operation failed";
        case error::resolve_failed:
            return "resolving hostname failed";
        case error::network_unreachable:
            return "unable to reach remote host";
        case error::address_in_use:
            return "address already in use";
        case error::listen_failed:
            return "incoming connection failed";
        case error::accept_failed:
            return "connection acceptance failed";
        case error::bad_stream:
            return "bad data stream";
        case error::channel_timeout:
            return "connection timed out";
        case error::address_blocked:
            return "address blocked by policy";
        case error::channel_stopped:
            return "channel stopped";

        // block pool
        case error::duplicate_block:
            return "duplicate block";
        case error::orphan_block:
            return "missing block parent";
        case error::invalid_previous_block:
            return "previous block failed to validate";
        case error::insufficient_work:
            return "insufficient work to reorganize";

        // transaction pool
        case error::orphan_transaction:
            return "missing transaction parent";

        // check header
        case error::invalid_proof_of_work:
            return "proof of work invalid";
        case error::futuristic_timestamp:
            return "timestamp too far in the future";

        // accept header
        case error::checkpoints_failed:
            return "block hash rejected by checkpoint";
        case error::old_version_block:
            return "block version rejected at current height";
        case error::incorrect_proof_of_work:
            return "proof of work does not match bits field";
        case error::timestamp_too_early:
            return "block timestamp is too early";

        // check block
        case error::block_size_limit:
            return "block size limit exceeded";
        case error::empty_block:
            return "block has no transactions";
        case error::first_not_coinbase:
            return "first transaction not a coinbase";
        case error::extra_coinbases:
            return "more than one coinbase";
        case error::internal_duplicate:
            return "matching transaction hashes in block";
        case error::internal_double_spend:
            return "double spend internal to block";
        case error::merkle_mismatch:
            return "merkle root mismatch";
        case error::block_legacy_sigop_limit:
            return "too many block legacy signature operations";

        // accept block
        case error::non_final_transaction:
            return "block contains a non-final transaction";
        case error::coinbase_height_mismatch:
            return "block height mismatch in coinbase";
        case error::coinbase_value_limit:
            return "coinbase value too high";
        case error::block_embedded_sigop_limit:
            return "too many block embedded signature operations";

        // check transaction
        case error::empty_transaction:
            return "transaction inputs or outputs empty";
        case error::previous_output_null:
            return "non-coinbase transaction has input with null previous output";
        case error::spend_overflow:
            return "spend outside valid range";
        case error::invalid_coinbase_script_size:
            return "coinbase script too small or large";
        case error::coinbase_transaction:
            return "coinbase transaction disallowed in memory pool";
        case error::transaction_legacy_sigop_limit:
            return "too many transaction legacy signature operations";

        // accept transaction
        case error::unspent_duplicate:
            return "matching transaction with unspent outputs";
        case error::missing_previous_output:
            return "previous output not found";
        case error::double_spend:
            return "double spend of input";
        case error::coinbase_maturity:
            return "immature coinbase spent";
        case error::spend_exceeds_value:
            return "spend exceeds value of inputs";
        case error::transaction_embedded_sigop_limit:
            return "too many transaction embedded signature operations";

        // connect input
        case error::invalid_script:
            return "invalid script";
        case error::invalid_script_size:
            return "invalid script size";
        case error::invalid_push_data_size:
            return "invalid push data size";
        case error::invalid_operation_count:
            return "invalid operation count";
        case error::invalid_stack_size:
            return "invalid stack size";
        case error::invalid_stack_scope:
            return "invalid stack scope";
        case error::invalid_script_embed:
            return "invalid script embed";
        case error::invalid_signature_encoding:
            return "invalid signature encoding";
        case error::invalid_signature_lax_encoding:
            return "invalid signature lax encoding";
        case error::incorrect_signature:
            return "incorrect signature";
        case error::stack_false:
            return "stack false";

        // op eval
        case error::op_disabled:
            return "op_disabled";
        case error::op_reserved:
            return "op_reserved";
        case error::op_push_size:
            return "op_push_size";
        case error::op_push_data:
            return "op_push_data";
        case error::op_if:
            return "op_if";
        case error::op_notif:
            return "op_notif";
        case error::op_else:
            return "op_else";
        case error::op_endif:
            return "op_endif";
        case error::op_verify1:
            return "op_verify1";
        case error::op_verify2:
            return "op_verify2";
        case error::op_return:
            return "op_return";
        case error::op_to_alt_stack:
            return "op_to_alt_stack";
        case error::op_from_alt_stack:
            return "op_from_alt_stack";
        case error::op_drop2:
            return "op_drop2";
        case error::op_dup2:
            return "op_dup2";
        case error::op_dup3:
            return "op_dup3";
        case error::op_over2:
            return "op_over2";
        case error::op_rot2:
            return "op_rot2";
        case error::op_swap2:
            return "op_swap2";
        case error::op_if_dup:
            return "op_if_dup";
        case error::op_drop:
            return "op_drop";
        case error::op_dup:
            return "op_dup";
        case error::op_nip:
            return "op_nip";
        case error::op_over:
            return "op_over";
        case error::op_pick:
            return "op_pick";
        case error::op_roll:
            return "op_roll";
        case error::op_rot:
            return "op_rot";
        case error::op_swap:
            return "op_swap";
        case error::op_tuck:
            return "op_tuck";
        case error::op_size:
            return "op_size";
        case error::op_equal:
            return "op_equal";
        case error::op_equal_verify1:
            return "op_equal_verify1";
        case error::op_equal_verify2:
            return "op_equal_verify2";
        case error::op_add1:
            return "op_add1";
        case error::op_sub1:
            return "op_sub1";
        case error::op_negate:
            return "op_negate";
        case error::op_abs:
            return "op_abs";
        case error::op_not:
            return "op_not";
        case error::op_nonzero:
            return "op_nonzero";
        case error::op_add:
            return "op_add";
        case error::op_sub:
            return "op_sub";
        case error::op_bool_and:
            return "op_bool_and";
        case error::op_bool_or:
            return "op_bool_or";
        case error::op_num_equal:
            return "op_num_equal";
        case error::op_num_equal_verify1:
            return "op_num_equal_verify1";
        case error::op_num_equal_verify2:
            return "op_num_equal_verify2";
        case error::op_num_not_equal:
            return "op_num_not_equal";
        case error::op_less_than:
            return "op_less_than";
        case error::op_greater_than:
            return "op_greater_than";
        case error::op_less_than_or_equal:
            return "op_less_than_or_equal";
        case error::op_greater_than_or_equal:
            return "op_greater_than_or_equal";
        case error::op_min:
            return "op_min";
        case error::op_max:
            return "op_max";
        case error::op_within:
            return "op_within";
        case error::op_ripemd160:
            return "op_ripemd160";
        case error::op_sha1:
            return "op_sha1";
        case error::op_sha256:
            return "op_sha256";
        case error::op_hash160:
            return "op_hash160";
        case error::op_hash256:
            return "op_hash256";
        case error::op_code_seperator:
            return "op_code_seperator";
        case error::op_check_sig_verify1:
            return "op_check_sig_verify1";
        case error::op_check_sig:
            return "op_check_sig";
        case error::op_check_multisig_verify1:
            return "op_check_multisig_verify1";
        case error::op_check_multisig_verify2:
            return "op_check_multisig_verify2";
        case error::op_check_multisig_verify3:
            return "op_check_multisig_verify3";
        case error::op_check_multisig_verify4:
            return "op_check_multisig_verify4";
        case error::op_check_multisig_verify5:
            return "op_check_multisig_verify5";
        case error::op_check_multisig_verify6:
            return "op_check_multisig_verify6";
        case error::op_check_multisig_verify7:
            return "op_check_multisig_verify7";
        case error::op_check_multisig:
            return "op_check_multisig";
        case error::op_check_locktime_verify1:
            return "op_check_locktime_verify1";
        case error::op_check_locktime_verify2:
            return "op_check_locktime_verify2";
        case error::op_check_locktime_verify3:
            return "op_check_locktime_verify3";
        case error::op_check_locktime_verify4:
            return "op_check_locktime_verify4";
        case error::op_check_locktime_verify5:
            return "op_check_locktime_verify5";
        case error::op_check_locktime_verify6:
            return "op_check_locktime_verify6";

        // invalid codes
        default:
            return "invalid code";
    }
}

// We are not currently using this.
std::error_condition error_category_impl::default_error_condition(int ev)
    const BC_NOEXCEPT
{
    return std::error_condition(ev, *this);
}

namespace libbitcoin {
namespace error {

    code make_error_code(error_code_t e)
    {
        return code(e, get_error_category_instance());
    }

    std::error_condition make_error_condition(error_condition_t e)
    {
        return std::error_condition(e, get_error_category_instance());
    }

    error_code_t boost_to_error_code(const boost_code& ec)
    {
        namespace boost_error = boost::system::errc;

#ifdef _MSC_VER
        // TODO: is there a means to map ASIO errors to boost errors?
        // ASIO codes are unique on Windows but not on Linux.
        namespace asio_error = boost::asio::error;
#endif
        // TODO: use a static map (hash table)
        switch (ec.value())
        {
            // There should be no boost mapping to the shutdown sentinel.
            //    return error::service_stopped;

            case boost_error::success:
                return error::success;

            // network errors
#ifdef _MSC_VER
            case asio_error::connection_aborted:
            case asio_error::connection_reset:
            case asio_error::operation_aborted:
            case asio_error::operation_not_supported:
#endif
            case boost_error::connection_aborted:
            case boost_error::connection_refused:
            case boost_error::connection_reset:
            case boost_error::not_connected:
            case boost_error::operation_canceled:
            case boost_error::operation_not_permitted:
            case boost_error::operation_not_supported:
            case boost_error::owner_dead:
            case boost_error::permission_denied:
                return error::operation_failed;

#ifdef _MSC_VER
            case asio_error::address_family_not_supported:
#endif
            case boost_error::address_family_not_supported:
            case boost_error::address_not_available:
            case boost_error::bad_address:
            case boost_error::destination_address_required:
                return error::resolve_failed;

            case boost_error::broken_pipe:
            case boost_error::host_unreachable:
            case boost_error::network_down:
            case boost_error::network_reset:
            case boost_error::network_unreachable:
            case boost_error::no_link:
            case boost_error::no_protocol_option:
            case boost_error::no_such_file_or_directory:
            case boost_error::not_a_socket:
            case boost_error::protocol_not_supported:
            case boost_error::wrong_protocol_type:
                return error::network_unreachable;

            case boost_error::address_in_use:
            case boost_error::already_connected:
            case boost_error::connection_already_in_progress:
            case boost_error::operation_in_progress:
                return error::address_in_use;

            case boost_error::bad_message:
            case boost_error::illegal_byte_sequence:
            case boost_error::io_error:
            case boost_error::message_size:
            case boost_error::no_message_available:
            case boost_error::no_message:
            case boost_error::no_stream_resources:
            case boost_error::not_a_stream:
            case boost_error::protocol_error:
                return error::bad_stream;

#ifdef _MSC_VER
            case asio_error::timed_out:
#endif
            case boost_error::stream_timeout:
            case boost_error::timed_out:
                return error::channel_timeout;

            // file system errors
            case boost_error::cross_device_link:
            case boost_error::bad_file_descriptor:
            case boost_error::device_or_resource_busy:
            case boost_error::directory_not_empty:
            case boost_error::executable_format_error:
            case boost_error::file_exists:
            case boost_error::file_too_large:
            case boost_error::filename_too_long:
            case boost_error::invalid_seek:
            case boost_error::is_a_directory:
            case boost_error::no_space_on_device:
            case boost_error::no_such_device:
            case boost_error::no_such_device_or_address:
            case boost_error::read_only_file_system:
            // same as operation_would_block on non-windows
            //case boost_error::resource_unavailable_try_again:
            case boost_error::text_file_busy:
            case boost_error::too_many_files_open:
            case boost_error::too_many_files_open_in_system:
            case boost_error::too_many_links:
            case boost_error::too_many_symbolic_link_levels:
                return error::file_system;

            // unknown errors
            case boost_error::argument_list_too_long:
            case boost_error::argument_out_of_domain:
            case boost_error::function_not_supported:
            case boost_error::identifier_removed:
            case boost_error::inappropriate_io_control_operation:
            case boost_error::interrupted:
            case boost_error::invalid_argument:
            case boost_error::no_buffer_space:
            case boost_error::no_child_process:
            case boost_error::no_lock_available:
            case boost_error::no_such_process:
            case boost_error::not_a_directory:
            case boost_error::not_enough_memory:
            case boost_error::operation_would_block:
            case boost_error::resource_deadlock_would_occur:
            case boost_error::result_out_of_range:
            case boost_error::state_not_recoverable:
            case boost_error::value_too_large:
            default:
                return error::unknown;
        }
    }

} // namespace error
} // namespace libbitcoin
