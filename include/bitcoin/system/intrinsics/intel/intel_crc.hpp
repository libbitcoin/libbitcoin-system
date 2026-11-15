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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_CRC_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_INTEL_CRC_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/intel/intel.hpp>
#include <bitcoin/system/intrinsics/intel/intel_128.hpp>

#if defined(HAVE_AESNI)

namespace libbitcoin {
namespace system {
namespace crc {

/// CRC-32 (reflected 0xedb88320) by carryless multiplication.
/// [Gopal, Ozturk, Guilford, Wolrich, Feghali, Dixon, Karakoyunlu] Fast CRC
/// Computation for Generic Polynomials Using PCLMULQDQ Instruction.

/// The 64 bit lanes of a multiplied by those of k, summed with b.
INLINE xint128_t fold(xint128_t a, xint128_t k, xint128_t b) NOEXCEPT
{
    const auto low = _mm_clmulepi64_si128(a, k, 0x00);
    const auto high = _mm_clmulepi64_si128(a, k, 0x11);
    return _mm_xor_si128(_mm_xor_si128(low, high), b);
}

/// The crc register continued over size bytes, a multiple of 16 of at least
/// 64.
INLINE uint32_t checksum(uint32_t crc, const uint8_t* data,
    size_t size) NOEXCEPT
{
    const auto k1k2 = _mm_set_epi64x(0x01c6e41596, 0x0154442bd4);
    const auto k3k4 = _mm_set_epi64x(0x00ccaa009e, 0x01751997d0);
    const auto k5k0 = _mm_set_epi64x(0x0000000000, 0x0163cd6124);
    const auto poly = _mm_set_epi64x(0x01f7011641, 0x01db710641);
    const auto mask = _mm_set_epi64x(0x00000000ffffffff, 0x00000000ffffffff);
    const auto load = [&](size_t position) NOEXCEPT
    {
        return f::load<xint128_t>(std::next(data, position));
    };

    const auto initial = _mm_cvtsi32_si128(sign_cast<int32_t>(crc));
    auto x1 = _mm_xor_si128(load(0), initial);
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
    x2 = _mm_clmulepi64_si128(x1, k3k4, 0x10);
    x1 = _mm_xor_si128(_mm_srli_si128(x1, 8), x2);
    x2 = _mm_srli_si128(x1, 4);
    x1 = _mm_and_si128(x1, mask);
    x1 = _mm_xor_si128(_mm_clmulepi64_si128(x1, k5k0, 0x00), x2);

    // Barrett reduction to 32 bits.
    x2 = _mm_and_si128(x1, mask);
    x2 = _mm_and_si128(_mm_clmulepi64_si128(x2, poly, 0x10), mask);
    x2 = _mm_clmulepi64_si128(x2, poly, 0x00);
    return sign_cast<uint32_t>(_mm_extract_epi32(_mm_xor_si128(x1, x2), 1));
}

} // namespace crc
} // namespace system
} // namespace libbitcoin

#endif // HAVE_AESNI

#endif
