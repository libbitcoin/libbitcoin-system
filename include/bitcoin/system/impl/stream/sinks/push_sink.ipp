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
////#ifndef LIBBITCOIN_SYSTEM_STREAM_SINKS_PUSH_SINK_IPP
////#define LIBBITCOIN_SYSTEM_STREAM_SINKS_PUSH_SINK_IPP
////
////#include <iterator>
////#include <bitcoin/system/math/limits.hpp>
////#include <bitcoin/system/type_constraints.hpp>
////
////namespace libbitcoin {
////namespace system {
////    
////template <typename Container, bool>
////push_sink<Container, true>::push_sink(Container& data) noexcept
////  : device(limit<size_type>(data.max_size() - data.size())),
////    container_(data),
////    to_(data.begin())
////{
////}
////
////template <typename Container, bool>
////void push_sink<Container, true>::do_write(const value_type* from,
////    size_type size) noexcept
////{
////    to_ = std::next(container_.insert(to_, from, std::next(from, size)), size);
////}
////
////} // namespace system
////} // namespace libbitcoin
////
////#endif
