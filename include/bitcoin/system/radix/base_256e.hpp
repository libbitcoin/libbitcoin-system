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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_256E_HPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_256E_HPP

#include <cstdint>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/constraints.hpp>
#include <bitcoin/system/math/math.hpp>

namespace libbitcoin {
namespace system {
namespace base256e {

/// base256e
/// A base 256 exponential form representation of 32 byte (uint256_t) values,
/// with 24 bits of precision. Two bits wasted in exponential representation.

// TODO: templatize on parameters (and strict).

/// All derived from parameters, no magic numbers.

/// Inherent
constexpr auto base = power2(8);

/// Parameters
constexpr auto precision = 24u;
constexpr auto source_bits = 256u;

/// Sizes
constexpr auto factor = ceilinged_log2(sub1(base));
constexpr auto mantissa_bytes = precision / factor;
constexpr auto exponent_bits = ceilinged_log2(source_bits / factor);
constexpr auto exponent_bytes = to_ceilinged_bytes(exponent_bits);
constexpr auto compact_bytes = mantissa_bytes + exponent_bytes;

/// Types
using compact_type = unsigned_type<compact_bytes>;
using number_type = unsigned_exact_type<to_bytes<source_bits>()>;

/// Widths
constexpr auto compact_width = width<compact_type>();
constexpr auto exponent_width = to_bits(exponent_bytes);

/// Constraints (avoid byte and bit padding respectively)
static_assert(sizeof(compact_type) == compact_bytes);
static_assert(is_bytes_width(source_bits) && is_bytes_width(precision));

/// Verification
static_assert(!is_limited(exponent_width - exponent_bits, sub1(byte_bits)));
static_assert(is_integral<compact_type>());
static_assert(is_integral<number_type>() ||
    is_same<number_type, uintx_t<source_bits>>());

/// Functions

/// A zero value implies an invalid (including zero) parameter.
/// Invalid if either padding bit is set. Allows non-minimal exponent encoding.
number_type expand(compact_type exponential) noexcept;

/// (m * 256^e) bit-encoded as [00eeeee][mmmmmmmm][mmmmmmmm][mmmmmmmm].
/// The highest two bits are padded with zeros, uses minimal exponent encoding.
compact_type compress(const number_type& number) noexcept;

} // namespace base256e
} // namespace system
} // namespace libbitcoin

#endif
