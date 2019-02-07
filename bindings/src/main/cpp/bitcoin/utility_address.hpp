#ifndef LIBBITCOIN__UTILITY_ADDRESS_HPP
#define LIBBITCOIN__UTILITY_ADDRESS_HPP

#include <boost/asio/ip/address.hpp>

namespace libbitcoin {
namespace api {

class utility_address {
public:
	boost::asio::ip::address getValue() {
		return value;
	}

	void setValue(boost::asio::ip::address value) {
		this->value = value;
	}
private:
	boost::asio::ip::address value;
};

} // namespace api
} // namespace libbitcoin
#endif
