/*
 * Copyright 2011-2015
 *
 * Modified from https://github.com/BoostGSoC13/boost.trie
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See http://www.boost.org/LICENSE_1_0.txt)
 *
 */

#ifndef LIBBITCOIN_TRIE_REVERSE_ITERATOR_HPP
#define LIBBITCOIN_TRIE_REVERSE_ITERATOR_HPP

#include <bitcoin/bitcoin/trie/trie_iterator.hpp>
#include <bitcoin/bitcoin/utility/binary.hpp>

namespace libbitcoin {

template<typename TrieIterator>
class trie_reverse_iterator
{
public:
    // iterator_traits required typedefs
    typedef typename std::iterator_traits<TrieIterator>::difference_type differcent_type;
    typedef typename std::iterator_traits<TrieIterator>::iterator_category iterator_category;
    typedef typename std::iterator_traits<TrieIterator>::pointer pointer;
    typedef typename std::iterator_traits<TrieIterator>::reference reference;
    typedef typename std::iterator_traits<TrieIterator>::value_type value_type;

    typedef TrieIterator base_iter_type;
    typedef trie_reverse_iterator<base_iter_type> iter_type;

public:

    // constructors
    trie_reverse_iterator();

    explicit trie_reverse_iterator(base_iter_type it);

    trie_reverse_iterator(const iter_type& other);

    template<typename Iter>
    trie_reverse_iterator(const trie_reverse_iterator<Iter> &other);

    // iterator methods
    reference operator*() const;

    pointer operator->() const;

    bool operator==(const iter_type& other) const;

    bool operator!=(const iter_type& other) const;

    iter_type& operator++();

    iter_type operator++(int);

    iter_type& operator--();

    iter_type operator--(int);

    // accessors
    base_iter_type base() const;

    binary_type get_key();

private:

    base_iter_type base_iterator_;
};

}

#include <bitcoin/bitcoin/impl/trie/trie_reverse_iterator.ipp>

#endif
