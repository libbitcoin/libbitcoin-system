#ifndef LIBBITCOIN__P_MACHINE_OPCODE_HPP
#define LIBBITCOIN__P_MACHINE_OPCODE_HPP

#include <bitcoin/bitcoin/machine/opcode.hpp>

namespace libbitcoin {
namespace api {

class p_machine_opcode {
public:
	p_machine_opcode() : value_(new machine::opcode){
  }

  void set(machine::opcode value) {
      value_ = value;
  }

  const machine::opcode cast() const {
    return value_;
  }

  machine::opcode get() {
    return value_;
  }

  ~p_machine_opcode() {
	delete value_;
  }

private:
  machine::opcode value_;
};

} // namespace api
} // namespace libbitcoin
#endif
