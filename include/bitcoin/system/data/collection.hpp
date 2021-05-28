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
template <typename Element, typename Container>
int binary_search(const Container& list, const Element& value);

/// Cast vector/enumerable elements into a new vector.
template <typename Target, typename Source>
std::vector<Target> cast(const std::vector<Source>& source);

/// Determine if a collection contains the specified element.
template <typename Container, typename Element>
bool contains(const Container& list, const Element& value);

/// Obtain the sorted distinct elements of the list.
template <typename Element>
std::vector<Element> distinct(std::vector<Element>&& list);

/// Find the position of a pair in an ordered list.
template <typename Pair, typename Key>
int find_pair_position(const std::vector<const Pair>& list, Key& key);

/// Find the position of an element in an ordered collection.
template <typename Element, typename Container>
int find_position(const Container& list, const Element& value);

/// Facilitate a list insertion sort by inserting into a sorted position.
template <typename Type, typename Predicate>
typename std::vector<Type>::iterator insert_sorted(std::vector<Type>& list,
    const Type& element, Predicate predicate);

/// Determine if a collection contains only distinct members.
template <typename Element>
bool is_distinct(std::vector<Element>&& list);

/// Determine if a collection is lexically sorted.
template <typename Container>
bool is_sorted(const Container& list);

/// Move members of a source list to end of a target list. Source members are 
/// undefined upon return.
template <typename Type>
void move_append(std::vector<Type>& target, std::vector<Type>& source);

/// Pop an element from the stack and return its value.
template <typename Element>
Element pop(std::vector<Element>& stack);

/// Create a reversed copy of a collection.
template <typename Collection>
Collection reverse(const Collection& source);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/collection.ipp>

#endif
