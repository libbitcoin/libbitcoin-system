#ifndef LIBBITCOIN__MATH_EC_POINT_HPP
#define LIBBITCOIN__MATH_EC_POINT_HPP

#include <bitcoin/bitcoin/math/ec_point.hpp>

namespace libbitcoin {
namespace api {

class math_ec_point {
public:
	ec_point* getValue() {
        return value_;
    }

    void setValue(ec_point* value) {
        value_ = value;
    }
private:
    ec_point* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
