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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NEON_CRC_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NEON_CRC_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/neon/neon.hpp>
#include <bitcoin/system/intrinsics/neon/neon_128.hpp>
#include <bitcoin/system/intrinsics/neon/neon_aes.hpp>

#if defined(HAVE_NEON_AES)

namespace libbitcoin {
namespace system {
namespace crc {

/// CRC-32 (reflected 0xedb88320) by carryless multiplication. The algorithm
/// is that of intel_crc, expressed in neon.

INLINE xint128_t constant(uint64_t low, uint64_t high) NOEXCEPT
{
    return vreinterpretq_u32_u64(vcombine_u64(vcreate_u64(low),
        vcreate_u64(high)));
}

/// The carryless product of 64 bit lane A of a and lane B of b.
template <size_t A, size_t B>
INLINE xint128_t multiply(xint128_t a, xint128_t b) NOEXCEPT
{
    return aes::words_of(aes::carryless<A, B>(a, b));
}

template <size_t Bytes>
INLINE xint128_t shift_right(xint128_t a) NOEXCEPT
{
    return aes::words_of(aes::shift_bytes_right<Bytes>(aes::bytes_of(a)));
}

/// The 64 bit lanes of a multiplied by those of k, summed with b.
INLINE xint128_t fold(xint128_t a, xint128_t k, xint128_t b) NOEXCEPT
{
    const auto low = multiply<0, 0>(a, k);
    const auto high = multiply<1, 1>(a, k);
    return veorq_u32(veorq_u32(low, high), b);
}

/// The crc register continued over size bytes, a multiple of 16 of at least
/// 64.
INLINE uint32_t checksum(uint32_t crc, const uint8_t* data,
    size_t size) NOEXCEPT
{
    const auto k1k2 = constant(0x0154442bd4, 0x01c6e41596);
    const auto k3k4 = constant(0x01751997d0, 0x00ccaa009e);
    const auto k5k0 = constant(0x0163cd6124, 0x0000000000);
    const auto poly = constant(0x01db710641, 0x01f7011641);
    const auto mask = constant(0x00000000ffffffff, 0x00000000ffffffff);
    const auto load = [&](size_t position) NOEXCEPT
    {
        return f::load<xint128_t>(std::next(data, position));
    };

    const auto initial = vsetq_lane_u32(crc, vdupq_n_u32(0), 0);
    auto x1 = veorq_u32(load(0), initial);
    auto x2 = load(16);
    auto x3 = load(32);
    auto x4 = load(48);

    size_t position{ 64 };
    for (; size - position >= 64; position += 64)
    {
        x1 = fold(x1, k1k2, load(position));
        x2 = fold(x2, k1k2, load(position + 16));
        x3 = fold(x3, k1k2, load(position + 32));
        x4 = fold(x4, k1k2, load(position + 48));
    }

    x1 = fold(x1, k3k4, x2);
    x1 = fold(x1, k3k4, x3);
    x1 = fold(x1, k3k4, x4);
    for (; size - position >= 16; position += 16)
        x1 = fold(x1, k3k4, load(position));

    // 128 to 64 bits.
    x2 = multiply<0, 1>(x1, k3k4);
    x1 = veorq_u32(shift_right<8>(x1), x2);
    x2 = shift_right<4>(x1);
    x1 = vandq_u32(x1, mask);
    x1 = veorq_u32(multiply<0, 0>(x1, k5k0), x2);

    // Barrett reduction to 32 bits.
    x2 = vandq_u32(x1, mask);
    x2 = vandq_u32(multiply<0, 1>(x2, poly), mask);
    x2 = multiply<0, 0>(x2, poly);
    return vgetq_lane_u32(veorq_u32(x1, x2), 1);
}

} // namespace crc
} // namespace system
} // namespace libbitcoin

#endif // HAVE_NEON_AES

#endif
