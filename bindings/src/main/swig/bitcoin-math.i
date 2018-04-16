%module math
%{
#include <bitcoin/bitcoin.hpp>
%}

%import "../../../../include/bitcoin/bitcoin/compat.h"
%import "../../../../include/bitcoin/bitcoin/compat.hpp"
%import "../../../../include/bitcoin/bitcoin/define.hpp"

%import "bitcoin-ext.i"
%import "bitcoin-ext-typemap.i"
%import "bitcoin-chain-ext.i"
%import "bitcoin-chain-ext-typemap.i"
%import "bitcoin-utility-ext.i"
%import "bitcoin-utility-ext-typemap.i"
%include "bitcoin-math-ext.i"

%include "../include/bitcoin/bitcoin/math/checksum.hpp"
%include "../include/bitcoin/bitcoin/math/crypto.hpp"
%include "../include/bitcoin/bitcoin/math/hash.hpp"
//%include "../include/bitcoin/bitcoin/math/limits.hpp"
%include "../include/bitcoin/bitcoin/math/stealth.hpp"
%include "../include/bitcoin/bitcoin/math/uint256.hpp"
