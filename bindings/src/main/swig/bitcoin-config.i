%module config
%{
#include <bitcoin/bitcoin.hpp>
%}

%import <stdint.i>

%include <p_std_string.i>
%apply std::string &INOUT { std::string& };

%import <bitcoin/bitcoin/define.hpp>

%import "bitcoin-ext.i"
%import "bitcoin-ext-typemap.i"
%import "bitcoin-math-ext.i"
%import "bitcoin-math-ext-typemap.i"
%import "bitcoin-utility-ext.i"
%import "bitcoin-utility-ext-typemap.i"
%import "bitcoin-chain-ext.i"
%import "bitcoin-chain-ext-typemap.i"

%include "bitcoin-config-ext.i"

%rename(toData_chunk) libbitcoin::config::base16::operator const data_chunk&() const;
%rename(toData_slice) libbitcoin::config::base16::operator data_slice() const;
%ignore operator>>(std::istream& input, base16& argument);
%ignore operator<<(std::ostream& output, const base16& argument);
%include "../include/bitcoin/bitcoin/config/base16.hpp"

%rename(toBinary) libbitcoin::config::base2::operator const binary&() const;
%ignore operator>>(std::istream& input, base2& argument);
%ignore operator<<(std::ostream& output, const base2& argument);
%include "../include/bitcoin/bitcoin/config/base2.hpp"

%rename(toData_chunk) libbitcoin::config::base58::operator const data_chunk&() const;
%rename(toData_slice) libbitcoin::config::base58::operator data_slice() const;
%ignore operator>>(std::istream& input, base58& argument);
%ignore operator<<(std::ostream& output, const base58& argument);
%include "../include/bitcoin/bitcoin/config/base58.hpp"

%rename(toData_chunk) libbitcoin::config::base64::operator const data_chunk&() const;
%rename(toData_slice) libbitcoin::config::base64::operator data_slice() const;
%ignore operator>>(std::istream& input, base64& argument);
%ignore operator<<(std::ostream& output, const base64& argument);
%include "../include/bitcoin/bitcoin/config/base64.hpp"

%include "../include/bitcoin/bitcoin/config/directory.hpp"

%rename(toBoolean) libbitcoin::config::endpoint::operator bool() const;
%rename(eq) libbitcoin::config::endpoint::operator==(const endpoint& other) const;
%ignore operator>>(std::istream& input, endpoint& argument);
%ignore operator<<(std::ostream& output, const endpoint& argument);
%include "../include/bitcoin/bitcoin/config/endpoint.hpp"

%rename(toShort_hash) libbitcoin::config::hash160::operator const short_hash&() const;
%ignore operator>>(std::istream& input, hash160& argument);
%ignore operator<<(std::ostream& output, const hash160& argument);
%include "../include/bitcoin/bitcoin/config/hash160.hpp"

%rename(eq) libbitcoin::config::hash256::operator==(const hash256& other) const;
%rename(toHash_digest) libbitcoin::config::hash256::operator const hash_digest&() const;
%ignore operator>>(std::istream& input, hash256& argument);
%ignore operator<<(std::ostream& output, const hash256& argument);
%include "../include/bitcoin/bitcoin/config/hash256.hpp"

%ignore operator>>(std::istream& input, output& argument);
%include "../include/bitcoin/bitcoin/config/output.hpp"
%include "../include/bitcoin/bitcoin/config/parameter.hpp"
%include "../include/bitcoin/bitcoin/config/parser.hpp"

%rename(toOutput_point) libbitcoin::config::point::operator const chain::output_point&() const;
%ignore operator>>(std::istream& input, point& argument);
%ignore operator<<(std::ostream& output, const point& argument);
%include "../include/bitcoin/bitcoin/config/point.hpp"

%ignore libbitcoin::config::printer::commandline(std::ostream& output);
%ignore libbitcoin::config::printer::settings(std::ostream& output);
%include "../include/bitcoin/bitcoin/config/printer.hpp"

%rename(toScript) libbitcoin::config::script::operator const chain::script&() const;
%ignore operator>>(std::istream& input, script& argument);
%ignore operator<<(std::ostream& output, const script& argument);
%include "../include/bitcoin/bitcoin/config/script.hpp"
%include "../include/bitcoin/bitcoin/config/settings.hpp"

%rename(toBoolean) operator bool() const;
%rename(toHash_digest) operator const hash_digest&() const;
%rename(toData_slice) operator data_slice() const;
%ignore operator>>(std::istream& input, sodium& argument);
%ignore operator<<(std::ostream& output, const sodium& argument);
%include "../include/bitcoin/bitcoin/config/sodium.hpp"


%template(CheckpointVector) std::vector<libbitcoin::config::checkpoint>;
%template(ParameterVector) std::vector<libbitcoin::config::parameter>;

