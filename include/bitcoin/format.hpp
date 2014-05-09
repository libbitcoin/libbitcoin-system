/*
 * Copyright (c) 2011-2013 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_FORMAT_HPP
#define LIBBITCOIN_FORMAT_HPP

#include <type_traits>
#include <boost/range/iterator_range.hpp>
#include <boost/range/sub_range.hpp>
#include <boost/detail/endian.hpp>
#include <bitcoin/define.hpp>
#include <bitcoin/constants.hpp>
#include <bitcoin/types.hpp>
#include <bitcoin/utility/assert.hpp>

namespace libbitcoin {

// Forwarding definitions because primitives.hpp depends on this header.
struct output_point;
// We don't need a stream operator for input_point:
typedef output_point input_point;

template <typename D, typename T>
void extend_data(D& chunk, const T& other)
{
    chunk.insert(std::end(chunk), std::begin(other), std::end(other));
}

#define VERIFY_UNSIGNED(T) static_assert(std::is_unsigned<T>::value, \
    "The endian functions only work on unsigned types")

template <typename T, typename Iterator>
T from_big_endian(Iterator in)
{
    VERIFY_UNSIGNED(T);
    T out = 0;
    size_t i = sizeof(T);
    while (0 < i)
        out |= static_cast<T>(*in++) << (8 * --i);
    return out;
}

template <typename T, typename Iterator>
T from_little_endian(Iterator in)
{
    VERIFY_UNSIGNED(T);
    T out = 0;
    size_t i = 0;
    while (i < sizeof(T))
        out |= static_cast<T>(*in++) << (8 * i++);
    return out;
}

template <typename T>
byte_array<sizeof(T)> to_big_endian(T n)
{
    VERIFY_UNSIGNED(T);
    byte_array<sizeof(T)> out;
    for (auto i = out.rbegin(); i != out.rend(); ++i)
    {
        *i = n;
        n >>= 8;
    }
    return out;
}

template <typename T>
byte_array<sizeof(T)> to_little_endian(T n)
{
    VERIFY_UNSIGNED(T);
    byte_array<sizeof(T)> out;
    for (auto i = out.begin(); i != out.end(); ++i)
    {
        *i = n;
        n >>= 8;
    }
    return out;
}

#undef VERIFY_UNSIGNED

template <typename T>
T cast_chunk(data_chunk chunk, bool reverse=false)
{
    if (reverse)
        return from_big_endian<T>(chunk.begin());
    else
        return from_little_endian<T>(chunk.begin());
}

template <typename T>
data_chunk uncast_type(T value, bool reverse=false)
{
    if (reverse)
        return to_data_chunk(to_big_endian(value));
    else
        return to_data_chunk(to_little_endian(value));
}

template <typename T>
std::string encode_hex(T data)
{
    std::stringstream ss;
    ss << std::hex;
    for (int val: data)
        ss << std::setw(2) << std::setfill('0') << val;
    return ss.str();
}

// ADL cannot work on templates
BC_API std::ostream& operator<<(
    std::ostream& stream, const data_chunk& data);
BC_API std::ostream& operator<<(
    std::ostream& stream, const hash_digest& hash);
BC_API std::ostream& operator<<(
    std::ostream& stream, const short_hash& hash);
BC_API std::ostream& operator<<(
    std::ostream& stream, const output_point& point);

BC_API data_chunk decode_hex(std::string hex_str);

/**
 * Turns a hash hex string into HashType.
 * byte_stream.size() == 2 * HashType.size()
 *
 * On error, returns null_hash for hash_digest, or
 * null_short_hash for short_hash.
 */
template <typename HashType>
HashType decode_hex_digest(std::string hex_str)
{
    data_chunk raw_bytes = decode_hex(hex_str);
    HashType result;
    if (raw_bytes.size() != result.size())
    {
        // null_hash for hash_digest
        // null_short_hash for short_hash
        result.fill(0);
        return result;
    }
    BITCOIN_ASSERT(raw_bytes.size() == result.size());
    std::copy(raw_bytes.begin(), raw_bytes.end(), result.begin());
    return result;
}

/**
 * Convert a hex string into hash bytes.
 * On error, returns null_hash.
 */
BC_API hash_digest decode_hash(const std::string& hex);

/**
 * Convert a hex string into short hash bytes.
 * On error, returns null_short_hash.
 */
BC_API short_hash decode_short_hash(const std::string& hex);

BC_API std::string satoshi_to_btc(uint64_t value);

} // namespace libbitcoin

#endif

