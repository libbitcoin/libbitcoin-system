/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_TRIE_ITERATOR_IPP
#define LIBBITCOIN_TRIE_ITERATOR_IPP

namespace libbitcoin {

// trie_iterator implementation
template<typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator()
    : trie_node_(nullptr), value_node_(nullptr), cached_key_()
{
}

template<typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator(
    structure_node_type* node)
    : trie_node_(node), value_node_(node->value_head), cached_key_()
{
}

template<typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator(value_node_type* value)
    : trie_node_(value->anchor), value_node_(value), cached_key_()
{
}

template<typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator(
    structure_node_type* node, value_node_type* value)
    : trie_node_(node), value_node_(value), cached_key_()
{
}

template<typename Value, typename Reference, typename Pointer>
trie_iterator<Value, Reference, Pointer>::trie_iterator(const iterator& it)
    : trie_node_(it.trie_node_), value_node_(it.value_node_),
        cached_key_(it.cached_key_)
{
}

template<typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::reference trie_iterator<
    Value, Reference, Pointer>::operator*() const
{
    return value_node_->value;
}

template<typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::pointer trie_iterator<Value,
    Reference, Pointer>::operator->() const
{
    return &(operator*());
}

template<typename Value, typename Reference, typename Pointer>
bool trie_iterator<Value, Reference, Pointer>::operator==(
    const trie_iterator& other) const
{
    return (trie_node_ == other.trie_node_)
        && (value_node_ == other.value_node_);
}

template<typename Value, typename Reference, typename Pointer>
bool trie_iterator<Value, Reference, Pointer>::operator!=(
    const trie_iterator& other) const
{
    return !(*this == other);
}

template<typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::iter_type&
trie_iterator<Value, Reference, Pointer>::operator++()
{
    increment();
    return *this;
}

template<typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::iter_type trie_iterator<
    Value, Reference, Pointer>::operator++(int)
{
    iter_type tmp = *this;
    increment();
    return tmp;
}

template<typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::iter_type&
trie_iterator<Value, Reference, Pointer>::operator--()
{
    decrement();
    return *this;
}

template<typename Value, typename Reference, typename Pointer>
typename trie_iterator<Value, Reference, Pointer>::iter_type trie_iterator<
    Value, Reference, Pointer>::operator--(int)
{
    iter_type tmp = *this;
    decrement();
    return tmp;
}

template<typename Value, typename Reference, typename Pointer>
void trie_iterator<Value, Reference, Pointer>::trie_node_increment()
{
    cached_key_.resize(0);

    if (trie_node_->parent != nullptr)
    {
        trie_node_ = trie_node_->next;
    }
}

template<typename Value, typename Reference, typename Pointer>
void trie_iterator<Value, Reference, Pointer>::trie_node_decrement()
{
    cached_key_.resize(0);

    if (trie_node_->previous->parent != nullptr)
    {
        trie_node_ = trie_node_->previous;
    }
}

template<typename Value, typename Reference, typename Pointer>
void trie_iterator<Value, Reference, Pointer>::increment()
{
    // if not at end of current trie node values, continue walk
    if ((value_node_ != nullptr) && (value_node_->next != nullptr))
    {
        value_node_ = value_node_->next;
    }
    // else move to next trie node
    else
    {
        trie_node_increment();
        value_node_ = trie_node_->value_head;
    }
}

template<typename Value, typename Reference, typename Pointer>
void trie_iterator<Value, Reference, Pointer>::decrement()
{
    if ((value_node_ != nullptr) && (value_node_->previous != nullptr))
    {
        value_node_ = value_node_->previous;
    }
    else
    {
        trie_node_decrement();
        value_node_ = trie_node_->value_tail;
    }
}

template<typename Value, typename Reference, typename Pointer>
binary_type trie_iterator<Value, Reference, Pointer>::get_key()
{
    if (cached_key_.size() == 0)
    {
        std::vector<structure_node_type*> nodes;

        auto current = trie_node_;

        while (current != nullptr)
        {
            nodes.push_back(current);
            current = current->parent;
        }

        auto it = nodes.rbegin();

        while (it != nodes.rend())
        {
            cached_key_.append((*it)->label);
            it++;
        }
    }

    return cached_key_;
}

}

#endif
