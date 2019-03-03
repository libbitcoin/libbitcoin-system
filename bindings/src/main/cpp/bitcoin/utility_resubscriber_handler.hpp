#ifndef LIBBITCOIN__UTILITY_RESUBSCRIBER_HANDLER_HPP
#define LIBBITCOIN__UTILITY_RESUBSCRIBER_HANDLER_HPP

#include <functional>

namespace libbitcoin {
namespace api {

template <typename... Args>
class utility_resubscriber_handler  {
public:
	std::function<bool (Args...)> getValue() {
        return value;
    }

    void setValue(std::function<bool (Args...)> value) {
        this->value = value;
    }
private:
    std::function<bool (Args...)> value;
};

} // namespace api
} // namespace libbitcoin
#endif
