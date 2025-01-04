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
#ifndef LIBBITCOIN_SYSTEM_DATA_ARRAY_CAST_IPP
#define LIBBITCOIN_SYSTEM_DATA_ARRAY_CAST_IPP

#include <iterator>
#include <utility>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Sequence generation.
// ----------------------------------------------------------------------------

// Suppress bogus warnings to use constexpr when function is consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)

// Helper to extract Index parameter pack from std::make_index_sequence<Size>.
template<typename Integer, size_t Size, size_t... Index>
CONSTEVAL auto to_array(std::index_sequence<Index...>) NOEXCEPT
{
    return std_array<Integer, Size>{ { Index... } };
}

template<typename Integer, size_t Size>
CONSTEVAL std_array<Integer, Size> to_sequence() NOEXCEPT
{
    return to_array<Integer, Size>(std::make_index_sequence<Size>{});
}

BC_POP_WARNING()

// Array casting helpers.
// ----------------------------------------------------------------------------
    
template <typename To, typename From>
constexpr bool is_portional(size_t to_count, size_t from_count) NOEXCEPT
{
    // Guards against the protion being greater than available.
    return !is_multiply_overflow(to_count, size_of<To>())
        && !is_multiply_overflow(from_count, size_of<From>())
        && (to_count * size_of<To>() <= from_count * size_of<From>());
}

template <typename From, typename To>
constexpr bool is_proportional(size_t from_count) NOEXCEPT
{
    return !is_multiply_overflow(from_count, size_of<From>())
        && is_multiple(from_count * size_of<From>(), size_of<To>());
}

// Cast array(T1) to same-sized array(T2).
// ----------------------------------------------------------------------------

template <typename To, size_t Count, typename From>
inline std_array<To, proportion<Count, From, To>>&
array_cast(std_array<From, Count>& values) NOEXCEPT
{
    using to = std_array<To, proportion<Count, From, To>>;
    return *pointer_cast<to>(values.data());
}

template <typename To, size_t Count, typename From>
inline const std_array<To, proportion<Count, From, To>>&
array_cast(const std_array<From, Count>& values) NOEXCEPT
{
    using to = std_array<To, proportion<Count, From, To>>;
    return *pointer_cast<const to>(values.data());
}

// Avoids cast of rvalue to reference, which would dangle.
template <typename To, size_t Count, typename From>
inline std_array<To, proportion<Count, From, To>>
array_cast(std_array<From, Count>&& values) NOEXCEPT
{
    return array_cast<To>(unmove(values));
}

// Cast array(T1) to not-greater-sized array(T2).
// ----------------------------------------------------------------------------

template <typename To, size_t ToCount, size_t FromOffset, typename From,
    size_t FromCount,
    if_lesser<FromOffset, FromCount>,
    if_portional<ToCount, To, FromCount - FromOffset, From>>
inline std_array<To, ToCount>&
array_cast(std_array<From, FromCount>& values) NOEXCEPT
{
    using to = std_array<To, ToCount>;
    return *pointer_cast<to>(std::next(values.data(), FromOffset));
}

template <typename To, size_t ToCount, size_t FromOffset, typename From,
    size_t FromCount,
    if_lesser<FromOffset, FromCount>,
    if_portional<ToCount, To, FromCount - FromOffset, From>>
inline const std_array<To, ToCount>&
array_cast(const std_array<From, FromCount>& values) NOEXCEPT
{
    using to = std_array<To, ToCount>;
    return *pointer_cast<const to>(std::next(values.data(), FromOffset));
}

// Avoids cast of rvalue to reference, which would dangle.
// Cannot offset into move assignment as the whole instance must be moved.
template <typename To, size_t ToCount, typename From, size_t FromCount,
    if_portional<ToCount, To, FromCount, From>>
inline std_array<To, ToCount>
array_cast(std_array<From, FromCount>&& values) NOEXCEPT
{
    return array_cast<To, ToCount>(unmove(values));
}

// Cast Integral1* to array(Integral2).
// ----------------------------------------------------------------------------

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
inline std_array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT
{
    return *pointer_cast<std_array<To, Size>>(bytes);
}

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_integer<To>>
inline const std_array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT
{
    return *pointer_cast<const std_array<To, Size>>(bytes);
}

// Cast Integral1* to array(array(Integral2), Size).
// ----------------------------------------------------------------------------

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_array<To>>
inline std_array<To, Size>&
unsafe_array_cast(From* bytes) NOEXCEPT
{
    return *pointer_cast<std_array<To, Size>>(bytes);
}

template <typename To, size_t Size, typename From,
    if_integral_integer<From>,
    if_integral_array<To>>
inline const std_array<To, Size>&
unsafe_array_cast(const From* bytes) NOEXCEPT
{
    return *pointer_cast<const std_array<To, Size>>(bytes);
}

// Cast Integral1* to a vector(array(Integral)&, count).
// ----------------------------------------------------------------------------

template <typename To, typename From,
    if_integral_integer<From>,
    if_integral_array<To>>
inline std_vector<std::reference_wrapper<To>>
unsafe_vector_cast(From* bytes, size_t count) NOEXCEPT
{
    using inner_type = array_element<To>;
    constexpr auto inner_count = array_count<To>;
    std_vector<std::reference_wrapper<To>> out{};
    out.reserve(count);

    for (size_t element = 0; element < count; ++element)
    {
        out.emplace_back(unsafe_array_cast<inner_type, inner_count>(bytes));
        std::advance(bytes, size_of<To>());
    }

    return out;
}

template <typename To, typename From,
    if_integral_integer<From>,
    if_integral_array<To>>
inline std_vector<std::reference_wrapper<const To>>
unsafe_vector_cast(const From* bytes, size_t count) NOEXCEPT
{
    using inner_type = array_element<To>;
    constexpr auto inner_count = array_count<To>;
    std_vector<std::reference_wrapper<const To>> out{};
    out.reserve(count);

    for (size_t element = 0; element < count; ++element)
    {
        out.emplace_back(unsafe_array_cast<inner_type, inner_count>(bytes));
        std::advance(bytes, size_of<To>());
    }

    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
