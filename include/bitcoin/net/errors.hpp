#ifndef LIBBITCOIN_NETWORK_ERRORS_H
#define LIBBITCOIN_NETWORK_ERRORS_H

#include <system_error>

namespace libbitcoin {
namespace net {

enum class network_error
{
    system_network_error = 1
};

class network_category_impl
  : public std::error_category
{
public:
    virtual const char* name() const;
    virtual std::string message(int ev) const;
};

const std::error_category& network_category();

std::error_code make_error_code(network_error e);
std::error_condition make_error_condition(network_error e);

} // net
} // libbitcoin

namespace std
{
    template <>
    struct is_error_code_enum<libbitcoin::net::network_error>
      : public true_type {};
}

#endif

