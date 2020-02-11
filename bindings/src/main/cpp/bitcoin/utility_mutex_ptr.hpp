#ifndef LIBBITCOIN__UTILITY_MUTEX_PTR_HPP
#define LIBBITCOIN__UTILITY_MUTEX_PTR_HPP

#include <bitcoin/bitcoin/utility/conditional_lock.hpp>

namespace libbitcoin {
namespace api {

class utility_mutex_ptr {
public:
	std::shared_ptr<shared_mutex>* getValue() {
        return value_;
    }

    void setValue(std::shared_ptr<shared_mutex>* value) {
        value_ = value;
    }
private:
    std::shared_ptr<shared_mutex>* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
