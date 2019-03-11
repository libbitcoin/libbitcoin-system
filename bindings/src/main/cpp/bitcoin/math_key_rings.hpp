#ifndef LIBBITCOIN__MATH_KEY_RINGS_HPP
#define LIBBITCOIN__MATH_KEY_RINGS_HPP

#include <bitcoin/bitcoin/math/ring_signature.hpp>

namespace libbitcoin {
namespace api {

class math_key_rings {
public:
	key_rings getValue() {
        return value;
    }

    void setValue(key_rings value) {
        this->value = value;
    }
private:
    key_rings value;
};

} // namespace api
} // namespace libbitcoin
#endif
