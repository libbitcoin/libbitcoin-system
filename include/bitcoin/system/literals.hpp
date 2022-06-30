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
#include <limits>
#include <type_traits>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/exceptions.hpp>

// May be used to exclude test evaluation.
// Remove this when all platforms suppoty consteval.
#if !defined(HAVE_CONSTEVAL)
    #define RUNTIME_LITERALS
#endif

namespace libbitcoin {
namespace system {
namespace literals {

/// en.cppreference.com/w/cpp/language/user_literal

/// This uses no libbitcoin utilities, so that it may have few dependencies and
/// therefore be useful everywhere in the library. It is also important that it
/// not be subject to regressions in other code, as a break here causes a large
/// number of test and other failures. See tests for usage and detailed info.
/// ---------------------------------------------------------------------------
/// These should be consteval for safety, but waiting on clang++20 to catch up.

// Casting is the whole point.
// It is consteval where available.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)

template <typename Integer,
    std::enable_if_t<!std::is_signed_v<Integer>, bool> = true>
CONSTEVAL Integer lower() NOEXCEPT
{
    return std::numeric_limits<Integer>::min();
}

template <typename Integer,
    std::enable_if_t<!std::is_signed_v<Integer>, bool> = true>
CONSTEVAL Integer upper() NOEXCEPT
{
    return std::numeric_limits<Integer>::max();
}

template <typename Integer, std::enable_if_t<
    std::is_signed_v<Integer>, bool> = true>
CONSTEVAL std::make_unsigned_t<Integer> lower() NOEXCEPT
{
    return Integer{1} + static_cast<
        std::make_unsigned_t<Integer>>(
            std::numeric_limits<Integer>::max());
}

template <typename Integer, std::enable_if_t<
    std::is_signed_v<Integer>, bool> = true>
CONSTEVAL std::make_unsigned_t<Integer> upper() NOEXCEPT
{
    return static_cast<
        std::make_unsigned_t<Integer>>(
            std::numeric_limits<Integer>::max());
}

template <typename Domain, std::enable_if_t<
    std::numeric_limits<Domain>::is_integer, bool> = true>
CONSTEVAL Domain positive(uint64_t value) THROWS
{
    typedef std::make_unsigned_t<Domain> narrow, limit;

    if (value > upper<limit>())
        throw overflow_exception{ "literal overflow" };

    const auto narrowed = static_cast<narrow>(value);
    return static_cast<Domain>(narrowed);
}

template <typename Domain, std::enable_if_t<
    std::numeric_limits<Domain>::is_integer, bool> = true>
CONSTEVAL Domain negative(uint64_t value) THROWS
{
    using limit = std::make_signed_t<Domain>;
    using narrow = std::make_unsigned_t<Domain>;

    if (value > lower<limit>())
        throw overflow_exception{ "literal overflow" };

    const auto narrowed = static_cast<narrow>(value);
    return static_cast<Domain>(~narrowed + narrow{1});
}

BC_POP_WARNING()
BC_POP_WARNING()

#define DECLARE_LITERAL(name, type, sign) \
CONSTEVAL type operator "" name(uint64_t value) THROWS \
{ return sign<type>(value); }

/// Supported represenations.
/// ---------------------------------------------------------------------------
/// All integer literals are positive, so this is what there is to customize.
/// To achieve negative representation we use a positive domain with the
/// magnitide of the negative domain with all values entered as absolute.
/// Integrals do not have negative signs, and applying the negative operator
/// to a literal changes it to an operation, which promotes the type. All
/// numeric representations are possible (binary, octal, hex, decimal) as are
/// digit separators. A built-in suffix cannot be used with a user-defined
/// suffix, and there would be no reason to.

/// positive signed integer
DECLARE_LITERAL(_i08, int8_t,  positive)
DECLARE_LITERAL(_i16, int16_t, positive)
DECLARE_LITERAL(_i32, int32_t, positive)
DECLARE_LITERAL(_i64, int64_t, positive)

/// positive unsigned integer
DECLARE_LITERAL(_u08, uint8_t,  positive)
DECLARE_LITERAL(_u16, uint16_t, positive)
DECLARE_LITERAL(_u32, uint32_t, positive)
DECLARE_LITERAL(_u64, uint64_t, positive)

/// negative signed integer
DECLARE_LITERAL(_ni08, int8_t,  negative)
DECLARE_LITERAL(_ni16, int16_t, negative)
DECLARE_LITERAL(_ni32, int32_t, negative)
DECLARE_LITERAL(_ni64, int64_t, negative)

/// negative unsigned integer
DECLARE_LITERAL(_nu08, uint8_t,  negative)
DECLARE_LITERAL(_nu16, uint16_t, negative)
DECLARE_LITERAL(_nu32, uint32_t, negative)
DECLARE_LITERAL(_nu64, uint64_t, negative)

/// size_t
DECLARE_LITERAL(_size,  size_t, positive)
DECLARE_LITERAL(_nsize, size_t, negative)

/// aliases (preferred unless vertical alignment is helpful)
DECLARE_LITERAL(_i8,  int8_t,  positive)
DECLARE_LITERAL(_u8,  uint8_t, positive)
DECLARE_LITERAL(_ni8, int8_t,  negative)
DECLARE_LITERAL(_nu8, uint8_t, negative)

/// ---------------------------------------------------------------------------

#undef DECLARE_LITERAL

} // namespace literals
} // namespace system
} // namespace libbitcoin

#endif
