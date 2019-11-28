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
#ifndef LIBBITCOIN_SYSTEM_COLLECTION_HPP
#define LIBBITCOIN_SYSTEM_COLLECTION_HPP

#include <iterator>
#include <iostream>
#include <vector>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

#define BC_SENTENCE_DELIMITER " "

/**
 * Cast vector/enumerable elements into a new vector.
 * @param      <Source>  The source element type.
 * @param      <Target>  The target element type.
 * @param[in]  source    The enumeration of Source elements to cast.
 * @returns              A new enumeration with elements cast to Target.
 */
template <typename Source, typename Target>
std::vector<Target> cast(const std::vector<Source>& source);

/**
 * Obtain the sorted distinct elements of the list.
 * @param           <Element>  The list element type.
 * @param[in, out]  list       The list.
 * @return                     The sorted list reduced to distinct elements.
 */
template <typename Element>
std::vector<Element>& distinct(std::vector<Element>& list);

/**
 * Find the position of a pair in an ordered list.
 * @param      <Pair>  The type of list member elements.
 * @param[in]  list    The list to search.
 * @param[in]  key     The key to the element to find.
 * @return             The position or -1 if not found.
 */
template <typename Pair, typename Key>
int find_pair_position(const std::vector<Pair>& list, const Key& key);

/**
 * Find the position of an element in an ordered list.
 * @param      <Element>  The type of list member elements.
 * @param[in]  list       The list to search.
 * @param[in]  value      The value of the element to find.
 * @return                The position or -1 if not found.
 */
template <typename Element, typename Container>
int find_position(const Container& list, const Element& value);

/**
 * Facilitate a list insertion sort by inserting into a sorted position.
 * @param      <Type>       The type of list member elements.
 * @param      <Predicate>  The sort predicate function signature.
 * @param[in]  list         The list to modify.
 * @param[in]  element      The element to insert.
 * @param[in]  predicate    The sort predicate.
 * @return                  The vector iterator.
 */
template <typename Type, typename Predicate>
typename std::vector<Type>::iterator insert_sorted(std::vector<Type>& list,
    const Type& element, Predicate predicate);

/**
 * Move members of a source list to end of a target list. Source is cleared.
 * @param      <Type>     The type of list member elements.
 * @param[in]  target     The target list.
 * @param[in]  source     The source list
 */
template <typename Type>
void move_append(std::vector<Type>& target, std::vector<Type>& source);

/**
 * Pop an element from the stack and return its value.
 * @param      <Element>  The stack element type.
 * @param[in]  stack      The stack.
 */
template <typename Element>
Element pop(std::vector<Element>& stack);

} // namespace system
} // namespace libbitcoin

namespace std {

/**
 * Make vectors of serializable elements serializable to std::ostream.
 * @param      <Type>  The type of list member elements.
 * @param[in]  output  The output stream to serialize to.
 * @param[in]  list    The list to serialize.
 * @return             The output stream.
 */
template <class Type>
std::ostream& operator<<(std::ostream& output, const std::vector<Type>& list);

} // namespace std

#include <bitcoin/system/impl/utility/collection.ipp>

#endif
