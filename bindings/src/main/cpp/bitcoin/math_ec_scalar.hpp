#ifndef LIBBITCOIN__MATH_EC_SCALAR_HPP
#define LIBBITCOIN__MATH_EC_SCALAR_HPP

#include <bitcoin/bitcoin/math/ec_scalar.hpp>

namespace libbitcoin {
namespace api {

class math_ec_scalar {
public:
	ec_scalar getValue() {
        return value;
    }

    void setValue(ec_scalar value) {
        this->value = value;
    }
private:
    ec_scalar value;
};

} // namespace api
} // namespace libbitcoin
#endif
