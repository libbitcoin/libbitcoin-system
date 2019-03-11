#ifndef LIBBITCOIN__MATH_AES_BLOCK_HPP
#define LIBBITCOIN__MATH_AES_BLOCK_HPP

#include <bitcoin/bitcoin/math/crypto.hpp>

namespace libbitcoin {
namespace api {

class math_aes_block {
public:
	aes_block getValue() {
        return value;
    }

    void setValue(aes_block value) {
        this->value = value;
    }
private:
    aes_block value;
};

} // namespace api
} // namespace libbitcoin
#endif
