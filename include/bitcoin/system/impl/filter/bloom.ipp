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
#ifndef LIBBITCOIN_SYSTEM_FILTER_BLOOM_IPP
#define LIBBITCOIN_SYSTEM_FILTER_BLOOM_IPP

namespace libbitcoin {
namespace system {

TEMPLATE
constexpr bool CLASS::is_collision(type previous, type next) NOEXCEPT
{
    return next == previous;
}

TEMPLATE
constexpr bool CLASS::is_screened(type value, uint64_t entropy) NOEXCEPT
{
    if constexpr (disabled)
    {
        return true;
    }
    else
    {
        if (is_empty(value))
            return false;

        for (auto k = zero; k < K; ++k)
            if (get_right(value, get_bit(k, entropy)))
                return false;

        // All selected bits are set (to zero, default is one).
        return true;
    }
}

TEMPLATE
constexpr CLASS::type CLASS::screen(type value, uint64_t entropy) NOEXCEPT
{
    if constexpr (disabled)
    {
        return value;
    }
    else
    {
        if (is_saturated(value))
            return value;

        for (auto k = zero; k < K; ++k)
            set_right_into(value, get_bit(k, entropy), false);

        // All selected bits have been set (to zero, default is one).
        return value;
    }
}

// protected
// ----------------------------------------------------------------------------

TEMPLATE
constexpr bool CLASS::is_empty(type value) NOEXCEPT
{
    return value == empty;
}

TEMPLATE
constexpr bool CLASS::is_saturated(type value) NOEXCEPT
{
    return value == saturated;
}

TEMPLATE
constexpr size_t CLASS::get_bit(size_t k, uint64_t entropy) NOEXCEPT
{
    constexpr auto mask = unmask_right<type>(select);
    const auto shifted = shift_right(entropy, k * select);
    const auto bit = bit_and(possible_narrow_cast<type>(shifted), mask);

    // When 2^clog2(M) > M must modulo selection to remain within M.
    if constexpr (mask >= M)
    {
        return bit % M;
    }
    else
    {
        return bit;
    }
}

} // namespace system
} // namespace libbitcoin

#endif
