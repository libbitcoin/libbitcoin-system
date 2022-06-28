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
#include <bitcoin/system/math/limits.hpp>
#include <bitcoin/system/math/log.hpp>
#include <bitcoin/system/math/power.hpp>
#include <bitcoin/system/math/safe.hpp>
#include <bitcoin/system/math/sign.hpp>

// Inclusion dependencies:
// safe     ->
// sign     -> safe
// division -> sign
// power    -> sign, safe
// addition -> sign, safe, limits
// limits   -> sign, safe, power
// log      -> sign, safe, division
// bits     -> sign, safe, log
// bytes    -> sign, log

// sign/safe should not call any other math libs and are safe from all others.
// bits/bytes should otherwise call only log. Otherwise only the natural:
// addition->limits, limits->power, log->division.

// TODO: create type constraints for unitx/unitx_t<> optimization.
// TODO: verify non-integral ops do not unnecessarily copy unitx/unitx_t<>.

#endif
