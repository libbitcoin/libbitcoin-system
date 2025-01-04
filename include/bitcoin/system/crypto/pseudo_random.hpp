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
#ifndef LIBBITCOIN_SYSTEM_CRYPTO_PSEUDO_RANDOM_HPP
#define LIBBITCOIN_SYSTEM_CRYPTO_PSEUDO_RANDOM_HPP

#include <chrono>
#include <random>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// This class uses a 32 bit clock seed for entropy and a not cryptographically
/// secure pseudo random sequence generator (mt19937). There is no source of
/// true randomness available to any general purpose software library. This
/// class produces an explicitly pseudorandom sequence of numbers. If there
/// is any doubt as to the meaning of "pseudorandom", read definition below.
///
/// "A pseudorandom sequence of numbers is one that appears to be statistically
/// random, despite having been produced by a completely deterministic and
/// repeatable process. Simply put, the problem is that many of the sources
/// of randomness available to humans (such as rolling dice) rely on physical
/// processes not readily available to computer programs."
/// - en.wikipedia.org/wiki/Pseudorandomness
/// 
/// This class is used to randomize such things as address pool selections,
/// which do not require true randomness. Do not ever use a pseudorandom
/// sequence of numbers for live wallet entropy unless you want to lose money.
class BC_API pseudo_random
{
  public:
    /// Fill a byte array with pseudo random sequence of values.
    template<size_t Size>
    static void fill(data_array<Size>& out) NOEXCEPT
    {
        std::transform(out.begin(), out.end(), out.begin(), [](uint8_t) NOEXCEPT
        {
            return next();
        });
    }

    /// Fill a byte vector with pseudo random sequence of values.
    static void fill(data_chunk& out) NOEXCEPT;

    /// Generate a pseudo random number within the uint8_t domain.
    /// Specialized: uniform_int_distribution is undefined for sizes < 16 bits.
    static uint8_t next() NOEXCEPT;

    /// Generate a pseudo random number within [begin, end].
    /// Specialized: uniform_int_distribution is undefined for sizes < 16 bits.
    static uint8_t next(uint8_t begin, uint8_t end) NOEXCEPT;

    /// Generate a pseudo random number within the Type domain.
    template<typename Type, if_integer<Type> = true>
    static Type next() NOEXCEPT
    {
        return next(minimum<Type>, maximum<Type>);
    }

    /// Generate a pseudo random integer value within [begin, end].
    template<typename Integer, if_integer<Integer> = true>
    static Integer next(Integer begin, Integer end) NOEXCEPT
    {
        // std::uniform_int_distribution() is undefined if begin > end.
        if (begin > end)
            return {};

        std::uniform_int_distribution<Integer> distribution(begin, end);
        return distribution(get_twister());
    }

    /// Shuffle a container elements pseudo randomly.
    template<class Container>
    static void shuffle(Container& out) NOEXCEPT
    {
        std::shuffle(out.begin(), out.end(), get_twister());
    }

    /// Convert a time duration to a value in the range [max/ratio, max].
    /// maximum is the maximum value to return. ratio is the determinant
    /// of the minimum duration as the inverse portion of the maximum
    /// duration. Returns the pseudo randomized duration.
    static std::chrono::steady_clock::duration duration(
        const std::chrono::steady_clock::duration& maximum,
        uint8_t ratio=2) NOEXCEPT;

private:
    static std::mt19937& get_twister() NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
