%module formats
%{
#include <bitcoin/bitcoin.hpp>
%}

//%include <java.swg>
%include <stdint.i>
%include <typemaps.i>

%include <std_string.i>
//%include "std_container.i"
%include <std_vector.i>
%template(ucharVector) std::vector<unsigned char>;
%import <bitcoin/bitcoin/define.hpp>
%import <bitcoin/bitcoin/compat.hpp>
%include <bitcoin/bitcoin/utility/array_slice.hpp>
%include <bitcoin/bitcoin/utility/data.hpp>
%include <bitcoin/bitcoin/math/hash.hpp>
%include <std_array.i>
%template(HashDigest) std::array<uint8_t, hash_size>;
%template(HalfHash) std::array<uint8_t, half_hash_size>;
%template(QuarterHash) std::array<uint8_t, quarter_hash_size>;
%template(LongHash) std::array<uint8_t, long_hash_size>;
%template(ShortHash) std::array<uint8_t, short_hash_size>;
%template(MiniHash) std::array<uint8_t, mini_hash_size>;
%template(HashList) std::vector<hash_digest>;
%template(HalfHashList) std::vector<half_hash>;
%template(QuarterHashList) std::vector<quarter_hash>;
%template(LongHashList) std::vector<long_hash>;
%template(ShortHashList) std::vector<short_hash>;
%template(MiniHashList) std::vector<mini_hash>;

%include "../include/bitcoin/bitcoin/formats/base_10.hpp"
%include "../include/bitcoin/bitcoin/formats/base_16.hpp"
%include "../include/bitcoin/bitcoin/formats/base_32.hpp"
%include "../include/bitcoin/bitcoin/formats/base_58.hpp"
%include "../include/bitcoin/bitcoin/formats/base_64.hpp"
%include "../include/bitcoin/bitcoin/formats/base_85.hpp"
