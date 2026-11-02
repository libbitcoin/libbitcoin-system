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
#include <bitcoin/system/hash/crc32.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// rfc-editor.org/rfc/rfc1952.txt (section 8)

namespace libbitcoin {
namespace system {

constexpr auto polynomial = 0xedb88320_u32;

constexpr auto table = []() NOEXCEPT
{
    std_array<uint32_t, power2(byte_bits)> out{};
    for (size_t byte = 0; byte < out.size(); ++byte)
    {
        auto value = possible_narrow_cast<uint32_t>(byte);
        for (auto bit = zero; bit < byte_bits; ++bit)
        {
            const auto low = get_right(value);
            value = shift_right(value);
            if (low)
                value = bit_xor(value, polynomial);
        }

        out.at(byte) = value;
    }

    return out;
}();

uint32_t crc32(const data_slice& data) NOEXCEPT
{
    auto crc = max_uint32;
    for (const auto byte: data)
    {
        const auto index = bit_and(bit_xor(crc, wide_cast<uint32_t>(byte)),
            wide_cast<uint32_t>(max_uint8));
        crc = bit_xor(table.at(index), shift_right(crc, byte_bits));
    }

    return bit_not(crc);
}

} // namespace system
} // namespace libbitcoin
