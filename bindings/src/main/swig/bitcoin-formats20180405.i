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
%template(HashDigest) std::array<uint8_t, 32>;
%template(HalfHash) std::array<uint8_t, 32/2>;
%template(QuarterHash) std::array<uint8_t, 32/4>;
%template(LongHash) std::array<uint8_t, 2*32>;
%template(ShortHash) std::array<uint8_t, 20>;
%template(MiniHash) std::array<uint8_t, 6>;
%template(HashList) std::vector<std::array<uint8_t, 32>>;
%template(HalfHashList) std::vector<std::array<uint8_t, 32/2>>;
%template(QuarterHashList) std::vector<std::array<uint8_t, 32/4>>;
%template(LongHashList) std::vector<std::array<uint8_t, 2*32>>;
%template(ShortHashList) std::vector<std::array<uint8_t, 20>>;
%template(MiniHashList) std::vector<std::array<uint8_t, 6>>;

%include "../include/bitcoin/bitcoin/formats/base_10.hpp"
%include "../include/bitcoin/bitcoin/formats/base_16.hpp"
%include "../include/bitcoin/bitcoin/formats/base_32.hpp"
%include "../include/bitcoin/bitcoin/formats/base_58.hpp"
%include "../include/bitcoin/bitcoin/formats/base_64.hpp"
%include "../include/bitcoin/bitcoin/formats/base_85.hpp"
