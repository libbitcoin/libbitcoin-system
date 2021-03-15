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
#include <bitcoin/system/utility/qr_code.hpp>

#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#ifdef WITH_QRENCODE
    #include <qrencode.h>
#endif
#include <bitcoin/system/constants.hpp>
#include <bitcoin/system/define.hpp>
#include <bitcoin/system/utility/data.hpp>
#include <bitcoin/system/utility/container_sink.hpp>
#include <bitcoin/system/utility/container_source.hpp>
#include <bitcoin/system/utility/istream_bit_reader.hpp>
#include <bitcoin/system/utility/istream_reader.hpp>
#include <bitcoin/system/utility/ostream_bit_writer.hpp>
#include <bitcoin/system/utility/ostream_writer.hpp>
#include <bitcoin/system/utility/tiff.hpp>

namespace libbitcoin {
namespace system {

#ifdef WITH_QRENCODE
static QRecLevel recovery_level_to_qr_recovery_level(
    qr_code::recovery_level level)
{
    switch (level)
    {
        case qr_code::recovery_level::low:
            return QR_ECLEVEL_L;
        case qr_code::recovery_level::medium:
            return QR_ECLEVEL_M;
        case qr_code::recovery_level::high:
            return QR_ECLEVEL_Q;
        default:
            return QR_ECLEVEL_H;
    }
}

static QRencodeMode encode_mode_to_qr_encode_mode(qr_code::encode_mode mode)
{
    switch (mode)
    {
        case qr_code::encode_mode::numeric:
            return QR_MODE_NUM;
        case qr_code::encode_mode::alpha_numeric:
            return QR_MODE_AN;
        case qr_code::encode_mode::eight_bit:
            return QR_MODE_8;
        case qr_code::encode_mode::kanji:
            return QR_MODE_KANJI;
        case qr_code::encode_mode::eci_mode:
            return QR_MODE_ECI;
        case qr_code::encode_mode::fcn1_1:
            return QR_MODE_FNC1FIRST;
        case qr_code::encode_mode::fcn1_2:
            return QR_MODE_FNC1SECOND;
        default:
            return QR_MODE_NUL;
    }
}
#endif

bool qr_code::encode(std::ostream& out, const std::string& value,
    uint32_t version, uint16_t scale, uint16_t margin, bool case_sensitive,
    recovery_level level, encode_mode mode)
{
#ifdef WITH_QRENCODE
    const auto qrcode = QRcode_encodeString(value.c_str(), version,
        recovery_level_to_qr_recovery_level(level),
        encode_mode_to_qr_encode_mode(mode), case_sensitive);

    if (qrcode == nullptr)
        return false;

    // Any scale increase can potentially cause an overflow.
    const auto scaled_width = scale * static_cast<uint64_t>(qrcode->width);

    // Guard against scaling overflow.
    if (scaled_width > max_uint16)
        return false;

    // Limit of bc::tiff conversion maximum size.
    const auto width = static_cast<uint16_t>(scaled_width);

    // Width is limited to max_uint32, safe in 64 bit space.
    const auto area = qrcode->width * static_cast<uint64_t>(qrcode->width);

    // Guard against overflow (to_image_data verifies logical size).
    if (area > std::numeric_limits<data_chunk::size_type>::max())
        return false;

    // Map coded data into a data_chunk (must rely on width correctness).
    const data_chunk coded(&qrcode->data[0], &qrcode->data[area - 1u]);

    // Convert to scaled image pixel bit stream.
    const auto pixels = to_image_data(coded, scale, margin);

    // Convert to TIFF image stream (does not fail as long as sizes match).
    return tiff::to_image(out, pixels, width);
#else
    return false;
#endif
}

// Scale may move the image off of a byte-aligned square of pixels in bytes.
// So the dimensions cannot be derived from the result, caller must retain.
// pixel_width = width * scale.
data_chunk qr_code::to_image_data(const data_chunk& coded, uint16_t scale,
    uint16_t margin)
{
    // For reading the qrcode byte stream.
    data_source image_source(coded);
    istream_reader image_reader(image_source);

    // Skip over the serialized qrencode version (may not match requested).
    image_reader.skip(sizeof(uint32_t));

    // Read the serialized qrencode width (encoding is one pixel per byte).
    const auto width = image_reader.read_4_bytes_little_endian();

    // Guard against insufficient stream length (invalid width).
    if (!image_reader)
        return {};

    // Guard against mismatched sizes from qrencode.
    if (coded.size() != width * static_cast<uint64_t>(width))
        return {};

    // Any scale increase can potentially cause an overflow.
    const auto scaled_width = scale * static_cast<uint64_t>(width);

    // Guard against scaling overflow.
    if (scaled_width > max_uint32)
        return {};

    // Pixel is the least significant bit of a qrencode byte.
    constexpr auto pixel_mask = uint8_t{ 0x01 };
    constexpr auto pixel_off = false;

    // For readability (image is always square).
    const auto height = width;
    const auto vertical_margin = margin;
    const auto horizontal_margin = margin;
    const auto full_row_width = horizontal_margin + width + horizontal_margin;

    // For writing the image bit stream.
    data_chunk image_out;
    data_sink image_sink(image_out);
    ostream_writer image_writer(image_sink);
    ostream_bit_writer image_bit_writer(image_writer);

    // Write top margin.
    for (size_t row = 0; row < vertical_margin; ++row)
        for (size_t column = 0; column < full_row_width; ++column)
            image_bit_writer.write_bit(pixel_off);

    // Write each row.
    for (size_t row = 0; row < height; ++row)
    {
        // For repeatedly writing a row buffer.
        data_chunk row_out;
        data_sink row_sink(row_out);
        ostream_writer row_writer(row_sink);
        ostream_bit_writer row_bit_writer(row_writer);

        // Buffer left margin.
        for (size_t column = 0; column < horizontal_margin; ++column)
            row_bit_writer.write_bit(pixel_off);

        // Buffer scaled row pixels.
        for (size_t column = 0; column < width; ++column)
        {
            // Read byte and extract pixel (least significant) bit.
            const auto pixel_on = (image_reader.read_byte() & pixel_mask) != 0;

            // Buffer scaled pixel.
            for (size_t scaled = 0; scaled < scale; ++scaled)
                row_bit_writer.write_bit(pixel_on);
        }

        // Buffer right margin.
        for (size_t column = 0; column < horizontal_margin; ++column)
            row_bit_writer.write_bit(pixel_off);

        // Write row buffer scale times.
        for (size_t scaled = 0; scaled < scale; ++scaled)
        {
            // For repeatedly reading the row buffer.
            data_source row_source(row_out);
            istream_reader row_reader(row_source);
            istream_bit_reader row_bit_reader(row_reader);

            image_bit_writer.write_bit(row_bit_reader.read_bit());
        }
    }

    // Write bottom margin.
    for (size_t row = 0; row < vertical_margin; ++row)
        for (size_t column = 0; column < full_row_width; ++column)
            image_bit_writer.write_bit(pixel_off);

    // Guard against writer failure and unexpected stream length.
    if (!image_bit_writer || !image_reader || !image_reader.is_exhausted())
        return {};

    return image_out;
}

} // namespace system
} // namespace libbitcoin
