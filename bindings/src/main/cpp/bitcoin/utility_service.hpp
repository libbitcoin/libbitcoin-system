#ifndef LIBBITCOIN__UTILITY_SERVICE_HPP
#define LIBBITCOIN__UTILITY_SERVICE_HPP

#include <boost/asio/io_service.hpp>

namespace libbitcoin {
namespace api {

class utility_service {
	boost::asio::io_service getValue() {
       return value;
   }

   void setValue(boost::asio::io_service value) {
       this->value = value;
   }
private:
   boost::asio::io_service value;
};

} // namespace api
} // namespace libbitcoin
#endif
