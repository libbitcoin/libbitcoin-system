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
constexpr auto to = 32u;
constexpr auto from = 256u;
constexpr auto precision = 24u;

/// Sizes
constexpr auto mantissa_bytes = to_bytes<precision>();
constexpr auto exponent_bits = ceilinged_log2(to_bytes<from>());
constexpr auto exponent_bytes = to_ceilinged_bytes(exponent_bits);
constexpr auto compact_bytes = mantissa_bytes + exponent_bytes;

/// Types
using number_type = uintx_t<from>;
static_assert(std::is_same<number_type, uint256_t>::value);
using compact_type = unsigned_type<compact_bytes>;
static_assert(std::is_same<compact_type, uint32_t>::value);
using exponent_type = unsigned_type<exponent_bytes>;
static_assert(std::is_same<exponent_type, uint8_t>::value);

/// Widths
constexpr auto compact_width = width<compact_type>();
constexpr auto exponent_width = width<exponent_type>();
static_assert(exponent_width - exponent_bits == 2u);

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
