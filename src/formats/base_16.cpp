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

bool is_base16(char character)
{
    return
        ('0' <= character && character <= '9') ||
        ('A' <= character && character <= 'F') ||
        ('a' <= character && character <= 'f');
}

std::string encode_base16(const data_slice& data)
{
    std::string out;
    out.reserve(data.size() * 2u);

    const auto to_hex = [](uint8_t offset)
    {
        const char base = (0x0 <= offset && offset <= 0x9) ? '0' : 'a';
        return std::string(1, base + offset);
    };

    for (const auto byte: data)
        out += to_hex(byte >> 4) + to_hex(byte & 0x0f);

    return out;
}

bool decode_base16(data_chunk& out, const std::string& in)
{
    // Prevents a last odd character from being ignored.
    if (in.size() % 2u != 0u)
        return false;

    if (!std::all_of(in.begin(), in.end(), is_base16))
        return false;

    const auto from_hex = [](char character)
    {
        if ('A' <= character && character <= 'F')
            return 10u + character - 'A';

        if ('a' <= character && character <= 'f')
            return 10u + character - 'a';

        return 0u + character - '0';
    };

    out.clear();
    out.reserve(in.size() / 2u);
    auto to = out.begin();

    for (auto from = in.begin(); from != in.end(); std::advance(from, 2))
        *to++ = (from_hex(in[0]) << 4u) | from_hex(in[1]);

    return true;
}

} // namespace system
} // namespace libbitcoin
