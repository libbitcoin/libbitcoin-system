#ifndef LIBBITCOIN__UTILITY_SUBSCRIBER_HANDLER_HPP
#define LIBBITCOIN__UTILITY_SUBSCRIBER_HANDLER_HPP

#include <functional>

namespace libbitcoin {
namespace api {

template<typename ... Args>
class utility_subscriber_handler {
public:
	std::function<void(Args...)>* getValue() {
		return value_;
	}

	void setValue(std::function<void(Args...)>* value) {
		value_ = value;
	}
private:
	std::function<void(Args...)>* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
