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
#ifndef LIBBITCOIN_COLLECTION_IPP
#define LIBBITCOIN_COLLECTION_IPP

#include <iterator>
#include <vector>

/* NOTE: don't declare 'using namespace foo' in headers. */

namespace libbitcoin {

template <typename Pair, typename Key>
int find_pair_position(const std::vector<Pair>& list, const Key& key)
{
    const auto predicate = [&](const Pair& pair)
    {
        return pair.first == key;
    };

    auto it = std::find_if(list.begin(), list.end(), predicate);
    if (it == list.end())
        return -1;

    return static_cast<int>(distance(list.begin(), it));
}

template <typename Element, typename Container>
int find_position(const Container& list, const Element& value)
{
    const auto it = std::find(std::begin(list), std::end(list), value);
    if (it == std::end(list))
        return -1;

    return static_cast<int>(std::distance(list.begin(), it));
}

template <typename Consequent, typename Alternate>
Consequent if_else(bool antecedent, const Consequent consequent,
    const Alternate alternative)
{
    if (antecedent)
        return consequent;
    else
        return alternative;
}

template<typename Type, typename Predicate>
typename std::vector<Type>::iterator insert_sorted(std::vector<Type>& list,
    Type& element, Predicate predicate)
{
    return list.insert(
        std::upper_bound(list.begin(), list.end(), element, predicate),
        element);
}

} // namespace libbitcoin

#endif
