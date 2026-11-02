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
#ifndef LIBBITCOIN_SYSTEM_UINTX_BITWISE_IPP
#define LIBBITCOIN_SYSTEM_UINTX_BITWISE_IPP

#include <bit>

namespace libbitcoin {

BC_PUSH_WARNING(NO_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

// Logical.
// ----------------------------------------------------------------------------

TEMPLATE
constexpr CLASS& CLASS::operator&=(const uintx_t& other) NOEXCEPT
{
    for (size_t index = 0; index < limbs; ++index)
        limbs_[index] &= other.limbs_[index];

    return *this;
}

TEMPLATE
constexpr CLASS& CLASS::operator|=(const uintx_t& other) NOEXCEPT
{
    for (size_t index = 0; index < limbs; ++index)
        limbs_[index] |= other.limbs_[index];

    return *this;
}

TEMPLATE
constexpr CLASS& CLASS::operator^=(const uintx_t& other) NOEXCEPT
{
    for (size_t index = 0; index < limbs; ++index)
        limbs_[index] ^= other.limbs_[index];

    return *this;
}

// private
TEMPLATE
constexpr CLASS CLASS::complement(const uintx_t& value) NOEXCEPT
{
    auto out = value;
    for (auto& limb: out.limbs_)
        limb = ~limb;

    out.normalize();
    return out;
}

// Shifts at or beyond Bits produce zero.
// ----------------------------------------------------------------------------

TEMPLATE
constexpr CLASS& CLASS::operator<<=(size_t shift) NOEXCEPT
{
    if (shift >= Bits)
    {
        limbs_.fill(0u);
        return *this;
    }

    const auto whole = shift / limb_bits;
    const auto part = shift % limb_bits;
    for (auto index = limbs; index > 0u; --index)
    {
        const auto to = index - 1u;
        if (to < whole)
        {
            limbs_[to] = 0u;
            continue;
        }

        const auto from = to - whole;
        auto limb = limbs_[from] << part;
        if (part != 0u && from > 0u)
            limb |= limbs_[from - 1u] >> (limb_bits - part);

        limbs_[to] = limb;
    }

    normalize();
    return *this;
}

TEMPLATE
constexpr CLASS& CLASS::operator>>=(size_t shift) NOEXCEPT
{
    if (shift >= Bits)
    {
        limbs_.fill(0u);
        return *this;
    }

    const auto whole = shift / limb_bits;
    const auto part = shift % limb_bits;
    for (size_t to = 0; to < limbs; ++to)
    {
        const auto from = to + whole;
        if (from >= limbs)
        {
            limbs_[to] = 0u;
            continue;
        }

        auto limb = limbs_[from] >> part;
        if (part != 0u && from + 1u < limbs)
            limb |= limbs_[from + 1u] << (limb_bits - part);

        limbs_[to] = limb;
    }

    return *this;
}

// Bits.
// ----------------------------------------------------------------------------

TEMPLATE
constexpr bool CLASS::bit(size_t index) const NOEXCEPT
{
    if (index >= Bits)
        return false;

    const auto limb = limbs_[index / limb_bits];
    return ((limb >> (index % limb_bits)) & 1u) != 0u;
}

TEMPLATE
constexpr CLASS& CLASS::set(size_t index) NOEXCEPT
{
    if (index < Bits)
        limbs_[index / limb_bits] |= limb_t{ 1 } << (index % limb_bits);

    return *this;
}

TEMPLATE
constexpr size_t CLASS::msb() const NOEXCEPT
{
    for (auto index = limbs; index > 0u; --index)
    {
        const auto limb = limbs_[index - 1u];
        if (limb != 0u)
            return (index - 1u) * limb_bits + limb_bits - 1u -
                static_cast<size_t>(std::countl_zero(limb));
    }

    return 0u;
}

BC_POP_WARNING()
BC_POP_WARNING()

} // namespace libbitcoin

#endif
