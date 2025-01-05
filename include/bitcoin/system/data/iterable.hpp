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

////template <size_t Lanes, typename Iterable,
////    if_std_array<typename Iterable::value_t> = true>
////inline std_array<typename Iterable::value_t, Lanes>&
////array_cast(Iterable& it) NOEXCEPT
////{
////    return unsafe_array_cast<typename Iterable::value_t, Lanes>(it.data());
////}
////
////template <size_t Lanes, typename Iterable,
////    if_std_array<typename Iterable::value_t> = true>
////inline const std_array<typename Iterable::value_t, Lanes>&
////array_cast(const Iterable& it) NOEXCEPT
////{
////    return unsafe_array_cast<typename Iterable::value_t, Lanes>(it.data());
////}
////
////template <size_t Lanes, typename Iterable,
////    if_std_array<typename Iterable::value_t> = true>
////inline std_array<typename Iterable::value_t, Lanes>&
////array_cast(Iterable& it, size_t offset) NOEXCEPT
////{
////    using block = typename Iterable::value_t;
////    constexpr auto size = size_of<block>();
////    return unsafe_array_cast<block, Lanes>(std::next(it.data(), offset * size));
////}
////
////template <size_t Lanes, typename Iterable,
////    if_std_array<typename Iterable::value_t> = true>
////inline const std_array<typename Iterable::value_t, Lanes>&
////array_cast(const Iterable& it, size_t offset) NOEXCEPT
////{
////    using block = typename Iterable::value_t;
////    constexpr auto size = size_of<block>();
////    return unsafe_array_cast<block, Lanes>(std::next(it.data(), offset * size));
////}

/// Iterate any data souce as a set of const std_array&.
template<typename Array, if_std_array<Array> = true>
class iterable
{
public:
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = Array;
        using difference_type = ptrdiff_t;
        using pointer = value_type const*;
        using reference = value_type const&;

        inline iterator(uint8_t const* position) NOEXCEPT
          : position_(position)
        {
        }

        inline reference operator*() const NOEXCEPT
        {
            using element = array_element<value_type>;
            constexpr auto count = array_count<value_type>;
            return unsafe_array_cast<element, count>(position_);
        }

        inline iterator& operator++() NOEXCEPT
        {
            constexpr auto size = size_of<value_type>();
            std::advance(position_, size);
            return *this;
        }

        inline iterator operator++(int) NOEXCEPT
        {
            auto self = *this;
            ++(*this);
            return self;
        }

        inline bool operator==(const iterator& other) const NOEXCEPT
        {
            return position_ == other.position_;
        }

        inline bool operator!=(const iterator& other) const NOEXCEPT
        {
            return !(*this == other);
        }

    private:
        uint8_t const* position_;
    };

public:
    using value_t = Array;
    static constexpr auto value_size = size_of<Array>();

    inline iterable() NOEXCEPT
      : count_(zero), begin_(nullptr), end_(nullptr)
    {
    }

    template <size_t Size>
    inline iterable(const data_array<Size>& data) NOEXCEPT
      : count_(count(Size)),
        begin_(data.data()),
        end_(std::next(begin_, count_ * value_size))
    {
    }

    inline iterable(size_t size, uint8_t const* data) NOEXCEPT
      : count_(count(size)),
        begin_(data),
        end_(std::next(begin_, count_ * value_size))
    {
    }

    inline iterable(const data_chunk& data) NOEXCEPT
      : count_(count(data.size())),
        begin_(data.data()),
        end_(std::next(begin_, count_ * value_size))
    {
    }

    inline iterator begin() const NOEXCEPT
    {
        return iterator(begin_);
    }

    inline const iterator end() const NOEXCEPT
    {
        return iterator(end_);
    }

    inline size_t size() const NOEXCEPT
    {
        return count_;
    }

    inline bool empty() const NOEXCEPT
    {
        return is_zero(size());
    }

    inline uint8_t const* data() const NOEXCEPT
    {
        return begin_;
    }

    template <size_t Elements = one>
    inline iterable& advance() NOEXCEPT
    {
        // This is safe for overflow, will advance to end.
        const auto size = std::min(Elements, count_);
        count_ -= size;
        std::advance(begin_, size * value_size);
        return *this;
    }

    template <size_t Elements = one>
    inline const std_array<value_t, Elements>& to_array() const NOEXCEPT
    {
        return unsafe_array_cast<value_t, Elements>(begin_);
    }

private:
    static constexpr size_t count(size_t size) NOEXCEPT
    {
        if constexpr (is_zero(value_size))
        {
            return zero;
        }
        else
        {
            return size / value_size;
        }
    }

    size_t count_;
    uint8_t const* begin_;
    uint8_t const* end_;
};

/// Iterate any non-const data souce as a non-const set of std_array&.
template<typename Array, if_std_array<Array> = true>
class mutable_iterable
{
public:
    class iterator
    {
    public:
        // std::iterator_traits
        using iterator_category = std::forward_iterator_tag;
        using value_type = Array;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        inline iterator(uint8_t* position) NOEXCEPT
          : position_(position)
        {
        }

        inline reference operator*() NOEXCEPT
        {
            using element = array_element<value_type>;
            constexpr auto count = array_count<value_type>;
            return unsafe_array_cast<element, count>(position_);
        }

        inline iterator& operator++() NOEXCEPT
        {
            constexpr auto size = size_of<value_type>();
            std::advance(position_, size);
            return *this;
        }

        inline iterator operator++(int) NOEXCEPT
        {
            auto self = *this;
            ++(*this);
            return self;
        }

        inline bool operator==(const iterator& other) const NOEXCEPT
        {
            return position_ == other.position_;
        }

        inline bool operator!=(const iterator& other) const NOEXCEPT
        {
            return !(*this == other);
        }

    private:
        uint8_t* position_;
    };

public:
    using value_t = Array;
    static constexpr auto value_size = size_of<Array>();

    inline mutable_iterable() NOEXCEPT
      : count_(zero), begin_(nullptr), end_(nullptr)
    {
    }

    template <size_t Size>
    inline mutable_iterable(data_array<Size>& data) NOEXCEPT
      : count_(count(Size)),
        begin_(data.data()),        
        end_(std::next(begin_, count_ * value_size))
    {
    }

    inline mutable_iterable(size_t size, uint8_t* data) NOEXCEPT
      : count_(count(size)),
        begin_(data),        
        end_(std::next(begin_, count_ * value_size))
    {
    }

    inline mutable_iterable(data_chunk& data) NOEXCEPT
      : count_(count(data.size())),
        begin_(data.data()),
        end_(std::next(begin_, count_ * value_size))
    {
    }

    inline iterator begin() NOEXCEPT
    {
        return iterator(begin_);
    }

    inline const iterator end() NOEXCEPT
    {
        return iterator(end_);
    }

    inline size_t size() const NOEXCEPT
    {
        return count_;
    }

    inline bool empty() const NOEXCEPT
    {
        return is_zero(size());
    }

    inline uint8_t* data() NOEXCEPT
    {
        return begin_;
    }

    template <size_t Elements>
    inline mutable_iterable& advance() NOEXCEPT
    {
        // This is safe for overflow, will advance to end.
        const auto size = std::min(Elements, count_);
        count_ -= size;
        std::advance(begin_, size * value_size);
        return *this;
    }

    template <size_t Elements>
    inline std_array<value_t, Elements>& to_array() NOEXCEPT
    {
        return unsafe_array_cast<value_t, Elements>(begin_);
    }

private:
    static constexpr size_t count(size_t size) NOEXCEPT
    {
        if constexpr (is_zero(value_size))
        {
            return zero;
        }
        else
        {
            return size / value_size;
        }
    }

    size_t count_;
    uint8_t* begin_;
    uint8_t* end_;
};

} // namespace system
} // namespace libbitcoin

#endif
