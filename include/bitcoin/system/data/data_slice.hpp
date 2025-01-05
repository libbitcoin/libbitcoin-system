/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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
#include <initializer_list>
#include <string>
#include <vector>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Lifetime:
/// "An rvalue may be used to initialize an rvalue reference, in which case
/// the lifetime of the object identified by the rvalue is extended until
/// the scope of the reference ends.":
/// en.cppreference.com/w/cpp/language/value_category#rvalue
/// A prvalue has the "same properties" as an rvalue:
/// en.cppreference.com/w/cpp/language/value_category#prvalue
/// A temporary is a prvalue, and includes initializer_list, but a copy is not:
/// en.cppreference.com/w/cpp/language/implicit_conversion#Temporary_materialization
/// When the data_slice is accepted as a const& it is either passed by
/// reference or materialized as a prvalue. A function/method reference extends
/// lifetime, but a member reference does not.

/// Downsizable but otherwise const iterable wrapper for const buffer.
/// Identical to data_slab except pointer is const, and therefore accepts
/// construction from const sources (including literals and initializers).
/// Not a substitute for move overrides or containment.
/// Accepts any sizeof(T) == 1 type as a "byte" and emits uint8_t.
/// [] iteration past end is safe and returns zeros.
/// Negative size construction yields a valid empty object.
class data_slice
{
public:
    DEFAULT_COPY_MOVE_DESTRUCT(data_slice);

    typedef size_t size_type;
    typedef uint8_t value_type;

    /// A pointer to non-mutable bytes (pointer itself is mutable).
    typedef const value_type* pointer;

    /// For stream source compatibility, until data_slice has an iterator.
    typedef pointer const_iterator;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Empty slice.
    constexpr data_slice() NOEXCEPT;

    /// Literal bytes constructor.
    /// Integral null terminator is not indexed.
    template <size_type Size>
    constexpr data_slice(const char(&text)[Size]) NOEXCEPT;

    /// Byte array constructor (casts Byte to uint8_t).
    template <size_type Size, typename Byte, if_one_byte<Byte> = true>
    constexpr data_slice(const std_array<Byte, Size>& data) NOEXCEPT;

    /// Byte vector constructor (casts Byte to uint8_t).
    template <typename Byte, if_one_byte<Byte> = true>
    VCONSTEXPR data_slice(const std_vector<Byte>& data) NOEXCEPT;

    // TODO: restrict to iterator-to-const references.
    /// Byte iterators constructor (casts to uint8_t).
    template <typename Iterator>
    constexpr data_slice(const Iterator& begin, const Iterator& end) NOEXCEPT;

    // TODO: change to begin/size construction.
    /// Byte pointer to const constructor (casts Byte to uint8_t).
    template <typename Byte, if_one_byte<Byte> = true>
    constexpr data_slice(const Byte* begin, const Byte* end) NOEXCEPT;

    /// String constructor (casts char to uint8_t).
    SCONSTEXPR data_slice(const std::string& text) NOEXCEPT;

    /// Byte initializer list constructor.
    SVCONSTEXPR data_slice(std::initializer_list<value_type> bytes) NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    /// Copy data to an array.
    /// Underfill is padded with 0x00, excess is truncated.
    template <size_type Size>
    constexpr std_array<value_type, Size> to_array() const NOEXCEPT;

    /// Copy data to a pmr vector (not constexpr).
    inline std_vector<value_type> to_chunk() const NOEXCEPT;

    /// Convert data to a string (casts uint8_t to char).
    SCONSTEXPR std::string to_string() const NOEXCEPT;

    /// Resize the slice by decrementing the end pointer.
    /// This is the only mutable action that can be taken on the slice.
    /// Returns true if the size was reduced (expansion is not allowed).
    constexpr bool resize(size_t size) NOEXCEPT;

    /// Properties.
    /// -----------------------------------------------------------------------
    constexpr pointer data() const NOEXCEPT;
    constexpr pointer begin() const NOEXCEPT;
    constexpr pointer end() const NOEXCEPT;
    constexpr value_type front() const NOEXCEPT;
    constexpr value_type back() const NOEXCEPT;
    constexpr size_type size() const NOEXCEPT;
    constexpr bool empty() const NOEXCEPT;

    /// Operators.
    /// -----------------------------------------------------------------------
    ////template<size_type Size>
    ////constexpr operator std_array<value_type, Size>() const NOEXCEPT;
    ////VCONSTEXPR operator std_vector<value_type>() const NOEXCEPT;
    constexpr value_type operator[](size_type index) const NOEXCEPT;

private:
    constexpr data_slice(pointer begin, pointer end, size_type size) NOEXCEPT;

    template <size_type Size, typename Byte>
    static constexpr data_slice from_literal(const Byte(&text)[Size]) NOEXCEPT;

    template <typename Iterator>
    static constexpr data_slice from_iterators(const Iterator& begin,
        const Iterator& end) NOEXCEPT;

    // TODO: not constexpr (reinterpret_cast).
    template <typename Pointer>
    static constexpr data_slice from_size(
        Pointer begin, size_type size) NOEXCEPT;

    template <typename Pointer>
    static SVCONSTEXPR data_slice from_size_(
        Pointer begin, size_type size) NOEXCEPT;

    pointer begin_;
    pointer end_;
    size_type size_;
};

/// Binary operators.
constexpr bool operator==(const data_slice& left, const data_slice& right) NOEXCEPT;
constexpr bool operator!=(const data_slice& left, const data_slice& right) NOEXCEPT;

typedef std::initializer_list<data_slice> data_loaf;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/data_slice.ipp>

#endif
