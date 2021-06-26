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
////#ifndef LIBBITCOIN_SYSTEM_STREAM_SOURCES_MOVE_SOURCE_IPP
////#define LIBBITCOIN_SYSTEM_STREAM_SOURCES_MOVE_SOURCE_IPP
////
////#include <algorithm>
////#include <iterator>
////#include <bitcoin/system/math/limits.hpp>
////
////namespace libbitcoin {
////namespace system {
////
////template <typename Container>
////move_source<Container>::move_source(const Container& data) noexcept
////  : device(limit<size_type>(data.size())),
////    container_(data),
////    from_(data.begin())
////{
////}
////
////template <typename Container>
////void move_source<Container>::do_read(value_type* to, size_type size) noexcept
////{
////    // std::move does not have a size overload.
////    // std::move returns iterator past last element moved to.
////    auto end = std::next(from_, size);
////    std::move(from_, end, to);
////    from_ = end;
////}
////
////} // namespace system
////} // namespace libbitcoin
////
////#endif
