%module usingshorttestexpected
%{
//#include <bitcoin/bitcoin.hpp>
%}

%include <stdint.i>
%include <typemaps.i>

%include <std_string.i>
%include <std_vector.i>
%template(ucharVector) std::vector<unsigned char>;


%inline %{
namespace libbitcoin {
} // namespace libbitcoin

namespace bc = libbitcoin;

    #define BC_API
    #define BC_INTERNAL

namespace libbitcoin {

#define BC_CONSTEXPR constexpr

template <size_t Size>
using byte_array = std::array<uint8_t, Size>;

typedef std::array<uint8_t, 1> one_byte;

static BC_CONSTEXPR size_t hash_size = 32;

//#define hash_size 32

typedef std::array<uint8_t, hash_size> hash_digest;

typedef std::vector<hash_digest> hash_list;

BC_API std::string encode_hash(hash_digest hash);

BC_API bool decode_hash(hash_digest& out, const std::string& in);

BC_API hash_digest hash_literal(const char (&string)[2 * hash_size + 1]);

}

%}

%include <std_array.i>
%template(HashDigest) std::array<uint8_t, 32>;
//%template(HashDigest) std::array<uint8_t, hash_size>;
%template(HashList) std::vector<std::array<uint8_t, 32>>;

