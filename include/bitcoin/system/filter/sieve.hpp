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
#ifndef LIBBITCOIN_SYSTEM_FILTER_SIEVE_HPP
#define LIBBITCOIN_SYSTEM_FILTER_SIEVE_HPP

#include <bitcoin/system/define.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {

/// Sieve is limited to integral types.
/// There are no false negatives, with the goal of minimizing false positives.
template <size_t SieveBits, size_t SelectBits,
    if_not_greater<SelectBits, SieveBits> = true,
    if_not_greater<SieveBits, bits<uint64_t>> = true>
class sieve
{
public:
    /// This produces size_t when disabled.
    using type = unsigned_type<to_ceilinged_bytes(SieveBits)>;

    /// Sieve is bypassed.
    static constexpr bool disabled = is_zero(SieveBits) || is_zero(SelectBits);

    /// Is sieve saturated did fingerprint collide with it.
    static constexpr bool is_collision(type previous, type next) NOEXCEPT;

    /// Is potential duplicate (saturated or screened).
    static constexpr bool is_screened(type value, uint64_t entropy) NOEXCEPT;

    /// Add fingerprint to sieve. Changes determined by return/value.
    /// Only "value added to sieve" implies a negative result.
    /// All other results imply a positive result (potential existence).
    /// return == value && return == saturated: sieve was saturated.
    /// return != value && return == saturated: sieve now saturated
    /// return == value && return != saturated: value was screened.
    /// return != value && return != saturated: value added to sieve.
    static constexpr type screen(type value, uint64_t entropy) NOEXCEPT;

protected:
    static constexpr auto screen_bits = SieveBits - SelectBits;
    static constexpr auto screens = power2(SelectBits);
    static constexpr auto limit = sub1(screens);
    static constexpr auto sentinel = sub1(screen_bits);
    static constexpr auto empty = unmask_right<type>(SieveBits);
    static constexpr auto saturated = mask_right(empty, sentinel);
    static constexpr auto first_mask = unmask_right<type>(screen_bits);
    static constexpr auto select_mask = first_mask;
    static constexpr auto mask_count = to_half(safe_multiply(screens,
        add1(screens)));

    using masks_t = std_array<type, mask_count>;
    using offsets_t = std_array<type, screens>;

    /// Generate compression offsets at compile time.
    static CONSTEVAL offsets_t generate_offsets() NOEXCEPT;

    /// Generate compressed mask table at compile time.
    static CONSTEVAL masks_t generate_masks() NOEXCEPT;

    /// Read member compressed mask array as if it was a two-dimesional array.
    static constexpr type masks(size_t row, size_t column) NOEXCEPT;

    /// Is sentinel value for empty filter.
    static constexpr bool is_empty(type value) NOEXCEPT;

    /// Is sentinel value for saturated filter.
    static constexpr bool is_saturated(type value) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#define TEMPLATE template <size_t SieveBits, size_t SelectBits, \
    if_not_greater<SelectBits, SieveBits> If1, \
    if_not_greater<SieveBits, bits<uint64_t>> If2>

#define CLASS sieve<SieveBits, SelectBits, If1, If2>

#include <bitcoin/system/impl/filter/sieve.ipp>

#undef CLASS
#undef TEMPLATE

#endif
