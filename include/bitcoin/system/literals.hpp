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
#ifndef LIBBITCOIN_SYSTEM_LITERALS_HPP
#define LIBBITCOIN_SYSTEM_LITERALS_HPP

#include <cstddef>
#include <cstdint>
#include <exception>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>

// TODO: move to math.
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// en.cppreference.com/w/cpp/language/user_literal

template <typename Domain, if_integral_integer<Domain> = true>
CONSTEVAL Domain positive(uint64_t value) noexcept
{
    using narrow = to_unsigned_type<Domain>;
    if (value > unsigned_maximum<Domain>()) std::terminate();
    const auto narrowed = possible_narrow_and_sign_cast<narrow>(value);
    return possible_narrow_and_sign_cast<Domain>(narrowed);
}

template <typename Domain, if_integral_integer<Domain> = true>
CONSTEVAL Domain negative(uint64_t value) noexcept
{
    using narrow = to_unsigned_type<Domain>;
    if (value > absolute_minimum<Domain>()) std::terminate();
    const auto narrowed = possible_narrow_and_sign_cast<narrow>(value);
    return possible_narrow_and_sign_cast<Domain>(twos_complement(narrowed));
}

#define DECLARE_LITERAL(name, type, sign) \
CONSTEVAL type operator "" name(uint64_t value) noexcept \
{ \
    return sign<type>(value); \
}

DECLARE_LITERAL(_i08, int8_t, positive)
DECLARE_LITERAL(_i16, int16_t, positive)
DECLARE_LITERAL(_i32, int32_t, positive)
DECLARE_LITERAL(_i64, int64_t, positive)

DECLARE_LITERAL(_u08, uint8_t, positive)
DECLARE_LITERAL(_u16, uint16_t, positive)
DECLARE_LITERAL(_u32, uint32_t, positive)
DECLARE_LITERAL(_u64, uint64_t, positive)

DECLARE_LITERAL(_ni08, int8_t, negative)
DECLARE_LITERAL(_ni16, int16_t, negative)
DECLARE_LITERAL(_ni32, int32_t, negative)
DECLARE_LITERAL(_ni64, int64_t, negative)

DECLARE_LITERAL(_nu08, uint8_t, negative)
DECLARE_LITERAL(_nu16, uint16_t, negative)
DECLARE_LITERAL(_nu32, uint32_t, negative)
DECLARE_LITERAL(_nu64, uint64_t, negative)

// Aliases
DECLARE_LITERAL(_i8, int8_t, positive)
DECLARE_LITERAL(_u8, uint8_t, positive)
DECLARE_LITERAL(_ni8, int8_t, negative)
DECLARE_LITERAL(_nu8, uint8_t, negative)
DECLARE_LITERAL(_size, size_t, positive)
DECLARE_LITERAL(_nsize, size_t, negative)

#undef DECLARE_LITERAL

} // namespace system
} // namespace libbitcoin

#endif
