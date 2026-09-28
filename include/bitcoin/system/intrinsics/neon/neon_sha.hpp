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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_NEON_SHA_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_NEON_SHA_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/intrinsics/types.hpp>
#include <bitcoin/system/intrinsics/neon/neon.hpp>

#if defined(HAVE_CRYPTO)

namespace libbitcoin {
namespace system {
namespace sha {

INLINE void schedule(xint128_t& message0, xint128_t message1) NOEXCEPT
{
    message0 = vsha256su0q_u32(message0, message1);
}

INLINE void schedule(xint128_t& message0, xint128_t message1,
    xint128_t message2) NOEXCEPT
{
    message0 = vsha256su1q_u32(message0, message1, message2);
}

INLINE void compress(xint128_t& state0, xint128_t& state1,
    xint128_t wk) NOEXCEPT
{
    const auto state = state0;
    state0 = vsha256hq_u32(state, state1, wk);
    state1 = vsha256h2q_u32(state1, state, wk);
}

INLINE void shuffle(xint128_t&, xint128_t&) NOEXCEPT
{
}

INLINE void unshuffle(xint128_t&, xint128_t&) NOEXCEPT
{
}

INLINE void schedule_160(xint128_t& message0, xint128_t message1,
    xint128_t message2) NOEXCEPT
{
    message0 = vsha1su0q_u32(message0, message1, message2);
}

INLINE void schedule_160(xint128_t& message0, xint128_t message3) NOEXCEPT
{
    message0 = vsha1su1q_u32(message0, message3);
}

template <size_t Round>
INLINE void compress_160(xint128_t& abcd, xint128_t& carry,
    xint128_t message) NOEXCEPT
{
    constexpr uint32_t k =
        Round == 0 ? 0x5a827999 :
        Round == 1 ? 0x6ed9eba1 :
        Round == 2 ? 0x8f1bbcdc : 0xca62c1d6;

    const auto e = vsha1h_u32(vgetq_lane_u32(carry, 0));
    const auto wk = vaddq_u32(message, vdupq_n_u32(k));
    carry = abcd;

    if constexpr (Round == 0)
        abcd = vsha1cq_u32(abcd, e, wk);
    else if constexpr (Round == 2)
        abcd = vsha1mq_u32(abcd, e, wk);
    else
        abcd = vsha1pq_u32(abcd, e, wk);
}

INLINE xint128_t next_160(xint128_t carry, xint128_t e) NOEXCEPT
{
    const auto next = vsha1h_u32(vgetq_lane_u32(carry, 0));
    return vsetq_lane_u32(next + vgetq_lane_u32(e, 0), e, 0);
}

INLINE xint128_t order_160(xint128_t message) NOEXCEPT
{
    return message;
}

INLINE void shuffle_160(xint128_t&) NOEXCEPT
{
}

INLINE void unshuffle_160(xint128_t&) NOEXCEPT
{
}

INLINE xint128_t set_160(uint32_t e) NOEXCEPT
{
    return vsetq_lane_u32(e, vdupq_n_u32(0), 0);
}

INLINE uint32_t get_160(xint128_t e) NOEXCEPT
{
    return vgetq_lane_u32(e, 0);
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif // HAVE_CRYPTO

#if defined(HAVE_CRYPTO512)

namespace libbitcoin {
namespace system {
namespace sha {

/// Four 64 bit words.
struct xquad_t
{
    uint64x2_t lo;
    uint64x2_t hi;
};

INLINE xquad_t set_512(uint64_t a, uint64_t b, uint64_t c, uint64_t d) NOEXCEPT
{
    return
    {
        vcombine_u64(vcreate_u64(a), vcreate_u64(b)),
        vcombine_u64(vcreate_u64(c), vcreate_u64(d))
    };
}

INLINE xquad_t load_512(const xquad_t& words) NOEXCEPT
{
    return
    {
        vld1q_u64((const uint64_t*)&words),
        vld1q_u64((const uint64_t*)&words + 2)
    };
}

INLINE void store_512(xquad_t& words, xquad_t a) NOEXCEPT
{
    vst1q_u64((uint64_t*)&words, a.lo);
    vst1q_u64((uint64_t*)&words + 2, a.hi);
}

INLINE xquad_t add_512(xquad_t a, xquad_t b) NOEXCEPT
{
    return { vaddq_u64(a.lo, b.lo), vaddq_u64(a.hi, b.hi) };
}

INLINE xquad_t swap_512(xquad_t a) NOEXCEPT
{
    return
    {
        (uint64x2_t)vrev64q_u8((uint8x16_t)a.lo),
        (uint64x2_t)vrev64q_u8((uint8x16_t)a.hi)
    };
}

INLINE void schedule_512(xquad_t& message0, xquad_t message1) NOEXCEPT
{
    message0.lo = vsha512su0q_u64(message0.lo, message0.hi);
    message0.hi = vsha512su0q_u64(message0.hi, message1.lo);
}

INLINE void schedule_512(xquad_t& message0, xquad_t message1,
    xquad_t message2) NOEXCEPT
{
    message0.lo = vsha512su1q_u64(message0.lo, message2.hi,
        vextq_u64(message1.lo, message1.hi, 1));
    message0.hi = vsha512su1q_u64(message0.hi, message0.lo,
        vextq_u64(message1.hi, message2.lo, 1));
}

INLINE void compress_512(uint64x2_t& ab, uint64x2_t& cd, uint64x2_t& ef,
    uint64x2_t& gh, uint64x2_t wk) NOEXCEPT
{
    const auto fg = vextq_u64(ef, gh, 1);
    const auto de = vextq_u64(cd, ef, 1);
    const auto hg = vaddq_u64(gh, vextq_u64(wk, wk, 1));
    const auto t1 = vsha512hq_u64(hg, fg, de);
    const auto next = vsha512h2q_u64(t1, cd, ab);
    gh = ef;
    ef = vaddq_u64(cd, t1);
    cd = ab;
    ab = next;
}

INLINE void compress_512(xquad_t& state0, xquad_t& state1,
    xquad_t wk) NOEXCEPT
{
    compress_512(state0.lo, state0.hi, state1.lo, state1.hi, wk.lo);
    compress_512(state0.lo, state0.hi, state1.lo, state1.hi, wk.hi);
}

INLINE void shuffle_512(xquad_t&, xquad_t&) NOEXCEPT
{
}

INLINE void unshuffle_512(xquad_t&, xquad_t&) NOEXCEPT
{
}

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif // HAVE_CRYPTO512

#endif
