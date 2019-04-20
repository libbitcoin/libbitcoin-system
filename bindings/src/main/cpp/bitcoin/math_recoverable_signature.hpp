#ifndef LIBBITCOIN__MATH_RECOVERABLE_SIGNATURE_HPP
#define LIBBITCOIN__MATH_RECOVERABLE_SIGNATURE_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_recoverable_signature {
public:
	recoverable_signature* getValue() {
        return value_;
    }

    void setValue(recoverable_signature* value) {
        value_ = value;
    }
private:
    recoverable_signature* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
