/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#include <bitcoin/system/compression/gzip.hpp>

#include <algorithm>
#include <bitcoin/system/compression/deflate.hpp>
#include <bitcoin/system/compression/inflate.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// rfc-editor.org/rfc/rfc1952.txt

namespace libbitcoin {
namespace system {

// Member identification and compression method (rfc1952 2.3.1).
constexpr uint8_t first_id = 0x1f;
constexpr uint8_t second_id = 0x8b;
constexpr uint8_t deflate_method = 8;

// Flags (rfc1952 2.3.1).
constexpr uint8_t header_crc_flag = 0x02;
constexpr uint8_t extra_flag = 0x04;
constexpr uint8_t name_flag = 0x08;
constexpr uint8_t comment_flag = 0x10;
constexpr uint8_t reserved_flags = 0xe0;

// No flags, no modification time, no extra flags, unknown file system.
constexpr uint8_t no_flags = 0x00;
constexpr uint8_t no_time = 0x00;
constexpr uint8_t no_extra_flags = 0x00;
constexpr uint8_t unknown_system = 0xff;

constexpr size_t header_size = 10;
constexpr size_t trailer_size = 2u * sizeof(uint32_t);

BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)

data_chunk gzip_deflate(const data_slice& in) NOEXCEPT
{
    data_chunk out
    {
        first_id, second_id, deflate_method, no_flags,
        no_time, no_time, no_time, no_time,
        no_extra_flags, unknown_system
    };

    const auto body = deflate(in);
    const auto check = to_little_endian(crc32(in));
    const auto size = to_little_endian(possible_narrow_cast<uint32_t>(
        in.size()));

    out.insert(out.end(), body.begin(), body.end());
    out.insert(out.end(), check.begin(), check.end());
    out.insert(out.end(), size.begin(), size.end());
    return out;
}

// Advance past a zero terminated string (rfc1952 2.3.1).
static bool skip_string(size_t& position, const data_slice& in) NOEXCEPT
{
    const auto begin = std::next(in.begin(), position);
    const auto it = std::find(begin, in.end(), 0_u8);
    if (it == in.end())
        return false;

    position += add1(sign_cast<size_t>(std::distance(begin, it)));
    return true;
}

static bool inflate_member(data_chunk& out, size_t& consumed,
    const data_slice& in, size_t limit) NOEXCEPT
{
    const auto size = in.size();
    if (size < header_size)
        return false;

    const auto at = [&](size_t position) NOEXCEPT
    {
        return std::next(in.begin(), position);
    };

    const auto flags = *at(3);
    if (*at(0) != first_id || *at(1) != second_id ||
        *at(2) != deflate_method || is_nonzero(bit_and(flags, reserved_flags)))
        return false;

    auto position = header_size;
    if (is_nonzero(bit_and(flags, extra_flag)))
    {
        if (size - position < sizeof(uint16_t))
            return false;

        const auto length = unsafe_from_little_endian<uint16_t>(at(position));
        position += sizeof(uint16_t);
        if (size - position < length)
            return false;

        position += length;
    }

    if (is_nonzero(bit_and(flags, name_flag)) && !skip_string(position, in))
        return false;

    if (is_nonzero(bit_and(flags, comment_flag)) && !skip_string(position, in))
        return false;

    if (is_nonzero(bit_and(flags, header_crc_flag)))
    {
        if (size - position < sizeof(uint16_t))
            return false;

        const auto check = narrow_cast<uint16_t>(crc32(data_slice(at(zero),
            at(position))));

        if (unsafe_from_little_endian<uint16_t>(at(position)) != check)
            return false;

        position += sizeof(uint16_t);
    }

    size_t length{};
    if (!inflate(out, length, data_slice(at(position), in.end()), limit))
        return false;

    position += length;
    if (size - position < trailer_size)
        return false;

    const auto check = unsafe_from_little_endian<uint32_t>(at(position));
    const auto modulo = unsafe_from_little_endian<uint32_t>(
        at(position + sizeof(uint32_t)));

    if (check != crc32(out) ||
        modulo != possible_narrow_cast<uint32_t>(out.size()))
        return false;

    consumed = position + trailer_size;
    return true;
}

bool gzip_inflate(data_chunk& out, const data_slice& in, size_t limit) NOEXCEPT
{
    out.clear();
    auto position = in.begin();
    do
    {
        size_t consumed{};
        data_chunk member{};
        if (!inflate_member(member, consumed, data_slice(position, in.end()),
            limit))
            return false;

        if (!is_zero(limit) && member.size() > limit - out.size())
            return false;

        out.insert(out.end(), member.begin(), member.end());
        position = std::next(position, consumed);
    } while (position != in.end());

    return true;
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin
