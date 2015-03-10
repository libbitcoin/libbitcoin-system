/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_TRIE_STRUCTURE_NODE_HPP
#define LIBBITCOIN_TRIE_STRUCTURE_NODE_HPP

#include <boost/utility.hpp>
#include <bitcoin/bitcoin/trie/trie_value_node.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>

namespace libbitcoin {

template <typename Value>
class trie_structure_node : private boost::noncopyable
{
public:

    typedef Value value_type;
    typedef trie_structure_node<value_type> structure_node_type;
    typedef trie_value_node<value_type> value_node_type;

    structure_node_type* parent;

    value_node_type* value_head;
    value_node_type* value_tail;

    value_node_type* value_leftmost;
    value_node_type* value_rightmost;

    structure_node_type* previous;
    structure_node_type* next;

    binary_type label;

public:

    explicit trie_structure_node();

    bool has_children() const;

    bool has_value() const;

    structure_node_type* get_child(bool matching_bit) const;

    void set_child(bool matching_bit, structure_node_type* node);

    structure_node_type* get_first_child() const;

    structure_node_type* get_last_child() const;

private:

    structure_node_type* children_[2];

};

}

#include <bitcoin/bitcoin/impl/trie/trie_structure_node.ipp>

#endif
