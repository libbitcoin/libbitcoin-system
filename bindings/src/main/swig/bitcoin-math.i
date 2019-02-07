%module math
%{
#include <bitcoin/bitcoin.hpp>
%}

%import "bitcoin/bitcoin/compat.h"
%import "bitcoin/bitcoin/compat.hpp"
%import "bitcoin/bitcoin/define.hpp"

%import "bitcoin-ext.i"
//%import "bitcoin-ext-typemap.i"
%import "bitcoin-chain-ext.i"
//%import "bitcoin-chain-ext-typemap.i"
%import "bitcoin-utility-ext.i"
//%import "bitcoin-utility-ext-typemap.i"
%include "bitcoin-math-ext.i"

%include "bitcoin/math_aes_block.hpp"
%include "bitcoin/math_aes_secret.hpp"
%include "bitcoin/math_checksum.hpp"
%include "bitcoin/math_crypto.hpp"
%include "bitcoin/math_ec_point.hpp"
%include "bitcoin/math_ec_uncompressed.hpp"
%include "bitcoin/math_ec_scalar.hpp"
%include "bitcoin/math_endorsement.hpp"
%ignore cast();
%include "bitcoin/math_half_hash.hpp"
%include "bitcoin/math_hash_list.hpp"
%include "bitcoin/math_hash.hpp"
//"syntax error [line 53]
//%include "bitcoin/math_limits.hpp"
%include "bitcoin/math_long_hash.hpp"
%include "bitcoin/math_mini_hash.hpp"
%include "bitcoin/math_point_list.hpp"
%include "bitcoin/math_quarter_hash.hpp"
//%import "bitcoin/bitcoin/math/ring_signature.hpp"
%include "bitcoin/math_key_rings.hpp"
%include "bitcoin/math_proof_list.hpp"
%include "bitcoin/math_secret_list.hpp"
%include "bitcoin/math_ring_signature.hpp"
%include "bitcoin/math_short_hash.hpp"
%include "bitcoin/math_stealth.hpp"
%import <bitcoin/bitcoin/math/hash.hpp>
%include "bitcoin/math_uint256_t.hpp"
