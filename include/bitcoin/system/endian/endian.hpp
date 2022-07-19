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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_ENDIAN_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_ENDIAN_HPP

#include <bitcoin/system/endian/set.hpp>
#include <bitcoin/system/endian/integers.hpp>
#include <bitcoin/system/endian/integrals.hpp>
#include <bitcoin/system/endian/minimal.hpp>
#include <bitcoin/system/endian/nominal.hpp>
#include <bitcoin/system/endian/stream.hpp>
#include <bitcoin/system/endian/swaps.hpp>
#include <bitcoin/system/endian/uintx_t.hpp>
#include <bitcoin/system/endian/unsafe.hpp>

// integers  ->
// integrals ->
// swaps     ->
// stream    -> swaps
// unsafe    -> swaps
// batch     -> swaps
// nominal   -> swaps, integers, integrals,
// uintx_t   -> nominal, minimal
// minimal   -> integers

#endif
