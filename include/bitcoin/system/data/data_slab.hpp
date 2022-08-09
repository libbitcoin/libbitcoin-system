/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_DATA_DATA_SLAB_HPP
#define LIBBITCOIN_SYSTEM_DATA_DATA_SLAB_HPP

#include <array>
#include <string>
#include <vector>
#include <bitcoin/system/data/data_slice.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Lifetime:
/// No prvalues are accepted, as they are not meaningfully-writeable.
/// The slab does not extend the lifetime of any rvalue passed to it.
/// The caller must ensure that the references or pointers passed to the slab
/// are not orphaned during its lifetime, which is consistent with any passage
/// by reference. See data_slice for a detailed explanation of lifetime.
/// en.cppreference.com/w/cpp/language/reference#Dangling_references

/// Downsizable but otherwise const iterable wrapper for non-const buffer.
/// Identical to data_slice except pointer is non-const, and therefore does not
/// accept construction from const or movable (non-writeable) sources.
/// Not a substitute for move overrides or containment.
/// Accepts any sizeof(T) == 1 type as a "byte" and emits uint8_t.
/// [] iteration past end is safe and returns zeros.
/// Negative size construction yields a valid empty object.
class data_slab
{
public:
    DEFAULT5(data_slab);

    typedef size_t size_type;
    typedef uint8_t value_type;

    /// A pointer to mutable bytes (pointer itself is also mutable).
    typedef value_type* pointer;

    /// For stream source compatibility, until data_slab has an iterator.
    typedef pointer iterator;

    /// Constructors.
    /// -----------------------------------------------------------------------

    /// Empty slab.
    constexpr data_slab() NOEXCEPT;

    /// Byte array constructor (casts Byte to uint8_t).
    template <size_type Size, typename Byte, if_one_byte<Byte> = true>
    constexpr data_slab(std_array<Byte, Size>& data) NOEXCEPT;

    // TODO: test.
    /// Byte vector constructor (casts Byte to uint8_t).
    template <typename Byte, if_one_byte<Byte> = true>
    VCONSTEXPR data_slab(std::vector<Byte>& data) NOEXCEPT;

    // TODO: restrict to iterator-to-non-const references.
    /// Byte iterators constructor (casts to uint8_t).
    template <typename Iterator>
    constexpr data_slab(const Iterator& begin, const Iterator& end) NOEXCEPT;

    // TODO: change to begin/size construction.
    /// Byte pointer to non-const constructor (casts Byte to uint8_t).
    template <typename Byte, if_one_byte<Byte> = true>
    constexpr data_slab(const Byte* begin, const Byte* end) NOEXCEPT;

    // TODO: test.
    /// String constructor (casts char to uint8_t).
    SCONSTEXPR data_slab(std::string& text) NOEXCEPT;

    /// Methods.
    /// -----------------------------------------------------------------------

    /// Copy data to an array.
    /// Underfill is padded with 0x00, excess is truncated.
    template <size_type Size>
    constexpr std_array<value_type, Size> to_array() const NOEXCEPT;

    /// Copy data to a vector.
    VCONSTEXPR std::vector<value_type> to_chunk() const NOEXCEPT;

    /// Convert data to a string (casts uint8_t to char).
    SCONSTEXPR std::string to_string() const NOEXCEPT;

    /// Cast buffer to a data_slice.
    constexpr data_slice to_slice() const NOEXCEPT;

    /// Resize the slab by decrementing the end pointer.
    /// This is the only mutable action that can be taken on the slab.
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
    template<size_type Size>
    constexpr operator std_array<value_type, Size>() const NOEXCEPT;
    VCONSTEXPR operator std::vector<value_type>() const NOEXCEPT;
    constexpr operator data_slice() const NOEXCEPT;
    constexpr value_type operator[](size_type index) const NOEXCEPT;

private:
    constexpr data_slab(const pointer begin, const pointer end,
        size_type size) NOEXCEPT;

    template <typename Iterator>
    static constexpr data_slab from_iterators(const Iterator& begin,
        const Iterator& end) NOEXCEPT;

    template <typename Pointer>
    static constexpr data_slab from_size(const Pointer begin,
        size_type size) NOEXCEPT;

    template <typename Pointer>
    static SVCONSTEXPR data_slab from_size_(const Pointer begin,
        size_type size) NOEXCEPT;

    pointer begin_;
    pointer end_;
    size_type size_;
};

/// Binary operators.
constexpr bool operator==(const data_slab& left, const data_slab& right) NOEXCEPT;
constexpr bool operator!=(const data_slab& left, const data_slab& right) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/data/data_slab.ipp>

#endif
