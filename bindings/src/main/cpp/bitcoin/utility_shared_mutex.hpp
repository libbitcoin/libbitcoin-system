#ifndef LIBBITCOIN__UTILITY_SHARED_MUTEX_HPP
#define LIBBITCOIN__UTILITY_SHARED_MUTEX_HPP

#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {
namespace api {

class utility_shared_mutex {
public:
	libbitcoin::shared_mutex* getValue() {
		return value_;
	}

	void setValue(libbitcoin::shared_mutex* value) {
		value_ = value;
	}
private:
	libbitcoin::shared_mutex* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
