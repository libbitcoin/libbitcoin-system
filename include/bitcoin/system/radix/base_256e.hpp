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

// For unsigned and byte-aligned { span, base, precision } where
// [precision <= span / log2(base)] and representation is integral:

// These two functions are defined:
// [compress(span).exponent = clog(base, span)]
// [compress(span).mantissa = span * base^(exponent - precision)]]
// [expand(exponent, mantissa) = mantissa * base^(precision - exponent)]
// Where compression reduces span to precision.

// Substituting in either relation [e = precision +/- exponent]:
// [m * base^e]

// Substituting power2(log2(base)) for base, and <</>> for * power2:
// [m * (power2(log2(base)))^e] == [m << (log2(base) * abs(e))] for +e.
// [m * (power2(log2(base)))^e] == [m >> (log2(base) * abs(e))] for -e.

// Substituting factor for log2(base):
// [m * (power2(factor))^e] == [m << (factor * abs(e))] for +e.
// [m * (power2(factor))^e] == [m >> (factor * abs(e))] for -e.

// Substituting shift for factor * abs(e):
// [m * (power2(factor))^e] == [m << shift] for +e.
// [m * (power2(factor))^e] == [m >> shift] for -e.

// Substituting base for power2(log2(base)) = power2(factor):
// [m * base^e] == [m << shift] for +e.
// [m * base^e] == [m >> shift] for -e.

/// All derived from parameters, no magic numbers.

/// Parameters
constexpr auto span = 256u;
constexpr auto base = 256u;
constexpr auto precision = 24u;
static_assert(is_zero(span% byte_bits));
static_assert(is_zero(base % byte_bits));
static_assert(is_zero(precision % byte_bits));

/// Sizes
constexpr auto factor = floored_log2(base);
constexpr auto e_max = span / factor;
constexpr auto e_bits = ceilinged_log2(e_max);
constexpr auto e_bytes = to_ceilinged_bytes(e_bits);
constexpr auto m_bytes = precision / factor;
static_assert(precision <= e_max);

/// Types
using compact_type = unsigned_type<m_bytes + e_bytes>;
using number_type = unsigned_exact_type<to_bytes<span>()>;
static_assert(is_integral<compact_type>());

/// Widths
constexpr auto e_width = to_bits(e_bytes);
constexpr auto p_width = precision;

/// Functions

/// A zero value implies an invalid (including zero) parameter.
/// Invalid if either padding bit is set. Allows non-minimal exponent encoding.
number_type expand(compact_type exponential) noexcept;

/// (m * 256^e) bit-encoded as [00eeeee][mmmmmmmm][mmmmmmmm][mmmmmmmm].
/// The highest two bits are padded with zeros, uses minimal exponent encoding.
compact_type compress(const number_type& value) noexcept;

} // namespace base256e
} // namespace system
} // namespace libbitcoin

#endif
