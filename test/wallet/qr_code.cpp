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
#include <boost/test/unit_test.hpp>

#include <string>
#include <bitcoin/system.hpp>

using namespace bc::system;
using namespace bc::system::wallet;

BOOST_AUTO_TEST_SUITE(qr_code_tests)

// encode

static const uint16_t no_scale = 0u;
static const uint16_t no_margin = 0u;
static const uint8_t default_version = 0u;

BOOST_AUTO_TEST_CASE(qr_code__encode__no_value__false)
{
    data_chunk out;
    data_sink sink(out);
    BOOST_REQUIRE(!qr_code::encode(sink, ""));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__excessive_character_value__false)
{
    data_chunk out;
    data_sink sink(out);

    // ERANGE returned by qrencode.
    std::string value(bc::max_uint16, 0x42);
    BOOST_REQUIRE(!qr_code::encode(sink, value));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__one_character_version_0_scale_1_margin_0__expected)
{
    static const data_chunk expected
    {
        // TIFF metadata.
        0x4d, 0x4d, 0x00, 0x2a,
        0x00, 0x00, 0x00, 0x08,
        0x00, 0x0a, 0x01, 0x00,
        0x00, 0x04, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x15, 0x01, 0x01,
        0x00, 0x04, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x15, 0x01, 0x03,
        0x00, 0x03, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x01,
        0x00, 0x00, 0x01, 0x06,
        0x00, 0x03, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x11,
        0x00, 0x04, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x9c, 0x01, 0x16,
        0x00, 0x04, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x15, 0x01, 0x17,
        0x00, 0x04, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x38, 0x01, 0x1a,
        0x00, 0x05, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x88, 0x01, 0x1b,
        0x00, 0x05, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x00,
        0x00, 0x90, 0x01, 0x28,
        0x00, 0x03, 0x00, 0x00,
        0x00, 0x01, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x48,
        0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x48,
        0x00, 0x00, 0x00, 0x01,

        // Delimiter.
        0xba, 0xad, 0xf0, 0x0d,

        // Image data.
        0x7f, 0xda, 0x3f, 0xc8,
        0x09, 0x76, 0x6d, 0xdd,
        0x2e, 0xa5, 0xdb, 0x45,
        0x75, 0x83, 0xa0, 0xe0,
        0x5f, 0xf5, 0x07, 0xd8,
        0x00, 0xf7, 0x6f, 0x84,
        0x11, 0x88, 0xf5, 0x57,
        0x44, 0x02, 0x20, 0x0e,
        0x77, 0x55, 0x01, 0x52,
        0xd5, 0x5f, 0xba, 0x0b,
        0xea, 0x5d, 0x5d, 0xed,
        0xb6, 0x0b, 0x88, 0x75,
        0x45, 0xc4, 0xa0, 0x23,
        0xfe, 0x37, 0x55, 0x01
    };

    data_chunk out;
    data_sink sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin));

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(out), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__invalid_version__false)
{
    data_chunk out;
    data_sink sink(out);

    // EINVAL returned by qrencode.
    BOOST_REQUIRE(!qr_code::encode(sink, "X", qr_code::maximum_version + 1u));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__maximum_version__expected_size)
{
    data_chunk out;
    data_sink sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", qr_code::maximum_version));
    BOOST_REQUIRE_EQUAL(out.size(), 252206u);
}

BOOST_AUTO_TEST_CASE(qr_code__encode__excessive_margin__false)
{
    data_chunk out;
    data_sink sink(out);

    // At scale 0, (2 * (2^16 - 1))^2 > (2^32 - 1), the TIFF pixel limit. 
    BOOST_REQUIRE(!qr_code::encode(sink, "X", default_version, no_scale, bc::max_uint16));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__excessive_scale__false)
{
    data_chunk out;
    data_sink sink(out);

    // At margin 0, scaling even a single pixel to 2^16 overflows the TIFF pixel limit.
    BOOST_REQUIRE(!qr_code::encode(sink, "X", default_version, bc::max_uint16, no_margin));
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

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__no_image_scale_0_margin_0__empty)
{
    // No image with no margin creates empty result (success).
    static const data_chunk expected{};

    static const data_chunk data;
    const auto pixels = qr_code_accessor::to_pixels(data, 0, 0, 0);

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__no_image_scale_8_margin_2__expected)
{
    // A 2 pixel margin on no image creates 16 zeroed pixels.
    static const data_chunk expected{ 0x00, 0x00 };

    static const data_chunk data;
    const auto pixels = qr_code_accessor::to_pixels(data, 0, 8, 2);

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__one_pixel_scale_0_margin_0__empty)
{
    // Scale zero creates a no image if there is no margin specified.
    static const data_chunk expected{};

    // This pixel state is true.
    static const data_chunk data{ 0xff };
    const auto pixels = qr_code_accessor::to_pixels(data, 1, 0, 0);

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__one_pixel_scale_1_margin_0__expected)
{
    // Scale 1 does not expand. A single pixel image with no margin creates a
    // one byte image with only the least significant bit set to 1.
    static const data_chunk expected{ 0x01 };

    // This pixel state is true.
    static const data_chunk data{ 0x01 };
    const auto pixels = qr_code_accessor::to_pixels(data, 1, 1, 0);

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__one_pixel_scale_1_margin_1__expected)
{
    // Scale 1 does not expand. A single pixel image with single pixel margin
    // creates a 9 pixel image with the 5th least significant bit set to 1.
    static const data_chunk expected{ 0x10, 0x00 };

    // This pixel state is true.
    static const data_chunk data{ 0x01 };
    const auto pixels = qr_code_accessor::to_pixels(data, 1, 1, 1);

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__endianness_independent__expected)
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
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__little_endian_dependent__expected)
{
    // Area is 2x2px so image is scaled to 4x4px and 2px margin makes it 8x8px.
    static const data_chunk expected
    {
        // byte  pixel
        0x00, // 00 0000 00 
        0x00, // 00 0000 00

        0x0c, // 00 0011 00
        0x0c, // 00 0011 00

        0x30, // 00 1100 00
        0x30, // 00 1100 00

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
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_SUITE_END()
