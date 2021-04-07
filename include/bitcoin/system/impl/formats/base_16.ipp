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
#ifndef LIBBITCOIN_SYSTEM_BASE_16_IPP
#define LIBBITCOIN_SYSTEM_BASE_16_IPP

#include <algorithm>
#include <bitcoin/system/utility/assert.hpp>

namespace libbitcoin {
namespace system {

template <size_t Size>
bool decode_base16(byte_array<Size>& out, const std::string& in)
{
    data_chunk data(Size);
    const auto result = decode_base16(data, in);
    std::copy(data.begin(), data.end(), out.begin());
    return result;
}

template <size_t Size>
std::string encode_hash(const byte_array<Size>& hash)
{
    auto reversed = hash;
    std::reverse(reversed.begin(), reversed.end());
    return encode_base16(reversed);
}

template <size_t Size>
bool decode_hash(byte_array<Size>& out, const std::string& in)
{
    data_chunk decoded;
    if (!decode_base16(decoded, in.data()))
        return false;

    std::reverse_copy(decoded.begin(), decoded.end(), out.begin());
    return true;
}

template <size_t Size>
byte_array<(Size - 1u) / 2u> base16_literal(const char (&string)[Size])
{
    byte_array<(Size - 1u) / 2u> out;
    DEBUG_ONLY(const auto result =) decode_base16(out, string);
    BITCOIN_ASSERT_MSG(result, "invalid base16 literal");
    return out;
}

template <size_t Size>
byte_array<(Size - 1u) / 2u> hash_literal(const char (&string)[Size])
{
    byte_array<(Size - 1u) / 2u> out;
    DEBUG_ONLY(const auto result =) decode_hash(out, string);
    BITCOIN_ASSERT_MSG(result, "invalid hash literal");
    return out;
}

} // namespace system
} // namespace libbitcoin

#endif
