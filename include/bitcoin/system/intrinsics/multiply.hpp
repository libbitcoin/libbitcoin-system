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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_MULTIPLY_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_MULTIPLY_HPP

#include <bitcoin/system/define.hpp>

// Use intrinsics if available (portable).
#if defined(HAVE_MSC) && (defined(HAVE_X64) || defined(HAVE_ARM64))
    // docs.microsoft.com/en-us/cpp/intrinsics/umul128
    // docs.microsoft.com/en-us/cpp/intrinsics/umulh
    #include <intrin.h>
#endif

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)

/// mul_wide (double width product as high and low words).
/// ---------------------------------------------------------------------------

INLINE constexpr void mul_wide64_nominal(uint64_t& hi, uint64_t& lo,
    uint64_t left, uint64_t right) NOEXCEPT
{
    constexpr auto half = to_half(bits<uint64_t>);
    constexpr uint64_t mask = 0x00000000ffffffff;
    const auto left_lo = left & mask;
    const auto left_hi = left >> half;
    const auto right_lo = right & mask;
    const auto right_hi = right >> half;
    const auto p0 = left_lo * right_lo;
    const auto p1 = left_lo * right_hi;
    const auto p2 = left_hi * right_lo;
    const auto p3 = left_hi * right_hi;
    const auto mid = (p0 >> half) + (p1 & mask) + (p2 & mask);
    lo = (mid << half) | (p0 & mask);
    hi = p3 + (p1 >> half) + (p2 >> half) + (mid >> half);
}

INLINE void mul_wide64(uint64_t& hi, uint64_t& lo, uint64_t left,
    uint64_t right) NOEXCEPT
{
#if defined(HAVE_MSC) && defined(HAVE_X64)
    lo = _umul128(left, right, &hi);
#elif defined(HAVE_MSC) && defined(HAVE_ARM64)
    hi = __umulh(left, right);
    lo = left * right;
#elif defined(__SIZEOF_INT128__)
    using wide = unsigned __int128;
    const auto product = wide{ left } * wide{ right };
    hi = static_cast<uint64_t>(product >> bits<uint64_t>);
    lo = static_cast<uint64_t>(product);
#else
    mul_wide64_nominal(hi, lo, left, right);
#endif
}

template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr void mul_wide(Unsigned& hi, Unsigned& lo, Unsigned left,
    Unsigned right) NOEXCEPT
{
    if constexpr (sizeof(Unsigned) < sizeof(uint64_t))
    {
        using wide = std::conditional_t<sizeof(Unsigned) < sizeof(uint32_t),
            uint32_t, uint64_t>;

        const auto product = wide{ left } * wide{ right };
        hi = static_cast<Unsigned>(product >> bits<Unsigned>);
        lo = static_cast<Unsigned>(product);
    }
    else if (std::is_constant_evaluated())
    {
        mul_wide64_nominal(hi, lo, left, right);
    }
    else
    {
        mul_wide64(hi, lo, left, right);
    }
}

/// madd52 (vpmadd52luq/vpmadd52huq semantics).
/// ---------------------------------------------------------------------------
/// The 104 bit product of the low 52 bits of left and right is split into 52
/// bit low and high halves, and one half is added (wrapping) to accumulator.

constexpr auto madd52_bits = 52u;
constexpr uint64_t madd52_mask = 0x000fffffffffffff;

INLINE constexpr uint64_t madd52lo(uint64_t accumulator, uint64_t left,
    uint64_t right) NOEXCEPT
{
    uint64_t hi{}, lo{};
    mul_wide(hi, lo, left & madd52_mask, right & madd52_mask);
    return accumulator + (lo & madd52_mask);
}

INLINE constexpr uint64_t madd52hi(uint64_t accumulator, uint64_t left,
    uint64_t right) NOEXCEPT
{
    constexpr auto shift = bits<uint64_t> - madd52_bits;
    uint64_t hi{}, lo{};
    mul_wide(hi, lo, left & madd52_mask, right & madd52_mask);
    return accumulator + ((hi << shift) | (lo >> madd52_bits));
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
