#ifndef LIBBITCOIN__MATH_SECRET_LIST_HPP
#define LIBBITCOIN__MATH_SECRET_LIST_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_secret_list {
public:
	secret_list* getValue() {
        return value_;
    }

    void setValue(secret_list* value) {
        value_ = value;
    }
private:
    secret_list* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
