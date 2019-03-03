%module config
%{
#include <bitcoin/bitcoin.hpp>
%}

%import <stdint.i>

//%include <p_std_string.i>
//%apply std::string &INOUT { std::string& };

%import <bitcoin/bitcoin/define.hpp>

%import "bitcoin-ext.i"
//%import "bitcoin-ext-typemap.i"
%import "bitcoin-math-ext.i"
//%import "bitcoin-math-ext-typemap.i"
%import "bitcoin-utility-ext.i"
//%import "bitcoin-utility-ext-typemap.i"
%import "bitcoin-chain-ext.i"
//%import "bitcoin-chain-ext-typemap.i"

%ignore getValue();

%include "bitcoin-config-ext.i"

//%rename(toData_chunk) libbitcoin::config::base16::operator const data_chunk&() const;
//%rename(toData_slice) libbitcoin::config::base16::operator data_slice() const;
//%ignore operator>>(std::istream& input, base16& argument);
//%ignore operator<<(std::ostream& output, const base16& argument);
%import "bitcoin/utility_data_slice.hpp"
%import "bitcoin/bitcoin/config/base16.hpp"
%include "bitcoin/config_base16.hpp"

//%rename(toBinary) libbitcoin::config::base2::operator const binary&() const;
//%ignore operator>>(std::istream& input, base2& argument);
//%ignore operator<<(std::ostream& output, const base2& argument);
%import "bitcoin/bitcoin/config/base2.hpp"
%include "bitcoin/config_base2.hpp"

//%rename(toData_chunk) libbitcoin::config::base58::operator const data_chunk&() const;
//%rename(toData_slice) libbitcoin::config::base58::operator data_slice() const;
//%ignore operator>>(std::istream& input, base58& argument);
//%ignore operator<<(std::ostream& output, const base58& argument);
%import "bitcoin/bitcoin/config/base58.hpp"
%include "bitcoin/config_base58.hpp"

//%rename(toData_chunk) libbitcoin::config::base64::operator const data_chunk&() const;
//%rename(toData_slice) libbitcoin::config::base64::operator data_slice() const;
//%ignore operator>>(std::istream& input, base64& argument);
//%ignore operator<<(std::ostream& output, const base64& argument);
%import "bitcoin/bitcoin/config/base64.hpp"
%include "bitcoin/config_base64.hpp"
%ignore setValue(config::block value);
%include "bitcoin/config_block.hpp"

%include "bitcoin/config_directory.hpp"

//%rename(toBoolean) libbitcoin::config::endpoint::operator bool() const;
//%rename(eq) libbitcoin::config::endpoint::operator==(const endpoint& other) const;
//%ignore operator>>(std::istream& input, endpoint& argument);
//%ignore operator<<(std::ostream& output, const endpoint& argument);
%import "bitcoin/bitcoin/config/endpoint.hpp"
%ignore setValue(config::endpoint value);
%include "bitcoin/config_endpoint.hpp"
%ignore setValue(config::endpoint::list value);
%include "bitcoin/config_endpoint_list.hpp"

//%rename(toShort_hash) libbitcoin::config::hash160::operator const short_hash&() const;
//%ignore operator>>(std::istream& input, hash160& argument);
//%ignore operator<<(std::ostream& output, const hash160& argument);
%import "bitcoin/bitcoin/config/hash160.hpp"
%include "bitcoin/config_hash160.hpp"

//%rename(eq) libbitcoin::config::hash256::operator==(const hash256& other) const;
//%rename(toHash_digest) libbitcoin::config::hash256::operator const hash_digest&() const;
//%ignore operator>>(std::istream& input, hash256& argument);
//%ignore operator<<(std::ostream& output, const hash256& argument);
%import "bitcoin/bitcoin/config/hash256.hpp"
%include "bitcoin/config_hash256.hpp"
%include "bitcoin/config_hash256_list.hpp"

//%ignore operator>>(std::istream& input, output& argument);
%import "bitcoin/bitcoin/config/output.hpp"
%import "bitcoin/math_short_hash.hpp"
%import "bitcoin/chain_script.hpp"
%include "bitcoin/config_output.hpp"
%ignore setValue(boost::program_options::option_description value);
%include "bitcoin/config_option_description.hpp"
%ignore setValue(boost::program_options::options_description value);
%include "bitcoin/config_options_description.hpp"
%ignore setValue(config::argument_list value);
%include "bitcoin/config_argument_list.hpp"
%ignore setValue(config::parameter value);
%include "bitcoin/config_parameter.hpp"
%ignore setValue(config::parameter_list value);
%include "bitcoin/config_parameter_list.hpp"

%import "bitcoin/bitcoin/config/parser.hpp"
%ignore setValue(variables_map value);
%include "bitcoin/config_variables_map.hpp"
%ignore setValue(option_metadata value);
%include "bitcoin/config_option_metadata.hpp"
%ignore setValue(options_metadata value);
%include "bitcoin/config_options_metadata.hpp"
%ignore setValue(arguments_metadata value);
%include "bitcoin/config_arguments_metadata.hpp"
%include "bitcoin/config_parser.hpp"

//%rename(toOutput_point) libbitcoin::config::point::operator const chain::output_point&() const;
//%ignore operator>>(std::istream& input, point& argument);
//%ignore operator<<(std::ostream& output, const point& argument);
%import "bitcoin/bitcoin/config/point.hpp"
%ignore setValue(config::point value);
%include "bitcoin/config_point.hpp"

%ignore setValue(boost::program_options::positional_options_description value);
%include "bitcoin/config_positional_options_description.hpp"

%include "bitcoin/config_printer.hpp"

%ignore setValue(config::script value);
%include "bitcoin/config_script.hpp"
%include "bitcoin/config_settings.hpp"

//%ignore libbitcoin::config::printer::commandline(std::ostream& output);
//%ignore libbitcoin::config::printer::settings(std::ostream& output);
%import "bitcoin/bitcoin/config/printer.hpp"
%import "bitcoin/string_vector.hpp"
%import "bitcoin/config_options_description.hpp"
%include "bitcoin/config_printer.hpp"

//%rename(toScript) libbitcoin::config::script::operator const chain::script&() const;
//%ignore operator>>(std::istream& input, script& argument);
//%ignore operator<<(std::ostream& output, const script& argument);
%import "bitcoin/utility_data_chunk.hpp"
%import "bitcoin/chain_script.hpp"
%include "bitcoin/config_settings.hpp"

//%rename(toBoolean) operator bool() const;
//%rename(toHash_digest) operator const hash_digest&() const;
//%rename(toData_slice) operator data_slice() const;
//%ignore operator>>(std::istream& input, sodium& argument);
//%ignore operator<<(std::ostream& output, const sodium& argument);
%import "bitcoin/bitcoin/config/sodium.hpp"
%ignore setValue(config::sodium value);
%include "bitcoin/config_sodium.hpp"
%ignore setValue(config::sodium::list value);
%include "bitcoin/config_sodium_list.hpp"


//%template(CheckpointVector) std::vector<libbitcoin::config::checkpoint>;
//%template(ParameterVector) std::vector<libbitcoin::config::parameter>;
