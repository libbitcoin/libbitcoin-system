#ifndef LIBBITCOIN__MESSAGE_IP_ADDRESS_HPP
#define LIBBITCOIN__MESSAGE_IP_ADDRESS_HPP

//#include <cstddef>
//#include <iostream>
#include <bitcoin/bitcoin/message/network_address.hpp>

namespace libbitcoin {
//namespace message {
namespace api {

class message_ip_address {

public:
	message_ip_address(message::ip_address *value) :
			value_(value) {
	}

	message_ip_address() :
			value_(new message::ip_address) {
	}

	~message_ip_address() {
		delete value_;
	}

	message::ip_address* getValue() {
		return value_;
	}

	void setValue(message::ip_address *value) {
		value_ = value;
	}
private:
	message::ip_address *value_;
};

} // namespace api
//} // namespace message
} // namespace libbitcoin
#endif
