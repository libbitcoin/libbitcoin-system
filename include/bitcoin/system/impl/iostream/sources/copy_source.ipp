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
#ifndef LIBBITCOIN_SYSTEM_IOSTREAM_SOURCES_COPY_SOURCE_IPP
#define LIBBITCOIN_SYSTEM_IOSTREAM_SOURCES_COPY_SOURCE_IPP

#include <algorithm>
#include <iterator>
#include <utility>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/math/limits.hpp>

namespace libbitcoin {
namespace system {

template <typename Container>
copy_source<Container>::copy_source(const Container& data) noexcept
  : base_source(limit<size_type>(data.size())),
    container_(data),
    from_(data.begin())
{
}

template <typename Container>
void copy_source<Container>::do_read(value_type* to, size_type size) noexcept
{
    // std::copy_n returns iterator past last element copied to.
    std::copy_n(from_, size, to);
    std::advance(from_, size);
}

template <typename Container>
typename copy_source<Container>::sequence
copy_source<Container>::input_sequence() noexcept
{
    // input_sequence requires non-const buffer pointers.
    const auto begin = const_cast<value_type*>(container_.data());
    const auto end = std::next(begin, container_.size());

    return std::make_pair(
        reinterpret_cast<char_type*>(begin),
        reinterpret_cast<char_type*>(end));
}

////template <typename Container>
////bool copy_source<Container>::putback(char_type) noexcept
////{
////    if (from_ == container_.begin())
////        return false;
////
////    // The character value is ignored, as it is presumed to still exist.
////    // This precludes putting back a different character and will not detect an
////    // error of attempting to do so, assuming that is even considered an error.
////    --size_;
////    std::advance(from_, negative_one);
////    return true;
////}

} // namespace system
} // namespace libbitcoin

#endif

// unfinished.
////template <typename Container>
////typename copy_source<Container>::position
////copy_source<Container>::seek(offset count, direction way) noexcept
////{
////    const auto scope = container_.size();
////    const auto expended = scope - size_;
////    const auto remaining = size_;
////
////    // Boost example code has over overflows depending on types in use.
////    if (way == std::ios_base::beg)
////    {
////        if (!is_negative(count) && is_lesser(count, scope))
////        {
////            size_ = container_.size() - count;
////            from_ = std::next(container_.begin(), count);
////            return count;
////        }
////    }
////    else if (way == std::ios_base::end)
////    {
////        // zero specifies the last byte, not the end.
////        if (is_negative(count) && is_lesser(absolute(count), scope))
////        {
////            size_ = container_.size() + count;
////            from_ = std::next(container_.begin(), count);
////            return count;
////        }
////    }
////    else if (way == std::ios_base::cur)
////    {
////        if (( is_negative(count) || is_lesser(count, remaining)) &&
////            (!is_negative(count) || is_lesser(absolute(count), expended))
////            return set_position(offset count, direction way);
////    }
////
////    // Avoid exceptions. If parameterization is invalid the predictable
////    // return is zero, resetting the position to start. This is non-standard.
////    return 0;
////}