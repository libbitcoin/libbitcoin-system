/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CONFIG_HPP
#define LIBBITCOIN_CONFIG_HPP

#include <iterator>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <bitcoin/bitcoin/define.hpp>

/* NOTE: don't declare 'using namespace foo' in headers. */

namespace libbitcoin {
namespace config {

#define BC_SENTENCE_DELIMITER " "

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
 * Avoid the ternary (just for fun). Must precede tempalte usage for gcc build.
 * You should use with expressions as consequent or alternative as they will be
 * executed regardless of the predicate.
 * @param      <Consequent>  The type of the consequent.
 * @param      <Alternate>   The type of the alternative.
 * @param[in]  antecedent    The proposition's antecedent.
 * @param[in]  consequent    The proposition's consequent (if predicate).
 * @param[in]  consequent    The proposition's alternative (if !predicate).
 * @return                   Either the consequent or the alternative.
 */
template <typename Consequent, typename Alternate>
Consequent if_else(bool antecedent, const Consequent consequent,
    const Alternate alternative);

/**
 * Facilitate a list insertion sort by inserting into a sorted position.
 * @param      <Type>       The type of list member elements.
 * @param      <Predicate>  The sort predicate function signature.
 * @param[in]  list         The list to search.
 * @param[in]  element      The element to insert.
 * @param[in]  predicate    The sort predicate.
 * @return                  Thevector iterator.
 */
template<typename Type, typename Predicate>
typename std::vector<Type>::iterator insert_sorted(std::vector<Type>& list,
    Type const& element, Predicate predicate);

/**
 * Join a list of strings into a single string, in order.
 * @param[in]  words      The list of strings to join.
 * @param[in]  delimiter  The delimiter, defaults to BC_SENTENCE_DELIMITER.
 * @return                The resulting string.
 */
BC_API std::string join(const std::vector<std::string>& words,
    const std::string& delimiter=BC_SENTENCE_DELIMITER);

/**
 * Split a list of strings into a string vector string, in order, white space
 * delimited.
 * @param[in]  sentence   The string to split.
 * @param[in]  delimiter  The delimeter, defaults to BC_SENTENCE_DELIMITER.
 * @return                The list of resulting strings.
 */
BC_API std::vector<std::string> split(const std::string& sentence,
    const std::string& delimiter=BC_SENTENCE_DELIMITER);

/**
 * Trim a string of whitespace.
 * @param[out] value  The string to trim.
 */
BC_API  void trim(std::string& value);

} // namespace config
} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/config/config.ipp>

#endif