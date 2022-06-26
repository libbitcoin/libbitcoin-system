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

/// Parameters
constexpr auto base = 256u;
constexpr auto precision = 24u;
constexpr auto span = 256u;

/// Sizes
constexpr auto factor = floored_log2(base);
constexpr auto e_bits = ceilinged_log2(span / factor);
constexpr auto e_bytes = to_ceilinged_bytes(e_bits);
constexpr auto m_bytes = precision / factor;

/// Types
using compact_type = unsigned_type<m_bytes + e_bytes>;
using number_type = unsigned_exact_type<to_bytes<span>()>;

/// Widths
constexpr auto e_width = to_bits(e_bytes);
constexpr auto p_width = precision;

/// Constraints (avoid byte and bit padding respectively)
static_assert(sizeof(compact_type) == m_bytes + e_bytes);
static_assert(is_bytes_width(span) && is_bytes_width(precision));

/// Verification
static_assert(!is_limited(e_width - e_bits, sub1(byte_bits)));
static_assert(is_integral<compact_type>());
static_assert(is_integral<number_type>() ||
    is_same<number_type, uintx_t<span>>());

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
