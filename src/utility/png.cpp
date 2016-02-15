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
#include <bitcoin/bitcoin/utility/png.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <boost/iostreams/stream.hpp>
#include <bitcoin/bitcoin/formats/base16.hpp>
#include <bitcoin/bitcoin/utility/container_sink.hpp>
#include <bitcoin/bitcoin/utility/container_source.hpp>
#include <bitcoin/bitcoin/utility/istream_reader.hpp>
#include <bitcoin/bitcoin/utility/ostream_writer.hpp>

namespace libbitcoin {

#ifdef WITH_PNG

bool write_png(const data_chunk& data, const uint32_t size,
    std::ostream& out)
{
    data_source istream(data);
    return write_png(istream, size, out);
}

bool write_png(const data_chunk& data, const uint32_t size,
    const uint32_t dpi, const uint32_t margin, const uint32_t inches_per_meter,
    const png::color foreground, const png::color background,
    std::ostream& out)
{
    data_source istream(data);
    return write_png(istream, size, dpi, margin, inches_per_meter,
        foreground, background, out);
}

bool write_png(std::istream& in, const uint32_t size,
    std::ostream& out)
{
    return write_png(in, size, png::dpi, png::margin,
        png::inches_per_meter, png::foreground, png::background, out);
}

extern "C" void internal_png_sink_write(png_structp png_ptr,
    png_bytep data, png_size_t length)
{
    ostream_writer& sink = *reinterpret_cast<ostream_writer*>(
        png_get_io_ptr(png_ptr));
    sink.write_data(reinterpret_cast<const uint8_t*>(data), size_t(length));
}

extern "C" void user_error_fn(png_structp png_ptr, png_const_charp error_msg)
{
    throw std::runtime_error(error_msg);
}

bool write_png(std::istream& in, const uint32_t size,
    const uint32_t dpi, const uint32_t margin, const uint32_t inches_per_meter,
    const png::color foreground, const png::color background,
    std::ostream& out)
{
    uint32_t version, width;
    int32_t x, y, xx, yy, bit;
    const auto bits_per_byte = 8;

    if (size == 0)
        return false;

    istream_reader source(in);
    source.read_data(reinterpret_cast<uint8_t*>(&version), sizeof(uint32_t));
    source.read_data(reinterpret_cast<uint8_t*>(&width), sizeof(uint32_t));

    data_chunk data = source.read_data(width * width);

    const auto margin_value = 0xff;
    const auto realwidth = (width + margin * 2) * size;
    const auto row_size = (realwidth + 7) / bits_per_byte;

    data_chunk row;
    row.reserve(row_size);

    png_structp png_ptr = png_create_write_struct(
        PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL)
        return false;

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
        return false;

    png_color raw_palette;
    png_colorp palette = &raw_palette;
    palette[0].red   = foreground[0];
    palette[0].green = foreground[1];
    palette[0].blue  = foreground[2];
    palette[1].red   = background[0];
    palette[1].green = background[1];
    palette[1].blue  = background[2];

    png_byte alpha_values[2];
    alpha_values[0] = foreground[3];
    alpha_values[1] = background[3];
    png_set_PLTE(png_ptr, info_ptr, palette, 2);
    png_set_tRNS(png_ptr, info_ptr, alpha_values, 2, NULL);


    ostream_writer sink(out);
    png_set_write_fn(png_ptr, &sink, internal_png_sink_write, NULL);
    png_set_error_fn(png_ptr, NULL, user_error_fn, NULL);

    try
    {
        png_set_IHDR(png_ptr, info_ptr,
                     realwidth, realwidth,
                     1,
                     PNG_COLOR_TYPE_PALETTE,
                     PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);
        png_set_pHYs(png_ptr, info_ptr,
                     dpi * inches_per_meter,
                     dpi * inches_per_meter,
                     PNG_RESOLUTION_METER);
        png_write_info(png_ptr, info_ptr);

        // write top margin
        row.assign(row_size, margin_value);
        for(y = 0; y < margin * size; y++)
            png_write_row(png_ptr, row.data());

        // write data
        unsigned char *p = data.data(), *q;
        for(y = 0; y < width; y++) {
            bit = bits_per_byte - 1;
            row.assign(row_size, margin_value);
            q = row.data();
            q += margin * size / bits_per_byte;
            bit = (bits_per_byte - 1) -
                (margin * size % bits_per_byte);
            for(x = 0; x < width; x++) {
                for(xx = 0; xx < size; xx++) {
                    *q ^= (*p & 1) << bit;
                    bit--;
                    if(bit < 0) {
                        q++;
                        bit = bits_per_byte - 1;
                    }
                }
                p++;
            }
            for(yy = 0; yy < size; yy++)
                png_write_row(png_ptr, row.data());
        }

        // write bottom margin
        row.assign(row_size, margin_value);
        for(y = 0; y < margin * size; y++)
            png_write_row(png_ptr, row.data());

        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, &info_ptr);

        out.flush();
    }
    catch(const std::runtime_error& error)
    {
        return false;
    }
    return true;
}

#endif

} // namespace libbitcoin
