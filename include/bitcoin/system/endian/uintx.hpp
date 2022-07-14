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
#ifndef LIBBITCOIN_SYSTEM_ENDIAN_UINTX_HPP
#define LIBBITCOIN_SYSTEM_ENDIAN_UINTX_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// data -> uintx (inferred size), uintx -> data_chunk (inferred size)

// uintx      from_big|little_endian(data_slice)
// data_chunk   to_big|little_endian(uintx)

// uintx is not constexpr.

inline uintx from_big_endian(const data_chunk& data) NOEXCEPT;
inline uintx from_little_endian(const data_chunk& data) NOEXCEPT;
inline data_chunk to_big_endian(const uintx& value) NOEXCEPT;
inline data_chunk to_little_endian(const uintx& value) NOEXCEPT;

} // namespace system
} // namespace libbitcoin

#include <bitcoin/system/impl/endian/uintx.ipp>

#endif
