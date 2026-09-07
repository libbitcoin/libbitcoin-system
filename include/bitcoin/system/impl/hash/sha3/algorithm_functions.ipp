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
#ifndef LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_FUNCTIONS_IPP
#define LIBBITCOIN_SYSTEM_HASH_SHA3_ALGORITHM_FUNCTIONS_IPP

// 3.2 Step Mappings, 3.3 KECCAK-p[b, nr]
// ============================================================================
// The state is a 5x5 array of 64 bit lanes, indexed as [x + 5 * y].

namespace libbitcoin {
namespace system {
namespace sha3 {

// protected
// ----------------------------------------------------------------------------

TEMPLATE
INLINE constexpr void CLASS::
theta(state_t& state) NOEXCEPT
{
    std_array<word_t, 5> column{};
    for (size_t x = 0; x < 5; ++x)
        column[x] = state[x] ^ state[x + 5] ^ state[x + 10] ^ state[x + 15] ^
            state[x + 20];

    for (size_t x = 0; x < 5; ++x)
    {
        const auto delta = column[(x + 4) % 5] ^ rotl(column[(x + 1) % 5], 1);
        for (size_t y = 0; y < 25; y += 5)
            state[x + y] ^= delta;
    }
}

TEMPLATE
INLINE constexpr void CLASS::
rho_pi(state_t& state) NOEXCEPT
{
    // A zero rotation (lane 0,0) is excluded (rotl by zero is undefined).
    state_t moved{};
    for (size_t x = 0; x < 5; ++x)
        for (size_t y = 0; y < 5; ++y)
        {
            const auto lane = x + 5 * y;
            const auto shift = rotation[lane];
            moved[y + 5 * ((2 * x + 3 * y) % 5)] =
                is_zero(shift) ? state[lane] : rotl(state[lane], shift);
        }

    state = moved;
}

TEMPLATE
INLINE constexpr void CLASS::
chi(state_t& state) NOEXCEPT
{
    for (size_t y = 0; y < 25; y += 5)
    {
        const std_array<word_t, 5> row
        {
            state[y], state[y + 1], state[y + 2], state[y + 3], state[y + 4]
        };

        for (size_t x = 0; x < 5; ++x)
            state[x + y] = row[x] ^ (~row[(x + 1) % 5] & row[(x + 2) % 5]);
    }
}

TEMPLATE
template <size_t Round>
INLINE constexpr void CLASS::
iota(state_t& state) NOEXCEPT
{
    state[0] ^= K::get[Round];
}

TEMPLATE
template <size_t Round>
INLINE constexpr void CLASS::
round(state_t& state) NOEXCEPT
{
    theta(state);
    rho_pi(state);
    chi(state);
    iota<Round>(state);
}

TEMPLATE
template <size_t ...Rounds>
INLINE constexpr void CLASS::
permute_(state_t& state, std::index_sequence<Rounds...>) NOEXCEPT
{
    (round<Rounds>(state), ...);
}

TEMPLATE
constexpr void CLASS::
permute(state_t& state) NOEXCEPT
{
    permute_(state, std::make_index_sequence<K::rounds>{});
}

} // namespace sha3
} // namespace system
} // namespace libbitcoin

#endif
