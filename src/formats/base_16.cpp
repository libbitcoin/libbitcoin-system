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
#include <bitcoin/system/formats/base_16.hpp>

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

std::string encode_base16(const data_slice& data)
{
    std::stringstream out;
    out << std::hex << std::setfill('0');

    for (const uint16_t value: data)
        out << std::setw(2) << value;

    return out.str();
}

bool is_base16(char character)
{
    return
        ('0' <= character && character <= '9') ||
        ('A' <= character && character <= 'F') ||
        ('a' <= character && character <= 'f');
}

static unsigned from_hex(char character)
{
    if ('A' <= character && character <= 'F')
        return 10 + character - 'A';

    if ('a' <= character && character <= 'f')
        return 10 + character - 'a';

    return character - '0';
}

bool decode_base16(data_chunk& out, const std::string& in)
{
    // This prevents a last odd character from being ignored:
    if (in.size() % 2 != 0)
        return false;

    data_chunk result(in.size() / 2);
    if (!decode_base16_private(result.data(), result.size(), in.data()))
        return false;

    out = result;
    return true;
}

// Bitcoin hash format (these are all reversed):
std::string encode_hash(hash_digest hash)
{
    std::reverse(hash.begin(), hash.end());
    return encode_base16(hash);
}

bool decode_hash(hash_digest& out, const std::string& in)
{
    if (in.size() != 2 * hash_size)
        return false;

    hash_digest result;
    if (!decode_base16_private(result.data(), result.size(), in.data()))
        return false;

    // Reverse:
    std::reverse_copy(result.begin(), result.end(), out.begin());
    return true;
}

hash_digest hash_literal(const char (&string)[2 * hash_size + 1])
{
    hash_digest out;
    DEBUG_ONLY(const auto success =) decode_base16_private(out.data(),
        out.size(), string);
    BITCOIN_ASSERT(success);
    std::reverse(out.begin(), out.end());
    return out;
}

// For support of template implementation only, do not call directly.
bool decode_base16_private(uint8_t* out, size_t size, const char* in)
{
    if (!std::all_of(in, in + 2 * size, is_base16))
        return false;

    for (size_t index = 0; index < size; ++index)
    {
        out[index] = (from_hex(in[0]) << 4) + from_hex(in[1]);
        in += 2;
    }

    return true;
}

} // namespace system
} // namespace libbitcoin
