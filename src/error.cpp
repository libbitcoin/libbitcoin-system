#include <bitcoin/error.hpp>

namespace libbitcoin {

const char* error_category_impl::name() const
{
    return "bitcoin";
}

std::string error_category_impl::message(int ev) const
{
    error ec = static_cast<error>(ev);
    switch (ec)
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
        case error::accept_failed:
            return "Accept connection failed";
        case error::bad_stream:
            return "Bad stream";
        case error::channel_stopped:
            return "Channel stopped";
        default:
            return "Unknown error";
    }
}

const std::error_category& error_category()
{
    static error_category_impl instance;
    return instance;
}

std::error_code make_error_code(error e)
{
    return std::error_code(static_cast<int>(e), error_category());
}

std::error_condition make_error_condition(error e)
{
    return std::error_condition(static_cast<int>(e), error_category());
}

} // libbitcoin

