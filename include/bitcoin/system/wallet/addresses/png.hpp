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
#ifndef LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_PNG_HPP
#define LIBBITCOIN_SYSTEM_WALLET_ADDRESSES_PNG_HPP

#include <bitcoin/system/data/data.hpp>
#include <bitcoin/system/define.hpp>

namespace libbitcoin {
namespace system {

/// Convert a data chunk to a black on white png file.
class BC_API png
{
public:
    /// The maximal image data size is 0x3ffe0000.
    /// The last byte of each image row may contain padding.
    static size_t max_image_bytes;

    /// False if width parameter is inconsistent with data size.
    /// Each row is padded to a whole byte, all rows are the same length.
    static bool to_image(std::ostream& out, const data_chunk& data,
        uint16_t width) NOEXCEPT;
};

} // namespace system
} // namespace libbitcoin

#endif
