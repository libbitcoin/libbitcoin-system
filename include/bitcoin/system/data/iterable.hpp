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
#ifndef LIBBITCOIN_SYSTEM_DATA_ITERABLE_HPP
#define LIBBITCOIN_SYSTEM_DATA_ITERABLE_HPP

#include <iterator>
#include <bitcoin/system/data/array_cast.hpp>
#include <bitcoin/system/data/data_chunk.hpp>
#include <bitcoin/system/data/exclusive_slice.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Iterate any data souce as a set of const std::array&.
template<typename Array, if_std_array<Array> = true>
class const_iterable
{
private:
    template <typename Array>
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Array;
        using difference_type = ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        inline explicit iterator(const uint8_t* bytes, size_t start) noexcept
          : index_(start), bytes_(bytes)
        {
        }

        inline reference operator*() const noexcept
        {
            using element = array_element<value_type>;
            constexpr auto size = array_count<value_type>;
            static_assert(size * size_of<element>() == size_of<value_type>());
            return unsafe_array_cast<element, size>(std::next(bytes_, index_));
        }

        inline iterator& operator++() noexcept
        {
            constexpr auto element_size = size_of<value_type>();
            index_ += element_size;
            return *this;
        }

        inline iterator operator++(int) noexcept
        {
            auto self = *this;
            ++(*this);
            return self;
        }

        inline bool operator==(const iterator& other) const noexcept
        {
            return index_ == other.index_;
        }

        inline bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

    private:
        size_t index_{};
        const uint8_t* bytes_;
    };

    static constexpr size_t count(size_t size) noexcept
    {
        return size - (size % size_of<Array>());
    }

    const size_t end_;
    const uint8_t* data_;

public:
    template <typename Type, size_t Size>
    inline const_iterable(const std_array<Type, Size>& data) noexcept
      : end_(count(Size)), data_(data.data())
    {
    }

    inline const_iterable(size_t size, const uint8_t* data) noexcept
      : end_(count(size)), data_(data)
    {
    }

    inline const_iterable(const data_chunk& data) noexcept
      : end_(count(data.size())), data_(data.data())
    {
    }

    inline explicit const_iterable(const exclusive_slice& data) noexcept
      : end_(count(data.size())), data_(data.data())
    {
    }

    inline iterator<Array> begin() const noexcept
    {
        return iterator<Array>(data_, zero);
    }

    inline iterator<Array> end() const noexcept
    {
        return iterator<Array>(data_, end_);
    }
};

/// Iterate any non-const data souce as a non-const set of std::array&.
template<typename Array, if_std_array<Array> = true>
class iterable
{
private:
    template <typename Array>
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Array;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        inline explicit iterator(uint8_t* bytes, size_t start) noexcept
          : index_(start), bytes_(bytes)
        {
        }

        inline reference operator*() noexcept
        {
            using element = array_element<value_type>;
            constexpr auto size = array_count<value_type>;
            static_assert(size * size_of<element>() == size_of<value_type>());
            return unsafe_array_cast<element, size>(std::next(bytes_, index_));
        }

        inline iterator& operator++() noexcept
        {
            constexpr auto element_size = size_of<value_type>();
            index_ += element_size;
            return *this;
        }

        inline iterator operator++(int) noexcept
        {
            auto self = *this;
            ++(*this);
            return self;
        }

        inline bool operator==(const iterator& other) const noexcept
        {
            return index_ == other.index_;
        }

        inline bool operator!=(const iterator& other) const noexcept
        {
            return !(*this == other);
        }

    private:
        size_t index_{};
        uint8_t* bytes_;
    };

    static constexpr size_t count(size_t size) noexcept
    {
        return size - (size % size_of<Array>());
    }

    const size_t end_;
    uint8_t* data_;

public:
    template <typename Type, size_t Size>
    inline iterable(std_array<Type, Size>& data) noexcept
      : end_(count(Size)), data_(data.data())
    {
    }

    inline iterable(size_t size, uint8_t* data) noexcept
      : end_(count(size)), data_(data)
    {
    }

    inline iterable(data_chunk& data) noexcept
      : end_(count(data.size())), data_(data.data())
    {
    }

    inline iterator<Array> begin() noexcept
    {
        return iterator<Array>(data_, zero);
    }

    inline iterator<Array> end() const noexcept
    {
        return iterator<Array>(data_, end_);
    }
};

} // namespace system
} // namespace libbitcoin

#endif
