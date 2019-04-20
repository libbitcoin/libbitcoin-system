#ifndef LIBBITCOIN__MATH_EC_COMPRESSED_HPP
#define LIBBITCOIN__MATH_EC_COMPRESSED_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_ec_compressed {
public:
	ec_compressed* getValue() {
		return value_;
	}

	void setValue(ec_compressed* value) {
		value_ = value;
	}
private:
	ec_compressed* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
