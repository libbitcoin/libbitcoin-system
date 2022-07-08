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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_ALGORITHM_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_ALGORITHM_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

// to_big_data()
template <typename Data, typename Integer>
constexpr Data to_big_chunk(Data&& bytes, Integer value) NOEXCEPT;

// to_big_data()
template <typename Data, typename Integer>
constexpr Data to_little_chunk(Data&& bytes, Integer value) NOEXCEPT;

// from_big_array()
template <typename Integer, size_t Size>
constexpr Integer from_big_array(const data_array<Size>& data) NOEXCEPT;

// from_little_array()
template <typename Integer, size_t Size>
constexpr Integer from_little_array(const data_array<Size>& data) NOEXCEPT;

// from_big_array<length>()
template <typename Integer, size_t Size>
constexpr Integer from_big_chunk(size_t size, const data_array<Size>& data) NOEXCEPT;

// from_big_chunk<length>()
template <typename Integer>
VCONSTEXPR Integer from_big_chunk(size_t size, const data_chunk& data) NOEXCEPT;

// from_little_array<length>()
template <typename Integer, size_t Size>
constexpr Integer from_little_chunk(size_t size, const data_array<Size>& data) NOEXCEPT;

// from_little_chunk<length>()
template <typename Integer>
VCONSTEXPR Integer from_little_chunk(size_t size, const data_chunk& data) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/algorithm.ipp>

#endif
