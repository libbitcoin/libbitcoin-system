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
#include <bitcoin/system/utility/png.hpp>

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/formats/base_16.hpp>
#include <bitcoin/system/utility/color.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>

namespace libbitcoin {
namespace system {

#ifdef WITH_PNG

bool png::write_png(const data_chunk& data, uint32_t size, std::ostream& out)
{
    data_source istream(data);
    return png::write_png(istream, size, out);
}

bool png::write_png(const data_chunk& data, uint32_t size,
    uint32_t dots_per_inch, uint32_t margin, uint32_t inches_per_meter,
    const color& foreground, const color& background, std::ostream& out)
{
    data_source istream(data);
    return png::write_png(istream, size, dots_per_inch, margin,
        inches_per_meter, foreground, background, out);
}

bool png::write_png(std::istream& in, uint32_t size, std::ostream& out)
{
    return png::write_png(in, size, dots_per_inch, margin, inches_per_meter,
        get_default_foreground(), get_default_background(), out);
}

extern "C" void sink_write(png_structp png_ptr, png_bytep data,
    png_size_t length)
{
    static_assert(sizeof(length) <= sizeof(size_t), "png_size_t too large");
    const auto size = static_cast<size_t>(length);

    auto& sink = *reinterpret_cast<ostream_writer*>(png_get_io_ptr(png_ptr));
    sink.write_bytes(reinterpret_cast<const uint8_t*>(data), size);
}

extern "C" void error_callback(png_structp, png_const_charp error_message)
{
    throw std::runtime_error(error_message);
}

bool png::write_png(std::istream& in, uint32_t size, uint32_t dots_per_inch,
    uint32_t margin, uint32_t inches_per_meter, const color& foreground,
    const color& background, std::ostream& out)
{
    if (size == 0)
        return false;

    istream_reader source(in);
    // Skip version.
    source.skip(4);
    auto width = source.read_4_bytes_little_endian();

    if (max_size_t / width < width)
        return false;

    const auto area = width * width;
    auto data = source.read_bytes(area);

    try
    {
        static constexpr int32_t bit_depth = 1;
        static constexpr int32_t bits_per_byte = 8;
        static constexpr uint8_t margin_value = 0xff;

        if ((size == 0) || (margin > max_size_t / size))
            return false;

        if ((width + margin * 2) > max_size_t / size)
            return false;

        const auto margin_size = margin * size;
        const auto realwidth = (width + margin * 2) * size;
        const auto row_size = (realwidth + 7) / bits_per_byte;

        data_chunk row;
        row.reserve(row_size);

        auto png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr,
            nullptr, nullptr);
        if (png_ptr == nullptr)
            return false;

        auto info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == nullptr)
            return false;

        png_color palette[2];
        palette[0].red = foreground.red;
        palette[0].green = foreground.green;
        palette[0].blue = foreground.blue;
        palette[1].red = background.red;
        palette[1].green = background.green;
        palette[1].blue = background.blue;

        png_byte alpha_values[2];
        alpha_values[0] = foreground.alpha;
        alpha_values[1] = background.alpha;

        png_set_PLTE(png_ptr, info_ptr, palette, 2);
        png_set_tRNS(png_ptr, info_ptr, alpha_values, 2, nullptr);

        ostream_writer sink(out);
        png_set_write_fn(png_ptr, &sink, sink_write, nullptr);
        png_set_error_fn(png_ptr, nullptr, error_callback, nullptr);

        png_set_IHDR(png_ptr, info_ptr, realwidth, realwidth, bit_depth,
            PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

        png_set_pHYs(png_ptr, info_ptr, dots_per_inch * inches_per_meter,
            dots_per_inch * inches_per_meter, PNG_RESOLUTION_METER);

        png_write_info(png_ptr, info_ptr);

        // write top margin
        row.assign(row_size, margin_value);
        for (size_t y = 0; y < margin_size; y++)
            png_write_row(png_ptr, row.data());

        // write data
        uint8_t* row_ptr = nullptr;
        auto data_ptr = data.data();
        for (size_t y = 0; y < width; y++)
        {
            // TODO: unguarded overflow conditions.
            auto bit = bits_per_byte - 1;
            row.assign(row_size, margin_value);
            row_ptr = row.data();
            row_ptr += margin_size / bits_per_byte;
            bit = (bits_per_byte - 1) - (margin_size % bits_per_byte);

            for (size_t x = 0; x < width; x++)
            {
                for (size_t xx = 0; xx < size; xx++)
                {
                    *row_ptr ^= (*data_ptr & 1) << bit;
                    bit--;
                    if (bit < 0)
                    {
                        row_ptr++;
                        bit = bits_per_byte - 1;
                    }
                }

                data_ptr++;
            }

            for (size_t yy = 0; yy < size; yy++)
                png_write_row(png_ptr, row.data());
        }

        // write bottom margin
        row.assign(row_size, margin_value);
        for (size_t y = 0; y < margin_size; y++)
            png_write_row(png_ptr, row.data());

        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, &info_ptr);

        out.flush();
    }
    catch (const std::runtime_error& error)
    {
        return false;
    }

    return true;
}

#endif

} // namespace system
} // namespace libbitcoin
