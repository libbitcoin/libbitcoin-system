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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_58_HPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_58_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

BC_API bool is_base58(const char character) NOEXCEPT;
BC_API bool is_base58(const std::string& text) NOEXCEPT;

/// Converts a base58 string to a number of bytes.
/// False if the input is malformed, or the wrong length.
template <size_t Size>
bool decode_base58(data_array<Size>& out, const std::string& in) NOEXCEPT;

/// Converts a base58 string literal to a data array.
template <size_t Size>
data_array<Size * 733 / 1000> base58_array(const char(&string)[Size]) NOEXCEPT;

/// Encode data as base58.
BC_API std::string encode_base58(const data_slice& unencoded) NOEXCEPT;

/// Attempt to decode base58 data.
/// False if the input contains non-base58 characters.
BC_API bool decode_base58(data_chunk& out, const std::string& in) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/radix/base_58.ipp>

#endif

