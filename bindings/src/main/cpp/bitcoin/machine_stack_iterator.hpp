#ifndef LIBBITCOIN__MACHINE_STACK_ITERATOR_HPP
#define LIBBITCOIN__MACHINE_STACK_ITERATOR_HPP

#include <bitcoin/bitcoin/machine/program.hpp>

namespace libbitcoin {
namespace api {

class machine_stack_iterator {
public:
	machine::program::stack_iterator getValue() {
        return value;
    }

    void setValue(machine::program::stack_iterator value) {
        this->value = value;
    }
private:
    machine::program::stack_iterator value;
};

} // namespace api
} // namespace libbitcoin
#endif
