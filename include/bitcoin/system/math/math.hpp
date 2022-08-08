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
#ifndef LIBBITCOIN_SYSTEM_OVERFLOW_HPP
#define LIBBITCOIN_SYSTEM_OVERFLOW_HPP

#include <bitcoin/system/math/addition.hpp>
#include <bitcoin/system/math/bits.hpp>
#include <bitcoin/system/math/bytes.hpp>
#include <bitcoin/system/math/byteswap.hpp>
#include <bitcoin/system/math/cast.hpp>
#include <bitcoin/system/math/division.hpp>
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/math/logarithm.hpp>
#include <bitcoin/system/math/overflow.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/math/rotate.hpp>
#include <bitcoin/system/math/sign.hpp>

// Inclusion dependencies:
// cast      ->
// sign      -> cast
// rotate    -> cast
// byteswap  -> cast
// division  -> sign
// overflow  -> sign, cast
// bits      -> sign, cast,           logarithm (for bit_width(clog2))
// bytes     -> sign, cast,           logarithm (for byte_width(clog256))
// limits    -> sign, cast,           [power]   (for sized domain limits)
// power     -> sign, cast, overflow, bits      (for shift optimization)
// logarithm -> sign, cast, overflow, division  (for ceiling/floor opts)
// addition  -> sign, cast, overflow, limits    (for ceiling/floor opts)
// multiply  -> sign, cast, overflow, limits    (for ceiling/floor opts)

// sign/cast/overflow should not call any other math libs and are safe from
// all others. bits/bytes should otherwise call only log. Otherwise only:
// addition/multiply->limits, limits->power, power->bits, logarithm->division.

// TODO: create type constraints for unitx/unitx_t<> optimization.
// TODO: verify non-integral ops do not unnecessarily copy unitx/unitx_t<>.

#endif
