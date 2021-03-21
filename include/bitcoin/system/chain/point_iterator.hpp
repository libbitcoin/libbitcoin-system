/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_CHAIN_POINT_ITERATOR_HPP
#define LIBBITCOIN_CHAIN_POINT_ITERATOR_HPP

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <bitcoin/bitcoin/define.hpp>

namespace libbitcoin {
namespace chain {

class point;

/// A point iterator for store serialization (does not support wire).
class BC_API point_iterator
{
public:
    typedef uint8_t pointer;
    typedef uint8_t reference;
    typedef uint8_t value_type;
    typedef ptrdiff_t difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;

    typedef point_iterator iterator;
    typedef point_iterator const_iterator;

    // Constructors.
    //-------------------------------------------------------------------------

    point_iterator(const point_iterator& other);
    point_iterator(const point& value, unsigned index=0);

    // Operators.
    //-------------------------------------------------------------------------

    operator bool() const;
    pointer operator->() const;
    reference operator*() const;
    point_iterator& operator++();
    point_iterator operator++(int);
    point_iterator& operator--();
    point_iterator operator--(int);
    point_iterator operator+(const int value) const;
    point_iterator operator-(const int value) const;
    bool operator==(const point_iterator& other) const;
    bool operator!=(const point_iterator& other) const;

    /// The iterator may only be assigned to another of the same point.
    point_iterator& operator=(const point_iterator& other);

protected:
    void increment();
    void decrement();
    point_iterator increase(unsigned value) const;
    point_iterator decrease(unsigned value) const;

private:
    uint8_t current() const;

    const point& point_;
    unsigned current_;
};

} // namespace chain
} // namespace libbitcoin

#endif
