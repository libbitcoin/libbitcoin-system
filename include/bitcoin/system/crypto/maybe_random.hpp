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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_MAYBE_RANDOM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_MAYBE_RANDOM_HPP

#include <random>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Entropy drawn from the operating system (and hardware, where the standard
/// library uses it). Whether those sources are random cannot be known, so
/// this is maybe random. It is suitable for ephemeral network session keys
/// and for randomization such as address pool selection.
/// Do not ever use it for live wallet seeding unless you want to lose money.
class BC_API maybe_random
{
public:
    /// Fill bytes with entropy, conditioned by sha256.
    static void fill(byte_span out) NOEXCEPT;

    /// Generate an integer value within [begin, end].
    template <typename Integer, if_integer<Integer> = true>
    static Integer next(Integer begin, Integer end) NOEXCEPT
    {
        // std::uniform_int_distribution is undefined for sizes < 16 bits.
        static_assert(sizeof(Integer) >= sizeof(uint16_t));

        // std::uniform_int_distribution() is undefined if begin > end.
        if (begin > end)
            return {};

        std::uniform_int_distribution<Integer> distribution(begin, end);
        return distribution(get_device());
    }

    /// Shuffle container elements.
    template <class Container>
    static void shuffle(Container& out) NOEXCEPT
    {
        std::shuffle(out.begin(), out.end(), get_device());
    }

private:
    static std::random_device& get_device() NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
