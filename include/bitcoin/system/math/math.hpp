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
#ifndef LIBBITCOIN_SYSTEM_MATH_HPP
#define LIBBITCOIN_SYSTEM_MATH_HPP

#include <bitcoin/system/math/addition.hpp>
#include <bitcoin/system/math/bits.hpp>
#include <bitcoin/system/math/bytes.hpp>
#include <bitcoin/system/math/division.hpp>
#include <bitcoin/system/math/bytes.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/sign.hpp>

// Inclusion dependencies:
// bits     <-
// bytes    <-
// limits   <- addition
// addition <-
// division <- [bits], power
// power    <- [bits, limits]
// safe     <- [bits, limits, addition], sign, power
// sign     <- [bits, limits, addition, division], power

// TODO: verify all non-integral operations do not unnecessarily copy unitx.

// safe.hpp
// ============================================================================
// Explicit [required]/possible operators for narrowing and sign casting.
//
// bits.hpp
// ============================================================================
// follow established return type (not standardized).
// sizes are all size_t (width, offset, bits, shift).
// unary operations : (including all shifts) return Value type.
// binary operations: return unpromoted std::common_type.
//
// addition.hpp
// ============================================================================
// Unsigned add/subtract may expand.
// Return defines expanded space, defaults to common_type.
// Objective is default native behavior with optional space/return typing.
// Always execute in max(common, expanded) and return min(common, expanded).
// These functions are intended as domain transitions, not overflow guards.
// Execution may still overflow, so functions are provided to detect.
//
// division.hpp
// ============================================================================
// div/mod contract (div is reducing, mod is [0..sub1(divisor)]).
// Return is unpromoted common_type (auto) with sign adj for ceilinged_modulo.
// ----------------------------------------------------------------------------
// ceilinged_divide: unpromoted common_type
// ceilinged_modulo: unpromoted common_type (coerces neg, common_type wrong)
// floored_divide  : unpromoted common_type
// floored_modulo  : unpromoted common_type (always naturally pos)
// truncated_divide: unpromoted common_type (aliases for native div)
// truncated_modulo: unpromoted common_type (aliases for native mod)

#endif
