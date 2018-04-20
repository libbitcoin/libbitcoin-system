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

%include "../include/bitcoin/bitcoin/machine/rule_fork.hpp"

%include "../include/bitcoin/bitcoin/machine/script_pattern.hpp"
%include "../include/bitcoin/bitcoin/machine/script_version.hpp"

namespace libbitcoin {
namespace machine {
%template(OperationVector) std::vector<operation>;
//%template(OperationVectorIterator) operation::list::const_iterator;
}
}
