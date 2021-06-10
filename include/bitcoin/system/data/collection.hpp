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
#ifndef LIBBITCOIN_SYSTEM_DATA_COLLECTION_HPP
#define LIBBITCOIN_SYSTEM_DATA_COLLECTION_HPP

#include <iterator>
#include <vector>
#include <bitcoin/system/data/data.hpp>

namespace libbitcoin {
namespace system {

/// Find the position of an element in a *lexically sorted* collection.
/// It is the responsibility of the caller to ensure that paramters implement
/// sufficient comparison operator overloads (LT and GT). Either the 'list' 
/// elements must implement (at least) member comparison operator overloads or
/// the 'value' parameter must implement binary comparison operator overloads.
/// Be aware that C-style arrays/strings are compared by pointers, not values.
/// A 'list' of C-style arrays of char may be seached with std::string 'value'.
/// std::string comparisons are not locale aware.
/// Returns the position or negative if not found or list size > max_int32.
template <typename Collection, typename Element>
typename Collection::difference_type
binary_search(const Collection& list, const Element& element);

/// Cast vector to a new vector of To members.
template <typename To, typename From>
std::vector<To> cast(const std::vector<From>& source);

/// Determine if a collection contains the specified element.
template <typename Collection>
bool contains(const Collection& list,
    const typename Collection::value_type& element);

/// Find the position of a std::pair in an ordered list.
template <typename Collection>
typename Collection::difference_type
find_pair_position(const Collection& list,
    typename Collection::value_type::first_type& key);

/// Find the position of an element in an ordered collection.
template <typename Collection>
typename Collection::difference_type
find_position(const Collection& list,
    const typename Collection::value_type& value);

/// Facilitate a list insertion sort by inserting into a sorted position.
template <typename Collection, typename Predicate>
typename Collection::iterator
insert_sorted(Collection& list, const typename Collection::value_type& element,
    Predicate predicate);

/// Move members of a source list to end of a target list. Source members are 
/// undefined upon return.
template <typename Collection>
void move_append(Collection& target, Collection& source);

/// Pop an element from the stack and return its value.
template <typename Collection>
typename Collection::value_type
pop(Collection& stack);

/// Determine if a collection contains only distinct members.
template <typename Collection>
bool is_distinct(Collection&& list);
template <typename Collection>
bool is_distinct(const Collection& list);

/// Determine if a collection is lexically sorted.
template <typename Collection>
bool is_sorted(const Collection& list);

/// Obtain the (sorted) distinct elements of a collection.
template <typename Collection>
Collection distinct(Collection&& list);
template <typename Collection>
Collection distinct(const Collection& list);

/// Reverse the order of collection elements.
/// Use boost::adaptors::reverse for reverse iteration.
template <typename Collection>
Collection reverse(Collection&& list);
template <typename Collection>
Collection reverse(const Collection& list);

/// Sort collection elements.
template <typename Collection>
Collection sort(Collection&& list);
template <typename Collection>
Collection sort(const Collection& list);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/collection.ipp>

#endif
