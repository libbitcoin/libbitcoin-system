%module machine
%{
#include <bitcoin/bitcoin.hpp>
%}

%import <stdint.i>

%import <bitcoin/bitcoin/define.hpp>
%import <bitcoin-ext.i>
%import <bitcoin-ext-typemap.i>
%import <bitcoin-chain-ext.i>
%import <bitcoin-chain-ext-typemap.i>
%import <bitcoin-utility-ext.i>
%import <bitcoin-utility-ext-typemap.i>

%rename(gt) libbitcoin::machine::number::operator>(int64_t value) const;
%rename(lt) libbitcoin::machine::number::operator<(int64_t value) const;
%rename(ge) libbitcoin::machine::number::operator>=(int64_t value) const;
%rename(le) libbitcoin::machine::number::operator<=(int64_t value) const;
%rename(eq) libbitcoin::machine::number::operator==(int64_t value) const;
%rename(notEqual) libbitcoin::machine::number::operator!=(int64_t value) const;

%rename(gt) libbitcoin::machine::number::operator>(const number& other) const;
%rename(lt) libbitcoin::machine::number::operator<(const number& other) const;
%rename(ge) libbitcoin::machine::number::operator>=(const number& other) const;
%rename(le) libbitcoin::machine::number::operator<=(const number& other) const;
%rename(eq) libbitcoin::machine::number::operator==(const number& other) const;
%rename(notEqual) libbitcoin::machine::number::operator!=(const number& other) const;

%rename(plus) libbitcoin::machine::number::operator+() const;
%rename(negative) libbitcoin::machine::number::operator-() const;
%rename(plus) libbitcoin::machine::number::operator+(int64_t value) const;
%rename(minus) libbitcoin::machine::number::operator-(int64_t value) const;
%rename(plus) libbitcoin::machine::number::operator+(const number& other) const;
%rename(minus) libbitcoin::machine::number::operator-(const number& other) const;

%rename(inc) libbitcoin::machine::number::operator+=(int64_t value);
%rename(dec) libbitcoin::machine::number::operator-=(int64_t value);
%rename(inc) libbitcoin::machine::number::operator+=(const number& other);
%rename(dec) libbitcoin::machine::number::operator-=(const number& other);

%include "../include/bitcoin/bitcoin/machine/number.hpp"

%include "../include/bitcoin/bitcoin/machine/opcode.hpp"


%ignore libbitcoin::machine::operation::operation(operation&& other);
%ignore libbitcoin::machine::operation::operation(data_chunk&& uncoded, bool minimal=true);
%ignore libbitcoin::machine::operation::operation(data_chunk&& uncoded, bool minimal);
%ignore libbitcoin::machine::operation::operation(opcode code, data_chunk&& data, bool valid);
%ignore libbitcoin::machine::operation::operator=(operation&& other);
%rename(assign) libbitcoin::machine::operation::operator=(const operation& other);
%rename(eq) libbitcoin::machine::operation::operator==(const operation& other) const;
%rename(ne) libbitcoin::machine::operation::operator!=(const operation& other) const;
%ignore libbitcoin::machine::operation::factory(std::istream& stream);
%ignore libbitcoin::machine::operation::factory(reader& source);
%ignore libbitcoin::machine::operation::from_data(std::istream& stream);
%ignore libbitcoin::machine::operation::from_data(reader& source);
%ignore libbitcoin::machine::operation::to_data(std::ostream& stream) const;
%ignore libbitcoin::machine::operation::to_data(writer& sink) const;
%include "../include/bitcoin/bitcoin/machine/operation.hpp"

%include "../include/bitcoin/bitcoin/machine/script_version.hpp"

%include "typemaps.i"
// knows about int32_t
%include "stdint.i"
// let's have an int32_t *OUTPUT for our int32_t *length
%apply int32_t *OUTPUT { int32_t& out_value }

//%apply int32_t* {int32_t& out_value}
//%rename(op_iterator) libbitcoin::program::operation::iterator;
%include "../include/bitcoin/bitcoin/machine/program.hpp"


%include "../include/bitcoin/bitcoin/machine/interpreter.hpp"

%include "../include/bitcoin/bitcoin/machine/rule_fork.hpp"
%include "../include/bitcoin/bitcoin/machine/script_pattern.hpp"
%include "../include/bitcoin/bitcoin/machine/sighash_algorithm.hpp"

%template(OperationVector) std::vector<libbitcoin::machine::operation>;

