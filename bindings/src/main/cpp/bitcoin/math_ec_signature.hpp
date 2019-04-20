#ifndef LIBBITCOIN__MATH_EC_SIGNATURE_HPP
#define LIBBITCOIN__MATH_EC_SIGNATURE_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_ec_signature {
public:
	ec_signature* getValue() {
		return value_;
	}

	void setValue(ec_signature* value) {
		value_ = value;
	}
private:
	ec_signature* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
