#ifndef LIBBITCOIN__P_STD_STRING_HPP
#define LIBBITCOIN__P_STD_STRING_HPP

#include <string>

namespace libbitcoin {
namespace api {

class p_std_string {
public:
	p_std_string() : value_(new std::string){
  }

  void set(std::string value) {
      value_ = value;
  }

  const std::string cast() const {
    return value_;
  }

  std::string get() {
    return value_;
  }

  ~p_std_string() {
	delete value_;
  }

private:
  std::string value_;
};

} // namespace api
} // namespace libbitcoin
#endif
