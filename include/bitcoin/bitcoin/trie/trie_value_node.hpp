/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_TRIE_VALUE_NODE_HPP
#define LIBBITCOIN_TRIE_VALUE_NODE_HPP

#include <boost/utility.hpp>

namespace libbitcoin {

template<typename Value>
class trie_structure_node;

template<typename Value>
class trie_value_node: protected boost::noncopyable
{
public:

    typedef Value value_type;
    typedef trie_structure_node<value_type> structure_node_type;
    typedef trie_value_node<value_type> value_node_type;

    structure_node_type* anchor;
    value_node_type* next;
    value_node_type* previous;
    value_type value;

public:

    explicit trie_value_node();

    explicit trie_value_node(const value_type& x);
};

}

#include <bitcoin/bitcoin/impl/trie/trie_value_node.ipp>

#endif
