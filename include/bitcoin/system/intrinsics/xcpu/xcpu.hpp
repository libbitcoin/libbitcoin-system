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
#ifndef LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_XCPU_HPP
#define LIBBITCOIN_SYSTEM_INTRINSICS_XCPU_XCPU_HPP

#include <bitcoin/system/intrinsics/xcpu/byteswap.hpp>
#include <bitcoin/system/intrinsics/xcpu/cpuid.hpp>
#include <bitcoin/system/intrinsics/xcpu/defines.hpp>
#include <bitcoin/system/intrinsics/xcpu/functional_128.hpp>
#include <bitcoin/system/intrinsics/xcpu/functional_256.hpp>
#include <bitcoin/system/intrinsics/xcpu/functional_512.hpp>
#include <bitcoin/system/intrinsics/xcpu/rotate.hpp>
#include <bitcoin/system/intrinsics/xcpu/sha.hpp>

/// Intel/AMD is always little-endian, and SHA is always big-endian (swap).
/// Primitives implement rotr/rotl because these are not available in SIMD.

#endif
