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
#include <bitcoin/system/intrinsics/intrinsics.hpp>
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

// Carryless multiplication folds 16 byte blocks, from four blocks.
constexpr size_t fold_block = 16;
constexpr size_t fold_minimum = 4u * fold_block;
constexpr auto use_clmul = have_aes && have_128;

uint32_t crc32(const data_slice& data, uint32_t crc) NOEXCEPT
{
    auto value = bit_not(crc);
    auto it = data.begin();
    if constexpr (use_clmul)
    {
        if (data.size() >= fold_minimum)
        {
            const auto size = data.size() - data.size() % fold_block;
            value = crc::checksum(value, it, size);
            it = std::next(it, size);
        }
    }

    for (; it != data.end(); ++it)
    {
        const auto index = bit_and<uint32_t>(bit_xor<uint32_t>(value, *it),
            max_uint8);
        value = bit_xor(table.at(index), shift_right(value, byte_bits));
    }

    return bit_not(value);
}

} // namespace system
} // namespace libbitcoin
