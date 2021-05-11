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
#include <cstdint>
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
 * Convert a byte vector to a hexidecimal string.
 */
BC_API std::string encode_base16(const data_slice& data);

/**
 * Convert a hexidecimal string to a byte vector.
 * @return false if the input is malformed.
 */
BC_API bool decode_base16(data_chunk& out, const std::string& in);

/**
 * Convert a hexidecimal string to a byte array.
 * @return false if the input is malformed, or the wrong length.
 */
template <size_t Size>
bool decode_base16(byte_array<Size>& out, const std::string& in);

/**
 * Convert a byte array to a reversed byte order hexidecimal string.
 * The bitcoin hash format is base16 with the bytes reversed.
 * This format is generally used only for display formatting.
 */
template <size_t Size>
std::string encode_hash(const byte_array<Size>& hash);

/**
 * Convert a reversed byte order hexidecimal string to a byte array.
 * The bitcoin hash format is base16 with the bytes reversed.
 * This format is generally used only for display formatting.
 * @return false if the input is malformed.
 */
template <size_t Size>
bool decode_hash(byte_array<Size>& out, const std::string& in);

/**
 * Convert a hexidecimal string literal to a byte array.
 * Zeroized array returned in case of invalid or odd count of hex characters.
 * TODO: en.cppreference.com/w/cpp/language/user_literal
 */
template <size_t Size>
byte_array<(Size - 1u) / 2u> base16_literal(const char(&string)[Size]);

/**
 * Convert a reversed byte order hexidecimal string literal to a byte array.
 * The bitcoin hash format is base16 with the bytes reversed.
 * This format is generally used only for display formatting.
 * Zeroized array returned in case of invalid or odd count of hex characters.
 * TODO: en.cppreference.com/w/cpp/language/user_literal
 */
template <size_t Size>
byte_array<(Size - 1u) / 2u> hash_literal(const char (&string)[Size]);


// TODO: replace base16_literal with this (rename).
template <size_t Size>
byte_array<(Size - 1u) / 2u> base16_array(const char(&string)[Size])
{
    return base16_literal(string);
}

// TODO: replace hash_literal with this (rename).
template <size_t Size>
byte_array<(Size - 1u) / 2u> base16_hash(const char(&string)[Size])
{
    return hash_literal(string);
}

// TODO: move to ipp and update code to use this (mostly test cases).
template <size_t Size>
data_chunk base16_chunk(const char(&string)[Size])
{
    return to_chunk(base16_literal(string));
}

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/formats/base_16.ipp>

#endif
