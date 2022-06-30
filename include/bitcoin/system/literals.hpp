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
#include <cstdlib>
#include <iostream>
#include <typeinfo>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/exceptions.hpp>

// TODO: move to math.
#include <bitcoin/system/math/math.hpp>

#define DECLARE_LITERAL(name, type, sign) \
CONSTEVAL type operator "" name(uint64_t value) noexcept \
{ return sign<type>(value); }

namespace libbitcoin {
namespace system {

/// en.cppreference.com/w/cpp/language/user_literal

// Abort is intended.
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

template <typename Domain, if_integral_integer<Domain> = true>
constexpr void overflow(uint64_t value, bool positive) noexcept(false)
{
    if (std::is_constant_evaluated())
    {
        // Under consteval this remains for compile time validation.
        throw overflow_exception{ "literal overflow" };
    }
    else
    {
        // Under consteval this is unreachable code.
        BC_ASSERT_MSG(false, "literal overflow");
        const auto sign = positive ? "positive" : "negative";
        const std::type_info& type = typeid(Domain);
        ////const auto number = encode_base16(to_big_endian_size(value));
        std::cout
            << "User-defined literal overflow: " << sign << "<"
            << type.name() << ">" << "(0x" << value << ")." << std::endl;
    }
}

template <typename Domain, if_integral_integer<Domain> = true>
CONSTEVAL Domain positive(uint64_t value) noexcept(BC_NO_THROW)
{
    typedef to_unsigned_type<Domain> narrow, limit;
    if (value > unsigned_maximum<limit>()) overflow<Domain>(value, true);
    const auto narrowed = possible_narrow_and_sign_cast<narrow>(value);
    return possible_narrow_and_sign_cast<Domain>(narrowed);
}

template <typename Domain, if_integral_integer<Domain> = true>
CONSTEVAL Domain negative(uint64_t value) noexcept(BC_NO_THROW)
{
    using limit = to_signed_type<Domain>;
    using narrow = to_unsigned_type<Domain>;
    if (value > absolute_minimum<limit>()) overflow<Domain>(value, false);
    const auto narrowed = possible_narrow_and_sign_cast<narrow>(value);
    return possible_narrow_and_sign_cast<Domain>(twos_complement(narrowed));
}

BC_POP_WARNING()

namespace literals {

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

} // namespace literals
} // namespace system
} // namespace libbitcoin

#undef DECLARE_LITERAL

#endif
