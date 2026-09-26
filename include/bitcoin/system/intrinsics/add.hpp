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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_ADD_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_ADD_HPP

#include <bitcoin/system/define.hpp>

// Use intrinsics if available (portable).
#if defined(HAVE_MSC) && defined(HAVE_X64)
    // docs.microsoft.com/en-us/cpp/intrinsics/x64-amd64-intrinsics-list
    #include <intrin.h>
#endif

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_CASTS_FOR_ARITHMETIC_CONVERSION)

/// add_carry (sum of addends and carry, returns carry).
/// ---------------------------------------------------------------------------

INLINE constexpr bool add_carry64_nominal(uint64_t& sum, uint64_t left,
    uint64_t right, bool carry) NOEXCEPT
{
    const auto partial = left + right;
    sum = partial + static_cast<uint64_t>(carry);
    return (partial < left) || (sum < partial);
}

INLINE bool add_carry64(uint64_t& sum, uint64_t left, uint64_t right,
    bool carry) NOEXCEPT
{
#if defined(HAVE_MSC) && defined(HAVE_X64)
    unsigned long long out{};
    const auto overflow = _addcarry_u64(static_cast<unsigned char>(carry),
        left, right, &out);
    sum = out;
    return !is_zero(overflow);
#elif defined(__SIZEOF_INT128__)
    using wide = unsigned __int128;
    const auto total = wide{ left } + wide{ right } + wide{ carry };
    sum = static_cast<uint64_t>(total);
    return !is_zero(static_cast<uint64_t>(total >> bits<uint64_t>));
#else
    return add_carry64_nominal(sum, left, right, carry);
#endif
}

template <typename Unsigned, if_unsigned_integral_integer<Unsigned> = true>
INLINE constexpr bool add_carry(Unsigned& sum, Unsigned left, Unsigned right,
    bool carry) NOEXCEPT
{
    if constexpr (sizeof(Unsigned) < sizeof(uint64_t))
    {
        using wide = std::conditional_t<sizeof(Unsigned) < sizeof(uint32_t),
            uint32_t, uint64_t>;

        const auto total = wide{ left } + wide{ right } + wide{ carry };
        sum = static_cast<Unsigned>(total);
        return !is_zero(total >> bits<Unsigned>);
    }
    else if (std::is_constant_evaluated())
    {
        return add_carry64_nominal(sum, left, right, carry);
    }
    else
    {
        return add_carry64(sum, left, right, carry);
    }
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
