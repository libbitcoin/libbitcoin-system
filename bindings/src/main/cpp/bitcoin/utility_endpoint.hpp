#ifndef LIBBITCOIN__UTILITY_ENDPOINT_HPP
#define LIBBITCOIN__UTILITY_ENDPOINT_HPP

#include <boost/asio/ip/tcp.hpp>

namespace libbitcoin {
namespace api {

class utility_endpoint {
public:
	boost::asio::ip::tcp::endpoint getValue() {
		return value;
	}

	void setValue(boost::asio::ip::tcp::endpoint value) {
		this->value = value;
	}
private:
	boost::asio::ip::tcp::endpoint value;
};

} // namespace api
} // namespace libbitcoin
#endif
