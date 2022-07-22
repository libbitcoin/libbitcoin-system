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
#ifndef LIBBITCOIN_SYSTEM_DATA_ARRAY_CAST_HPP
#define LIBBITCOIN_SYSTEM_DATA_ARRAY_CAST_HPP

#include <functional>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

/// C++20: std::bit_cast is a copy, not a cast, these are true casts.
/// True casts (not projections) of arrays, with total size and element
/// size constraints to preclude referencing fractional elements. A const
/// reference parameter returns a const array reference, non-const returns
/// a non-const array reference, and an rvalue array parameter returns a cast
/// instance. These enable C/pointer-style performance (no copies) with full
/// type safety at no cost (reinterpret_cast does not compile to any CPU
/// instructions).

/// Only arrays of integrals (or arrays of arrays of integrals) may currently
/// be cast (constrained by the size_of() typelet). Overloads to this function
/// alone can enable array-casting other types. Use byte_cast for casting
/// integrals to/from arrays of single byte types.

namespace libbitcoin {
namespace system {

/// Array casting helpers.
/// ---------------------------------------------------------------------------

/// portional (for narrowing constraint)
template <typename To, typename From>
constexpr bool is_portional(size_t to_count, size_t from_count) NOEXCEPT;
template <size_t ToCount, typename To, size_t FromCount, typename From>
using if_portional = bool_if<is_portional<To, From>(ToCount, FromCount)>;

/// proportional (for non-narrowing constrained size deduction)
template <typename From, typename To>
constexpr bool is_proportional(size_t from_count) NOEXCEPT;
template <size_t FromCount, typename From, typename To>
using if_proportional = bool_if<is_proportional<From, To>(FromCount)>;
template <size_t FromCount, typename From, typename To,
    if_proportional<FromCount, From, To> = true>
constexpr size_t proportion = (FromCount * size_of<From>()) / size_of<To>();

/// Cast array(T1)& to same-sized array(T2)&.
/// ---------------------------------------------------------------------------
/// Casting fractional portions of element T1 is precluded by "proportion".

template <typename To, size_t Count, typename From>
inline std_array<To, proportion<Count, From, To>>&
array_cast(std_array<From, Count>& values) NOEXCEPT;

template <typename To, size_t Count, typename From>
inline const std_array<To, proportion<Count, From, To>>&
array_cast(const std_array<From, Count>& values) NOEXCEPT;

template <typename To, size_t Count, typename From>
inline std_array<To, proportion<Count, From, To>>
array_cast(std_array<From, Count>&& values) NOEXCEPT;

/// Cast array(T1)& to not-greater-sized array(T2, Size)&.
/// ---------------------------------------------------------------------------
/// Casting fractional portions of element T1 is precluded by "if_portional".
/// Portional if ToCount*ToSize is <= of FromCount*FromSize.

template <typename To, size_t ToCount = one, typename From, size_t FromCount,
    if_portional<ToCount, To, FromCount, From> = true>
inline std_array<To, ToCount>&
narrow_array_cast(std_array<From, FromCount>& values) NOEXCEPT;

template <typename To, size_t ToCount = one, typename From, size_t FromCount,
    if_portional<ToCount, To, FromCount, From> = true>
inline const std_array<To, ToCount>&
narrow_array_cast(const std_array<From, FromCount>& values) NOEXCEPT;

template <typename To, size_t ToCount = one, typename From, size_t FromCount,
    if_portional<ToCount, To, FromCount, From> = true>
inline std_array<To, ToCount>
narrow_array_cast(std_array<From, FromCount>&& values) NOEXCEPT;

/// Cast Integral1* to array(Integral2)&.
/// ---------------------------------------------------------------------------
/// Safe if byte count (Size) is correct.

template <typename To, size_t Size = one, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
inline std_array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT;

template <typename To, size_t Size = one, typename From,
    if_integral_integer<From> = true,
    if_integral_integer<To> = true>
inline const std_array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT;

/// Cast Integral1* to array(array(Integral2), Size)&.
/// ---------------------------------------------------------------------------
/// Safe if byte count (Size) is correct.

template <typename To, size_t Size = one, typename From,
    if_integral_integer<From> = true,
    if_integral_array<To> = true>
inline std_array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT;

template <typename To, size_t Size = one, typename From,
    if_integral_integer<From> = true,
    if_integral_array<To> = true>
inline const std_array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT;

/// Cast Integral1* to a vector(array(Integral)&, count).
/// ---------------------------------------------------------------------------

template <typename To, typename From,
    if_integral_integer<From> = true,
    if_integral_array<To> = true>
inline std_vector<std::reference_wrapper<To>>
unsafe_vector_cast(From* bytes, size_t count) NOEXCEPT;

template <typename To, typename From,
    if_integral_integer<From> = true,
    if_integral_array<To> = true>
inline std_vector<std::reference_wrapper<const To>>
unsafe_vector_cast(const From* bytes, size_t count) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/array_cast.ipp>

#endif
