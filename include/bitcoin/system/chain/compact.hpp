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

struct compact
{
public:
    using exponent_type = uint8_t;
    using mantissa_type = uint32_t;

    /// An overflow or zero/negative mantissa returns zero (invalid).
    static inline uint256_t expand(uint32_t small) noexcept;

    /// Always a valid compact result (unless parameter is zero).
    static inline uint32_t compress(const uint256_t& big) noexcept;

    /// Same as !is_zero(expand(small)) without the conversion cost.
    static constexpr bool is_compact(uint32_t small) noexcept;

    bool negative;
    exponent_type exponent;
    mantissa_type mantissa;

protected:
    template <typename Integer>
    static constexpr Integer ratio(Integer value) noexcept;
    static constexpr compact to_compact(uint32_t small) noexcept;
    static constexpr uint32_t from_compact(const compact& compact) noexcept;
    static constexpr bool is_valid(const compact& compact) noexcept;

private:
    // All parameters are derived, no magic numbers.

    // base256 <=> base32 for bitcoin_hash compression.
    static constexpr auto hash_base = to_bits(hash_size);
    static constexpr auto compact_base = width<uint32_t>();
    static_assert(compact_base == 32u);
    static_assert(hash_base == 256u);

    // exponent domain (stored [0..32] vs. logical [-3..29] exponent)
    static constexpr auto stored_exponent_width = ceilinged_log2(compact_base);
    static constexpr auto alignment = byte_bits - stored_exponent_width;
    static constexpr auto exponent_width = stored_exponent_width + alignment;
    static_assert(width<exponent_type>() == exponent_width);
    static_assert(exponent_width == 8u);
    static_assert(alignment == 2u);

    // mantissa domain
    static constexpr auto mantissa_width = compact_base - exponent_width;
    static_assert(width<mantissa_type>() == compact_base);
    static_assert(mantissa_width == 24u);

    // shifting exponent reduces mantissa domain by the aligned exponent width
    static constexpr auto point = mantissa_width / (hash_base / compact_base);
    static constexpr auto maximum_positive_exponent = compact_base - point;
    static constexpr auto maximum_negative_exponent = point;
    static_assert(maximum_positive_exponent == 29u);
    static_assert(maximum_negative_exponent == 3u);
    static_assert(point == 3u);
};

} // namespace chain
} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/chain/compact.ipp>

#endif
