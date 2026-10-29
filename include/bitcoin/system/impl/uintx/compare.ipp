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
#ifndef LIBBITCOIN_SYSTEM_UINTX_COMPARE_IPP
#define LIBBITCOIN_SYSTEM_UINTX_COMPARE_IPP

namespace libbitcoin {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// private
TEMPLATE
constexpr std::strong_ordering CLASS::compare(const uintx_t& left,
    const uintx_t& right) NOEXCEPT
{
    for (auto index = limbs; index > 0u; --index)
    {
        const auto order = left.limbs_[index - 1u] <=> right.limbs_[index - 1u];
        if (order != 0)
            return order;
    }

    return std::strong_ordering::equal;
}

// private
TEMPLATE
template <typename Integral>
constexpr bool CLASS::exceeds(Integral value) NOEXCEPT
{
    if constexpr (std::numeric_limits<Integral>::digits <= Bits)
        return false;
    else if constexpr (std::is_signed_v<Integral>)
        return value > 0 && (static_cast<uint64_t>(value) >> Bits) != 0u;
    else
        return (static_cast<uint64_t>(value) >> Bits) != 0u;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace libbitcoin

#endif
