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

#include <utility>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/endian/endian.hpp>
#include <bitcoin/system/intrinsics/intrinsics.hpp>
#include <bitcoin/system/math/math.hpp>

// based on:
// rfc-editor.org/rfc/rfc1950.txt (section 9)

namespace libbitcoin {
namespace system {

constexpr uint32_t modulus = 65521;

// The most bytes summed before high can overflow 32 bits.
constexpr size_t reduction = 5552;

// Scalar.
// ----------------------------------------------------------------------------

INLINE void accumulate(uint32_t& low, uint32_t& high, const uint8_t*& data,
    size_t& size) NOEXCEPT
{
    while (!is_zero(size))
    {
        const auto count = std::min(size, reduction);
        size -= count;
        for (const auto end = std::next(data, count); data != end; ++data)
        {
            low += *data;
            high += low;
        }

        low %= modulus;
        high %= modulus;
    }
}

// Lanes.
// ----------------------------------------------------------------------------
// Each 32 bit lane sums each of its four bytes, by position, across the words
// of a round, and the prefix sums the bytes of all preceding words. The sums
// of a word at position j of the word contribute (bytes - j) to high.

template <typename xWord, size_t... Lane>
INLINE std_array<uint32_t, sizeof...(Lane)> to_lanes(xWord value,
    std::index_sequence<Lane...>) NOEXCEPT
{
    return { f::get<uint32_t, Lane>(value)... };
}

// Accumulates whole words, advancing data past them.
template <typename xWord>
INLINE void accumulate(uint32_t& low, uint32_t& high, const uint8_t*& data,
    size_t& size) NOEXCEPT
{
    if constexpr (have<xWord>)
    {
        constexpr auto bytes = sizeof(xWord);
        constexpr auto positions = sizeof(uint32_t);
        constexpr auto lanes = bytes / positions;
        constexpr auto words = reduction / bytes;
        constexpr auto sequence = std::make_index_sequence<lanes>{};
        const auto mask = f::broadcast<xWord, uint32_t>(max_uint8);

        while (size >= bytes)
        {
            const auto count = std::min(size / bytes, words);
            auto prefix = f::broadcast<xWord>(0_u32);
            std_array<xWord, positions> sums
            {
                prefix, prefix, prefix, prefix
            };

            for (size_t word{}; word < count; ++word)
            {
                const auto value = native_from_little_end<uint32_t>(
                    f::load<xWord>(data));

                data = std::next(data, bytes);
                const auto all = f::add<32>(f::add<32>(sums[0], sums[1]),
                    f::add<32>(sums[2], sums[3]));

                prefix = f::add<32>(prefix, all);
                sums[0] = f::add<32>(sums[0], f::and_(value, mask));
                sums[1] = f::add<32>(sums[1],
                    f::and_(f::shr<8, 32>(value), mask));
                sums[2] = f::add<32>(sums[2],
                    f::and_(f::shr<16, 32>(value), mask));
                sums[3] = f::add<32>(sums[3], f::shr<24, 32>(value));
            }

            size -= count * bytes;
            uint64_t sum{}, weighted{};
            for (size_t position{}; position < positions; ++position)
            {
                const auto values = to_lanes(sums[position], sequence);
                for (size_t lane{}; lane < lanes; ++lane)
                {
                    const uint64_t weight = bytes - (lane * positions +
                        position);

                    sum += values[lane];
                    weighted += weight * values[lane];
                }
            }

            uint64_t preceding{};
            for (const auto value: to_lanes(prefix, sequence))
                preceding += value;

            const uint64_t span = count * bytes;
            const auto increase = span * low + bytes * preceding + weighted;
            high = narrow_cast<uint32_t>((high + increase) % modulus);
            low = narrow_cast<uint32_t>((low + sum) % modulus);
        }
    }
}

uint32_t adler32(const data_slice& data, uint32_t adler) NOEXCEPT
{
    auto low = bit_and<uint32_t>(adler, max_uint16);
    auto high = shift_right(adler, bits<uint16_t>);
    auto it = data.data();
    auto size = data.size();
    accumulate<xint512_t>(low, high, it, size);
    accumulate<xint256_t>(low, high, it, size);
    accumulate<xint128_t>(low, high, it, size);
    accumulate(low, high, it, size);
    return bit_or(shift_left(high, bits<uint16_t>), low);
}

} // namespace system
} // namespace libbitcoin
