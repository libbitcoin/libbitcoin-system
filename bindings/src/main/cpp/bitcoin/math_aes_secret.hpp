#ifndef LIBBITCOIN__MATH_AES_SECRET_HPP
#define LIBBITCOIN__MATH_AES_SECRET_HPP

#include <bitcoin/bitcoin/math/crypto.hpp>

namespace libbitcoin {
namespace api {

class math_aes_secret {
public:
	aes_secret* getValue() {
        return value;
    }

    void setValue(aes_secret* value) {
        this->value = value;
    }
private:
    aes_secret* value;
};

} // namespace api
} // namespace libbitcoin
#endif
