#ifndef LIBBITCOIN__MATH_EC_SECRET_HPP
#define LIBBITCOIN__MATH_EC_SECRET_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_ec_secret {
public:
	ec_secret getValue() {
		return value;
	}

	void setValue(ec_secret value) {
		this->value = value;
	}
private:
	ec_secret value;
};

} // namespace api
} // namespace libbitcoin
#endif
