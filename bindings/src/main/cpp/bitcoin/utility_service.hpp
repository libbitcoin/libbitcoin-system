#ifndef LIBBITCOIN__UTILITY_SERVICE_HPP
#define LIBBITCOIN__UTILITY_SERVICE_HPP

#include <boost/asio/io_service.hpp>

namespace libbitcoin {
namespace api {

class utility_service {
public:
	boost::asio::io_service* getValue() {
       return value_;
   }

   void setValue(boost::asio::io_service* value) {
       value_ = value;
   }
private:
   boost::asio::io_service* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
