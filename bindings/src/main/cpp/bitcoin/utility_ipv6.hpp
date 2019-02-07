#ifndef LIBBITCOIN__UTILITY_IPV6_HPP
#define LIBBITCOIN__UTILITY_IPV6_HPP

#include <boost/asio.hpp>

namespace libbitcoin {
namespace api {

class utility_ipv6 {
public:
	boost::asio::ip::address_v6 getValue() {
		return value;
	}

	void setValue(boost::asio::ip::address_v6 value) {
		this->value = value;
	}
private:
	boost::asio::ip::address_v6 value;
};

} // namespace api
} // namespace libbitcoin
#endif
