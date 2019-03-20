#ifndef LIBBITCOIN__MACHINE_OP_ITERATOR_HPP
#define LIBBITCOIN__MACHINE_OP_ITERATOR_HPP

#include <bitcoin/bitcoin/machine/program.hpp>

namespace libbitcoin {
namespace api {

class machine_op_iterator {
public:
	machine::program::op_iterator getValue() {
        return value;
    }

    void setValue(machine::program::op_iterator value) {
        this->value = value;
    }
private:
    machine::program::op_iterator value;
};

} // namespace api
} // namespace libbitcoin
#endif
