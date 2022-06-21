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

// TODO: move implementation to .ipp.
class BC_API compact
{
public:
    static bool is_valid(uint32_t compact) noexcept;

    // TODO: constexpr.
    static uint32_t compress(const uint256_t& big) noexcept;

    // TODO: constexpr/inline.
    static bool expand(uint256_t& big, uint32_t small) noexcept;
    static uint256_t expand(uint32_t small) noexcept;

private:
    static constexpr size_t exponent_width = 8u;
    static constexpr auto bit_width = width<uint32_t>();
    static constexpr auto mantissa_width = bit_width - exponent_width;

    static constexpr auto exponent_bits = unmask_left<uint32_t>(exponent_width);
    static constexpr auto mantissa_bits = unmask_right<uint32_t>(mantissa_width);
    static_assert((exponent_bits + mantissa_bits) == sub1(power2(bit_width)));

    // The inflection point is a function of the mantissa domain.
    static constexpr auto point = ceilinged_log256(bit_and(max_uint32, mantissa_bits));
    static_assert(point == 3u);

    static bool is_signed(size_t compact) noexcept;
    static bool is_valid(size_t exponent, uint32_t mantissa) noexcept;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#endif
