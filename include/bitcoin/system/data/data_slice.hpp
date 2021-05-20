/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_SLICE_HPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_SLICE_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <string>
#include <vector>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
   
/// Const iterable wrapper for byte collection pointers.
/// Not a substitute for move overrides or containment.
/// Value (not pointer) iteration past end is safe and returns zeros.
/// Negative size construction yields a valid empty object.
class BC_API data_slice
{
public:
    typedef uint8_t value_type;
    typedef const value_type* const_pointer;
    typedef size_t size_type;
    
    /// Zero is returned when value would be undefined.
    /// This allows a caller to pad by value-iterating past end.
    static const data_slice::value_type pad;

    /// Constructors.

    /// Empty data object.
    data_slice();

    /// Move construction.
    data_slice(data_slice&& other);

    /// Literal text constructor.
    /// Integral null terminator is not indexed.
    template <size_type Size>
    data_slice(const char(&text)[Size]);

    /// Byte array constructor.
    template <size_type Size>
    data_slice(const std::array<value_type, Size>& data);

    /// Iterators (including pointers) constructor.
    template <typename Iterator>
    data_slice(const Iterator& begin, const Iterator& end);

    /// Byte initializer list constructor.
    data_slice(std::initializer_list<value_type> bytes);

    /// String constructor, casts char to uint8_t.
    /// Integral null terminator is not indexed.
    data_slice(const std::string& text);

    /// Byte vector constructor.
    data_slice(const std::vector<value_type>& data);

    /// Pointers constructor.
    data_slice(const_pointer begin, const_pointer end);

    /// Methods.

    /// Copy data to an array.
    /// Underfill is padded with 0x00, excess is truncated.
    template <size_type Size>
    std::array<value_type, Size> to_array() const;

    /// Copy data to a vector.
    std::vector<value_type> to_chunk() const;

    /// Convert data to a string, casts uint8_t to char.
    std::string to_string() const;

    /// Convert data to a base16 string.
    std::string encoded() const;

    /// Properties.
    const_pointer data() const;
    const_pointer begin() const;
    const_pointer end() const;
    value_type front() const;
    value_type back() const;
    size_type size() const;
    bool empty() const;

    /// Unary operators.
    template<size_type Size>
    operator std::array<value_type, Size>() const;
    operator std::vector<value_type>() const;
    value_type operator[](size_type index) const;

private:
    data_slice(const data_slice& other) = delete;
    data_slice(const_pointer begin, const_pointer end, size_type size);

    template <size_type Size>
    static data_slice from_literal(const char(&text)[Size]);

    template <typename Iterator>
    static data_slice from_iterators(const Iterator& begin,
        const Iterator& end);

    template <typename Pointer>
    static data_slice from_size(Pointer begin, size_type size);

    const const_pointer begin_;
    const const_pointer end_;
    const size_type size_;
};

/// Binary operators.
bool operator==(const data_slice& left, const data_slice& right);
bool operator!=(const data_slice& left, const data_slice& right);

typedef std::initializer_list<data_slice> data_loaf;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/data_slice.ipp>

#endif
