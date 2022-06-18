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
#ifndef LIBBITCOIN_SYSTEM_DATA_COLLECTION_IPP
#define LIBBITCOIN_SYSTEM_DATA_COLLECTION_IPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// Assert fails arbitrarily if 'list' contains pointer elements (e.g. to
// const char*) because the comparison is between pointers, not values.
// However the binary_search call remains valid in the case where the
// 'value' parameter implements sufficient binary comparison operator
// overloads (e.g. operator<(const char* left, const std::string& right).
// Therefore we avoid this assertion and rely entirely on caller sorting.
////BC_ASSERT(is_sorted(list));
// TODO: provide optional comparison function, std::sort accepts:
// en.cppreference.com/w/cpp/utility/functional/less

// TODO: constrain to iterable Collection and constrain Element to
// comparable against Collection::value_type.
// Element is not required to be the Collection::value_type. 
// Collection requires size and empty methods.
template <typename Collection, typename Element>
typename Collection::difference_type
constexpr binary_search(const Collection& list, const Element& element) noexcept
{
    if (std::empty(list))
        return negative_one;

    typename Collection::difference_type left = zero;
    typename Collection::difference_type right = sub1(std::size(list));

    while (left <= right)
    {
        auto middle = to_half(left + right);
        const auto& value = list[middle];

        if (value < element)
            left = add1(middle);
        else if (value > element)
            right = sub1(middle);
        else
            return middle;
    }

    return negative_one;
}

// C++17: Parallel policy for std::transform.
template <typename To, typename From>
inline std::vector<To> cast(const std::vector<From>& source) noexcept
{
    ////static no_fill_allocator<To> no_fill_to_allocator{};
    std::vector<To> out(std::size(source));

    std::transform(std::begin(source), std::end(source), std::begin(out),
        [](const auto& element) noexcept
        {
            return possible_sign_cast<To>(element);
        });

    return out;
}

// C++17: Parallel policy for std::transform.
template <typename To, typename From, size_t Size>
inline std::array<To, Size> cast(
    const std::array<From, Size>& source) noexcept
{
    ////static no_fill_allocator<To> no_fill_to_allocator{};
    std::array<To, Size> out{};

    std::transform(std::begin(source), std::end(source), std::begin(out),
        [](const auto& element) noexcept
        {
            return possible_sign_cast<To>(element);
        });

    return out;
}

// TODO: remove std::vector limitation.
// C++17: Parallel policy for std::transform.
template <typename To, typename From>
inline std::vector<To> pointer_cast(
    const std::vector<From>& source) noexcept
{
    ////static no_fill_allocator<To> no_fill_to_allocator{};
    std::vector<To> out(std::size(source));

    std::transform(std::begin(source), std::end(source), std::begin(out),
        [](const auto& smart_pointer) noexcept
        {
            return smart_pointer.get();
        });

    return out;
}

// TODO: remove std::vector limitation.
template <typename Element>
constexpr bool pointeds_equal(
    const std::vector<std::shared_ptr<const Element>>& left,
    const std::vector<std::shared_ptr<const Element>>& right) noexcept
{
    return std::equal(std::begin(left), std::end(left), std::begin(right),
        std::end(right),
        [](const auto& first, const auto& second) noexcept
        {
            return (first && second && (*first == *second)) ||
                (!first && !second);
        });
}

// C++17: Parallel policy for std::any_of.
template <typename Collection, typename Element>
constexpr bool contains(const Collection& list,
    const Element& element) noexcept
{
    return std::any_of(std::begin(list), std::end(list),
        [&element](const auto& value) noexcept
        {
            return value == element;
        });
}

// TODO: constrain to std::pair elements.
// C++17: Parallel policy for std::find_if.
// Collection requires std::pair elements.
template <typename Collection>
typename Collection::difference_type
constexpr find_pair_position(const Collection& list,
    const typename Collection::value_type::first_type& key) noexcept
{
    const auto position = std::find_if(std::begin(list), std::end(list),
        [&key](const auto& pair) noexcept
        {
            return pair.first == key;
        });

    return position == std::end(list) ? negative_one :
        std::distance(std::begin(list), position);
}

// C++17: Parallel policy for std::find.
template <typename Collection>
typename Collection::difference_type
constexpr find_position(const Collection& list,
    const typename Collection::value_type& element) noexcept
{
    const auto position = std::find(std::begin(list), std::end(list), element);
    return position == std::end(list) ? negative_one :
        std::distance(std::begin(list), position);
}

// TODO: specialize vector and generalize on element type.
// Collection requires insert method (vector).
template <typename Collection, typename Predicate>
typename Collection::iterator
inline insert_sorted(Collection& list, typename Collection::value_type& element,
    Predicate predicate) noexcept
{
    return list.insert(std::upper_bound(std::begin(list),
        std::end(list), element, predicate), element);
}

// TODO: specialize vector and generalize on element type.
// Collection requires reserve and size methods (vector).
template <typename Collection>
constexpr void move_append(Collection& target, Collection& source) noexcept
{
    // C++17: std::size.
    target.reserve(std::size(target) + std::size(source));
    std::move(std::begin(source), std::end(source), std::back_inserter(target));
}

// TODO: specialize vector and generalize on element type.
// Collection requires empty, back and pop_back methods (vector).
template <typename Collection>
typename Collection::value_type
inline pop(Collection& stack) noexcept
{
    if (std::empty(stack))
        return {};

    auto element = stack.back();
    stack.pop_back();
    return element;
}

// C++17: Parallel policy for std::sort, std::unique.
template <typename Collection>
constexpr bool is_distinct(Collection&& list) noexcept
{
    std::sort(std::begin(list), std::end(list));
    return std::unique(std::begin(list), std::end(list)) == std::end(list);
}

template <typename Collection>
constexpr bool is_distinct(const Collection& list) noexcept
{
    // A single move function would require a move and a copy (least ideal).
    // This is necessary because the argument will be const if not moved.
    // A single pass-by-value function requires an external copy (not ideal).
    // A single pass-by-reference requires an internal copy (not ideal).
    // In theory the compiler would elide unnecessary copies (not guaranteed).
    // Two overloads ensure 1 copy here or 1 move in the move overload (ideal).
    // By providing both overloads this is not only ensured but the benefit of
    // move is exposed to callers, as both options are explicitly available.
    // This is the approach implemented in the C++ std library.
    // Mutable parameter      : ref.
    // Integral parameter     : copy.
    // Non-mutable parameter  : const ref.
    // Non-mutable param copy : move with const ref overload that calls into move.
    auto copy = list;
    return is_distinct(copy);
}

// TODO: provide optional comparison function
// C++17: Parallel policy for std::is_sorted.
template <typename Collection>
constexpr bool is_sorted(const Collection& list) noexcept
{
    return std::is_sorted(std::begin(list), std::end(list));
}

// TODO: specialize vector and generalize on element type.
// C++17: Parallel policy for std::sort, std::erase.
// Collection requires erase and shrink_to_fit methods (vector).

template <typename Collection>
constexpr void distinct(Collection& list) noexcept
{
    std::sort(std::begin(list), std::end(list));
    list.erase(std::unique(std::begin(list), std::end(list)), std::end(list));
    list.shrink_to_fit();
}

template <typename Collection>
inline Collection distinct(Collection&& list) noexcept
{
    distinct(list);
    return std::forward<Collection>(list);
}

template <typename Collection>
inline Collection distinct_copy(const Collection& list) noexcept
{
    auto copy = list;
    distinct(copy);
    return copy;
}

// Collection requires size, reserve and shrink_to_fit methods (vector).
template <typename Left, typename Right>
inline Left difference(const Left& left, const Right& right) noexcept
{
    return difference<Left>(std::begin(left), std::end(left), right);
}

// Collection requires size, reserve and shrink_to_fit methods (vector).
template <typename Left, typename Right>
inline Left difference(const typename Left::const_iterator& begin,
    const typename Left::const_iterator& end, const Right& right) noexcept
{
    Left copy{};
    copy.reserve(std::distance(begin, end));
    
    // Linear copy since creating a copy, more efficient than multiple erases.
    for (auto min = begin; min != end; ++min)
        if (!contains(right, *min))
            copy.push_back(*min);
    
    copy.shrink_to_fit();
    return copy;
}

template <typename Left, typename Right>
constexpr bool intersecting(const Left& left, const Right& right) noexcept
{
    return intersecting<Left>(std::begin(left), std::end(left), right);
}

// C++17: Parallel policy for std::find_first_of.
template <typename Left, typename Right>
constexpr bool intersecting(const typename Left::const_iterator& begin,
    const typename Left::const_iterator& end, const Right& right) noexcept
{
    return std::find_first_of(begin, end, std::begin(right),
        std::end(right)) != end;
}

template <typename Collection>
constexpr void reverse(Collection& list) noexcept
{
    // std::ranges::reverse may vectorize when iterator is contigous.
    std::ranges::reverse(list);
}

template <typename Collection>
inline Collection reverse(Collection&& list) noexcept
{
    reverse(list);
    return std::forward<Collection>(list);
}

template <typename Collection>
inline Collection reverse_copy(const Collection& list) noexcept
{
    auto copy = list;
    reverse(copy);
    return copy;
}

// C++17: Parallel policy for std::sort.
template <typename Collection>
constexpr void sort(Collection& list) noexcept
{
    std::sort(std::begin(list), std::end(list));
}

template <typename Collection>
inline Collection sort(Collection&& list) noexcept
{
    sort(list);
    return std::forward<Collection>(list);
}

template <typename Collection>
inline Collection sort_copy(const Collection& list) noexcept
{
    auto copy = list;
    sort(copy);
    return copy;
}

template <typename Collection>
constexpr bool starts_with(
    const typename Collection::const_iterator& begin,
    const typename Collection::const_iterator& end,
    const Collection& value) noexcept
{
    return !is_lesser(std::distance(begin, end), std::size(value)) &&
        std::equal(std::begin(value), std::end(value), begin);
}

} // namespace system
} // namespace libbitcoin

#endif
