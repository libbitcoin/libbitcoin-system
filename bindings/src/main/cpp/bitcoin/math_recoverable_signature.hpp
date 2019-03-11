#ifndef LIBBITCOIN__MATH_RECOVERABLE_SIGNATURE_HPP
#define LIBBITCOIN__MATH_RECOVERABLE_SIGNATURE_HPP

#include <bitcoin/bitcoin/math/elliptic_curve.hpp>

namespace libbitcoin {
namespace api {

class math_recoverable_signature {
public:
	recoverable_signature getValue() {
        return value;
    }

    void setValue(recoverable_signature value) {
        this->value = value;
    }
private:
    recoverable_signature value;
};

} // namespace api
} // namespace libbitcoin
#endif
