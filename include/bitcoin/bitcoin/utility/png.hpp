/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin.
 *
 * libbitcoin is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_PNG_HPP
#define LIBBITCOIN_PNG_HPP

#include <cstdint>
#include <istream>
#include <bitcoin/bitcoin/compat.hpp>
#include <bitcoin/bitcoin/define.hpp>
#include <bitcoin/bitcoin/utility/data.hpp>

#ifdef WITH_PNG
#include "png.h"
#endif

namespace libbitcoin {

#ifdef WITH_PNG

namespace png {

typedef uint32_t color[4];

BC_CONSTEXPR uint32_t dpi = 72;
BC_CONSTEXPR uint32_t margin = 2;
BC_CONSTEXPR uint32_t inches_per_meter = (100.0 / 2.54);
BC_CONSTEXPR color foreground = {0, 0, 0, 255};
BC_CONSTEXPR color background = {255, 255, 255, 255};

} // namespace png

/**
 * A method that takes encoded qrcode as a data chunk and writes it to
 * an output stream in png format with the default parameters.  The
 * size parameter specifies the number of dots (pixels) per qr code
 * modules.
 */
BC_API bool write_png(const data_chunk& data, const uint32_t size,
    std::ostream& out);

/**
 * A method that takes encoded qrcode data as a data chunk and writes
 * it to an output stream in png format with the specified parameters.
 */
BC_API bool write_png(const data_chunk& data, const uint32_t size,
    const uint32_t dpi, const uint32_t margin, const uint32_t inches_per_meter,
    const png::color foreground, const png::color background,
    std::ostream& out);

/**
 * A method that reads encoded qrcode data via an input stream and
 * writes it to an output stream in png format with the default
 * parameters.  The size parameter specifies the number of dots
 * (pixels) per qr code modules.
 */
BC_API bool write_png(std::istream& in, const uint32_t size,
    std::ostream& out);

/**
 * A method that reads encoded qrcode data via an input stream and
 * writes it to an output stream in png format with the specified
 * parameters.
 */
BC_API bool write_png(std::istream& in, const uint32_t size,
    const uint32_t dpi, const uint32_t margin, const uint32_t inches_per_meter,
    const png::color foreground, const png::color background,
    std::ostream& out);

#endif

} // namespace libbitcoin

#endif
