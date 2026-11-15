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
#include "../../test.hpp"

BC_AUTO_TEST_SUITE(png_tests)

// The inflated image data (scanlines) of a png of one image data chunk.
static data_chunk scanlines(const data_chunk& png)
{
    constexpr size_t field = sizeof(uint32_t);
    const std::string type{ "IDAT" };
    const auto it = std::search(png.begin(), png.end(), type.begin(),
        type.end());

    const data_chunk length(std::prev(it, field), it);
    const auto begin = std::next(it, field);
    const auto end = std::next(begin, from_big_endian<uint32_t>(length));

    data_chunk out{};
    zlib_inflate(out, data_chunk(begin, end));
    return out;
}

// Rows of one byte, each prefixed by filter type zero.
static data_chunk filtered_rows(size_t count, uint8_t byte)
{
    data_chunk out{};
    for (size_t row{}; row < count; ++row)
        out.insert(out.end(), { 0x00, byte });

    return out;
}

BC_AUTO_TEST_CASE(png__to_image__rectangular_image__true)
{
    // A rectangle of 5 pixel width has 1 byte rows, so any number of bytes is ok.
    static const auto size = 42u;
    static const auto width = 5u;

    data_chunk bitmap(size, 'x');
    data_chunk png;
    stream::out::data stream(png);
    BC_REQUIRE(png::to_image(stream, bitmap, width));
}

BC_AUTO_TEST_CASE(png__to_image__mismatched_image_size__false)
{
    // A rectangle of 9 pixel width has 2 byte rows, so byte count must be even.
    static const auto size = 41u;
    static const auto width = 9u;

    data_chunk bitmap(size, 'x');
    data_chunk png;
    stream::out::data stream(png);
    BC_REQUIRE(!png::to_image(stream, bitmap, width));
}

BC_AUTO_TEST_CASE(png__to_image__empty__false)
{
    static const auto width = 1u;
    static const data_chunk bitmap;
    data_chunk png;
    stream::out::data stream(png);
    BC_REQUIRE(!png::to_image(stream, bitmap, width));
}

BC_AUTO_TEST_CASE(png__to_image__width_0__false)
{
    static const auto width = 0u;
    static const data_chunk bitmap{ 'x' };
    data_chunk png;
    stream::out::data stream(png);
    BC_REQUIRE(!png::to_image(stream, bitmap, width));
}

BC_AUTO_TEST_CASE(png__to_image__perfect_square__expected_true)
{
    // A square with sides of 4 pixels is 4 rows of 1 byte.
    static const auto width = 4u;
    static const data_chunk bitmap{ 0x20, 0xa0, 0x20, 0xb0 };

    // Manually-contructed expectation of png encoding.
    static const data_chunk expected
    {
        // Signature.
        0x89, 0x50, 0x4e, 0x47, // "\x89PNG"
        0x0d, 0x0a, 0x1a, 0x0a, // "\r\n\x1a\n"

        // IHDR
        0x00, 0x00, 0x00, 0x0d, // length
        0x49, 0x48, 0x44, 0x52, // "IHDR"
        0x00, 0x00, 0x00, 0x04, // width
        0x00, 0x00, 0x00, 0x04, // height
        0x01,                   // bit depth
        0x00,                   // grayscale (zero is black)
        0x00,                   // deflate
        0x00,                   // adaptive filtering
        0x00,                   // no interlace
        0x81, 0x8a, 0xa3, 0xd3, // crc32

        // IDAT
        0x00, 0x00, 0x00, 0x0e, // length
        0x49, 0x44, 0x41, 0x54, // "IDAT"
        0x78, 0x9c,             // zlib header (deflate, default)

        // Final fixed block of the scanlines, each prefixed by filter type,
        // inverted: 00 df (1101 1111), 00 5f (0101 1111), 00 df, 00 4f.
        0x63, 0xb8, 0xcf, 0x10,
        0x0f, 0xc4, 0xfe, 0x00,

        0x0a, 0xe8, 0x02, 0x6d, // adler32
        0x40, 0xaa, 0x6d, 0xa1, // crc32

        // IEND
        0x00, 0x00, 0x00, 0x00, // length
        0x49, 0x45, 0x4e, 0x44, // "IEND"
        0xae, 0x42, 0x60, 0x82  // crc32
    };

    data_chunk png;
    stream::out::data stream(png);
    BC_REQUIRE(png::to_image(stream, bitmap, width));

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(png), encode_base16(expected));
}

BC_AUTO_TEST_CASE(png__to_image__repeated_rows__compressed)
{
    // 40000 rows of 1 byte are 80000 scanline bytes, 'x' (0x78) inverted.
    static const auto size = 40000u;
    static const auto width = 8u;

    data_chunk bitmap(size, 'x');
    data_chunk png;
    stream::out::data stream(png);
    BC_REQUIRE(png::to_image(stream, bitmap, width));
    BC_REQUIRE_EQUAL(png.size(), 171u);
    BC_REQUIRE_EQUAL(scanlines(png), filtered_rows(size, 0x87));
}

BC_AUTO_TEST_CASE(png__to_image__excessive_image_size__false)
{
    const auto maximum = png::max_image_bytes;
    png::max_image_bytes = 1;

    data_chunk bitmap(2, 'x');
    data_chunk image;
    stream::out::data stream(image);
    const auto result = png::to_image(stream, bitmap, 5);
    png::max_image_bytes = maximum;
    BC_REQUIRE(!result);
}

BC_AUTO_TEST_SUITE_END()
