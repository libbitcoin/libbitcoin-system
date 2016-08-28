/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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

    // constructors
    point_iterator(const point& value);
    point_iterator(const point& value, bool end);
    point_iterator(const point& value, uint8_t offset);

    operator bool() const;

    // iterator methods
    reference operator*() const;
    pointer operator->() const;

    bool operator==(const iterator& other) const;
    bool operator!=(const iterator& other) const;

    iterator& operator++();
    iterator operator++(int);
    iterator& operator--();
    iterator operator--(int);

protected:
    void increment();
    void decrement();

private:
    const point& point_;
    uint8_t offset_;
};

} // namespace chain
} // namespace libbitcoin

#endif
