#ifndef LIBBITCOIN__MATH_PROOF_LIST_HPP
#define LIBBITCOIN__MATH_PROOF_LIST_HPP

#include <bitcoin/bitcoin/math/ring_signature.hpp>

namespace libbitcoin {
namespace api {

class math_proof_list {
public:
	ring_signature::proof_list* getValue() {
        return value_;
    }

    void setValue(ring_signature::proof_list* value) {
        value_ = value;
    }
private:
    ring_signature::proof_list* value_;
};

} // namespace api
} // namespace libbitcoin
#endif
