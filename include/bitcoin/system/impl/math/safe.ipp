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
#ifndef LIBBITCOIN_SYSTEM_MATH_SAFE_IPP
#define LIBBITCOIN_SYSTEM_MATH_SAFE_IPP

#include <exception>
#include <limits>
#include <type_traits>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// Explicit integral casts.
// ----------------------------------------------------------------------------

template <typename Restored, typename Common,
    if_not_lesser_size<to_common_sized_type<Restored>, Restored>,
    if_integral_integer<Restored>,
    if_integral_integer<Common>>
constexpr Restored depromote(Common value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_IDENTITY_CAST)
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<Restored>(value);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_lesser_size<To, From>,
    if_integral_integer<To>,
    if_integral_integer<From>,
    if_same_signed_integer<To, From>>
constexpr To narrow_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_not_lesser_size<To, From>,
    if_integral_integer<To>,
    if_integral_integer<From>,
    if_not_same_signed_integer<To, From>>
constexpr To sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_lesser_size<To, From>,
    if_integral_integer<To>,
    if_integral_integer<From>,
    if_not_same_signed_integer<To, From>>
constexpr To narrow_sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From,
    if_lesser_size<From, To>,
    if_same_signed_integer<To, From>>
constexpr To wide_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

// Possible integer casts.
// ----------------------------------------------------------------------------

template <typename To, typename From, if_same_signed_integer<To, From>>
constexpr To possible_narrow_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From, if_not_lesser_size<To, From>>
constexpr To possible_sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From, if_not_same_signed_integer<To, From>>
constexpr To possible_narrow_sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From, if_lesser_size<To, From>>
constexpr To possible_sign_narrow_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From>
constexpr To possible_narrow_and_sign_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
}

template <typename To, typename From, if_same_signed_integer<To, From>>
constexpr To possible_wide_cast(From value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_IDENTITY_CAST)
    BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)
    return static_cast<To>(value);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

// Explicit pointer casts.
// ----------------------------------------------------------------------------

template <typename To, typename From>
inline To* pointer_cast(From* value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return reinterpret_cast<To*>(value);
    BC_POP_WARNING()
}

template <typename To, typename From>
inline To* possible_pointer_cast(From* value) NOEXCEPT
{
    BC_PUSH_WARNING(NO_IDENTITY_CAST)
    BC_PUSH_WARNING(NO_REINTERPRET_CAST)
    return reinterpret_cast<To*>(value);
    BC_POP_WARNING()
    BC_POP_WARNING()
}

// Byte casts.
// ----------------------------------------------------------------------------

template <typename Integral,
    typename if_integral_integer<Integral>>
constexpr std::array<uint8_t, sizeof(Integral)>&
byte_cast(Integral& value) NOEXCEPT
{
    return *pointer_cast<std::array<uint8_t, sizeof(Integral)>>(&value);
}

template <typename Integral,
    typename if_integral_integer<Integral>>
constexpr const std::array<uint8_t, sizeof(Integral)>&
byte_cast(const Integral& value) NOEXCEPT
{
    return *pointer_cast<const std::array<uint8_t, sizeof(Integral)>>(&value);
}

template <size_t Size,
    typename if_integral_size<Size>>
constexpr unsigned_type<Size>&
byte_cast(std::array<uint8_t, Size>& value) NOEXCEPT
{
    return *pointer_cast<unsigned_type<Size>>(&value);
}

template <size_t Size,
    typename if_integral_size<Size>>
constexpr const unsigned_type<Size>&
byte_cast(const std::array<uint8_t, Size>& value) NOEXCEPT
{
    return *pointer_cast<const unsigned_type<Size>>(&value);
}

// Array casts.
// ----------------------------------------------------------------------------

// So passing an empty std::array is allowed.
// nullptr reinterpret_cast (at array size zero) is safe.
static_assert(std::array<uint8_t, 0>{}.data() == nullptr);

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_not_lesser<max_size_t / sizeof(From), Size>>
constexpr std::array<To, (Size * sizeof(From)) / sizeof(To)>&
array_cast(std::array<From, Size>& values) NOEXCEPT
{
    using to = std::array<To, (Size * sizeof(From)) / sizeof(To)>;
    return *pointer_cast<to>(values.data());
}

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_not_lesser<max_size_t / sizeof(From), Size>>
constexpr const std::array<To, (Size * sizeof(From)) / sizeof(To)>&
array_cast(const std::array<From, Size>& values) NOEXCEPT
{
    using to = std::array<To, (Size * sizeof(From)) / sizeof(To)>;
    return *pointer_cast<const to>(values.data());
}

template <typename To, size_t ToSize, typename From, size_t FromSize,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_not_lesser<max_size_t / sizeof(To), ToSize>,
    if_not_lesser<max_size_t / sizeof(From), FromSize>,
    if_lesser<ToSize * sizeof(To), FromSize * sizeof(From)>>
constexpr std::array<To, ToSize>&
narrowing_array_cast(std::array<From, FromSize>& values) NOEXCEPT
{
    using to = std::array<To, ToSize>;
    return *pointer_cast<to>(values.data());
}

template <typename To, size_t ToSize, typename From, size_t FromSize,
    if_integral_integer<From>,
    if_integral_integer<To>,
    if_not_lesser<max_size_t / sizeof(To), ToSize>,
    if_not_lesser<max_size_t / sizeof(From), FromSize>,
    if_lesser<ToSize * sizeof(To), FromSize * sizeof(From)>>
constexpr const std::array<To, ToSize>&
narrowing_array_cast(const std::array<From, FromSize>& values) NOEXCEPT
{
    using to = std::array<To, ToSize>;
    return *pointer_cast<const to>(values.data());
}

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
constexpr std::array<To, Size>&
unsafe_array_cast(From bytes[]) NOEXCEPT
{
    return *pointer_cast<std::array<To, Size>>(&bytes[0]);
}

// nullptr reinterpret_cast (at array size zero) is safe.
// &bytes[0] is undefined, so zero size is excluded.
template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
constexpr const std::array<To, Size>&
unsafe_array_cast(const From bytes[]) NOEXCEPT
{
    return *pointer_cast<const std::array<To, Size>>(&bytes[0]);
}

// Overflows.
// ----------------------------------------------------------------------------

// Abort is intended.
BC_PUSH_WARNING(THROW_FROM_NOEXCEPT)

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer safe_multiply(Integer left, Integer right) THROWS
{
    if (is_zero(left) || is_zero(right))
        return 0;

    if (left > (std::numeric_limits<Integer>::max() / right))
        throw overflow_exception("safe multiplication overflow");

    return possible_narrow_and_sign_cast<Integer>(left * right);
}

template <typename Integer, if_unsigned_integer<Integer>>
constexpr Integer safe_add(Integer left, Integer right) THROWS
{
    if (left > (std::numeric_limits<Integer>::max() - right))
        throw overflow_exception("safe addition overflow");

    return possible_narrow_and_sign_cast<Integer>(left + right);
}

template <typename Integer, if_signed_integer<Integer>>
constexpr Integer safe_negate(Integer value) THROWS
{
    // TODO: review full codebase.
    ////if (value == std::numeric_limits<Integer>::min())
    ////    throw overflow_exception("safe negate overflow");
    BC_ASSERT_MSG(value != std::numeric_limits<Integer>::min(),
        "negation overflow");

    return possible_narrow_and_sign_cast<Integer>(-value);
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
