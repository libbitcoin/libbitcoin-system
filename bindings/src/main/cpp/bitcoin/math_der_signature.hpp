#ifndef LIBBITCOIN__MATH_DER_SIGNATURE_HPP
#define LIBBITCOIN__MATH_DER_SIGNATURE_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_der_signature {
public:
	der_signature* getValue() {
		return value_;
	}

	void setValue(der_signature* value) {
		value_ = value;
	}
private:
	der_signature* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
