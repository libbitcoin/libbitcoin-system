#ifndef LIBBITCOIN__UTILITY_MUTEX_PTR_HPP
#define LIBBITCOIN__UTILITY_MUTEX_PTR_HPP

#include <bitcoin/bitcoin/utility/conditional_lock.hpp>

namespace libbitcoin {
namespace api {

class utility_mutex_ptr {
public:
	std::shared_ptr<shared_mutex> getValue() {
        return value;
    }

    void setValue(std::shared_ptr<shared_mutex> value) {
        this->value = value;
    }
private:
    std::shared_ptr<shared_mutex> value;
};

} // namespace api
} // namespace libbitcoin
#endif
