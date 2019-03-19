#ifndef LIBBITCOIN__MACHINE_OPERATION_LIST_HPP
#define LIBBITCOIN__MACHINE_OPERATION_LIST_HPP

#include <bitcoin/bitcoin/machine/operation.hpp>

namespace libbitcoin {
namespace api {

class machine_operation_list {
public:
	machine::operation::list getValue() {
        return value;
    }

    void setValue(machine::operation::list value) {
        this->value = value;
    }
private:
    machine::operation::list value;
};

} // namespace api
} // namespace libbitcoin
#endif
