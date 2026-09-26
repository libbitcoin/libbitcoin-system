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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NEON_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NEON_HPP

#include <bitcoin/system/define.hpp>

#if defined(HAVE_NEON)
#include <arm_neon.h>

namespace libbitcoin {
namespace system {

/// Provide 52 bit fused multiply-add to complete matrix.
/// ---------------------------------------------------------------------------
/// Emulated from 26 bit halves as there is no neon ifma.

inline void vmul52q_u64(uint64x2_t& lo, uint64x2_t& hi, uint64x2_t a,
    uint64x2_t b) NOEXCEPT
{
    const auto mask52 = vdupq_n_u64(0x000fffffffffffff);
    const auto mask26 = vdupq_n_u64(0x0000000003ffffff);
    const auto x = vandq_u64(a, mask52);
    const auto y = vandq_u64(b, mask52);
    const auto x0 = vmovn_u64(vandq_u64(x, mask26));
    const auto y0 = vmovn_u64(vandq_u64(y, mask26));
    const auto x1 = vmovn_u64(vshrq_n_u64(x, 26));
    const auto y1 = vmovn_u64(vshrq_n_u64(y, 26));
    const auto mid = vaddq_u64(vmull_u32(x0, y1), vmull_u32(x1, y0));
    const auto low = vaddq_u64(vmull_u32(x0, y0),
        vshlq_n_u64(vandq_u64(mid, mask26), 26));
    lo = vandq_u64(low, mask52);
    hi = vaddq_u64(vaddq_u64(vmull_u32(x1, y1), vshrq_n_u64(mid, 26)),
        vshrq_n_u64(low, 52));
}

inline uint64x2_t vmadd52loq_u64(uint64x2_t c, uint64x2_t a,
    uint64x2_t b) NOEXCEPT
{
    uint64x2_t lo{}, hi{};
    vmul52q_u64(lo, hi, a, b);
    return vaddq_u64(c, lo);
}

inline uint64x2_t vmadd52hiq_u64(uint64x2_t c, uint64x2_t a,
    uint64x2_t b) NOEXCEPT
{
    uint64x2_t lo{}, hi{};
    vmul52q_u64(lo, hi, a, b);
    return vaddq_u64(c, hi);
}

} // namespace system
} // namespace libbitcoin

#endif // HAVE_NEON

#endif
