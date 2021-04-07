/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_BASE_16_HPP
#define LIBBITCOIN_SYSTEM_BASE_16_HPP

#include <cstddef>
#include <string>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/hash.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

/**
 * Returns true if a character is a hexadecimal digit.
 * The C standard library function `isxdigit` depends on the current locale,
 * and does not necessarily match the base16 encoding.
 */
BC_API bool is_base16(char character);

/**
 * Convert data into a user-readable hex string.
 */
BC_API std::string encode_base16(const data_slice& data);

/**
 * Convert a hex string into bytes.
 * @return false if the input is malformed.
 */
BC_API bool decode_base16(data_chunk& out, const std::string& in);

/**
 * Converts a hex string to a number of bytes.
 * @return false if the input is malformed, or the wrong length.
 */
template <size_t Size>
bool decode_base16(byte_array<Size>& out, const std::string& in);

/**
 * Converts a bitcoin_hash to a string.
 * The bitcoin hash format is base16 with the bytes reversed.
 * This format is generally used only for display formatting.
 */
template <size_t Size>
std::string encode_hash(const byte_array<Size>& hash);

/**
 * Convert a string into a bitcoin hash.
 * The bitcoin hash format is base16 with the bytes reversed.
 * This format is generally used only for display formatting.
 * @return false if the input is malformed.
 */
template <size_t Size>
bool decode_hash(byte_array<Size>& out, const std::string& in);

/**
 * UNSAFE: used primarly for test cases.
 * Converts a hex string literal to a data array.
 */
template <size_t Size>
byte_array<(Size - 1u) / 2u> base16_literal(const char(&string)[Size]);

/**
 * UNSAFE: used primarly for test cases.
 * Convert a hex string literal into a bitcoin hash.
 * The bitcoin hash format is base16 with the bytes reversed.
 * This format is generally used only for display formatting.
 */
template <size_t Size>
byte_array<(Size - 1u) / 2u> hash_literal(const char (&string)[Size]);

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/formats/base_16.ipp>

#endif
