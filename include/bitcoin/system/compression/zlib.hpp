/**
 * Copyright (c) 2011-2026 libbitcoin developers
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
#ifndef LIBBITCOIN_SYSTEM_COMPRESSION_ZLIB_HPP
#define LIBBITCOIN_SYSTEM_COMPRESSION_ZLIB_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// zlib (rfc1950) stream of the deflate of the input.
BC_API data_chunk zlib_deflate(const data_slice& in) NOEXCEPT;

/// Decode of a zlib (rfc1950) stream that is the entire input. False if
/// invalid, preset dictionary, or the output would exceed limit (zero is
/// unlimited).
BC_API bool zlib_inflate(data_chunk& out, const data_slice& in,
    size_t limit=zero) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
