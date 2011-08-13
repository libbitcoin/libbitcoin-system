#include <bitcoin/network/errors.hpp>

namespace libbitcoin {
namespace net {

const char* network_category_impl::name() const
{
    return "storage";
}

std::string network_category_impl::message(int ev) const
{
    error ec = static_cast<network_error>(ev);
    switch (ec)
    {
    case error::system_network_error:
        return "System network error";
    default:
        return "Unknown storage error";
    }
}

const std::error_category& network_category()
{
    static network_category_impl instance;
    return instance;
}

std::error_code make_error_code(error e)
{
    return std::error_code(static_cast<int>(e), network_category());
}

std::error_condition make_error_condition(error e)
{
    return std::error_condition(static_cast<int>(e), network_category());
}

} // net
} // libbitcoin

