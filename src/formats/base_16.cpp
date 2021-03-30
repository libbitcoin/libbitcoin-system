/**
 * Copyright (c) 2011-2017 libbitcoin developers (see AUTHORS)
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
#include <iomanip>
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {

std::string encode_base16(const data_slice& data)
{
    std::stringstream stream;
    stream << std::hex << std::setfill('0');

    for (int value: data)
        stream << std::setw(2) << value;

    return stream.str();
}

bool is_base16(const char value)
{
    return
        ('0' <= value && value <= '9') ||
        ('A' <= value && value <= 'F') ||
        ('a' <= value && value <= 'f');
}

static unsigned from_hex(const char value)
{
    if ('A' <= value && value <= 'F')
        return 10 + value - 'A';

    if ('a' <= value && value <= 'f')
        return 10 + value - 'a';

    return value - '0';
}

bool decode_base16(data_chunk& out, const std::string& in)
{
    // This prevents a last odd character from being ignored:
    if (in.size() % 2u != 0u)
        return false;

    data_chunk result(in.size() / 2u);
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
    if (in.size() != 2u * hash_size)
        return false;

    hash_digest result;
    if (!decode_base16_private(result.data(), result.size(), in.data()))
        return false;

    // Reverse:
    std::reverse_copy(result.begin(), result.end(), out.begin());
    return true;
}

hash_digest hash_literal(const char (&string)[2u * hash_size + 1u])
{
    hash_digest out;
    DEBUG_ONLY(const auto success =) decode_base16_private(out.data(),
        out.size(), string);
    BITCOIN_ASSERT(success);
    std::reverse(out.begin(), out.end());
    return out;
}

// For support of template implementation only, do not call directly.
bool decode_base16_private(uint8_t* out, size_t out_size, const char* in)
{
    if (!std::all_of(in, in + 2u * out_size, is_base16))
        return false;

    for (size_t i = 0; i < out_size; ++i)
    {
        out[i] = (from_hex(in[0]) << 4u) + from_hex(in[1]);
        in += 2u;
    }

    return true;
}

} // namespace libbitcoin
