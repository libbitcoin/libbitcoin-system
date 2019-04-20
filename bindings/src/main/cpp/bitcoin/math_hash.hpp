/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN__MATH_HASH_HPP
#define LIBBITCOIN__MATH_HASH_HPP

//#include <cstddef>
//#include <string>
//#include <vector>
//#include <boost/functional/hash_fwd.hpp>
//#include <boost/multiprecision/cpp_int.hpp>
//#include <bitcoin/bitcoin/compat.hpp>
//#include <bitcoin/bitcoin/define.hpp>
//#include <bitcoin/bitcoin/utility/data.hpp>
#include <bitcoin/bitcoin/utility/endian.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>
#include <bitcoin/math_hash_digest.hpp>
#include <bitcoin/math_half_hash.hpp>
#include <bitcoin/math_quarter_hash.hpp>
#include <bitcoin/math_long_hash.hpp>
#include <bitcoin/math_short_hash.hpp>
#include <bitcoin/math_mini_hash.hpp>
#include <bitcoin/math_uint256_t.hpp>
#include <bitcoin/utility_data_slice.hpp>
#include <bitcoin/utility_data_chunk.hpp>

namespace libbitcoin {
namespace api {

// Common bitcoin hash container sizes.
//static BC_CONSTEXPR size_t hash_size = 32;
//static BC_CONSTEXPR size_t half_hash_size = hash_size / 2;
//static BC_CONSTEXPR size_t quarter_hash_size = half_hash_size / 2;
//static BC_CONSTEXPR size_t long_hash_size = 2 * hash_size;
//static BC_CONSTEXPR size_t short_hash_size = 20;
//static BC_CONSTEXPR size_t mini_hash_size = 6;

// Common bitcoin hash containers.
//typedef std::array<uint8_t, hash_size> hash_digest;
//typedef std::array<uint8_t, half_hash_size> half_hash;
//typedef std::array<uint8_t, quarter_hash_size> quarter_hash;
//typedef std::array<uint8_t, long_hash_size> long_hash;
//typedef std::array<uint8_t, short_hash_size> short_hash;
//typedef std::array<uint8_t, mini_hash_size> mini_hash;

// Lists of common bitcoin hashes.
//typedef std::vector<hash_digest> hash_list;
//typedef std::vector<half_hash> half_hash_list;
//typedef std::vector<quarter_hash> quarter_hash_list;
//typedef std::vector<long_hash> long_hash_list;
//typedef std::vector<short_hash> short_hash_list;
//typedef std::vector<mini_hash> mini_hash_list;

// Alias for boost big integer type.
//typedef boost::multiprecision::uint256_t uint256_t;

// Null-valued common bitcoin hashes.

//BC_CONSTEXPR math_hash_digest math_null_hash
//{
//	null_hash
//};
//
//BC_CONSTEXPR math_half_hash math_null_half_hash {
//	null_half_hash
//};
//
//BC_CONSTEXPR math_quarter_hash math_null_quarter_hash
//{
//	null_quarter_hash
//};
//
//BC_CONSTEXPR math_long_hash math_null_long_hash
//{
//	null_long_hash
//};
//
//BC_CONSTEXPR math_short_hash math_null_short_hash
//{
//	null_short_hash
//};
//
//BC_CONSTEXPR math_mini_hash math_null_mini_hash
//{
//	null_mini_hash
//};

inline math_uint256_t* to_uint256(const math_hash_digest& hash)
{
    return new math_uint256_t(libbitcoin::to_uint256(*hash.getValue()));
}

/// Generate a scrypt hash to fill a byte array.
template <size_t Size>
std::array<uint8_t, Size> scrypt(utility_data_slice data, utility_data_slice salt, uint64_t N,
    uint32_t p, uint32_t r);

/// Generate a scrypt hash of specified length.
BC_API utility_data_chunk scrypt(utility_data_slice data, utility_data_slice salt, uint64_t N,
    uint32_t p, uint32_t r, size_t length);

/// Generate a bitcoin hash.
BC_API math_hash_digest bitcoin_hash(utility_data_slice data);

/// Generate a scrypt hash.
BC_API math_hash_digest scrypt_hash(utility_data_slice data);

/// Generate a bitcoin short hash.
BC_API math_short_hash bitcoin_short_hash(utility_data_slice data);

/// Generate a ripemd160 hash
BC_API math_short_hash ripemd160_hash(utility_data_slice data);
BC_API utility_data_chunk ripemd160_hash_chunk(utility_data_slice data);

/// Generate a sha1 hash.
BC_API math_short_hash sha1_hash(utility_data_slice data);
BC_API utility_data_chunk sha1_hash_chunk(utility_data_slice data);

/// Generate a sha256 hash.
BC_API math_hash_digest sha256_hash(utility_data_slice data);
BC_API utility_data_chunk sha256_hash_chunk(utility_data_slice data);

/// Generate a sha256 hash.
/// This hash function was used in electrum seed stretching (obsoleted).
BC_API math_hash_digest sha256_hash(utility_data_slice first, utility_data_slice second);

// Generate a hmac sha256 hash.
BC_API math_hash_digest hmac_sha256_hash(utility_data_slice data, utility_data_slice key);

/// Generate a sha512 hash.
BC_API math_long_hash sha512_hash(utility_data_slice data);

/// Generate a hmac sha512 hash.
BC_API math_long_hash hmac_sha512_hash(utility_data_slice data, utility_data_slice key);

/// Generate a pkcs5 pbkdf2 hmac sha512 hash.
BC_API math_long_hash pkcs5_pbkdf2_hmac_sha512(utility_data_slice passphrase,
    utility_data_slice salt, size_t iterations);

} // namespace api
} // namespace libbitcoin

// Extend std and boost namespaces with our hash wrappers.
//-----------------------------------------------------------------------------

//namespace std
//{
//template <size_t Size>
//struct hash<std::array<uint8_t, Size>>
//{
//    size_t operator()(const std::array<uint8_t, Size>& hash) const
//    {
//        return boost::hash_range(hash.begin(), hash.end());
//    }
//};
//} // namespace std

//namespace boost
//{
//template <size_t Size>
//struct hash<std::array<uint8_t, Size>>
//{
//    size_t operator()(const std::array<uint8_t, Size>& hash) const
//    {
//        return boost::hash_range(hash.begin(), hash.end());
//    }
//};
//} // namespace boost

//#include <bitcoin/bitcoin/impl/math/hash.ipp>

#endif
