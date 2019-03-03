#ifndef LIBBITCOIN__UTILITY_SHARED_MUTEX_HPP
#define LIBBITCOIN__UTILITY_SHARED_MUTEX_HPP

#include <bitcoin/bitcoin/utility/thread.hpp>

namespace libbitcoin {
namespace api {

class utility_shared_mutex {
	libbitcoin::shared_mutex getValue() {
		return value;
	}

	void setValue(libbitcoin::shared_mutex value) {
		this->value = value;
	}
private:
	libbitcoin::shared_mutex value;
};

} // namespace api
} // namespace libbitcoin
#endif
