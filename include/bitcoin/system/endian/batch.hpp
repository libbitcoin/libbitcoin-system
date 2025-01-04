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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_BATCH_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_BATCH_HPP

#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {
    
/// Array endian conversion (return by value).
/// ---------------------------------------------------------------------------

/// Copy an array of native integral integers to big-endian.
template <typename Integral, size_t Count>
constexpr std_array<Integral, Count> to_big_endians(
    const std_array<Integral, Count>& values) NOEXCEPT;

/// Copy an array of native integral integers to little-endian.
template <typename Integral, size_t Count>
constexpr std_array<Integral, Count> to_little_endians(
    const std_array<Integral, Count>& values) NOEXCEPT;

/// Copy an array of big-endian integral integers to native.
template <typename Integral, size_t Count>
constexpr std_array<Integral, Count> from_big_endians(
    const std_array<Integral, Count>& values) NOEXCEPT;

/// Copy an array of little-endian integral integers to native.
template <typename Integral, size_t Count>
constexpr std_array<Integral, Count> from_little_endians(
    const std_array<Integral, Count>& values) NOEXCEPT;

/// Array endian conversion (return by reference).
/// ---------------------------------------------------------------------------

/// Copy an array of native integral integers to big-endian.
template <typename Integral, size_t Count>
constexpr void to_big_endians(std_array<Integral, Count>& out,
    const std_array<Integral, Count>& in) NOEXCEPT;

/// Copy an array of native integral integers to little-endian.
template <typename Integral, size_t Count>
constexpr void to_little_endians(std_array<Integral, Count>& out,
    const std_array<Integral, Count>& in) NOEXCEPT;

/// Copy an array of big-endian integral integers to native.
template <typename Integral, size_t Count>
constexpr void from_big_endians(std_array<Integral, Count>& out,
    const std_array<Integral, Count>& in) NOEXCEPT;

/// Copy an array of little-endian integral integers to native.
template <typename Integral, size_t Count>
constexpr void from_little_endians(std_array<Integral, Count>& out,
    const std_array<Integral, Count>& in) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/batch.ipp>

#endif
