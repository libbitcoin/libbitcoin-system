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
#ifndef LIBBITCOIN_SYSTEM_MATH_POWER_IPP
#define LIBBITCOIN_SYSTEM_MATH_POWER_IPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/bits.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/overflow.hpp>
#include <bitcoin/system/math/sign.hpp>

namespace libbitcoin {
namespace system {

// unpublished (normal form)
// ----------------------------------------------------------------------------

// Called by bc::base85 (for number coding).
// Called by wallet::electrum_v1 (for number coding).
template <typename Value, typename Base, typename Exponent,
    if_integer<Value> = true,
    if_integer<Base> = true,
    if_unsigned_integer<Exponent> = true>
constexpr Value power_(Base base, Exponent exponent) NOEXCEPT
{
    if (is_power_overflow(base, exponent))
        return 0;

    if (is_zero(exponent))
        return 1;

    auto value = possible_narrow_and_sign_cast<Value>(base);

    // Overflow is allowed behavior as this models a mathematical operator.
    BC_PUSH_WARNING(NARROWING_CONVERSION)
    BC_PUSH_WARNING(SIZE_NARROWING_CONVERSION)
    while (--exponent > 0u) { value *= base; }
    BC_POP_WARNING();
    BC_POP_WARNING();

    return value;
}

// published
// ----------------------------------------------------------------------------

// dispatch
template <size_t Base, typename Value, typename Exponent,
    if_unsigned_integer<Exponent>>
constexpr Value power(Exponent exponent) NOEXCEPT
{
    if constexpr (Base == 2u)
        return power2<Value>(exponent);
    else
        return power<Value>(Base, exponent);
}

template <typename Value, typename Base, typename Exponent,
    if_integer<Value>,
    if_signed_integer<Base>,
    if_unsigned_integer<Exponent>>
constexpr Value power(Base base, Exponent exponent) NOEXCEPT
{
    return power_<Value>(base, exponent);
}

template <typename Value, typename Base, typename Exponent,
    if_integer<Value>,
    if_unsigned_integer<Base>,
    if_unsigned_integer<Exponent>>
constexpr Value power(Base base, Exponent exponent) NOEXCEPT
{
    return power_<Value>(base, exponent);
}

// Called by bc::props (for json number limits).
// Called by chain::number (for limits::minimum/maximum).
template <typename Value, typename Exponent,
    if_integral_integer<Value>,
    if_unsigned_integer<Exponent>>
constexpr Value power2(Exponent exponent) NOEXCEPT
{
    // base2 integral optimization over normal form.
    return shift_left<Value>(1u, to_unsigned(exponent), true);
}

template <typename Value, typename Exponent,
    if_non_integral_integer<Value>,
    if_unsigned_integer<Exponent>>
constexpr Value power2(Exponent exponent) NOEXCEPT
{
    Value value{};

    // base2 uintx optimization over normal form.
    return boost::multiprecision::bit_set(value,
        possible_narrow_and_sign_cast<unsigned>(exponent));
}

} // namespace system
} // namespace libbitcoin

#endif
