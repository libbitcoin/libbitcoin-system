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
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/constants.hpp>
#include <bitcoin/bitcoin/types.hpp>
#include <bitcoin/bitcoin/utility/assert.hpp>

namespace libbitcoin {

// Forwarding definitions because primitives.hpp depends on this header.
struct output_point;
// We don't need a stream operator for input_point:
typedef output_point input_point;

/// Extend the data by appending other.
template <typename D, typename T>
void extend_data(D& data, const T& other);

template <typename T, typename Iterator>
T from_big_endian(Iterator in);

template <typename T, typename Iterator>
T from_little_endian(Iterator in);

template <typename T>
byte_array<sizeof(T)> to_big_endian(T n);

template <typename T>
byte_array<sizeof(T)> to_little_endian(T n);

/// Convert data into a user readable hex string.
template <typename T>
std::string encode_hex(T data);

// ADL cannot work on templates
BC_API std::ostream& operator<<(
    std::ostream& stream, const data_chunk& data);
BC_API std::ostream& operator<<(
    std::ostream& stream, const hash_digest& hash);
BC_API std::ostream& operator<<(
    std::ostream& stream, const short_hash& hash);
BC_API std::ostream& operator<<(
    std::ostream& stream, const output_point& point);

/**
 * Convert a hex string into bytes.
 * On error, result.empty() is true.
 */
BC_API data_chunk decode_hex(std::string hex);

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

/**
 * Convert BTC into number of satoshi. Returns true if successul.
 */
BC_API bool btc_to_satoshi(uint64_t& satoshi, const std::string& btc);

/**
 * Convert satoshis into human readable BTC string value.
 */
BC_API std::string satoshi_to_btc(uint64_t satoshi);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/format.ipp>

#endif