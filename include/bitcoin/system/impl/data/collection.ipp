/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#include <vector>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>

namespace libbitcoin {
namespace system {

// Assert fails arbitrarily if 'list' contains pointer elements (e.g. to
// const char*) because the comparison is between pointers, not values.
// However the binary_search call remains valid in the case where the
// 'value' parameter implements sufficient binary comparison operator
// overloads (e.g. operator<(const char* left, const std::string& right).
// Therefore we avoid this assertion and rely entirely on caller sorting.
////BITCOIN_ASSERT(is_sorted(list));

// TODO: constrain to iterable Collection and constrain Element to
// comparable against Collection::value_type.
// Element is not required to be the Collection::value_type. 
// Collection requires size and empty methods.
template <typename Collection, typename Element>
typename Collection::difference_type
binary_search(const Collection& list, const Element& element)
{
    const auto size = list.size();

    if (list.empty())
        return negative_one;

    typename Collection::difference_type left = zero;
    typename Collection::difference_type right = sub1(size);

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

template <typename To, typename From>
std::vector<To> cast(const std::vector<From>& source)
{
    ////std::vector<To> out(source.size());
    ////std::transform(std::begin(source), std::end(source), std::begin(out),
    ////    [](const From& element)
    ////    {
    ////        return static_cast<To>(element);
    ////    });

    // More efficient than array here because can be value-initialized.
    return { std::begin(source), std::end(source) };
}

template <typename To, typename From, size_t Size>
std::array<To, Size> cast(const std::array<From, Size>& source)
{
    std::array<To, Size> out;
    std::transform(std::begin(source), std::end(source), std::begin(out),
        [](const From& element)
        {
            return element;
        });

    return std::move(out);
}

template <typename Collection>
bool contains(const Collection& list,
    const typename Collection::value_type& element)
{
    return std::any_of(std::begin(list), std::end(list),
        [&element](const typename Collection::value_type& value)
        {
            return value == element;
        });
}

// TODO: constrain to std::pair elements.
// Collection requires std::pair elements.
template <typename Collection>
typename Collection::difference_type
find_pair_position(const Collection& list,
    const typename Collection::value_type::first_type& key)
{
    const auto position = std::find_if(std::begin(list), std::end(list),
        [&key](const typename Collection::value_type& pair)
        {
            return pair.first == key;
        });

    return position == std::end(list) ? negative_one :
        std::distance(std::begin(list), position);
}

template <typename Collection>
typename Collection::difference_type
find_position(const Collection& list,
    const typename Collection::value_type& element)
{
    const auto position = std::find(std::begin(list), std::end(list), element);
    return position == std::end(list) ? negative_one :
        std::distance(std::begin(list), position);
}

// TODO: specialize vector and generalize on element type.
// Collection requires insert method (vector).
template <typename Collection, typename Predicate>
typename Collection::iterator
insert_sorted(Collection& list, typename Collection::value_type& element,
    Predicate predicate)
{
    return list.insert(std::upper_bound(std::begin(list),
        std::end(list), element, predicate), element);
}

// TODO: specialize vector and generalize on element type.
// Collection requires reserve and size methods (vector).
template <typename Collection>
void move_append(Collection& target, Collection& source)
{
    target.reserve(target.size() + source.size());
    std::move(std::begin(source), std::end(source), std::back_inserter(target));
}

// TODO: specialize vector and generalize on element type.
// Collection requires empty, back and pop_back methods (vector).
template <typename Collection>
typename Collection::value_type
pop(Collection& stack)
{
    if (stack.empty())
        return {};

    auto element = stack.back();
    stack.pop_back();
    return std::move(element);
}

template <typename Collection>
bool is_distinct(Collection&& list)
{
    std::sort(std::begin(list), std::end(list));
    return std::unique(std::begin(list), std::end(list)) == std::end(list);
}

template <typename Collection>
bool is_distinct(const Collection& list)
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
    // Mutable parameter : ref.
    // Integral parameter: copy.
    // Const sufficient  : const ref.
    // Non-const required: move with const ref overload that calls into move.
    auto copy = list;
    return is_distinct(copy);
}

template <typename Collection>
bool is_sorted(const Collection& list)
{
    return std::is_sorted(std::begin(list), std::end(list));
}

// TODO: specialize vector and generalize on element type.
// Collection requires erase and shrink_to_fit methods (vector).
template <typename Collection>
Collection distinct(Collection&& list)
{
    std::sort(std::begin(list), std::end(list));
    list.erase(std::unique(std::begin(list), std::end(list)), std::end(list));
    list.shrink_to_fit();
    return list;
}

template <typename Collection>
Collection distinct(const Collection& list)
{
    // See above comments.
    auto copy = list;
    return distinct(copy);
}

template <typename Collection>
Collection reverse(Collection&& list)
{
    std::reverse(std::begin(list), std::end(list));
    return list;
}

template <typename Collection>
Collection reverse(const Collection& list)
{
    // See above comments.
    auto copy = list;
    return reverse(copy);
}

template <typename Collection>
Collection sort(Collection&& list)
{
    std::sort(std::begin(list), std::end(list));
    return list;
}

template <typename Collection>
Collection sort(const Collection& list)
{
    // See above comments.
    auto copy = list;
    return sort(copy);
}

} // namespace system
} // namespace libbitcoin

#endif
