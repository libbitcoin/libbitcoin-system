#ifndef LIBBITCOIN__MATH_POINT_LIST_HPP
#define LIBBITCOIN__MATH_POINT_LIST_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_point_list {
public:
	point_list getValue() {
		return value;
	}

	void setValue(point_list value) {
		this->value = value;
	}
private:
	point_list value;
};

} // namespace api
} // namespace libbitcoin
#endif
