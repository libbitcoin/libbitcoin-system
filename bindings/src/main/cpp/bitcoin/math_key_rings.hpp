#ifndef LIBBITCOIN__MATH_KEY_RINGS_HPP
#define LIBBITCOIN__MATH_KEY_RINGS_HPP

#include <bitcoin/bitcoin/math/ring_signature.hpp>

namespace libbitcoin {
namespace api {

class math_key_rings {
public:
	key_rings* getValue() {
        return value_;
    }

    void setValue(key_rings* value) {
        value_ = value;
    }
private:
    key_rings* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
