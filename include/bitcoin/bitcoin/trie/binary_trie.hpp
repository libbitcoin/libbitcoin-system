/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_BINARY_TRIE_HPP
#define LIBBITCOIN_BINARY_TRIE_HPP

#include <memory>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/utility.hpp>
#include <bitcoin/bitcoin/trie/trie_iterator.hpp>
#include <bitcoin/bitcoin/trie/trie_reverse_iterator.hpp>
#include <bitcoin/bitcoin/trie/trie_structure_node.hpp>
#include <bitcoin/bitcoin/trie/trie_value_node.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>

namespace libbitcoin {

template<typename Value, typename StructureNodeAllocator = std::allocator<
    trie_structure_node<Value>>, typename ValueNodeAllocator = std::allocator<
    trie_value_node<Value>>, typename Comparer = std::less<Value>>
class binary_trie: protected boost::noncopyable
{
public:

    typedef Value value_type;
    typedef StructureNodeAllocator structure_node_allocator;
    typedef ValueNodeAllocator value_node_allocator;
    typedef Comparer value_type_comparer;
    typedef binary_trie<value_type, structure_node_allocator,
        value_node_allocator, value_type_comparer> binary_trie_type;
    typedef trie_structure_node<value_type> structure_node_type;
    typedef trie_value_node<value_type> value_node_type;

    typedef trie_iterator<Value, Value&, Value*> iterator;
    typedef typename iterator::const_iterator const_iterator;
    typedef trie_reverse_iterator<iterator> reverse_iterator;
    typedef trie_reverse_iterator<const_iterator> const_reverse_iterator;

    typedef std::pair<iterator, bool> pair_iterator_bool;
    typedef std::pair<iterator, iterator> iterator_range;

    typedef std::pair<structure_node_type*, binary_type::size_type> pair_node_size;

public:

    // constructor/destructors
    binary_trie(structure_node_allocator trie_allocator,
        value_node_allocator value_allocator);

    ~binary_trie();

    // trie iterators
    iterator begin();

    iterator end();

    const_iterator begin() const;

    const_iterator end() const;

    const_iterator cbegin() const;

    const_iterator cend() const;

    reverse_iterator rbegin();

    reverse_iterator rend();

    const_reverse_iterator rbegin() const;

    const_reverse_iterator rend() const;

    const_reverse_iterator crbegin() const;

    const_reverse_iterator crend() const;

    // insertion
    pair_iterator_bool insert_equal(const binary_type& key,
        const value_type& value);

    pair_iterator_bool insert_unique(const binary_type& key,
        const value_type& value);

    // lookup
    // iterator range will share a key
    iterator_range find_equal(const binary_type& key);

    // iterator range will span keys with matching prefix
    iterator_range find_prefix(const binary_type& key);

    // deletion
    bool remove_equal(const binary_type& key);

    bool remove_prefix(const binary_type& key);

    iterator remove_value(iterator it);

protected:

    void destroy();

    // structure node management
    structure_node_type* create_structure_node();

    structure_node_type* create_structure_node(const binary_type& key);

    structure_node_type* create_structure_node(const binary_type& key,
        const value_node_type* value_node);

    structure_node_type* create_structure_node(const binary_type& key,
        const value_type& value);

    bool destroy_structure_node(structure_node_type* node);

    // value node management
    value_node_type* create_value_node(const value_type& value);

    bool destroy_value_node(value_node_type* node);

    // seek methods
    structure_node_type* get_leftmost_leaf(structure_node_type* origin) const;

    structure_node_type* get_rightmost_leaf(structure_node_type* origin) const;

    structure_node_type* get_leftmost_node(structure_node_type* origin) const;

    structure_node_type* get_rightmost_node(structure_node_type* origin) const;

    // next/previous/leftmost/rightmost pointer updates
    void update_left_and_right(structure_node_type* node);

    void update_left_and_right_branch(structure_node_type* node);

    structure_node_type* next_node_with_value(structure_node_type* node);

    void link_node(structure_node_type* node);

    void unlink_node(structure_node_type* node);

    // cleanup/teardown
    void erase_values(structure_node_type* node);

    void erase_subtree(structure_node_type* node);

    structure_node_type* compress_branch(structure_node_type* node);

    // tree append/insert
    value_node_type* append_value(structure_node_type* node,
        value_node_type* value_node);

    value_node_type* append_value(structure_node_type* node,
        const value_type& value);

    void attach_child(structure_node_type* parent, structure_node_type* child);

    structure_node_type* insert_at(structure_node_type* current,
        const binary_type& key);

    structure_node_type* insert_at(structure_node_type* current,
        const binary_type& key, const value_type& value);

    pair_iterator_bool insert(structure_node_type*& current,
        const binary_type& key, const value_type& value);

    pair_node_size find_closest_subkey_matching_node(structure_node_type* start,
        const binary_type& key);

private:

    structure_node_allocator structure_allocator_;
    value_node_allocator value_allocator_;
    value_type_comparer value_comparer_;
    structure_node_type* root_;
};

}

#include <bitcoin/bitcoin/impl/trie/binary_trie.ipp>

#endif
