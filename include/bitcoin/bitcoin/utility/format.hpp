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

#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/primitives.hpp>
#include <bitcoin/bitcoin/math/hash.hpp>

namespace libbitcoin {

/// Extend the data by appending other.
template <typename D, typename T>
void extend_data(D& data, const T& other);

template <typename T, typename Iterator>
T from_big_endian(Iterator start, Iterator end);

template <typename T, typename Iterator>
T from_little_endian(Iterator start, Iterator end);

template <typename T, typename Iterator>
T from_big_endian_unsafe(Iterator in);

template <typename T, typename Iterator>
T from_little_endian_unsafe(Iterator in);

template <typename T>
byte_array<sizeof(T)> to_big_endian(T n);

template <typename T>
byte_array<sizeof(T)> to_little_endian(T n);

byte_array<1> to_byte(uint8_t byte);

// ADL cannot work on templates
BC_API std::ostream& operator<<(
    std::ostream& stream, const data_chunk& data);
BC_API std::ostream& operator<<(
    std::ostream& stream, const hash_digest& hash);
BC_API std::ostream& operator<<(
    std::ostream& stream, const short_hash& hash);
BC_API std::ostream& operator<<(
    std::ostream& stream, const point_type& point);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/format.ipp>

#endif
