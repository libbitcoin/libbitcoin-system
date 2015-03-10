/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_TRIE_STRUCTURE_NODE_IPP
#define LIBBITCOIN_TRIE_STRUCTURE_NODE_IPP

namespace libbitcoin {

// trie_node implenentation
template<typename Value>
trie_structure_node<Value>::trie_structure_node()
: parent(0), value_head(nullptr), value_tail(nullptr), value_leftmost(nullptr),
    value_rightmost(nullptr), previous(nullptr), next(nullptr), label()
{
    children_[0] = nullptr;
    children_[1] = nullptr;
}

template<typename Value>
bool trie_structure_node<Value>::has_children() const
{
    return (children_[0] != nullptr) || (children_[1] != nullptr);
}

template<typename Value>
bool trie_structure_node<Value>::has_value() const
{
    return !(value_head == nullptr);
}

template<typename Value>
typename trie_structure_node<Value>::structure_node_type*
    trie_structure_node<Value>::get_child(
        bool matching_bit) const
{
    return matching_bit ? children_[1] : children_[0];
}

template<typename Value>
void trie_structure_node<Value>::set_child(
    bool matching_bit, structure_node_type* node)
{
    int index = matching_bit ? 1 : 0;
    children_[index] = node;
}

template<typename Value>
typename trie_structure_node<Value>::structure_node_type*
    trie_structure_node<Value>::get_first_child() const
{
    auto result = children_[0];

    if (result == nullptr)
    {
        result = children_[1];
    }

    return result;
}

template<typename Value>
typename trie_structure_node<Value>::structure_node_type*
trie_structure_node<Value>::get_last_child() const
{
    auto result = children_[1];

    if (result == nullptr)
    {
        result = children_[0];
    }

    return result;
}

}

#endif
