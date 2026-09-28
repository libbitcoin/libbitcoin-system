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

#endif
