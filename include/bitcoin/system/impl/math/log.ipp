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
#ifndef LIBBITCOIN_SYSTEM_MATH_LOG_IPP
#define LIBBITCOIN_SYSTEM_MATH_LOG_IPP

#include <bit>
#include <cstddef>
#include <type_traits>
// DELETEMENOW
// DELETEMENOW
// DELETEMENOW
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/division.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

namespace mp = boost::multiprecision;

// Ceilinged logarithms.
// ----------------------------------------------------------------------------
// Use depromote only for integral constrained functions.

// dispatch
template <size_t Base, typename Exponent, typename Value>
constexpr Exponent ceilinged_log(Value value) NOEXCEPT
{
    if constexpr (Base == 2u)
        return ceilinged_log2<Exponent>(value);
    else if constexpr (Base == 256u)
        return ceilinged_log256<Exponent>(value);
    else
        return ceilinged_log<Exponent>(Base, value);
}

// normal form
template <typename Exponent, typename Base, typename Value,
    if_integer<Exponent>, if_integer<Base>, if_integer<Value>>
constexpr Exponent ceilinged_log(Base base, Value value) NOEXCEPT
{
    if (base < 2 || value < 1)
        return 0;

    auto factor = possible_narrow_and_sign_cast<Value>(base);

    // Allow constexpr ceilinged_log(0).
    if (std::is_constant_evaluated() && is_zero(factor))
    {
        return 0;
    }
    else
    {
        Exponent exponent = 0;
        while (value > 0) { ++exponent; value /= factor; }
        return exponent;
    }
}

// optimizations

// Called by bc::bit_width.
template <typename Exponent, typename Value,
    if_integer<Exponent>,
    if_integral_integer<Value>>
constexpr Exponent ceilinged_log2(Value value) NOEXCEPT
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<Exponent>(
        std::bit_width(to_unsigned(value)));
}

// Called by bc::bit_width.
template <typename Exponent, typename Value,
    if_integer<Exponent>,
    if_non_integral_integer<Value>>
constexpr Exponent ceilinged_log2(Value value) NOEXCEPT
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<Exponent>(
        add1(mp::msb(value)));
}

// Called by bc::byte_width.
// Called by bc::base256 (constexpr uint32_t, for shift).
template <typename Exponent, typename Value,
    if_integer<Exponent>,
    if_integral_integer<Value>>
constexpr Exponent ceilinged_log256(Value value) NOEXCEPT
{
    if (value < 1)
        return 0;

    constexpr auto size = sizeof(Value);
    const auto compare = to_unsigned(value);

    if constexpr (size == sizeof(uint64_t))
    {
        if (compare > 0x00ffffffffffffffull) return 8;
        if (compare > 0x0000ffffffffffffull) return 7;
        if (compare > 0x000000ffffffffffull) return 6;
        if (compare > 0x00000000ffffffffull) return 5;
    }

    if constexpr (size >= sizeof(uint32_t))
    {
        if (compare > 0x00fffffful) return 4;
        if (compare > 0x0000fffful) return 3;
    }

    if constexpr (size >= sizeof(uint16_t))
    {
        if (compare > 0x00ffu) return 2;
    }

    return (compare > 0x00u) ? 1 : 0;
}

// Called by bc::byte_width.
template <typename Exponent, typename Value,
    if_integer<Exponent>,
    if_non_integral_integer<Value>>
constexpr Exponent ceilinged_log256(Value value) NOEXCEPT
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<Exponent>(
        ceilinged_divide(ceilinged_log2<Exponent>(value), 8u));
}

// Floored logarithms.
// ----------------------------------------------------------------------------
// Use depromote only for integral constrained functions.

// dispatch
template <size_t Base, typename Exponent, typename Value>
constexpr Exponent floored_log(Value value) NOEXCEPT
{
    if constexpr (Base == 2u)
        return floored_log2<Exponent>(value);
    else if constexpr (Base == 256u)
        return floored_log256<Exponent>(value);
    else
        return floored_log<Exponent>(Base, value);
}

// normal form
template <typename Exponent, typename Base, typename Value,
    if_integer<Exponent>,
    if_integer<Base>,
    if_integer<Value>>
constexpr Exponent floored_log(Base base, Value value) NOEXCEPT
{
    if (base < 2 || value < 1)
        return 0;

    auto factor = possible_narrow_and_sign_cast<Value>(base);

    // Allow constexpr floored_log(0).
    if (std::is_constant_evaluated() && is_zero(factor))
    {
        return 0;
    }
    else
    {
        Exponent exponent = 0;
        while (((value /= factor)) > 0) { ++exponent; }
        return exponent;
    }
}

// optimizations

// Called by bc::base2n (for sizing).
// Called by wallet::mnemonics (for dictionary size assertion).
template <typename Exponent, typename Value,
    if_integer<Exponent>,
    if_integral_integer<Value>>
constexpr Exponent floored_log2(Value value) NOEXCEPT
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<Exponent>(
        sub1(std::bit_width(to_unsigned(value))));
}

// Called by chain:::chain_state (for timewarp detection).
template <typename Exponent, typename Value,
    if_integer<Exponent>,
    if_non_integral_integer<Value>>
constexpr Exponent floored_log2(Value value) NOEXCEPT
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<Exponent>(
        mp::msb(value));
}

// Called by test only (for library consistency).
template <typename Exponent, typename Value,
    if_integer<Exponent>,
    if_integral_integer<Value>>
constexpr Exponent floored_log256(Value value) NOEXCEPT
{
    if (value < 1)
        return 0;

    constexpr auto size = sizeof(Value);
    const auto compare = to_unsigned(value);

    if constexpr (size == sizeof(uint64_t))
    {
        if (compare > 0x00ffffffffffffffull) return 7;
        if (compare > 0x0000ffffffffffffull) return 6;
        if (compare > 0x000000ffffffffffull) return 5;
    }

    if constexpr (size >= sizeof(uint32_t))
    {
        if (compare > 0x00fffffffful) return 4;
        if (compare > 0x0000fffffful) return 3;
    }

    if constexpr (size >= sizeof(uint16_t))
    {
        if (compare > 0xffffu) return 2;
    }

    return (compare > 0xffu) ? 1 : 0;
}

// Called by test only (for library consistency).
template <typename Exponent, typename Value,
    if_integer<Exponent>,
    if_non_integral_integer<Value>>
constexpr Exponent floored_log256(Value value) NOEXCEPT
{
    if (value < 1)
        return 0;

    return possible_narrow_and_sign_cast<Exponent>(
        floored_divide(floored_log2<Exponent>(value), 8u));
}

} // namespace system
} // namespace libbitcoin

#endif
