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
#include <bitcoin/system/type_constraints.hpp>

namespace libbitcoin {
namespace system {
   
/// Resizable but otherwise const iterable wrapper for a memory buffer.
/// Not a substitute for move overrides or containment.
/// Accepts any sizeof(T) == 1 type as a "byte" and emits uint8_t.
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

    /// Copy construction.
    data_slice(const data_slice& other);

    /// Literal bytes constructor.
    /// Integral null terminator is not indexed.
    template <size_type Size, typename Byte>
    data_slice(const Byte(&bytes)[Size]);

    /// Byte array constructor.
    template <size_type Size, typename Byte, if_byte<Byte> = true>
    data_slice(const std::array<Byte, Size>& data);

    /// Byte vector constructor.
    template <typename Byte, if_byte<Byte> = true>
    data_slice(const std::vector<Byte>& data);

    /// Byte iterators constructor.
    template <typename Iterator>
    data_slice(const Iterator& begin, const Iterator& end);

    /// Byte pointers constructor.
    template <typename Byte, if_byte<Byte> = true>
    data_slice(const Byte* begin, const Byte* end);

    /// Byte initializer list constructor.
    /// Accepts literal numbers as 'int' and truncates them to byte.
    /// All elements must be of the same type to satisfy the template.
    template <typename Byte>
    data_slice(std::initializer_list<Byte> bytes);

    /// String constructor, null terminator is not indexed.
    data_slice(const std::string& text);

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

    /// Resize the slice by decrementing the end pointer.
    /// This is the only mutable action that can be taken on the slice.
    /// Returns true if the size was reduced (expansion is not allowed).
    bool resize(size_t size);

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
    data_slice(const_pointer begin, const_pointer end, size_type size);

    template <size_type Size, typename Byte>
    static data_slice from_literal(const Byte(&text)[Size]);

    template <typename Iterator>
    static data_slice from_iterators(const Iterator& begin,
        const Iterator& end);

    template <typename Pointer>
    static data_slice from_size(Pointer begin, size_type size);

    const const_pointer begin_;
    const_pointer end_;
    size_type size_;
};

/// Binary operators.
bool operator==(const data_slice& left, const data_slice& right);
bool operator!=(const data_slice& left, const data_slice& right);

typedef std::initializer_list<data_slice> data_loaf;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/data_slice.ipp>

#endif
