/**
 * Copyright (c) 2011-2019 libbitcoin developers (see AUTHORS)
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
#ifndef LIBBITCOIN_SYSTEM_TIFF_HPP
#define LIBBITCOIN_SYSTEM_TIFF_HPP

#include <cstdint>
#include <iostream>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>

namespace libbitcoin {
namespace system {

/// Convert a data chunk or stream to a square black on white tiff file.
class BC_API tiff
{
public:
    /// The image bit stream is stored at the end of the file.
    /// This is the file offset in bytes to the start of the image data.
    /// The last image byte may contain padding, image bit size is width^2.
    static uint32_t image_offset;

    /// (((2^16) - 1)^2 + 7)/ 8 = 536,854,529 or 0x1fffc001 bytes.
    static uint32_t max_image_bytes;

    /// False if width parameter is inconsistent with data size.
    /// Data size must be (width^2 + 7) / 8. Last byte padded as necessary.
    static bool to_image(std::ostream& out, const data_chunk& data,
        uint16_t width);
};

} // namespace system
} // namespace libbitcoin

#endif
