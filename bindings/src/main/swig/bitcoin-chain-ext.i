%module chain
%{
#include <bitcoin/bitcoin.hpp>
%}

%include "vector.i"

%import <bitcoin/bitcoin/define.hpp>

%rename(assign) libbitcoin::chain::block::operator=(block&& other);
%rename(eq) libbitcoin::chain::block::operator==(const block& other) const;
%rename(ne) libbitcoin::chain::block::operator!=(const block& other) const;
%ignore block(block&& other);
%ignore block(chain::header&& header, transaction::list&& transactions);
%ignore libbitcoin::chain::block::header();
%ignore libbitcoin::chain::block::set_header(chain::header&& value);
%ignore libbitcoin::chain::block::set_transactions(transaction::list&& value);
%ignore factory(std::istream& stream, bool witness=false);
%ignore factory(reader& source, bool witness=false);
%ignore from_data(std::istream& stream, bool witness=false);
%ignore from_data(reader& source, bool witness=false);
%ignore to_data(std::ostream& stream, bool witness=false) const;
%ignore to_data(writer& sink, bool witness=false) const;
%include "../../../../include/bitcoin/bitcoin/chain/block.hpp"

%ignore compact(compact&& other);
%rename(big) libbitcoin::chain::compact::operator const uint256_t&() const;
%include "../../../../include/bitcoin/bitcoin/chain/compact.hpp"

%ignore libbitcoin::chain::header::operator=(header&& other);
%rename(assign) libbitcoin::chain::header::operator=(const header& other);
%rename(eq) libbitcoin::chain::header::operator==(const header& other) const;
%rename(ne) libbitcoin::chain::header::operator!=(const header& other) const;
%ignore libbitcoin::chain::header::header(header&& other);
%ignore libbitcoin::chain::header::header(uint32_t version, hash_digest&& previous_block_hash,
        hash_digest&& merkle, uint32_t timestamp, uint32_t bits,
        uint32_t nonce);
%ignore libbitcoin::chain::header::factory(std::istream& stream, bool wire=true);
%ignore libbitcoin::chain::header::factory(reader& source, const hash_digest& hash, bool wire=true);
%ignore libbitcoin::chain::header::factory(reader& source, bool wire=true);
%ignore libbitcoin::chain::header::factory(reader& source, hash_digest&& hash, bool wire=true);
%ignore libbitcoin::chain::header::from_data(const data_chunk& data, bool wire=true);
%ignore libbitcoin::chain::header::from_data(std::istream& stream, bool wire=true);
%ignore libbitcoin::chain::header::from_data(reader& source, bool wire=true);
%ignore libbitcoin::chain::header::from_data(reader& source, hash_digest&& hash, bool wire=true);
%ignore libbitcoin::chain::header::from_data(reader& source, const hash_digest& hash, bool wire=true);
%ignore libbitcoin::chain::header::set_previous_block_hash(hash_digest&& value);
%ignore libbitcoin::chain::header::set_merkle(hash_digest&& value);
%ignore libbitcoin::chain::header::to_data(std::ostream& stream, bool wire=true) const;
%ignore libbitcoin::chain::header::to_data(writer& sink, bool wire=true) const;
%include "../../../../include/bitcoin/bitcoin/chain/header.hpp"

%ignore libbitcoin::chain::input::input(input&& other);
%ignore libbitcoin::chain::input::input(output_point&& previous_output, chain::script&& script, uint32_t sequence);
%ignore libbitcoin::chain::input::input(output_point&& previous_output, chain::script&& script, chain::witness&& witness, uint32_t sequence);
%ignore libbitcoin::chain::input::operator=(input&& other);
%rename(assign) libbitcoin::chain::input::operator=(const input& other);
%rename(eq) libbitcoin::chain::input::operator==(const input& other) const;
%rename(ne) libbitcoin::chain::input::operator!=(const input& other) const;
%ignore libbitcoin::chain::input::previous_output();
%ignore libbitcoin::chain::input::set_previous_output(output_point&& value);
%ignore libbitcoin::chain::input::set_script(chain::script&& value);
%ignore libbitcoin::chain::input::set_witness(chain::witness&& value);
%ignore libbitcoin::chain::input::witness();
%ignore libbitcoin::chain::input::factory(std::istream& stream, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::input::factory(reader& source, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::input::from_data(std::istream& stream, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::input::from_data(reader& source, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::input::to_data(std::ostream& stream, bool wire=true, bool witness=false) const;
%ignore libbitcoin::chain::input::to_data(writer& sink, bool wire=true, bool witness=false) const;
%include "../../../../include/bitcoin/bitcoin/chain/input.hpp"

%include "../../../../include/bitcoin/bitcoin/chain/input_point.hpp"


%ignore point(point&& other);
%ignore point(hash_digest&& hash, uint32_t index);
%ignore libbitcoin::chain::point::operator=(point&& other);
%rename(assign) libbitcoin::chain::point::operator=(const point& other);
%rename(lt) libbitcoin::chain::point::operator<(const point& other) const;
%rename(eq) libbitcoin::chain::point::operator==(const point& other) const;
%rename(ne) libbitcoin::chain::point::operator!=(const point& other) const;
%ignore libbitcoin::chain::point::hash();
%ignore libbitcoin::chain::point::set_hash(hash_digest&& value);
%ignore libbitcoin::chain::point::factory(std::istream& stream, bool wire=true);
%ignore libbitcoin::chain::point::factory(reader& source, bool wire=true);
%ignore libbitcoin::chain::point::from_data(std::istream& stream, bool wire=true);
%ignore libbitcoin::chain::point::from_data(reader& source, bool wire=true);
%ignore libbitcoin::chain::point::to_data(std::ostream& stream, bool wire=true) const;
%ignore libbitcoin::chain::point::to_data(writer& sink, bool wire=true) const;
%include "../../../../include/bitcoin/bitcoin/chain/point.hpp"

%ignore libbitcoin::chain::output::output(output&& other);
%ignore libbitcoin::chain::output::output(uint64_t value, chain::script&& script);
%ignore libbitcoin::chain::output::operator=(output&& other);
%rename(assign) libbitcoin::chain::output::operator=(const output& other);
%rename(eq) libbitcoin::chain::output::operator==(const output& other) const;
%rename(ne) libbitcoin::chain::output::operator!=(const output& other) const;
%ignore libbitcoin::chain::output::set_script(chain::script&& value);
%ignore libbitcoin::chain::output::factory(std::istream& stream, bool wire=true);
%ignore libbitcoin::chain::output::factory(reader& source, bool wire=true);
%ignore libbitcoin::chain::output::from_data(std::istream& stream, bool wire=true);
%ignore libbitcoin::chain::output::from_data(reader& source, bool wire=true, bool unused=false);
%ignore libbitcoin::chain::output::to_data(std::ostream& stream, bool wire=true) const;
%ignore libbitcoin::chain::output::to_data(writer& sink, bool wire=true, bool unused=false) const;
%include "../../../../include/bitcoin/bitcoin/chain/output.hpp"

%ignore libbitcoin::chain::output_point::output_point(point&& value);
%ignore libbitcoin::chain::output_point::output_point(output_point&& other);
%ignore libbitcoin::chain::output_point::output_point(hash_digest&& hash, uint32_t index);
%ignore libbitcoin::chain::output_point::operator=(point&& other);
%rename(assign) libbitcoin::chain::output_point::operator=(const point&);
%ignore libbitcoin::chain::output_point::operator=(output_point&& other);
%rename(assign) libbitcoin::chain::output_point::operator=(const output_point&);
%rename(eq) libbitcoin::chain::output_point::operator==(const point& other) const;
%rename(ne) libbitcoin::chain::output_point::operator!=(const point& other) const;
%rename(eq) libbitcoin::chain::output_point::operator==(const output_point& other) const;
%rename(ne) libbitcoin::chain::output_point::operator!=(const output_point& other) const;
%include "../../../../include/bitcoin/bitcoin/chain/output_point.hpp"

%ignore libbitcoin::chain::point_value::point_value(point_value&& other);
%ignore libbitcoin::chain::point_value::point_value(point&& instance, uint64_t value);
%rename(assign) libbitcoin::chain::point_value::operator=(point_value other);
%rename(eq) libbitcoin::chain::point_value::operator==(const point_value& other) const;
%rename(ne) libbitcoin::chain::point_value::operator!=(const point_value& other) const;
%rename(swap_point_value) swap(point_value& left, point_value& right);
%include "../../../../include/bitcoin/bitcoin/chain/point_value.hpp"

%include "../../../../include/bitcoin/bitcoin/chain/points_value.hpp"

%ignore libbitcoin::chain::script::script(script&& other);
%ignore libbitcoin::chain::script::script(operation::list&& ops);
%ignore libbitcoin::chain::script::script(data_chunk&& encoded, bool prefix);
%ignore libbitcoin::chain::script::operator=(script&& other);
%rename(assign) libbitcoin::chain::script::operator=(const script& other);
%rename(eq) libbitcoin::chain::script::operator==(const script& other) const;
%rename(ne) libbitcoin::chain::script::operator!=(const script& other) const;
%ignore libbitcoin::chain::script::from_operations(operation::list&& ops);
%rename(getElement) libbitcoin::chain::script::operator[](size_t index) const;
%ignore libbitcoin::chain::script::factory(std::istream& stream, bool prefix);
%ignore libbitcoin::chain::script::factory(reader& source, bool prefix);
%ignore libbitcoin::chain::script::from_data(std::istream& stream, bool prefix);
%ignore libbitcoin::chain::script::from_data(reader& source, bool prefix);
%ignore libbitcoin::chain::script::to_data(std::ostream& stream, bool prefix) const;
%ignore libbitcoin::chain::script::to_data(writer& sink, bool prefix) const;
%include "../../../../include/bitcoin/bitcoin/chain/script.hpp"

%ignore libbitcoin::chain::transaction::transaction(transaction&& other);
%ignore libbitcoin::chain::transaction::transaction(uint32_t version, uint32_t locktime, ins&& inputs, outs&& outputs);
%ignore libbitcoin::chain::transaction::operator=(transaction&& other);
%rename(assign) libbitcoin::chain::transaction::operator=(const transaction& other);
%rename(eq) libbitcoin::chain::transaction::operator==(const transaction& other) const;
%rename(ne) libbitcoin::chain::transaction::operator!=(const transaction& other) const;
%ignore libbitcoin::chain::transaction::inputs();
%ignore libbitcoin::chain::transaction::set_inputs(ins&& value);
%ignore libbitcoin::chain::transaction::set_outputs(outs&& value);
%ignore libbitcoin::chain::transaction::outputs();
%ignore libbitcoin::chain::transaction::factory(std::istream& stream, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::transaction::factory(reader& source, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::transaction::factory(reader& source, hash_digest&& hash,bool wire=true, bool witness=false);
%ignore libbitcoin::chain::transaction::factory(reader& source, const hash_digest& hash,bool wire=true, bool witness=false);
%ignore libbitcoin::chain::transaction::from_data(std::istream& stream, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::transaction::from_data(reader& source, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::transaction::from_data(reader& source, hash_digest&& hash, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::transaction::from_data(reader& source, const hash_digest& hash, bool wire=true, bool witness=false);
%ignore libbitcoin::chain::transaction::to_data(std::ostream& stream, bool wire=true, bool witness=false) const;
%ignore libbitcoin::chain::transaction::to_data(writer& sink, bool wire=true, bool witness=false) const;
%include "../../../../include/bitcoin/bitcoin/chain/transaction.hpp"

%template(InputVector) std::vector<libbitcoin::chain::input>;
%template(OutputVector) std::vector<libbitcoin::chain::output>;
