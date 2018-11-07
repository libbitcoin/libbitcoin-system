/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/bitcoin/formats/base_64.hpp>

#include <cstdint>
#include <string>
#include <bitcoin/bitcoin/utility/data.hpp>

// This implementation derived from public domain:
// en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64

namespace libbitcoin {

const static char pad = '=';

const static char table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string encode_base64(data_slice unencoded)
{
    std::string encoded;
    const auto size = unencoded.size();
    encoded.reserve(((size / 3) + (size % 3 > 0)) * 4);

    uint32_t value;
    auto cursor = unencoded.begin();
    for (size_t position = 0; position < size / 3; position++)
    {
        // Convert to big endian.
        value = (*cursor++) << 16;

        value += (*cursor++) << 8;
        value += (*cursor++);
        encoded.append(1, table[(value & 0x00FC0000) >> 18]);
        encoded.append(1, table[(value & 0x0003F000) >> 12]);
        encoded.append(1, table[(value & 0x00000FC0) >> 6]);
        encoded.append(1, table[(value & 0x0000003F) >> 0]);
    }

    switch (size % 3)
    {
        case 1:
            // Convert to big endian.
            value = (*cursor++) << 16;

            encoded.append(1, table[(value & 0x00FC0000) >> 18]);
            encoded.append(1, table[(value & 0x0003F000) >> 12]);
            encoded.append(2, pad);
            break;
        case 2:
            // Convert to big endian.
            value = (*cursor++) << 16;

            value += (*cursor++) << 8;
            encoded.append(1, table[(value & 0x00FC0000) >> 18]);
            encoded.append(1, table[(value & 0x0003F000) >> 12]);
            encoded.append(1, table[(value & 0x00000FC0) >> 6]);
            encoded.append(1, pad);
            break;
    }

    return encoded;
}

bool decode_base64(data_chunk& out, const std::string& in)
{
    const static uint32_t mask = 0x000000FF;

    const auto length = in.length();
    if ((length % 4) != 0)
        return false;

    size_t padding = 0;
    if (length > 0)
    {
        if (in[length - 1] == pad)
            padding++;
        if (in[length - 2] == pad)
            padding++;
    }

    data_chunk decoded;
    decoded.reserve(((length / 4) * 3) - padding);

    uint32_t value = 0;
    for (auto cursor = in.begin(); cursor < in.end();)
    {
        for (size_t position = 0; position < 4; position++)
        {
            value <<= 6;
            if (*cursor >= 0x41 && *cursor <= 0x5A)
                value |= *cursor - 0x41;
            else if (*cursor >= 0x61 && *cursor <= 0x7A)
                value |= *cursor - 0x47;
            else if (*cursor >= 0x30 && *cursor <= 0x39)
                value |= *cursor + 0x04;
            else if (*cursor == 0x2B)
                value |= 0x3E;
            else if (*cursor == 0x2F)
                value |= 0x3F;
            else if (*cursor == pad)
            {
                // Handle 1 or 2 pad characters.
                switch (in.end() - cursor)
                {
                    case 1:
                        decoded.push_back((value >> 16) & mask);
                        decoded.push_back((value >> 8) & mask);
                        out = decoded;
                        return true;
                    case 2:
                        decoded.push_back((value >> 10) & mask);
                        out = decoded;
                        return true;
                    default:
                        return false;
                }
            }
            else
                return false;

            cursor++;
        }

        decoded.push_back((value >> 16) & mask);
        decoded.push_back((value >> 8) & mask);
        decoded.push_back((value >> 0) & mask);
    }

    out = decoded;
    return true;
}

} // namespace libbitcoin

