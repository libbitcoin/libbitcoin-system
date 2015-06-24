/*
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
#ifndef LIBBITCOIN_THREADSAFE_BINARY_TRIE_IPP
#define LIBBITCOIN_THREADSAFE_BINARY_TRIE_IPP

namespace libbitcoin {

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
threadsafe_binary_trie<Value, StructureNodeAllocator, ValueNodeAllocator,
    Comparer>::threadsafe_binary_trie()
{
}

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
bool threadsafe_binary_trie<Value, StructureNodeAllocator, ValueNodeAllocator,
    Comparer>::insert_equal(const binary_type& key, const Value& value)
{
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    auto result = data_.insert_equal(key, value);
    return result.second;
}

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
bool threadsafe_binary_trie<Value, StructureNodeAllocator, ValueNodeAllocator,
    Comparer>::insert_unique(const binary_type& key, const Value& value)
{
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    auto result = data_.insert_unique(key, value);
    return result.second;
}

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
typename threadsafe_binary_trie<Value, StructureNodeAllocator,
    ValueNodeAllocator, Comparer>::value_list threadsafe_binary_trie<
    Value, StructureNodeAllocator, ValueNodeAllocator, Comparer>::find_equal(
        const binary_type& key)
{
    boost::shared_lock<boost::shared_mutex> lock(mutex_);

    std::vector<Value> result;
    auto trie_result = data_.find_equal(key);

    for (auto it = trie_result.first; it != trie_result.second; it++)
        result.push_back(*it);

    return result;
}

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
typename threadsafe_binary_trie<Value, StructureNodeAllocator,
    ValueNodeAllocator, Comparer>::pair_values_next threadsafe_binary_trie<
    Value, StructureNodeAllocator, ValueNodeAllocator, Comparer>::find_prefix(
        const binary_type& key, uint32_t result_count)
{
    boost::shared_lock<boost::shared_mutex> lock(mutex_);

    pair_values_next result;
    auto trie_result = data_.find_prefix(key);

    uint32_t i = 0;

    for (auto it = trie_result.first; it != trie_result.second; it++)
    {
        if ((i >= result_count) && it.is_first_key_value())
        {
            result.second = it.get_key();
            break;
        }

        result.first.push_back(*it);
        i++;
    }

    return result;
}

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
bool threadsafe_binary_trie<Value, StructureNodeAllocator, ValueNodeAllocator,
    Comparer>::remove_equal(const binary_type& key)
{
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    auto result = data_.remove_equal(key);
    return result;
}

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
bool threadsafe_binary_trie<Value, StructureNodeAllocator, ValueNodeAllocator,
    Comparer>::remove_prefix(const binary_type& key)
{
    boost::unique_lock<boost::shared_mutex> lock(mutex_);
    auto result = data_.remove_prefix(key);
    return result;
}

}

#endif
