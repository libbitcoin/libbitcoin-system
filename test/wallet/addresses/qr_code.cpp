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

BC_AUTO_TEST_SUITE(qr_code_tests)

using namespace bc::system::wallet;

// encode

static const uint16_t no_scale = 0u;
static const uint16_t no_margin = 0u;
static const uint8_t default_version = 0u;
static const auto low = qr_code::recovery_level::low;

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

BC_AUTO_TEST_CASE(qr_code__encode__no_value__false)
{
    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(!qr_code::encode(sink, ""));
}

BC_AUTO_TEST_CASE(qr_code__encode__excessive_character_value__false)
{
    data_chunk out;
    stream::out::data sink(out);

    std::string value(bc::max_uint16, 0x42);
    BC_REQUIRE(!qr_code::encode(sink, value));
}

BC_AUTO_TEST_CASE(qr_code__encode__one_character_version_0_scale_1_margin_0__expected)
{
    static const data_chunk expected
    {
        // PNG signature, header, image data and trailer.
        0x89, 0x50, 0x4e, 0x47,
        0x0d, 0x0a, 0x1a, 0x0a,
        0x00, 0x00, 0x00, 0x0d,
        0x49, 0x48, 0x44, 0x52,
        0x00, 0x00, 0x00, 0x15,
        0x00, 0x00, 0x00, 0x15,
        0x01, 0x00, 0x00, 0x00,
        0x00, 0x81, 0x6c, 0x98,
        0x3b, 0x00, 0x00, 0x00,
        0x5c, 0x49, 0x44, 0x41,
        0x54, 0x78, 0x9c, 0x63,
        0x60, 0x5c, 0xc2, 0xce,
        0x50, 0xdb, 0xfb, 0x9d,
        0xc1, 0x55, 0x55, 0x9c,
        0xc1, 0x75, 0x2d, 0x10,
        0x5f, 0x15, 0x67, 0xa8,
        0xfd, 0xfa, 0x9d, 0x81,
        0x31, 0x84, 0x9d, 0xe1,
        0xbf, 0xfa, 0x7f, 0x06,
        0x01, 0xce, 0xfb, 0x0c,
        0xe7, 0xfe, 0x9e, 0x67,
        0x70, 0x08, 0x2f, 0x67,
        0xb8, 0xff, 0xf7, 0x38,
        0xc3, 0x47, 0xd1, 0x70,
        0x86, 0xff, 0x59, 0xeb,
        0x19, 0x18, 0x33, 0xfa,
        0x19, 0x6a, 0x9d, 0xf4,
        0x19, 0x5c, 0x3d, 0xa6,
        0x33, 0xb8, 0x02, 0xe5,
        0x5d, 0xcb, 0xcb, 0x19,
        0x6a, 0x65, 0x8f, 0x33,
        0x30, 0x9a, 0x86, 0x03,
        0x00, 0xc6, 0x3f, 0x1d,
        0x47, 0xdf, 0x18, 0x39,
        0x73, 0x00, 0x00, 0x00,
        0x00, 0x49, 0x45, 0x4e,
        0x44, 0xae, 0x42, 0x60,
        0x82
    };

    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin));

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(out), encode_base16(expected));
}

BC_AUTO_TEST_CASE(qr_code__encode__invalid_version__false)
{
    data_chunk out;
    stream::out::data sink(out);

    BC_REQUIRE(!qr_code::encode(sink, "X", qr_code::maximum_version + 1u));
}

BC_AUTO_TEST_CASE(qr_code__encode__maximum_version__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(qr_code::encode(sink, "X", qr_code::maximum_version));
    BC_REQUIRE_EQUAL(scanlines(out).size(), 254180u);
}

BC_AUTO_TEST_CASE(qr_code__encode__excessive_margin__false)
{
    data_chunk out;
    stream::out::data sink(out);

    // At scale 0, (2 * (2^16 - 1))^2 > (2^32 - 1), the TIFF pixel limit. 
    BC_REQUIRE(!qr_code::encode(sink, "X", default_version, no_scale, bc::max_uint16));
}

BC_AUTO_TEST_CASE(qr_code__encode__excessive_scale__false)
{
    data_chunk out;
    stream::out::data sink(out);

    // At margin 0, scaling even a single pixel to 2^16 overflows the TIFF pixel limit.
    BC_REQUIRE(!qr_code::encode(sink, "X", default_version, bc::max_uint16, no_margin));
}

// recovery_level

BC_AUTO_TEST_CASE(qr_code__encode__recovery_level_low__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, qr_code::recovery_level::low));
    BC_REQUIRE_EQUAL(scanlines(out).size(), 84u);
}

BC_AUTO_TEST_CASE(qr_code__encode__recovery_level_medium__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, qr_code::recovery_level::medium));
    BC_REQUIRE_EQUAL(scanlines(out).size(), 84u);
}

BC_AUTO_TEST_CASE(qr_code__encode__recovery_level_high__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, qr_code::recovery_level::high));
    BC_REQUIRE_EQUAL(scanlines(out).size(), 84u);
}

BC_AUTO_TEST_CASE(qr_code__encode__recovery_level_highest__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, qr_code::recovery_level::highest));
    BC_REQUIRE_EQUAL(scanlines(out).size(), 84u);
}

// encode_mode

BC_AUTO_TEST_CASE(qr_code__encode__encode_mode_eight_bit__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, low, qr_code::encode_mode::eight_bit));
    BC_REQUIRE_EQUAL(scanlines(out).size(), 84u);
}

BC_AUTO_TEST_CASE(qr_code__encode__encode_mode_kanji__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BC_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, low, qr_code::encode_mode::kanji));
    BC_REQUIRE_EQUAL(scanlines(out).size(), 84u);
}

// to_modules

BC_AUTO_TEST_CASE(qr_code__to_modules__empty__empty_zero_width)
{
    size_t width{ 42 };
    BC_REQUIRE(qr_code::to_modules(width, "").empty());
    BC_REQUIRE_EQUAL(width, 0u);
}

BC_AUTO_TEST_CASE(qr_code__to_modules__invalid_version__empty_zero_width)
{
    size_t width{ 42 };
    BC_REQUIRE(qr_code::to_modules(width, "X", qr_code::maximum_version + 1u).empty());
    BC_REQUIRE_EQUAL(width, 0u);
}

BC_AUTO_TEST_CASE(qr_code__to_modules__one_character__version_1)
{
    size_t width{};
    const auto modules = qr_code::to_modules(width, "X");
    BC_REQUIRE_EQUAL(width, 21u);
    BC_REQUIRE_EQUAL(modules.size(), 441u);
}

BC_AUTO_TEST_CASE(qr_code__to_modules__minimum_version__minimum_version_width)
{
    size_t width{};
    const auto modules = qr_code::to_modules(width, "X", 7);
    BC_REQUIRE_EQUAL(width, 45u);
    BC_REQUIRE_EQUAL(modules.size(), 2025u);
}

// to_pixels

class qr_code_accessor
  : qr_code
{
public:
    static data_chunk to_pixels(const data_chunk& coded, uint32_t coded_width,
        uint16_t scale=8, uint16_t margin=2)
    {
        return qr_code::to_pixels(coded, coded_width, scale, margin);
    }
};

BC_AUTO_TEST_CASE(qr_code__to_pixels__no_image_scale_0_margin_0__empty)
{
    // No image with no margin creates empty result (success).
    static const data_chunk expected{};

    static const data_chunk data;
    const auto pixels = qr_code_accessor::to_pixels(data, 0, 0, 0);

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BC_AUTO_TEST_CASE(qr_code__to_pixels__no_image_scale_8_margin_2__expected)
{
    // A 2 pixel margin on no image creates 16 zeroed pixels.
    static const data_chunk expected{ 0x00, 0x00, 0x00, 0x00 };

    static const data_chunk data;
    const auto pixels = qr_code_accessor::to_pixels(data, 0, 8, 2);

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BC_AUTO_TEST_CASE(qr_code__to_pixels__one_pixel_scale_0_margin_0__empty)
{
    // Scale zero creates a no image if there is no margin specified.
    static const data_chunk expected{};

    // This pixel state is true.
    static const data_chunk data{ 0xff };
    const auto pixels = qr_code_accessor::to_pixels(data, 1, 0, 0);

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BC_AUTO_TEST_CASE(qr_code__to_pixels__one_pixel_scale_1_margin_0__expected)
{
    // Scale 1 does not expand. A single pixel image with no margin creates a
    // one byte image with only the most significant bit set to 1.
    static const data_chunk expected{ 0x80 };

    // This pixel state is true.
    static const data_chunk data{ 0x01 };
    const auto pixels = qr_code_accessor::to_pixels(data, 1, 1, 0);

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BC_AUTO_TEST_CASE(qr_code__to_pixels__one_pixel_scale_1_margin_1__expected)
{
    // Scale 1 does not expand. A single pixel image with single pixel margin
    // creates a 9 pixel image with the 5th most significant bit set to 1,
    // plus row padding.
    static const data_chunk expected{ 0x00, 0x40, 0x00 };

    // This pixel state is true.
    static const data_chunk data{ 0x01 };
    const auto pixels = qr_code_accessor::to_pixels(data, 1, 1, 1);

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BC_AUTO_TEST_CASE(qr_code__to_pixels__endianness_independent__expected)
{
    // Area is 2x2px so image is scaled to 4x4px and 2px margin makes it 8x8px.
    static const data_chunk expected
    {
        // byte  pixel
        0x00, // 00 0000 00 
        0x00, // 00 0000 00

        0x3c, // 00 1111 00
        0x3c, // 00 1111 00

        0x00, // 00 0000 00
        0x00, // 00 0000 00

        0x00, // 00 0000 00
        0x00  // 00 0000 00
    };

    // The pixel states are true, true, false, false.
    static const data_chunk data
    {
        0x01, 0xff,
        0xfe, 0xfe
    };

    // This test is mirror image with respect to bitwise endianness so it
    // succeeds whether bits are written big endian or little endian. 
    const auto pixels = qr_code_accessor::to_pixels(data, 2, 2, 2);

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BC_AUTO_TEST_CASE(qr_code__to_pixels__little_endian_dependent__expected)
{
    // Area is 2x2px so image is scaled to 4x4px and 2px margin makes it 8x8px.
    static const data_chunk expected
    {
        // byte  pixel
        0x00, // 00 0000 00 
        0x00, // 00 0000 00

        0x30, // 00 1100 00
        0x30, // 00 1100 00

        0x0c, // 00 0011 00
        0x0c, // 00 0011 00

        0x00, // 00 0000 00
        0x00  // 00 0000 00
    };

    // The pixel states are true, false, false, true.
    static const data_chunk data
    {
        0x01, 0xfe,
        0xfe, 0xff
    };

    // This test is not mirror image with respect to bitwise endianness.
    const auto pixels = qr_code_accessor::to_pixels(data, 2, 2, 2);

    // Encode as base16 so that failure message is intelligible.
    BC_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}


BC_AUTO_TEST_CASE(qr_code__to_pixels__size_mismatch__empty)
{
    static const data_chunk data{ 0xff };
    BC_REQUIRE(qr_code_accessor::to_pixels(data, 2, 8, 2).empty());
}

BC_AUTO_TEST_SUITE_END()
