/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_TRIE_ITERATOR_HPP
#define LIBBITCOIN_TRIE_ITERATOR_HPP

#include <bitcoin/bitcoin/trie/trie_structure_node.hpp>
#include <bitcoin/bitcoin/trie/trie_value_node.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>

namespace libbitcoin {

template<typename Value, typename StructureNodeAllocator,
    typename ValueNodeAllocator, typename Comparer>
class binary_trie;

template<typename Value, typename Reference, typename Pointer>
class trie_iterator
{
    template<class V, class StructureNodeAllocator, class ValueNodeAllocator,
        class Comparer> friend class binary_trie;

public:
    // iterator_traits required typedefs
    typedef ptrdiff_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef Pointer pointer;
    typedef Reference reference;
    typedef Value value_type;

    typedef trie_iterator<Value, Reference, Pointer> iter_type;
    typedef trie_iterator<Value, Value&, Value*> iterator;
    typedef trie_iterator<Value, const Value&, const Value*> const_iterator;
    typedef trie_structure_node<Value> structure_node_type;
    typedef trie_value_node<Value> value_node_type;

public:

    // constructors
    explicit trie_iterator();

    trie_iterator(structure_node_type* node);

    trie_iterator(value_node_type* value);

    trie_iterator(structure_node_type* node, value_node_type* value);

    trie_iterator(const iterator& it);

    binary_type get_key();

    // iterator methods
    reference operator*() const;

    pointer operator->() const;

    bool operator==(const trie_iterator& other) const;

    bool operator!=(const trie_iterator& other) const;

    iter_type& operator++();

    iter_type operator++(int);

    iter_type& operator--();

    iter_type operator--(int);

protected:
    // increment/decrement implementation
    void trie_node_increment();

    void trie_node_decrement();

    void increment();

    void decrement();

private:

    structure_node_type* trie_node_;
    value_node_type* value_node_;
    binary_type cached_key_;
};

}

#include <bitcoin/bitcoin/impl/trie/trie_iterator.ipp>

#endif
