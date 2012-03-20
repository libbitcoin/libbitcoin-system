#include <bitcoin/error.hpp>

namespace libbitcoin {

namespace error
{
    std::error_code make_error_code(error_code_t e)
    {
        return std::error_code(static_cast<int>(e), error_category());
    }

    std::error_condition make_error_condition(error_condition_t e)
    {
        return std::error_condition(static_cast<int>(e), error_category());
    }
}

const char* error_category_impl::name() const
{
    return "bitcoin";
}

std::string error_category_impl::message(int ev) const
{
    switch (ev)
    {
        case error::missing_object:
            return "Object does not exist";
        case error::object_already_exists:
            return "Matching previous object found";
        case error::unspent_output:
            return "Unspent output";
        case error::bad_transaction:
            return "Transaction failed to validate";
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
        case error::channel_stopped:
            return "Channel stopped";
        case error::channel_timeout:
            return "Channel timed out";
        // validate
        case error::previous_block_invalid:
            return "Previous block failed to validate";
        // check_transaction()
        case error::empty_transaction:
            return "Transaction inputs or outputs are empty";
        case error::output_value_overflow:
            return "Overflow in output value outside range";
        case error::invalid_coinbase_script_size:
            return "Coinbase script is too small or large";
        case error::previous_output_null:
            return "Non-coinbase transaction has null previous in an input";
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
        case error::accept_block:
            return "accept_block() validation failed";
        // connect_block()
        case error::connect_block:
            return "connect_block() validation failed";
        default:
            return "Unknown error";
    }
}

std::error_condition
    error_category_impl::default_error_condition(int ev) const
{
    switch (ev)
    {
        case error::previous_block_invalid:
        case error::empty_transaction:
        case error::output_value_overflow:
        case error::invalid_coinbase_script_size:
        case error::previous_output_null:
        case error::size_limits:
        case error::proof_of_work:
        case error::futuristic_timestamp:
        case error::first_not_coinbase:
        case error::extra_coinbases:
        case error::too_many_sigs:
        case error::merkle_mismatch:
        case error::accept_block:
        case error::connect_block:
            return error::validate_failed;
        default:
            return std::error_condition(ev, *this);
    }
}

const std::error_category& error_category()
{
    static error_category_impl instance;
    return instance;
}

} // libbitcoin

