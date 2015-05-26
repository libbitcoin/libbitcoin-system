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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/bitcoin/error.hpp>

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
    switch (ev)
    {
        case error::service_stopped:
            return "Service stopped";
        case error::operation_failed:
            return "Operation failed";
        case error::not_found:
            return "Object does not exist";
        case error::duplicate:
            return "Matching previous object found";
        case error::unspent_output:
            return "Unspent output";
        case error::unsupported_payment_type:
            return "Unsupport payment type";
        case error::resolve_failed:
            return "Resolving hostname failed";
        case error::network_unreachable:
            return "Unable to reach remote network";
        case error::address_in_use:
            return "Address already in use";
        case error::listen_failed:
            return "Listen incoming connections failed";
        case error::accept_failed:
            return "Accept connection failed";
        case error::bad_stream:
            return "Bad stream";
        case error::channel_timeout:
            return "Channel timed out";
        // transaction pool
        case error::blockchain_reorganized:
            return "Transactions invalidated from blockchain reorganization";
        case error::pool_filled:
            return "Forced removal of old transaction from full pool";
        // validate tx
        case error::coinbase_transaction:
            return "Memory pool coinbase transaction";
        case error::is_not_standard:
            return "Transaction is not standard";
        case error::double_spend:
            return "Double spend of input";
        case error::input_not_found:
            return "Spent input not found";
        // check_transaction()
        case error::empty_transaction:
            return "Transaction inputs or outputs are empty";
        case error::output_value_overflow:
            return "Overflow in output value outside range";
        case error::invalid_coinbase_script_size:
            return "Coinbase script is too small or large";
        case error::previous_output_null:
            return "Non-coinbase transaction has null previous in an input";
        // validate block
        case error::previous_block_invalid:
            return "Previous block failed to validate";
        // check_block()
        case error::size_limits:
            return "Size limits failed";
        case error::proof_of_work:
            return "Proof of work failed";
        case error::futuristic_timestamp:
            return "Timestamp too far in the future";
        case error::first_not_coinbase:
            return "First transaction is not a coinbase";
        case error::extra_coinbases:
            return "More than one coinbase";
        case error::too_many_sigs:
            return "Too many script *SIG operations";
        case error::merkle_mismatch:
            return "Merkle root mismatch";
        // accept_block()
        case error::incorrect_proof_of_work:
            return "Proof of work does not match bits field";
        case error::timestamp_too_early:
            return "Block timestamp is too early";
        case error::non_final_transaction:
            return "Contains a non-final transaction";
        case error::checkpoints_failed:
            return "Block hash rejected by checkpoint lockins";
        case error::old_version_block:
            return "Reject version=1 block";
        case error::coinbase_height_mismatch:
            return "Block height mismatch in coinbase";
        // connect_block()
        case error::duplicate_or_spent:
            return "Duplicate transaction when with unspent outputs";
        case error::validate_inputs_failed:
            return "Validation of inputs failed";
        case error::fees_out_of_range:
            return "Fees are out of range";
        case error::coinbase_too_large:
            return "Reported coinbase value is too large";
        default:
            return "Unknown error";
    }
}

std::error_condition error_category_impl::default_error_condition(int ev)
    const BC_NOEXCEPT
{
    switch (ev)
    {
        // validate tx
        case error::coinbase_transaction:
        case error::is_not_standard:
        case error::double_spend:
        case error::input_not_found:
        // check_transaction()
        case error::empty_transaction:
        case error::output_value_overflow:
        case error::invalid_coinbase_script_size:
        case error::previous_output_null:
        // validate block
        case error::previous_block_invalid:
        // check_block()
        case error::size_limits:
        case error::proof_of_work:
        case error::futuristic_timestamp:
        case error::first_not_coinbase:
        case error::extra_coinbases:
        case error::too_many_sigs:
        case error::merkle_mismatch:
        // accept_block()
        case error::incorrect_proof_of_work:
        case error::timestamp_too_early:
        case error::non_final_transaction:
        case error::checkpoints_failed:
        case error::old_version_block:
        case error::coinbase_height_mismatch:
        // connect_block()
        case error::duplicate_or_spent:
        case error::validate_inputs_failed:
        case error::fees_out_of_range:
        case error::coinbase_too_large:
            return error::validate_failed;
        case error::blockchain_reorganized:
        case error::pool_filled:
            return error::forced_removal;
        default:
            return std::error_condition(ev, *this);
    }
}

namespace libbitcoin {
namespace error {

    std::error_code make_error_code(error_code_t e)
    {
        return std::error_code(e, get_error_category_instance());
    }

    std::error_condition make_error_condition(error_condition_t e)
    {
        return std::error_condition(e, get_error_category_instance());
    }

} // namespace error
} // namespace libbitcoin
