#ifndef LIBBITCOIN__P_MACHINE_OPCODE_HPP
#define LIBBITCOIN__P_MACHINE_OPCODE_HPP

#include <bitcoin/bitcoin/machine/opcode.hpp>

namespace libbitcoin {
namespace api {

class p_machine_opcode {
public:
	p_machine_opcode() :
			value_(new libbitcoin::machine::opcode) {
	}

	~p_machine_opcode() {
		delete value_;
	}

	void set(libbitcoin::machine::opcode *value) {
		value_ = value;
	}

	const libbitcoin::machine::opcode *cast() const {
		return value_;
	}

	libbitcoin::machine::opcode* get() {
		return value_;
	}

	libbitcoin::machine::opcode* getValue() {
        return value_;
    }

    void setValue(libbitcoin::machine::opcode* value) {
        value_ = value;
    }

private:
	libbitcoin::machine::opcode *value_;
};

} // namespace api
} // namespace libbitcoin
#endif
