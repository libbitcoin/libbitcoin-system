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
#include <bitcoin/bitcoin/formats/base_32.hpp>

#include <cstdint>

namespace libbitcoin {

const static char lut[] = "qpzry9x8gf2tvdw0s3jn54khce6mua7l";
const int8_t rev_lut[128] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15, -1, 10, 17, 21, 20, 26, 30,  7,  5, -1, -1, -1, -1, -1, -1,
    -1, 29, -1, 24, 13, 25,  9,  8, 23, -1, 18, 22, 31, 27, 19, -1,
     1,  0,  3, 16, 11, 28, 12, 14,  6,  4,  2, -1, -1, -1, -1, -1,
    -1, 29, -1, 24, 13, 25,  9,  8, 23, -1, 18, 22, 31, 27, 19, -1,
     1,  0,  3, 16, 11, 28, 12, 14,  6,  4,  2, -1, -1, -1, -1, -1
};
const static char separator = '1';
const static size_t checksum_length = 6;
const static size_t hr_part_min_length = 1;
const static size_t hr_part_max_length = 83;
const static size_t data_part_min_length = checksum_length;
const static size_t base32_min_length = hr_part_min_length + 1 + data_part_min_length;
const static size_t base32_max_length = 90;

uint32_t polymod_base32(const data_chunk& values)
{
    static const std::vector<uint32_t> gen = 
        {0x3b6a57b2, 0x26508e6d, 0x1ea119fa, 0x3d4233dd, 0x2a1462b3};
    uint32_t chk = 1;
    for (const auto& value : values)
    {
        const uint32_t b = (chk >> 25);
        chk = (chk & 0x1ffffff) << 5 ^ value;
        for (auto i = 0; i < 5; ++i)
        {
            if ((b >> i) & 1)
                chk ^= gen[i];
            else
                chk ^= 0;
        }
    }
    return chk;
}

data_chunk hrp_expand_base32(const std::string& text)
{
    data_chunk result(2 * text.size() + 1);
    auto iterator = result.begin();
    for (const auto character : text)
    {
        *iterator = character >> 5;
        ++iterator;
    }
    ++iterator; // is already '0'
    for (const auto character : text)
    {
        *iterator = character & 31;
        ++iterator;
    }
    return result;
}

bool verify_checksum_base32(const std::string& hr_part, const data_chunk& data_part)
{
    data_chunk expanded = hrp_expand_base32(hr_part);
    expanded.insert(expanded.end(), data_part.begin(), data_part.end());
    return polymod_base32(expanded) == 1;
}

data_chunk create_checksum_base32(const std::string& hr_part, const data_chunk& data_part)
{
    data_chunk values = hrp_expand_base32(hr_part);
    values.insert(values.end(), data_part.begin(), data_part.end());
    values.resize(values.size() + checksum_length);
    uint32_t polymod = polymod_base32(values) ^ 1;
    data_chunk result(checksum_length);
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = (polymod >> (5 * (5 - i))) & 31;
    return result;
}

std::string encode_base32(const std::string& hr_part, const data_chunk& data_part)
{
    std::string encoded = hr_part + separator;
    encoded.reserve(encoded.size() + data_part.size() + checksum_length);
    data_chunk checksum = create_checksum_base32(hr_part, data_part);
    for (const auto& value : data_part)
        encoded += lut[value];
    for (const auto& value : checksum)
        encoded += lut[value];
    return encoded;
}

inline char toLowercase(const char& c)
{
    return c + ('a' - 'A');
}

bool decode_base32(std::string& hr_part, data_chunk& values, const std::string& in)
{
    // Basic input check
    if (in.length() < base32_min_length || in.length() > base32_max_length)
        return false;
    
    // Find separator and check valid hr_part and data_part sizes
    size_t pos_separator = in.rfind(separator);
    if (pos_separator == std::string::npos 
        || pos_separator < hr_part_min_length
        || in.length() - (pos_separator + 1) < data_part_min_length)
        return false;
    
    // Convert to a lowercase string and check for valid characters
    bool uppercase = false;
    bool lowercase = false;
    std::string encoded;
    encoded.reserve(in.length());
    for (auto c : in)
    {
        if (c < 33 || c > 126)
            return false;
        if (c >= 'A' && c <= 'Z')
        {
            uppercase = true;
            c = toLowercase(c);
        }
        else if (c >= 'a' && c <= 'z')
            lowercase = true;
        encoded += c;
    }
    // Must NOT accept mixed case strings
    if (uppercase && lowercase)
        return false;
    
    // Convert data part to values
    const size_t data_part_length = encoded.size() - (pos_separator + 1);
    data_chunk data_part_values;
    data_part_values.reserve(data_part_length);
    for (const unsigned char c : encoded.substr(pos_separator + 1))
    {
        if (rev_lut[c] == -1)
            return false;
        data_part_values.push_back(rev_lut[c]);
    }
    
    // Verify checksum
    hr_part = encoded.substr(0, pos_separator);
    if (!verify_checksum_base32(hr_part, data_part_values))
        return false;

    values = data_chunk(data_part_values.begin(), 
        data_part_values.end() - checksum_length);
    return true;
}

} // namespace libbitcoin
