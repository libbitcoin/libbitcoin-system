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
#include <bitcoin/system/data/data.hpp>

namespace libbitcoin {
namespace system {

/// A base 256 exponential form representation of 32 byte (uint256_t) values.
/// The zero value is used as an invalid value sentinel. Otherwise it is not
/// possible to create an invalid compact form from 32 bytes. The compression
/// loses all but the highest 29 or 30 significant bits of precision.

// TODO: templatize on both types.

uint256_t expand_base256e(uint32_t small) noexcept;
uint32_t compress_base256e(const uint256_t& big) noexcept;

} // namespace system
} // namespace libbitcoin

#endif
