#ifndef LIBBITCOIN__MACHINE_INTERPRETER_RESULT_HPP
#define LIBBITCOIN__MACHINE_INTERPRETER_RESULT_HPP

#include <bitcoin/bitcoin/machine/interpreter.hpp>

namespace libbitcoin {
namespace api {

class machine_interpreter_result {
public:
	machine::interpreter::result getValue() {
        return value;
    }

    void setValue(machine::interpreter::result value) {
        this->value = value;
    }
private:
    machine::interpreter::result value;
};

} // namespace api
} // namespace libbitcoin
#endif
