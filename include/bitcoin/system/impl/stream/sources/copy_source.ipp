/////**
//// * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
//// *
//// * This file is part of libbitcoin.
//// *
//// * This program is free software: you can redistribute it and/or modify
//// * it under the terms of the GNU Affero General Public License as published by
//// * the Free Software Foundation, either version 3 of the License, or
//// * (at your option) any later version.
//// *
//// * This program is distributed in the hope that it will be useful,
//// * but WITHOUT ANY WARRANTY; without even the implied warranty of
//// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// * GNU Affero General Public License for more details.
//// *
//// * You should have received a copy of the GNU Affero General Public License
//// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
//// */
////#ifndef LIBBITCOIN_SYSTEM_STREAM_SOURCES_COPY_SOURCE_IPP
////#define LIBBITCOIN_SYSTEM_STREAM_SOURCES_COPY_SOURCE_IPP
////
////#include <algorithm>
////#include <iterator>
////#include <bitcoin/system/math/limits.hpp>
////
////namespace libbitcoin {
////namespace system {
////
////template <typename Container>
////copy_source<Container>::copy_source(const Container& data) noexcept
////  : device(limit<size_type>(data.size())),
////    container_(data),
////    from_(data.begin())
////{
////}
////
////template <typename Container>
////typename copy_source<Container>::sequence
////copy_source<Container>::do_sequence() noexcept
////{
////    // input_sequence/output_sequence both require non-const buffer pointers,
////    // but the data member is const, so we must cast it for direct devices.
////    const auto begin = const_cast<value_type*>(container_.data());
////    const auto end = std::next(begin, container_.size());
////
////    return std::make_pair(
////        reinterpret_cast<char_type*>(begin),
////        reinterpret_cast<char_type*>(end));
////}
////
////// TODO: It seems likely that this is never called on a direct device.
////template <typename Container>
////void copy_source<Container>::do_read(value_type* to, size_type size) noexcept
////{
////    // std::copy_n returns iterator past last element copied to.
////    std::copy_n(from_, size, to);
////    std::advance(from_, size);
////}
////
////} // namespace system
////} // namespace libbitcoin
////
////#endif
