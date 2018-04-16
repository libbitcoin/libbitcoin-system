%module usingtest
%{
#include <bitcoin/bitcoin.hpp>
%}


%inline %{
// define.hpp

#ifndef LIBBITCOIN_DEFINE_HPP
#define LIBBITCOIN_DEFINE_HPP

#include <bitcoin/bitcoin/compat.hpp>

// Create bc namespace alias.
namespace libbitcoin {
} // namespace libbitcoin

namespace bc = libbitcoin;

// See http://gcc.gnu.org/wiki/Visibility

// Generic helper definitions for shared library support
#if defined _MSC_VER || defined __CYGWIN__
    #define BC_HELPER_DLL_IMPORT __declspec(dllimport)
    #define BC_HELPER_DLL_EXPORT __declspec(dllexport)
    #define BC_HELPER_DLL_LOCAL
#else
    #if __GNUC__ >= 4
        #define BC_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
        #define BC_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
        #define BC_HELPER_DLL_LOCAL  __attribute__ ((visibility ("internal")))
    #else
        #define BC_HELPER_DLL_IMPORT
        #define BC_HELPER_DLL_EXPORT
        #define BC_HELPER_DLL_LOCAL
    #endif
#endif

// Now we use the generic helper definitions above to define BC_API
// and BC_INTERNAL. BC_API is used for the public API symbols. It either DLL
// imports or DLL exports (or does nothing for static build) BC_INTERNAL is
// used for non-api symbols.

#if defined BC_STATIC
    #define BC_API
    #define BC_INTERNAL
#elif defined BC_DLL
    #define BC_API      BC_HELPER_DLL_EXPORT
    #define BC_INTERNAL BC_HELPER_DLL_LOCAL
#else
    #define BC_API      BC_HELPER_DLL_IMPORT
    #define BC_INTERNAL BC_HELPER_DLL_LOCAL
#endif

// Tag to deprecate functions and methods.
// Gives a compiler warning when they are used.
#if defined _MSC_VER || defined __CYGWIN__
    #define BC_DEPRECATED __declspec(deprecated)
#else
    #if __GNUC__ >= 4
        #define BC_DEPRECATED __attribute__((deprecated))
    #else
        #define BC_DEPRECATED
    #endif
#endif

// Avoid namespace conflict between boost::placeholders and std::placeholders.
#define BOOST_BIND_NO_PLACEHOLDERS

// Define so we can have better visibility of lcov exclusion ranges.
#define LCOV_EXCL_START(text)
#define LCOV_EXCL_STOP()

#endif


namespace libbitcoin {

// compat.hpp
#ifndef LIBBITCOIN_COMPAT_HPP
#define LIBBITCOIN_COMPAT_HPP

#include <cstdint>
#include <limits>

// CTP_Nov2013 implements noexcept but unfortunately VC12 and CTP_Nov2013
// both identify as _MSC_VER = 1800, otherwise we could include CTP_Nov2013.
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
    #define BC_NOEXCEPT _NOEXCEPT
    #define BC_CONSTEXPR const
    #define BC_CONSTFUNC inline
    #define BC_CONSTCTOR
#else
    #define BC_NOEXCEPT noexcept
    #define BC_CONSTEXPR constexpr
    #define BC_CONSTFUNC constexpr
    #define BC_CONSTCTOR constexpr
#endif

// TODO: prefix names with BC_
#if defined(_MSC_VER) && (_MSC_VER <= 1800)
    #define MIN_INT64 INT64_MIN
    #define MAX_INT64 INT64_MAX
    #define MIN_INT32 INT32_MIN
    #define MAX_INT32 INT32_MAX
    #define MAX_UINT64 UINT64_MAX
    #define MAX_UINT32 UINT32_MAX
    #define MAX_UINT16 UINT16_MAX
    #define MAX_UINT8 UINT8_MAX
    #define BC_MAX_SIZE SIZE_MAX
#else
    #define MIN_INT64 std::numeric_limits<int64_t>::min()
    #define MAX_INT64 std::numeric_limits<int64_t>::max()
    #define MIN_INT32 std::numeric_limits<int32_t>::min()
    #define MAX_INT32 std::numeric_limits<int32_t>::max()
    #define MAX_UINT64 std::numeric_limits<uint64_t>::max()
    #define MAX_UINT32 std::numeric_limits<uint32_t>::max()
    #define MAX_UINT16 std::numeric_limits<uint16_t>::max()
    #define MAX_UINT8 std::numeric_limits<uint8_t>::max()
    #define BC_MAX_SIZE std::numeric_limits<size_t>::max()
#endif

#endif

// data.hpp

// Define a byte array of a specified length.
template <size_t Size>
using byte_array = std::array<uint8_t, Size>;

template <size_t Size>
struct byte_array_parts
{
    byte_array<Size> left;
    byte_array<Size> right;
};

// Define arbitrary byte storage types.
typedef byte_array<1> one_byte;
typedef array_slice<uint8_t> data_slice;
typedef std::vector<uint8_t> data_chunk;
typedef std::queue<data_chunk> data_queue;
typedef std::vector<data_chunk> data_stack;
typedef std::initializer_list<data_slice> loaf;


// hash.hpp
// Common bitcoin hash container sizes.
static BC_CONSTEXPR size_t hash_size = 32;
static BC_CONSTEXPR size_t half_hash_size = hash_size / 2;
static BC_CONSTEXPR size_t quarter_hash_size = half_hash_size / 2;
static BC_CONSTEXPR size_t long_hash_size = 2 * hash_size;
static BC_CONSTEXPR size_t short_hash_size = 20;
static BC_CONSTEXPR size_t mini_hash_size = 6;

// Common bitcoin hash containers.
typedef byte_array<hash_size> hash_digest;
typedef byte_array<half_hash_size> half_hash;
typedef byte_array<quarter_hash_size> quarter_hash;
typedef byte_array<long_hash_size> long_hash;
typedef byte_array<short_hash_size> short_hash;
typedef byte_array<mini_hash_size> mini_hash;

// Lists of common bitcoin hashes.
typedef std::vector<hash_digest> hash_list;
typedef std::vector<half_hash> half_hash_list;
typedef std::vector<quarter_hash> quarter_hash_list;
typedef std::vector<long_hash> long_hash_list;
typedef std::vector<short_hash> short_hash_list;
typedef std::vector<mini_hash> mini_hash_list;



// base_16.hpp

bool is_base16(const char c);

BC_API std::string encode_base16(data_slice data);

BC_API bool decode_base16(data_chunk& out, const std::string &in);

template <size_t Size>
bool decode_base16(byte_array<Size>& out, const std::string &in);

template <size_t Size>
byte_array<(Size - 1) / 2> base16_literal(const char (&string)[Size]);

BC_API std::string encode_hash(hash_digest hash);

BC_API bool decode_hash(hash_digest& out, const std::string& in);

BC_API hash_digest hash_literal(const char (&string)[2 * hash_size + 1]);

}

%}

//%include <java.swg>
%include <stdint.i>
%include <typemaps.i>

%include <std_string.i>
//%include "std_container.i"
%include <std_vector.i>
%template(ucharVector) std::vector<unsigned char>;
//%import <bitcoin/bitcoin/define.hpp>
//%import <bitcoin/bitcoin/compat.hpp>
//%include <bitcoin/bitcoin/utility/array_slice.hpp>
//%include <bitcoin/bitcoin/utility/data.hpp>
//%include <bitcoin/bitcoin/math/hash.hpp>
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



//%import "../../../../include/bitcoin/bitcoin/compat.h"
//%import "../../../../include/bitcoin/bitcoin/compat.hpp"
//%import "../../../../include/bitcoin/bitcoin/define.hpp"


