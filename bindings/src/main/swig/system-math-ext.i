%include <stdint.i>
//%include <typemaps.i>

%include <std_string.i>
//%include "std_container.i"
%import <bitcoin/bitcoin/define.hpp>
%import <bitcoin/bitcoin/compat.hpp>

%ignore getValue();

%import <bitcoin/bitcoin/math/hash.hpp>
%import <bitcoin/bitcoin/math/elliptic_curve.hpp>
%ignore setValue(der_signature* value);
%include <bitcoin/math_der_signature.hpp>
%ignore setValue(ec_compressed* value);
%include <bitcoin/math_ec_compressed.hpp>
%ignore setValue(ec_uncompressed* value);
%include <bitcoin/math_ec_uncompressed.hpp>
%ignore setValue(point_list* value);
%include <bitcoin/math_point_list.hpp>
%ignore setValue(ec_secret* value);
%include <bitcoin/math_ec_secret.hpp>
%ignore setValue(ec_signature* value);
%include <bitcoin/math_ec_signature.hpp>
%ignore math_hash_digest(hash_digest hash);
%ignore getValue() const;
%ignore setValue(hash_digest* value);
%ignore libbitcoin::api::getValue() const;
%include <bitcoin/math_hash_digest.hpp>
%ignore setValue(recoverable_signature* value);
%include <bitcoin/math_recoverable_signature.hpp>
%include "bitcoin/math_elliptic_curve.hpp"

//%include <std_array.i>

//%import <vector.i>

//%template(HashDigest) std::array<unsigned char, libbitcoin::hash_size>;
//%template(HalfHash) std::array<unsigned char, libbitcoin::half_hash_size>;
//%template(QuarterHash) std::array<unsigned char, libbitcoin::quarter_hash_size>;
//%template(LongHash) std::array<unsigned char, libbitcoin::long_hash_size>;
//%template(ShortHash) std::array<unsigned char, libbitcoin::short_hash_size>;
//%template(MiniHash) std::array<unsigned char, libbitcoin::mini_hash_size>;
//%template(HashList) std::vector<libbitcoin::hash_digest>;
//%template(HalfHashList) std::vector<libbitcoin::half_hash>;
//%template(QuarterHashList) std::vector<libbitcoin::quarter_hash>;
//%template(LongHashList) std::vector<libbitcoin::long_hash>;
//%template(ShortHashList) std::vector<libbitcoin::short_hash>;
//%template(MiniHashList) std::vector<libbitcoin::mini_hash>;

