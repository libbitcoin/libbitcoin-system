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
#ifndef LIBBITCOIN_THREADSAFE_BINARY_TRIE_HPP
#define LIBBITCOIN_THREADSAFE_BINARY_TRIE_HPP

#include <vector>
#include <boost/thread/shared_mutex.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/trie/binary_trie.hpp>

namespace libbitcoin {

template<typename Value, typename StructureNodeAllocator = std::allocator<
    trie_structure_node<Value>>, typename ValueNodeAllocator = std::allocator<
    trie_value_node<Value>>, typename Comparer = std::less<Value>>
class BC_API threadsafe_binary_trie : protected boost::noncopyable
{
public:

    typedef std::vector<Value> value_list;
    typedef std::pair<std::vector<Value>, binary_type> pair_values_next;

    threadsafe_binary_trie();

    // insertion
    bool insert_equal(const binary_type& key, const Value& value);

    bool insert_unique(const binary_type& key, const Value& value);

    // lookup
    value_list find_equal(const binary_type& key);

    pair_values_next find_prefix(const binary_type& key,
        uint32_t result_count);

    // deletion
    bool remove_equal(const binary_type& key);

    bool remove_prefix(const binary_type& key);

private:

    binary_trie<Value,
        StructureNodeAllocator,
        ValueNodeAllocator,
        Comparer> data_;

    boost::shared_mutex mutex_;

};

} // namespace libbitcoin

#include <bitcoin/bitcoin/impl/trie/threadsafe_binary_trie.ipp>

#endif
