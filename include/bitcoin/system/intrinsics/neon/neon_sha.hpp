/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

namespace libbitcoin {
namespace system {
namespace sha {

#if defined(HAVE_CRYPTO)

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

INLINE void shuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
}

INLINE void unshuffle(xint128_t& state0, xint128_t& state1) NOEXCEPT
{
}

#endif // HAVE_SHANI

} // namespace sha
} // namespace system
} // namespace libbitcoin

#endif
