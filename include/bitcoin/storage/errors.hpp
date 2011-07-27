#ifndef LIBBITCOIN_STORAGE_ERRORS_H
#define LIBBITCOIN_STORAGE_ERRORS_H

#include <system_error>

namespace libbitcoin {

enum class storage_error
{
    block_doesnt_exist = 1,
    output_doesnt_exist
};

class storage_category_impl
  : public std::error_category
{
public:
    virtual const char* name() const;
    virtual std::string message(int ev) const;
};

const std::error_category& storage_category();

std::error_code make_error_code(storage_error e);
std::error_condition make_error_condition(storage_error e);

} // libbitcoin

namespace std
{
    template <>
    struct is_error_code_enum<libbitcoin::storage_error>
      : public true_type {};
}

#endif

