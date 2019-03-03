#ifndef LIBBITCOIN__UTILITY_SUBSCRIBER_HANDLER_HPP
#define LIBBITCOIN__UTILITY_SUBSCRIBER_HANDLER_HPP

#include <functional>

namespace libbitcoin {
namespace api {

template<typename ... Args>
class utility_subscriber_handler {
	std::function<void(Args...)> getValue() {
		return value;
	}

	void setValue(std::function<void(Args...)> value) {
		this->value = value;
	}
private:
	std::function<void(Args...)> value;
};

} // namespace api
} // namespace libbitcoin
#endif
