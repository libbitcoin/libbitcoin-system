#include <bitcoin/storage/errors.hpp>

namespace libbitcoin {

const char* storage_category_impl::name() const
{
    return "storage";
}

std::string storage_category_impl::message(int ev) const
{
    storage_error ec = static_cast<storage_error>(ev);
    switch (ec)
    {
    case storage_error::block_doesnt_exist:
        return "Block does not exist";
    case storage_error::output_doesnt_exist:
        return "Ouput doesnt exist";
    default:
        return "Unknown storage error";
    }
}

const std::error_category& storage_category()
{
    static storage_category_impl instance;
    return instance;
}

std::error_code make_error_code(storage_error e)
{
    return std::error_code(static_cast<int>(e), storage_category());
}

std::error_condition make_error_condition(storage_error e)
{
    return std::error_condition(static_cast<int>(e), storage_category());
}

} // libbitcoin

