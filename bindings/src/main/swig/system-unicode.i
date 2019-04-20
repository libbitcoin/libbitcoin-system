%module unicode
%{
#include <bitcoin/bitcoin.hpp>
%}

//%import "../../../../include/bitcoin/bitcoin/compat.h"
//%import "../../../../include/bitcoin/bitcoin/compat.hpp"
%import "../../../../include/bitcoin/bitcoin/define.hpp"

//%include "../include/bitcoin/bitcoin/unicode/console_streambuf.hpp"
//%include "../include/bitcoin/bitcoin/unicode/file_lock.hpp"
//%include "../include/bitcoin/bitcoin/unicode/ifstream.hpp"
//%include "../include/bitcoin/bitcoin/unicode/ofstream.hpp"

%ignore libbitcoin::cin;
%ignore libbitcoin::cout;
%ignore libbitcoin::cerr;

%include "../include/bitcoin/bitcoin/unicode/unicode.hpp"
//%include "../include/bitcoin/bitcoin/unicode/unicode_istream.hpp"
//%include "../include/bitcoin/bitcoin/unicode/unicode_ostream.hpp"
//%include "../include/bitcoin/bitcoin/unicode/unicode_streambuf.hpp"
