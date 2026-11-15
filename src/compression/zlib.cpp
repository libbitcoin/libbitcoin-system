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
#include <bitcoin/system/compression/zlib.hpp>

#include <bitcoin/system/compression/deflate.hpp>
#include <bitcoin/system/compression/inflate.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/hash/hash.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// rfc-editor.org/rfc/rfc1950.txt

namespace libbitcoin {
namespace system {

// Deflate with a 32K window, default level, no dictionary (rfc1950 2.2).
constexpr uint8_t method_info = 0x78;
constexpr uint8_t level_flags = 0x9c;

constexpr uint8_t deflate_method = 8;
constexpr uint8_t method_mask = 0x0f;
constexpr size_t info_shift = 4;
constexpr uint8_t maximum_info = 7;
constexpr uint8_t dictionary_flag = 0x20;
constexpr size_t check_modulus = 31;
constexpr size_t header_size = 2;
constexpr size_t trailer_size = sizeof(uint32_t);

data_chunk zlib_deflate(const data_slice& in) NOEXCEPT
{
    BC_PUSH_WARNING(NO_THROW_IN_NOEXCEPT)
    data_chunk out{ method_info, level_flags };
    const auto body = deflate(in);
    const auto check = to_big_endian(adler32(in));
    out.insert(out.end(), body.begin(), body.end());
    out.insert(out.end(), check.begin(), check.end());
    BC_POP_WARNING()
    return out;
}

bool zlib_inflate(data_chunk& out, const data_slice& in, size_t limit) NOEXCEPT
{
    if (in.size() < header_size + trailer_size)
        return false;

    const auto method = in.front();
    const auto flags = *std::next(in.begin());
    const auto check = bit_or(shift_left(wide_cast<size_t>(method),
        byte_bits), wide_cast<size_t>(flags));

    if (bit_and(method, method_mask) != deflate_method ||
        shift_right(method, info_shift) > maximum_info ||
        is_nonzero(bit_and(flags, dictionary_flag)) ||
        is_nonzero(check % check_modulus))
        return false;

    const auto begin = std::next(in.begin(), header_size);
    const auto end = std::next(in.begin(), in.size() - trailer_size);
    if (!inflate(out, data_slice(begin, end), limit))
        return false;

    return unsafe_from_big_endian<uint32_t>(end) == adler32(out);
}

} // namespace system
} // namespace libbitcoin
