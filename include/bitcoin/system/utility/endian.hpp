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
#ifndef LIBBITCOIN_ENDIAN_HPP
#define LIBBITCOIN_ENDIAN_HPP

#include <istream>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

namespace libbitcoin {

template <typename Integer, typename Iterator>
Integer from_big_endian(Iterator start, Iterator end);

template <typename Integer, typename Iterator>
Integer from_little_endian(Iterator start, Iterator end);

template <typename Integer, typename Iterator>
Integer from_big_endian_unsafe(Iterator start);

template <typename Integer, typename Iterator>
Integer from_little_endian_unsafe(Iterator start);

template <typename Integer>
Integer from_big_endian_stream_unsafe(std::istream& stream);

template <typename Integer>
Integer from_little_endian_stream_unsafe(std::istream& stream);

template <typename Integer>
byte_array<sizeof(Integer)> to_big_endian(Integer value);

template <typename Integer>
byte_array<sizeof(Integer)> to_little_endian(Integer value);

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/utility/endian.ipp>

#endif
