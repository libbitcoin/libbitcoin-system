/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#include <bitcoin/system/radix/base_16.hpp>

#include <algorithm>
#include <cstdint>
#include <string>
#include <bitcoin/system/boost.hpp>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>

// base16 (hexidecimal):
// Base 16 is an ascii data encoding with a domain of 16 symbols (characters).
// 16 is 2^4 so base16 is a 4<=>4 bit mapping.
// Invalid padding in byte packing is not detectable as it is byte-aligned.
// So this implementation is limited to an 8<=>8 bit mapping.
// There is no need for bit streaming or padding, and both sides are
// authoritative. Therefore base16 is our canonical byte encoding.

namespace libbitcoin {
namespace system {

// Undefined (but safe) behavior if characters are not base16. 
uint8_t encode_octet(const char(&string)[add1(octet_width)]) noexcept
{
    return from_base16_characters(string[0], string[1]);
}

std::string encode_base16(const data_slice& data) noexcept
{
    std::string out;
    out.resize(data.size() * octet_width);
    auto digit = out.begin();

    for (const auto byte: data)
    {
        *digit++ = to_base16_character(byte >> to_half(byte_bits));
        *digit++ = to_base16_character(byte & 0x0f);
    }

    return out;
}


std::string encode_hash(const data_slice& hash) noexcept
{
    std::string out;
    out.resize(hash.size() * octet_width);
    auto digit = out.begin();

    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    for (const auto byte: boost::adaptors::reverse(hash))
    BC_POP_WARNING()
    {
        *digit++ = to_base16_character(byte >> to_half(byte_bits));
        *digit++ = to_base16_character(byte & 0x0f);
    }

    return out;
}


bool decode_base16(data_chunk& out, const std::string& in) noexcept
{
    if (is_odd(in.size()))
        return false;

    if (!std::all_of(in.begin(), in.end(), is_base16<char>))
        return false;

    out.resize(in.size() / octet_width);
    auto data = out.begin();

    for (auto digit = in.begin(); digit != in.end();)
    {
        const auto hi = *digit++;
        const auto lo = *digit++;
        *data++ = from_base16_characters(hi, lo);
    }

    return true;
}

} // namespace system
} // namespace libbitcoin
