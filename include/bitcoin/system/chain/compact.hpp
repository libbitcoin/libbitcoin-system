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

#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace chain {

/// A base 256 exponential form representation of 32 byte (uint256_t) values.
/// The zero value is used as an invalid value sentinel. Otherwise it is not
/// possible to create an invalid compact form from 32 bytes. The compression
/// loses all but the highest 29 or 30 significant bits of precision.

/// TODO: Inherit from base256e static template with compact parameters.
/// TODO: Provide only sign bug compensation. Add lax overflow option.

struct compact
{
public:
    // All parameters are derived, no magic numbers.

    // Parameters
    static constexpr auto to = 32u;
    static constexpr auto from = 256u;
    static constexpr auto precision = 24u;

    // Sizes
    static constexpr auto mantissa_bytes = to_bytes<precision>();
    static constexpr auto exponent_bits = ceilinged_log2(to_bytes<from>());
    static constexpr auto exponent_bytes = to_ceilinged_bytes(exponent_bits);
    static constexpr auto compact_bytes = mantissa_bytes + exponent_bytes;

    // Types
    using number_type = uintx_t<from>;
    static_assert(is_same<number_type, uint256_t>());
    using compact_type = unsigned_type<compact_bytes>;
    static_assert(is_same<compact_type, uint32_t>());
    using exponent_type = unsigned_type<exponent_bytes>;
    static_assert(is_same<exponent_type, uint8_t>());

    // Widths
    static constexpr auto compact_width = width<compact_type>();
    static constexpr auto exponent_width = width<exponent_type>();
    static_assert(exponent_width - exponent_bits == 2u);

    /// A zero value implies an invalid (including zero) parameter.
    /// Non-minimal exponent encoding allowed only for high bit hack.
    static inline number_type expand(compact_type exponential) noexcept;

    /// (m * 256^e) bit-encoded as [0eeeeee][mmmmmmmm][mmmmmmmm][mmmmmmmm].
    /// Uses non-minimal exponent encoding to avoid high mantissa bit (hack).
    static inline compact_type compress(const number_type& number) noexcept;

    bool negative;
    exponent_type exponent;
    compact_type mantissa;

protected:
    template <typename Integer>
    static constexpr Integer ratio(Integer value) noexcept;
    static constexpr compact to_compact(compact_type small) noexcept;
    static constexpr compact_type from_compact(const compact& compact) noexcept;
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/chain/compact.ipp>

#endif
