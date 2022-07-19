/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_STREAM_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_STREAM_HPP

#include <iostream>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// std::iostreams are not constexpr.

template <typename Integral, typename IStream = std::istream,
    if_integral_integer<Integral> = true,
    if_base_of<std::istream, IStream> = true,
    if_one_byte<typename IStream::char_type> = true>
inline Integral from_big_endian(IStream& stream) NOEXCEPT;

template <typename Integral, typename IStream = std::istream,
    if_integral_integer<Integral> = true,
    if_base_of<std::istream, IStream> = true,
    if_one_byte<typename IStream::char_type> = true>
inline Integral from_little_endian(IStream& stream) NOEXCEPT;

template <typename Integral, typename OStream = std::ostream,
    if_integral_integer<Integral> = true,
    if_base_of<std::ostream, OStream> = true,
    if_one_byte<typename OStream::char_type> = true>
inline void to_big_endian(OStream& stream, Integral value) NOEXCEPT;

template <typename Integral, typename OStream = std::ostream,
    if_integral_integer<Integral> = true,
    if_base_of<std::ostream, OStream> = true,
    if_one_byte<typename OStream::char_type> = true>
inline void to_little_endian(OStream& stream, Integral value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/stream.ipp>

#endif
