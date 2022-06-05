/**
 * Copyright (c) 2011-2022 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_PSEUDO_RANDOM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_PSEUDO_RANDOM_HPP

#include <algorithm>
#include <chrono>
#include <random>
#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

class BC_API pseudo_random
{
  public:
    /// Fill a byte array with randomness using the default random engine.
    template<size_t Size>
    static void fill(data_array<Size>& out) noexcept
    {
        // C++17: Parallel policy for std::transform.
        std::transform(out.begin(), out.end(), out.begin(), [](uint8_t) noexcept
        {
            return next();
        });
    }

    /// Fill a byte vector with randomness using the default random engine.
    static void fill(data_chunk& out) noexcept;

    /// Generate a pseudo random number within the uint8_t domain.
    /// Specialized: uniform_int_distribution is undefined for sizes < 16 bits.
    static uint8_t next() noexcept;

    /// Generate a pseudo random number within [begin, end].
    /// Specialized: uniform_int_distribution is undefined for sizes < 16 bits.
    static uint8_t next(uint8_t begin, uint8_t end) noexcept;

    /// Generate a pseudo random number within the Type domain.
    template<typename Type, if_integer<Type> = true>
    static Type next() noexcept
    {
        return next(std::numeric_limits<Type>::min(),
            std::numeric_limits<Type>::max());
    }

    /// Generate a pseudo random integer value within [begin, end].
    template<typename Integer, if_integer<Integer> = true>
    static Integer next(Integer begin, Integer end) noexcept
    {
        std::uniform_int_distribution<Integer> distribution(begin, end);
        return distribution(get_twister());
    }

    /// Shuffle a container elements using the random engine.
    template<class Container>
    static void shuffle(Container& out) noexcept
    {
        std::shuffle(out.begin(), out.end(), get_twister());
    }

    /// Convert a time duration to a value in the range [max/ratio, max].
    /// maximum is the maximum value to return. ratio is the determinant
    /// of the minimum duration as the inverse portion of the maximum
    /// duration. Returns the randomized duration.
    static std::chrono::steady_clock::duration duration(
        const std::chrono::steady_clock::duration& maximum,
        uint8_t ratio=2) noexcept;

private:
    static std::mt19937& get_twister() noexcept;
};

} // namespace system
} // namespace libbitcoin

#endif
