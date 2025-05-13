/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

#include <limits>
#include <type_traits>
#include <bitcoin/system/constants.hpp>

// RUNTIME_LITERALS may be used to exclude test evaluation (until consteval).
#if !defined(HAVE_CONSTEVAL)
    #define RUNTIME_LITERALS
#endif

namespace libbitcoin {

/// This uses no libbitcoin utilities, so that it may have few dependencies and
/// therefore be useful everywhere in the library. It is also important that it
/// not be subject to regressions in other code, as a break here causes a large
/// number of test and other failures. See tests for usage and detailed info.
/// Functions should all be consteval for safety, but waiting on clang++ v17.
/// ---------------------------------------------------------------------------

// Functions are consteval where available (bogus warning).
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)

// Casting arithmetic expressions is the whole point here.
BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)

/// en.cppreference.com/w/cpp/language/user_literal
using integer_type = unsigned long long int;

template <typename Integer,
    std::enable_if_t<!std::is_signed_v<Integer>, bool> = true>
CONSTEVAL Integer lower() noexcept
{
    return std::numeric_limits<Integer>::min();
}

template <typename Integer,
    std::enable_if_t<!std::is_signed_v<Integer>, bool> = true>
CONSTEVAL Integer upper() noexcept
{
    return std::numeric_limits<Integer>::max();
}

template <typename Integer, std::enable_if_t<
    std::is_signed_v<Integer>, bool> = true>
CONSTEVAL std::make_unsigned_t<Integer> lower() noexcept
{
    // unsigned(|signed_min|) = unsigned(signed_min) + 1.
    return static_cast<
        std::make_unsigned_t<Integer>>(
            std::numeric_limits<Integer>::max()) + 1u;
}

template <typename Integer, std::enable_if_t<
    std::is_signed_v<Integer>, bool> = true>
CONSTEVAL std::make_unsigned_t<Integer> upper() noexcept
{
    // unsigned(|signed_max|) = unsigned(signed_max).
    return static_cast<
        std::make_unsigned_t<Integer>>(
            std::numeric_limits<Integer>::max());
}

template <typename Domain, std::enable_if_t<
    std::numeric_limits<Domain>::is_integer, bool> = true>
CONSTEVAL Domain positive(integer_type value) noexcept(false)
{
    using limit = std::make_unsigned_t<Domain>;
    using narrow = std::make_unsigned_t<Domain>;

    if (value > upper<limit>())
        throw overflow_exception{ "literal overflow" };

    // Cast to unsigned width of domain.
    const auto narrowed = static_cast<narrow>(value);

    // Cast to sign of domain.
    return static_cast<Domain>(narrowed);
}

template <typename Domain, std::enable_if_t<
    std::numeric_limits<Domain>::is_integer, bool> = true>
CONSTEVAL Domain negative(integer_type value) noexcept(false)
{
    using limit = std::make_signed_t<Domain>;
    using narrow = std::make_unsigned_t<Domain>;

    if (value > lower<limit>())
        throw overflow_exception{ "literal overflow" };

    // Cast to unsigned width of domain.
    const auto narrowed = static_cast<narrow>(value);

    // Invert sign in the unsigned domain (safe) and cast to sign of domain.
    return static_cast<Domain>(~narrowed + narrow{1});
}

template <size_t Size>
struct string_holder
{
    char str[Size];

    CONSTEVAL string_holder(const char(&string)[Size]) noexcept
    {
        for (size_t i = 0; i < Size; ++i)
            str[i] = string[i];
    }
};

BC_POP_WARNING()
BC_POP_WARNING()

/// Text representations.
/// ---------------------------------------------------------------------------

static_assert(__cpp_nontype_template_args >= 201911);

template <text_t Text>
CONSTEVAL auto operator "" _array() noexcept
{
    return Text.data;
}

/// Integer representations.
/// ---------------------------------------------------------------------------
/// All integer literals are positive, so this is what there is to customize.
/// Literals do not have negative signs, and applying the negative operator to
/// a literal creates an aritmetic expression which promotes the literal type.
/// To achieve negative representation we use a positive domain with the
/// magnitide of the negative domain, and all values entered as absolute. All
/// numeric representations are possible (binary, octal, hex, decimal) as are
/// digit separators. A built-in suffix cannot be used with a user-defined
/// suffix, and there would be no reason to do so.

#define DECLARE_INTEGER_LITERAL(name, sign, type) \
CONSTEVAL type operator "" name(integer_type value) noexcept \
{ return sign<type>(value); }

/// Literals suppress exception, causing runtime abort if !defined(CONSTEVAL).
/// This precludes unnecessary warning on each literal in noexcept functions.
BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

/// positive signed integer
DECLARE_INTEGER_LITERAL(_i08, positive, int8_t)
DECLARE_INTEGER_LITERAL(_i16, positive, int16_t)
DECLARE_INTEGER_LITERAL(_i32, positive, int32_t)
DECLARE_INTEGER_LITERAL(_i64, positive, int64_t)

/// positive unsigned integer
DECLARE_INTEGER_LITERAL(_u08, positive, uint8_t)
DECLARE_INTEGER_LITERAL(_u16, positive, uint16_t)
DECLARE_INTEGER_LITERAL(_u32, positive, uint32_t)
DECLARE_INTEGER_LITERAL(_u64, positive, uint64_t)

/// negative signed integer
DECLARE_INTEGER_LITERAL(_ni08, negative, int8_t)
DECLARE_INTEGER_LITERAL(_ni16, negative, int16_t)
DECLARE_INTEGER_LITERAL(_ni32, negative, int32_t)
DECLARE_INTEGER_LITERAL(_ni64, negative, int64_t)

/// negative unsigned integer
DECLARE_INTEGER_LITERAL(_nu08, negative, uint8_t)
DECLARE_INTEGER_LITERAL(_nu16, negative, uint16_t)
DECLARE_INTEGER_LITERAL(_nu32, negative, uint32_t)
DECLARE_INTEGER_LITERAL(_nu64, negative, uint64_t)

/// aliases (preferred)
DECLARE_INTEGER_LITERAL(_i8,  positive, int8_t)
DECLARE_INTEGER_LITERAL(_u8,  positive, uint8_t)
DECLARE_INTEGER_LITERAL(_ni8, negative, int8_t)
DECLARE_INTEGER_LITERAL(_nu8, negative, uint8_t)

/// size_t
DECLARE_INTEGER_LITERAL(_size, positive, size_t)
DECLARE_INTEGER_LITERAL(_nsize, negative, signed_size_t)

BC_POP_WARNING()

/// ---------------------------------------------------------------------------

#undef DECLARE_INTEGER_LITERAL

} // namespace libbitcoin

#endif
