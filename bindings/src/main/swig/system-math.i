%module math
%{
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/system_math.hpp>
%}

%import "bitcoin/bitcoin/compat.h"
%import "bitcoin/bitcoin/compat.hpp"
%import "bitcoin/bitcoin/define.hpp"

%import "system-ext.i"
//%import "system-ext-typemap.i"
//%import "system-chain-ext.i"
%import "bitcoin/chain_script.hpp"
//%import "system-chain-ext-typemap.i"
%import "system-utility-ext.i"
//%import "system-utility-ext-typemap.i"

%ignore getValue();
%ignore cast();

%include "system-math-ext.i"

%ignore setValue(aes_block* value);
%include "bitcoin/math_aes_block.hpp"
%ignore setValue(aes_secret* value);
%include "bitcoin/math_aes_secret.hpp"
%include "bitcoin/math_checksum.hpp"
%include "bitcoin/math_crypto.hpp"
%ignore setValue(ec_point* value);
%include "bitcoin/math_ec_point.hpp"
%include "bitcoin/math_ec_uncompressed.hpp"
%ignore setValue(ec_scalar* value);
%include "bitcoin/math_ec_scalar.hpp"
%include "bitcoin/math_endorsement.hpp"
%ignore math_half_hash(half_hash hash);
%ignore cast();
%ignore setValue(half_hash* value);
%include "bitcoin/math_half_hash.hpp"
%ignore get(size_t i);
%ignore set(size_t i, hash_digest *t);
%ignore setValue(half_hash_list* value);
//"syntax error [line 53]
//%include "bitcoin/math_limits.hpp"
%ignore math_long_hash(long_hash hash);
%ignore setValue(long_hash* value);
%include "bitcoin/math_long_hash.hpp"
%ignore math_mini_hash(mini_hash hash);
%ignore setValue(mini_hash* value);
%include "bitcoin/math_mini_hash.hpp"
%import <bitcoin/bitcoin/math/elliptic_curve.hpp>
//%ignore setValue(point_list* value); // FIXME doesn't work
//%include "bitcoin/math_point_list.hpp"
%ignore math_quarter_hash(quarter_hash hash);
%ignore setValue(quarter_hash* value);
%include "bitcoin/math_quarter_hash.hpp"
%ignore math_short_hash(short_hash hash);
%ignore setValue(short_hash* value);
%include "bitcoin/math_short_hash.hpp"
%import <bitcoin/bitcoin/math/hash.hpp>
%ignore setValue(uint256_t* value);
%ignore math_uint256_t(uint256_t* value);
%include "bitcoin/math_uint256_t.hpp"
%ignore setValue(hash_list* value);
%include "bitcoin/math_hash_list.hpp"
%include "bitcoin/math_hash.hpp"
//%import "bitcoin/bitcoin/math/ring_signature.hpp"
%ignore setValue(key_rings* value);
%include "bitcoin/math_key_rings.hpp"
%ignore setValue(ring_signature::proof_list* value);
%include "bitcoin/math_proof_list.hpp"
%ignore setValue(secret_list* value);
%include "bitcoin/math_secret_list.hpp"
%include "bitcoin/math_ring_signature.hpp"
%include "bitcoin/math_stealth.hpp"
