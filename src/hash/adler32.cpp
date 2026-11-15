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
#include <bitcoin/system/hash/adler32.hpp>

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// rfc-editor.org/rfc/rfc1950.txt (section 9)

namespace libbitcoin {
namespace system {

constexpr uint32_t modulus = 65521;

// The most bytes summed before high can overflow 32 bits.
constexpr size_t reduction = 5552;

uint32_t adler32(const data_slice& data) NOEXCEPT
{
    uint32_t low{ one };
    uint32_t high{ zero };
    auto it = data.begin();
    for (auto remaining = data.size(); !is_zero(remaining);)
    {
        const auto count = std::min(remaining, reduction);
        remaining -= count;
        for (const auto end = std::next(it, count); it != end; ++it)
        {
            low += *it;
            high += low;
        }

        low %= modulus;
        high %= modulus;
    }

    return bit_or(shift_left(high, bits<uint16_t>), low);
}

} // namespace system
} // namespace libbitcoin
