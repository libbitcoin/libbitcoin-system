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
        0xfe, 0x5b, 0xfc, 0x13,
        0x90, 0x6e, 0xb6, 0xbb,
        0x74, 0xa5, 0xdb, 0xa2,
        0xae, 0xc1, 0x05, 0x07,
        0xfa, 0xaf, 0xe0, 0x1b,
        0x00, 0xef, 0xf6, 0x21,
        0x88, 0x11, 0xaf, 0xea,
        0x22, 0x40, 0x04, 0x70,
        0xee, 0xaa, 0x80, 0x4a,
        0xab, 0xfa, 0x5d, 0xd0,
        0x57, 0xba, 0xba, 0xb7,
        0x6d, 0xd0, 0x11, 0xae,
        0xa2, 0x23, 0x05, 0xc4,
        0x7f, 0xec, 0xaa, 0x80
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
    // one byte image with only the most significant bit set to 1.
    static const data_chunk expected{ 0x80 };

    // This pixel state is true.
    static const data_chunk data{ 0x01 };
    const auto pixels = qr_code_accessor::to_pixels(data, 1, 1, 0);

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__one_pixel_scale_1_margin_1__expected)
{
    // Scale 1 does not expand. A single pixel image with single pixel margin
    // creates a 9 pixel image with the 5th most significant bit set.
    static const data_chunk expected{ 0x08, 0x00 };

    // This pixel state is true.
    static const data_chunk data{ 0x01 };
    const auto pixels = qr_code_accessor::to_pixels(data, 1, 1, 1);

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_pixels__four_pixels_scale_2_margin_2__expected)
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

    const auto pixels = qr_code_accessor::to_pixels(data, 2, 2, 2);

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}

BOOST_AUTO_TEST_SUITE_END()
