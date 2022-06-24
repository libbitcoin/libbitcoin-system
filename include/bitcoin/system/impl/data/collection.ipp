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
#include <type_traits>
#include <utility>
#include <vector>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

// TODO: std::binary_search?
// TODO: provide optional comparison function?
// en.cppreference.com/w/cpp/utility/functional/less
// It is the responsibility of the caller to ensure that parameters implement
// sufficient comparison operator overloads (LT and GT). Either the 'list' 
// elements must implement (at least) member comparison operator overloads or
// the 'value' parameter must implement binary comparison operator overloads.
// Be aware that C-style arrays/strings are compared by pointers, not values.
// A 'list' of C-style arrays of char may be seached with std::string 'value'.
// std::string comparisons are not locale aware.
template <typename Collection, typename Element>
typename Collection::difference_type
constexpr binary_search(const Collection& list,
    const Element& element) noexcept
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
inline To projection(const From& source) noexcept
{
    ////static no_fill_allocator<To::value_type> no_fill_to_allocator{};
    To out(std::size(source));

    std::transform(std::begin(source), std::end(source), std::begin(out),
        [](const auto& element) noexcept
        {
            return element;
        });

    return out;
}

// C++17: Parallel policy for std::equal.
template <typename Left, typename Right>
constexpr bool deep_equal(const Left& left, const Right& right) noexcept
{
    return std::equal(std::begin(left), std::end(left),
        std::begin(right), std::end(right),
        [](const auto& first, const auto& second) noexcept
        {
            return 
                ((first != nullptr && second != nullptr) &&
                (*first == *second)) ||
                (first == nullptr && second == nullptr);
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

// C++17: Parallel policy for std::find_if.
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

// Collection requires insert method (vector).
template <typename Collection, typename Predicate>
typename Collection::iterator
inline insert_sorted(Collection& list,
    typename Collection::value_type& element, Predicate predicate) noexcept
{
    return list.insert(std::upper_bound(std::begin(list),
        std::end(list), element, predicate), element);
}

// Collection requires back and pop_back methods (vector).
template <typename Collection>
typename Collection::value_type
inline pop(Collection& stack) noexcept
{
    if (std::empty(stack))
        return {};

    typename Collection::value_type element{ std::move(stack.back()) };
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
    Collection copy{ list };
    return is_distinct(copy);
}

// C++17: Parallel policy for std::is_sorted.
template <typename Collection>
constexpr bool is_sorted(const Collection& list) noexcept
{
    return std::is_sorted(std::begin(list), std::end(list));
}

// C++17: Parallel policy for std::sort, std::erase.
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
    return distinct(Collection{ list });
}

template <typename Left, typename Right>
inline Left difference(const Left& left, const Right& right) noexcept
{
    return difference<Left>(std::begin(left), std::end(left), right);
}

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
constexpr bool is_intersecting(const Left& left, const Right& right) noexcept
{
    return is_intersecting<Left>(std::begin(left), std::end(left), right);
}

// C++17: Parallel policy for std::find_first_of.
template <typename Left, typename Right>
constexpr bool is_intersecting(const typename Left::const_iterator& begin,
    const typename Left::const_iterator& end, const Right& right) noexcept
{
    return std::find_first_of(begin, end, std::begin(right),
        std::end(right)) != end;
}

template <typename Collection>
inline Collection reverse(Collection&& list) noexcept
{
    // Returns iterator to last element, implementations may vectorize.
    std::ranges::reverse(list);
    return std::forward<Collection>(list);
}

template <typename Collection>
inline Collection reverse_copy(const Collection& list) noexcept
{
    return reverse(Collection{ list });
}

template <typename Collection>
inline auto reverse_view(Collection& list) noexcept
{
#ifdef HAVE_STD_RANGES
    return std::views::reverse(list);
#else
    return boost::adaptors::reverse(list);
#endif
}

template <typename Collection>
inline auto reverse_view(const Collection& list) noexcept
{
#ifdef HAVE_STD_RANGES
    return std::views::reverse(list);
#else
    return boost::adaptors::reverse(list);
#endif
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
    return sort(Collection{ list });
}

// C++17: Parallel policy for std::equal.
template <typename Collection>
constexpr bool starts_with(const typename Collection::const_iterator& begin,
    const typename Collection::const_iterator& end,
    const Collection& value) noexcept
{
    return !is_lesser(std::distance(begin, end), std::size(value)) &&
        std::equal(std::begin(value), std::end(value), begin);
}

} // namespace system
} // namespace libbitcoin

#endif
