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

#include <bitcoin/system/data/data_array.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Byte casts (require pointer_cast, not constexpr).
/// ---------------------------------------------------------------------------
/// C++20: std::bit_cast is a copy, not a cast, these are true casts.

/// Cast integral& to a data array& with byte length of the integral.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr data_array<sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT;

/// Cast const integral& to a const data array& with byte length of the integral.
template <typename Integral, if_integral_integer<Integral> = true>
constexpr const data_array<sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT;

/// Cast data array& to unsigned integral& of same byte length.
template <size_t Size, if_integral_size<Size> = true>
constexpr unsigned_type<Size>&
byte_cast(data_array<Size>& value) NOEXCEPT;

/// Cast const data array& to const unsigned integral& of same byte length.
template <size_t Size, if_integral_size<Size> = true>
constexpr const unsigned_type<Size>&
byte_cast(const data_array<Size>& value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/byte_cast.ipp>

#endif
