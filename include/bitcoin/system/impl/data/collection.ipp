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
#include <cstdint>
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
template <typename Collection, typename Element>
typename Collection::difference_type
binary_search(const Collection& list, const Element& element)
{
    const auto size = std::size(list);

    if (std::empty(list))
        return negative_one;

    Collection::difference_type left = zero;
    Collection::difference_type right = sub1(size);

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

// TODO: generalize from std::vector (any iterable).
template <typename To, typename From>
std::vector<To> cast(const std::vector<From>& source)
{
    std::vector<To> target(std::size(source));
    target.assign(std::begin(source), std::end(source));
    return target;
}

template <typename Collection>
bool contains(const Collection& list,
    const typename Collection::value_type& element)
{
    return std::any_of(std::begin(list), std::end(list),
        [&element](const Collection::value_type& value)
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
        [&key](const Collection::value_type& pair)
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
// Collection requires reserve method (vector).
template <typename Collection>
void move_append(Collection& target, Collection& source)
{
    target.reserve(std::size(target) + std::size(source));
    std::move(std::begin(source), std::end(source), std::back_inserter(target));
}

// TODO: specialize vector and generalize on element type.
// Collection requires back and pop_back methods (vector).
template <typename Collection>
typename Collection::value_type
pop(Collection& stack)
{
    if (std::empty(stack))
        return {};

    const auto element = stack.back();
    stack.pop_back();
    return element;
}

template <typename Collection>
bool is_distinct(Collection list)
{
    std::sort(std::begin(list), std::end(list));
    return std::unique(std::begin(list), std::end(list)) == std::end(list);
}

template <typename Collection>
bool is_sorted(const Collection& list)
{
    return std::is_sorted(std::begin(list), std::end(list));
}

// TODO: specialize vector and generalize on element type.
// Collection requires erase and shrink_to_fit methods (vector).
template <typename Collection>
Collection distinct(Collection list)
{
    std::sort(std::begin(list), std::end(list));
    list.erase(std::unique(std::begin(list), std::end(list)), std::end(list));
    list.shrink_to_fit();
    return std::move(list);
}

template <typename Collection>
Collection reverse(Collection list)
{
    std::reverse(std::begin(list), std::end(list));
    return std::move(list);
}

template <typename Collection>
Collection sort(Collection list)
{
    std::sort(std::begin(list), std::end(list));
    return std::move(list);
}

} // namespace system
} // namespace libbitcoin

#endif
