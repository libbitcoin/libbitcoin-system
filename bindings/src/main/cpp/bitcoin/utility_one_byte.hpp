#ifndef LIBBITCOIN__UTILITY_ONE_BYTE_HPP
#define LIBBITCOIN__UTILITY_ONE_BYTE_HPP

#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace api {

class utility_one_byte {
public:
	one_byte getValue() {
		return value;
	}

	void setValue(one_byte value) {
		this->value = value;
	}
private:
	one_byte value;
};

} // namespace api
} // namespace libbitcoin
#endif
