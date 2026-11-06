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
#ifndef LIBBITCOIN_SYSTEM_FILTER_BLOOM_ARRAY_IPP
#define LIBBITCOIN_SYSTEM_FILTER_BLOOM_ARRAY_IPP

namespace libbitcoin {
namespace system {

BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)

TEMPLATE
constexpr bool CLASS::is_screened(const type& value,
    uint64_t entropy) NOEXCEPT
{
    if constexpr (disabled)
    {
        return true;
    }
    else
    {
        return is_screened(value, entropy, K);
    }
}

TEMPLATE
constexpr void CLASS::screen(type& value, uint64_t entropy) NOEXCEPT
{
    if constexpr (!disabled)
    {
        screen(value, entropy, K);
    }
}

TEMPLATE
constexpr bool CLASS::is_screened(const type& value, uint64_t entropy,
    size_t k) NOEXCEPT
{
    if constexpr (disabled)
    {
        return true;
    }
    else
    {
        BC_ASSERT(!is_zero(k) && k <= K);
        for (size_t index{}; index < k; ++index)
        {
            const auto bit = get_bit(index, entropy);
            if (!get_right(value[bit / bits<uint64_t>], bit % bits<uint64_t>))
                return false;
        }

        return true;
    }
}

TEMPLATE
constexpr void CLASS::screen(type& value, uint64_t entropy,
    size_t k) NOEXCEPT
{
    if constexpr (!disabled)
    {
        BC_ASSERT(!is_zero(k) && k <= K);
        for (size_t index{}; index < k; ++index)
        {
            const auto bit = get_bit(index, entropy);
            set_right_into(value[bit / bits<uint64_t>], bit % bits<uint64_t>);
        }
    }
}

// protected
// ----------------------------------------------------------------------------

// Double hashing over the two halves of the entropy, reduced to M.
TEMPLATE
constexpr size_t CLASS::get_bit(size_t k, uint64_t entropy) NOEXCEPT
{
    const auto low = lo_word<uint32_t>(entropy);
    const auto high = bit_or(hi_word<uint32_t>(entropy), 1_u32);
    const auto index = possible_narrow_cast<uint32_t>(k);
    const auto step = multiply(index, high);
    const auto mixed = add(low, step);
    const auto scaled = multiply<uint64_t>(mixed, M);
    return hi_word<uint32_t>(scaled);
}

BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
