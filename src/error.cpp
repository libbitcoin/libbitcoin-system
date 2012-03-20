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
        case error::previous_block_invalid:
            return "Previous block failed to validate";
        case error::check_block:
            return "check_block() validation failed";
        case error::accept_block:
            return "accept_block() validation failed";
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
        case error::check_block:
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

