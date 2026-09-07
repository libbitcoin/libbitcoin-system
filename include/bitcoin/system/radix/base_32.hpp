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
#ifndef LIBBITCOIN_SYSTEM_RADIX_BASE_32_HPP
#define LIBBITCOIN_SYSTEM_RADIX_BASE_32_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Encode data as base32 (RFC 4648, upper case, padded).
BC_API std::string encode_base32(const data_slice& unencoded) NOEXCEPT;

/// Attempt to decode base32 data (RFC 4648, either case, padding optional).
/// False if the input contains non-base32 characters or invalid padding.
BC_API bool decode_base32(data_chunk& out, const std::string& in) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#endif
