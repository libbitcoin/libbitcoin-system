#ifndef LIBBITCOIN__CHAIN_POINT_HPP
#define LIBBITCOIN__CHAIN_POINT_HPP

#include <bitcoin/bitcoin/chain/point.hpp>

namespace libbitcoin {
namespace api {

class chain_point {
public:
	chain::point getValue() {
        return value;
    }

    void setValue(chain::point value) {
        this->value = value;
    }
private:
    chain::point value;
};

} // namespace api
} // namespace libbitcoin
#endif
