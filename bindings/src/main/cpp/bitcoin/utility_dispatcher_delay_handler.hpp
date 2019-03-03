#ifndef LIBBITCOIN__UTILITY_DISPATCHER_DELAY_HANDLER_HPP
#define LIBBITCOIN__UTILITY_DISPATCHER_DELAY_HANDLER_HPP

#include <functional>
#include <bitcoin/bitcoin/error.hpp>

namespace libbitcoin {
namespace api {

class utility_dispatcher_delay_handler {
	std::function<void(const code&)> getValue() {
       return value;
   }

   void setValue(std::function<void(const code&)> value) {
       this->value = value;
   }
private:
   std::function<void(const code&)> value;
};

} // namespace api
} // namespace libbitcoin
#endif
