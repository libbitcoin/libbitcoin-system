/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_TRIE_REVERSE_ITERATOR_IPP
#define LIBBITCOIN_TRIE_REVERSE_ITERATOR_IPP

namespace libbitcoin {

// trie_reverse_iterator implementation
template <typename TrieIterator>
trie_reverse_iterator<TrieIterator>::trie_reverse_iterator()
: base_iterator_()
{
}

template <typename TrieIterator>
trie_reverse_iterator<TrieIterator>::trie_reverse_iterator(base_iter_type it)
: base_iterator_(it)
{
}

template <typename TrieIterator>
trie_reverse_iterator<TrieIterator>::trie_reverse_iterator(
    const iter_type& other)
: base_iterator_(other.base())
{
}

template <typename TrieIterator>
template<typename Iter>
trie_reverse_iterator<TrieIterator>::trie_reverse_iterator(
    const trie_reverse_iterator<Iter>& other)
: base_iterator_(other.base())
{
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::reference
    trie_reverse_iterator<TrieIterator>::operator*() const
{
    base_iter_type tmp = base_iterator_;
    return *--tmp;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::pointer
    trie_reverse_iterator<TrieIterator>::operator->() const
{
    return &(operator*());
}

template <typename TrieIterator>
bool trie_reverse_iterator<TrieIterator>::operator==(
    const iter_type& other) const
{
    return base() == other.base();
}

template <typename TrieIterator>
bool trie_reverse_iterator<TrieIterator>::operator!=(
    const iter_type& other) const
{
    return !(*this == other);
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::iter_type&
    trie_reverse_iterator<TrieIterator>::operator++()
{
    --base_iterator_;
    return *this;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::iter_type
    trie_reverse_iterator<TrieIterator>::operator++(int)
{
    iter_type tmp = *this;
    --base_iterator_;
    return tmp;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::iter_type&
    trie_reverse_iterator<TrieIterator>::operator--()
{
    ++base_iterator_;
    return *this;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::iter_type
    trie_reverse_iterator<TrieIterator>::operator--(int)
{
    iter_type tmp = *this;
    ++base_iterator_;
    return tmp;
}

template <typename TrieIterator>
typename trie_reverse_iterator<TrieIterator>::base_iter_type
    trie_reverse_iterator<TrieIterator>::base() const
{
    return base_iterator_;
}

template<typename TrieIterator>
binary_type trie_reverse_iterator<TrieIterator>::get_key()
{
    iter_type tmp = base_iterator_;
    return (--tmp).get_key();
}

}

#endif
