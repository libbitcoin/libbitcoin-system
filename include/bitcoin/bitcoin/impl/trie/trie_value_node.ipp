/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_TRIE_VALUE_NODE_IPP
#define LIBBITCOIN_TRIE_VALUE_NODE_IPP

namespace libbitcoin {

// trie_value_node implementation
template<typename Value>
trie_value_node<Value>::trie_value_node()
    : anchor(nullptr), next(nullptr), previous(nullptr), value()
{
}

template<typename Value>
trie_value_node<Value>::trie_value_node(const value_type& value)
    : anchor(nullptr), next(nullptr), previous(nullptr), value(value)
{
}

}

#endif
