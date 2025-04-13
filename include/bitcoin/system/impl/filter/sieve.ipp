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
#ifndef LIBBITCOIN_SYSTEM_FILTER_SIEVE_IPP
#define LIBBITCOIN_SYSTEM_FILTER_SIEVE_IPP

// [------------------sieve-----------][--------------link--------------]
// [[select][--------screens---------]][--------------link--------------]

// To minimize computation in alignment the sieve is defined as two dimensional
// table of precomputed masks. One dimension is determined by the screen
// selector and other is used to iterate through masks for the selected screen.
// A single screen requires one mask, a double two, and so on. The max selector
// value indicates that the first bit of the screen is a sentinel. This is set
// for either terminal (all other bits set) or overflow (no other bits set).
// If the sentinel is unset then the max screen is in used on remaining bits.
// The seive should be terminal if and only if the link is terminal, and when
// overflowed implies that the bucket is saturated, rendering it unscreened.

namespace libbitcoin {
namespace system {

// Suppress bogus warnings to use constexpr when function is consteval.
BC_PUSH_WARNING(USE_CONSTEXPR_FOR_FUNCTION)
BC_PUSH_WARNING(NO_DYNAMIC_ARRAY_INDEXING)
BC_PUSH_WARNING(NO_ARRAY_INDEXING)

TEMPLATE
constexpr bool CLASS::is_collision(type previous, type next) NOEXCEPT
{
    return next == previous || is_saturated(next);
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
        const auto row = shift_right(value, screen_bits);

        if (row == limit)
        {
            if (is_empty(value))
                return false;

            if (is_saturated(value))
                return true;
        }

        for (type column{}; column <= row; ++column)
        {
            const auto mask = masks(row, column);

            if (is_zero(bit_xor
                (
                    bit_and(value, mask),
                    bit_and(possible_narrow_cast<type>(entropy), mask)
                ))) return true;
        }

        return false;
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
        auto row = shift_right(value, screen_bits);

        if (row == limit)
        {
            if (is_empty(value))
            {
                zeroize(row);
            }
            else
            {
                if (!is_screened(value, entropy))
                {
                    value = saturated;
                }

                BC_ASSERT(!is_empty(value));
                return value;
            }
        }
        else
        {
            if (is_screened(value, entropy))
            {
                BC_ASSERT(!is_empty(value));
                return value;
            }
            else
            {
                ++row;
            }
        }

        // Both indexes are screen selector, as each new screen adds one mask.
        const auto mask = masks(row, row);

        // Mask inversion requires clearing the selector bits (and above).
        auto unmask = bit_and(bit_not(mask), select_mask);

        // Mask sentinel bit for limit screen (also excluded by limit masks).
        if (row == limit) set_right_into(unmask, sentinel, false);

        value = bit_or
        (
            shift_left(row, screen_bits),
            bit_or
            (
                bit_and(value, unmask),
                bit_and(possible_narrow_cast<type>(entropy), mask)
            )
        );

        BC_ASSERT(!is_empty(value));
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
constexpr CLASS::type CLASS::masks(size_t row, size_t column) NOEXCEPT
{
    BC_ASSERT(column <= row);
    constexpr auto offsets = generate_offsets();
    constexpr auto masks = generate_masks();
    return masks[offsets[row] + column];
}

TEMPLATE
CONSTEVAL CLASS::offsets_t CLASS::generate_offsets() NOEXCEPT
{
    // Generate compression offsets at compile, generally 16 or 32 elements.
    offsets_t offsets{};
    for (type index{}; index < screens; ++index)
        offsets[index] = to_half(ceilinged_multiply(index, add1(index)));

    return offsets;
}

// Logically sparse, e.g. 16 x 16 = 256 table of uint32_t (1024 bytes).
// Compressed to one-dimensional 136 element array of uint32_t (544 bytes).
TEMPLATE
CONSTEVAL CLASS::masks_t CLASS::generate_masks() NOEXCEPT
{
    constexpr auto off = generate_offsets();

    // Start with all screen bits set.
    masks_t table{ first_mask };

    // Progressively divide previous row masks into subsequent row.
    // Each row adds one mask, with previous row masks sacrificing bits for it.
    for (type row = 1; row < screens; ++row)
    {
        for (type col = 0; col < row; ++col)
            table[off[row] + col] = table[off[sub1(row)] + col];

        for (auto mask = row; !is_zero(mask); --mask)
        {
            const auto col = sub1(mask);
            const auto div = floored_divide(screen_bits, add1(row));
            const auto mod = floored_modulo(screen_bits, add1(row));
            const auto target = div + to_int(col < mod);
            const auto prior = table[off[row] + col];
            auto bits = floored_subtract(ones_count(prior), target);
            while (!is_zero(bits))
            {
                const auto bit = right_zeros(table[off[row] + col]);
                if (bit < screen_bits)
                {
                    set_right_into(table[off[row] + col], bit, false);
                    set_right_into(table[off[row] + row], bit, true);
                    --bits;
                }
            }
        }
    }

    // Unset last row high order screen bit to avoid sentinel conflict.
    // This may result in one empty mask, and therefore a false positive.
    for (type col = 0; col < screens; ++col)
        set_right_into(table[off[sub1(screens)] + col], sentinel, false);

    return table;
}

BC_POP_WARNING()
BC_POP_WARNING()
BC_POP_WARNING()

} // namespace system
} // namespace libbitcoin

#endif
