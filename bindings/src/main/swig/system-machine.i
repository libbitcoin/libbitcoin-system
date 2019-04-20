%module machine
%{
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/system_machine.hpp>
%}

%import <stdint.i>

%ignore getValue();

%import <bitcoin/bitcoin/define.hpp>
%import <bitcoin/bitcoin/compat.hpp>
%import <system-ext.i>
//%import <system-ext-typemap.i>
%import <system-chain-ext.i>
//%import <system-chain-ext-typemap.i>
%import <system-utility-ext.i>
//%import <system-utility-ext-typemap.i>

%include <system-machine-ext.i>

//%rename(gt) libbitcoin::machine::number::operator>(int64_t value) const;
//%rename(lt) libbitcoin::machine::number::operator<(int64_t value) const;
//%rename(ge) libbitcoin::machine::number::operator>=(int64_t value) const;
//%rename(le) libbitcoin::machine::number::operator<=(int64_t value) const;
//%rename(eq) libbitcoin::machine::number::operator==(int64_t value) const;
//%rename(notEqual) libbitcoin::machine::number::operator!=(int64_t value) const;

//%rename(gt) libbitcoin::machine::number::operator>(const number& other) const;
//%rename(lt) libbitcoin::machine::number::operator<(const number& other) const;
//%rename(ge) libbitcoin::machine::number::operator>=(const number& other) const;
//%rename(le) libbitcoin::machine::number::operator<=(const number& other) const;
//%rename(eq) libbitcoin::machine::number::operator==(const number& other) const;
//%rename(notEqual) libbitcoin::machine::number::operator!=(const number& other) const;

//%rename(plus) libbitcoin::machine::number::operator+() const;
//%rename(negative) libbitcoin::machine::number::operator-() const;
//%rename(plus) libbitcoin::machine::number::operator+(int64_t value) const;
//%rename(minus) libbitcoin::machine::number::operator-(int64_t value) const;
//%rename(plus) libbitcoin::machine::number::operator+(const number& other) const;
//%rename(minus) libbitcoin::machine::number::operator-(const number& other) const;

//%rename(inc) libbitcoin::machine::number::operator+=(int64_t value);
//%rename(dec) libbitcoin::machine::number::operator-=(int64_t value);
//%rename(inc) libbitcoin::machine::number::operator+=(const number& other);
//%rename(dec) libbitcoin::machine::number::operator-=(const number& other);

%import "bitcoin/bitcoin/machine/number.hpp"
%include "bitcoin/machine_number.hpp"

//%include "typemaps.i"
//// knows about int32_t
//%include "stdint.i"
//// let's have an int32_t *OUTPUT for our int32_t *length
//%apply int32_t *OUTPUT { int32_t& out_value }

//%apply int32_t* {int32_t& out_value}
//%rename(op_iterator) libbitcoin::program::operation::iterator;
%import "bitcoin/bitcoin/machine/program.hpp"
%ignore setValue(machine::program::stack_iterator value);
%include "bitcoin/machine_stack_iterator.hpp"
%ignore setValue(machine::program::op_iterator value);
%include "bitcoin/machine_op_iterator.hpp"
%ignore setValue(machine::program* value);
%include "bitcoin/machine_program.hpp"

%ignore setValue(machine::interpreter::result value);
%include "bitcoin/machine_interpreter_result.hpp"

%import "bitcoin/bitcoin/machine/interpreter.hpp"
%include "bitcoin/machine_interpreter.hpp"

%import "bitcoin/bitcoin/machine/sighash_algorithm.hpp"
%include "bitcoin/machine_sighash_algorithm.hpp"

//%template(OperationVector) std::vector<libbitcoin::machine::operation>;

