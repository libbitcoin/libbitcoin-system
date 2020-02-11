#ifndef LIBBITCOIN__UTILITY_ONE_BYTE_HPP
#define LIBBITCOIN__UTILITY_ONE_BYTE_HPP

#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {
namespace api {

class utility_one_byte {
public:
	one_byte* getValue() {
		return value_;
	}

	void setValue(one_byte* value) {
		value_ = value;
	}
private:
	one_byte* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
