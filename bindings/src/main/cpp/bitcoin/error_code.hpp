#ifndef LIBBITCOIN__ERROR_CODE_HPP
#define LIBBITCOIN__ERROR_CODE_HPP

#include <cstddef>
#include <iostream>
#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
namespace api {

class error_code {

public:
  error_code(code code) {
    this->code_ = code;
  }

  code cast() {
    return code_;
  }

  ~error_code() {
	  delete code_;
  }

private:
  code code_;
};

} // namespace api
} // namespace libbitcoin
#endif
