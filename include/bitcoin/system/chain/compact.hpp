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
#ifndef LIBBITCOIN_SYSTEM_CHAIN_COMPACT_HPP
#define LIBBITCOIN_SYSTEM_CHAIN_COMPACT_HPP

#include <cstddef>
#include <cstdint>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

class BC_API compact
{
public:
    // Used only for chain_state assertion against previous block bits.
    static constexpr bool is_valid(uint32_t compact) noexcept
    {
        const auto mantissa = bit_and(compact, mantissa_bits);
        const size_t exponent = shift_right(compact, mantissa_width);
        return is_valid(exponent, mantissa);
    }

    static uint32_t compress(const uint256_t& big) noexcept;
    static bool expand(uint256_t& big, uint32_t small) noexcept;
    static uint256_t expand(uint32_t small) noexcept;

private:
    static constexpr size_t exponent_width = 8u;
    static constexpr auto bit_width = width<uint32_t>();
    static constexpr auto mantissa_width = bit_width - exponent_width;

    static constexpr auto exponent_bits = unmask_left<uint32_t>(exponent_width);
    static constexpr auto mantissa_bits = unmask_right<uint32_t>(mantissa_width);

    // The inflection point is a function of the mantissa domain.
    static constexpr auto point = ceilinged_log256(bit_and(max_uint32, mantissa_bits));
    static_assert((exponent_bits + mantissa_bits) == sub1(power2(bit_width)));
    static_assert(point == 3u);

    static constexpr bool is_signed(size_t compact) noexcept
    {
        return get_right(compact, sub1(mantissa_width));
    }

    static constexpr bool is_valid(size_t exponent, uint32_t mantissa) noexcept
    {
        //*********************************************************************
        // CONSENSUS: High mantissa bit (sign) set returns zero (invalid).
        // Zero mantissa would not be shifted and would return zero (invalid).
        // Presumably these were originally caused by "if (mantissa > 0)..."
        //*********************************************************************
        if (is_zero(mantissa) || is_signed(mantissa))
            return false;

        // Nothing to shift (above), or no left shift to cause an overflow.
        if (!(exponent > point))
            return true;

        // If (<< bits) > (256 - mantissa width) then would shift to zero.
        return !((exponent - point) > (bit_width - ceilinged_log256(mantissa)));
    }

    static bool from_compact(uint256_t& out, uint32_t compact) noexcept;
    static uint32_t from_big(const uint256_t& big) noexcept;

    uint256_t big_;
    uint32_t normal_;
    bool overflowed_;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
