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
#ifndef LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_HPP
#define LIBBITCOIN_SYSTEM_DATA_BYTE_CAST_HPP

#include <array>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Byte casts (require pointer_cast, not constexpr).
/// ---------------------------------------------------------------------------
/// C++20: std::bit_cast is a copy, not a cast, these are true casts.

/// Cast integral& to a byte array& with byte length of the integral.
template <typename Byte = uint8_t, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline std::array<Byte, sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT;

/// Cast const integral& to const byte array& with byte length of the integral.
template <typename Byte = uint8_t, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline const std::array<Byte, sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT;

/// Cast integral&& to byte array with byte length of the integral.
template <typename Byte = uint8_t, typename Integral,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline std::array<Byte, sizeof(Integral)>
byte_cast(Integral&& value) NOEXCEPT;

/// Cast byte array& to unsigned integral& of same byte length.
template <typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_size<Size> = true>
inline unsigned_type<Size>&
byte_cast(std::array<Byte, Size>& bytes) NOEXCEPT;

/// Cast const byte array& to const unsigned integral& of same byte length.
template <typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_size<Size> = true>
inline const unsigned_type<Size>&
byte_cast(const std::array<Byte, Size>& bytes) NOEXCEPT;

/// Cast byte array&& to unsigned integral of same byte length.
template <typename Byte, size_t Size,
    if_one_byte<Byte> = true,
    if_integral_size<Size> = true>
inline unsigned_type<Size>
byte_cast(std::array<Byte, Size>&& bytes) NOEXCEPT;

/// Cast byte* to integral&.
template <typename Integral, typename Byte,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline Integral& unsafe_byte_cast(Byte* bytes) NOEXCEPT;

/// Cast const byte* to const integral&.
template <typename Integral, typename Byte,
    if_one_byte<Byte> = true,
    if_integral_integer<Integral> = true>
inline const Integral& unsafe_byte_cast(const Byte* bytes) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/byte_cast.ipp>

#endif
