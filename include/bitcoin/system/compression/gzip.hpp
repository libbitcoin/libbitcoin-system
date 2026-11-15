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
#ifndef LIBBITCOIN_SYSTEM_COMPRESSION_GZIP_HPP
#define LIBBITCOIN_SYSTEM_COMPRESSION_GZIP_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// gzip (rfc1952) member of the deflate of the input.
BC_API data_chunk gzip_deflate(const data_slice& in) NOEXCEPT;

/// Decode of the gzip (rfc1952) members that are the entire input. False if
/// invalid, or the output would exceed limit (zero is unlimited).
BC_API bool gzip_inflate(data_chunk& out, const data_slice& in,
    size_t limit=zero) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
