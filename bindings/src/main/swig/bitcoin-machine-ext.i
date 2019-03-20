
%import "bitcoin/bitcoin/machine/opcode.hpp"
%include "bitcoin/p_machine_opcode.hpp"
%include "bitcoin/machine_opcode.hpp"
%include "bitcoin/machine_opcode_static.hpp"

//%ignore libbitcoin::machine::operation::operation(operation&& other);
//%ignore libbitcoin::machine::operation::operation(data_chunk&& uncoded, bool minimal=true);
//%ignore libbitcoin::machine::operation::operation(data_chunk&& uncoded, bool minimal);
//%ignore libbitcoin::machine::operation::operation(opcode code, data_chunk&& data, bool valid);
//%ignore libbitcoin::machine::operation::operator=(operation&& other);
//%rename(assign) libbitcoin::machine::operation::operator=(const operation& other);
//%rename(eq) libbitcoin::machine::operation::operator==(const operation& other) const;
//%rename(ne) libbitcoin::machine::operation::operator!=(const operation& other) const;
//%ignore libbitcoin::machine::operation::factory(std::istream& stream);
//%ignore libbitcoin::machine::operation::factory(reader& source);
//%ignore libbitcoin::machine::operation::from_data(std::istream& stream);
//%ignore libbitcoin::machine::operation::from_data(reader& source);
//%ignore libbitcoin::machine::operation::to_data(std::ostream& stream) const;
//%ignore libbitcoin::machine::operation::to_data(writer& sink) const;
%import "bitcoin/bitcoin/machine/operation.hpp"
%include "bitcoin/machine_operation.hpp"
%ignore setValue(machine::operation::list value);
%include "bitcoin/machine_operation_list.hpp"
%include "bitcoin/machine_operation_iterator.hpp"

%import "bitcoin/bitcoin/machine/rule_fork.hpp"
%include "bitcoin/machine_rule_fork.hpp"

%import "bitcoin/bitcoin/machine/script_pattern.hpp"
%include "bitcoin/machine_script_pattern.hpp"
%import "bitcoin/bitcoin/machine/script_version.hpp"
%include "bitcoin/machine_script_version.hpp"

//namespace libbitcoin {
//namespace machine {
//%template(OperationVector) std::vector<operation>;
////%template(OperationVectorIterator) operation::list::const_iterator;
//}
//}
