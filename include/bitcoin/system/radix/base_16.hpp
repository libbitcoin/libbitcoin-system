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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_16_HPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_16_HPP

#include <string_view>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// The bitcoin hash format is base16 with the bytes reversed.
/// This reversed format is used by this library only for display formatting.

/// Utilities.
/// ---------------------------------------------------------------------------

constexpr size_t octet_width = 2;

/// True if the character is a hexadecimal digit.
template <typename Byte, if_one_byte<Byte> = true>
constexpr bool is_base16(Byte character) NOEXCEPT;

/// Byte value of the literal octet, undefined (but safe) if not base16.
constexpr uint8_t encode_octet(const char(&string)[add1(octet_width)]) NOEXCEPT;

/// Encoding of data_slice (e.g. data_array/data_chunk/string) to hex string.
/// ---------------------------------------------------------------------------

/// Convert a byte vector to a hexidecimal string.
constexpr std::string encode_base16(const data_slice& data) NOEXCEPT;

/// Convert a byte array to a reversed byte order hexidecimal string.
constexpr std::string encode_hash(const data_slice& hash) NOEXCEPT;

/// Decoding of hex string to data_array or data_chunk.
/// ---------------------------------------------------------------------------

/// Convert a hexidecimal string to a byte vector.
/// False if the input is malformed.
constexpr bool decode_base16(data_chunk& out, const std::string& in) NOEXCEPT;

/// Convert a hexidecimal string to a byte array.
/// False if the input is malformed, or the wrong length.
template <size_t Size>
constexpr bool decode_base16(data_array<Size>& out,
    const std::string_view& in) NOEXCEPT;

/// Convert a reversed byte order hexidecimal string to a byte array.
/// False if the input is malformed, or the wrong length.
template <size_t Size>
constexpr bool decode_hash(data_array<Size>& out,
    const std::string_view& in) NOEXCEPT;

/// Literal decodings of hex string, errors reflected as zero-filled data.
/// ---------------------------------------------------------------------------

/// Convert a literal hex string to a string (bytes are cast to string chars).
/// Empty string returned if decoding fails.
template <size_t Size, if_odd<Size> = true>
constexpr std::string base16_string(const char(&string)[Size]) NOEXCEPT;

/// Convert a literal hexidecimal string literal to pmr vector (not constexpr).
/// Empty chunk returned if decoding fails.
template <size_t Size, if_odd<Size> = true>
data_chunk base16_chunk(const char(&string)[Size]) NOEXCEPT;

/// Convert a hexidecimal string literal to a byte array.
/// Zeroized array returned if decoding fails.
template <size_t Size, if_odd<Size> = true>
constexpr data_array<to_half(sub1(Size))>
base16_array(const char(&string)[Size]) NOEXCEPT;

/// Convert a reversed byte order hexidecimal string literal to a byte array.
/// Zeroized array returned if decoding fails.
template <size_t Size, if_odd<Size> = true>
constexpr data_array<to_half(sub1(Size))>
base16_hash(const char(&string)[Size]) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/radix/base_16.ipp>

#endif
