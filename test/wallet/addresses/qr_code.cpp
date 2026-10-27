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

BOOST_AUTO_TEST_SUITE(qr_code_tests)

using namespace bc::system::wallet;

// encode

static const uint16_t no_scale = 0u;
static const uint16_t no_margin = 0u;
static const uint8_t default_version = 0u;
static const auto low = qr_code::recovery_level::low;

BOOST_AUTO_TEST_CASE(qr_code__encode__no_value__false)
{
    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(!qr_code::encode(sink, ""));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__excessive_character_value__false)
{
    data_chunk out;
    stream::out::data sink(out);

    std::string value(bc::max_uint16, 0x42);
    BOOST_REQUIRE(!qr_code::encode(sink, value));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__one_character_version_0_scale_1_margin_0__expected)
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
        0x5f, 0x49, 0x44, 0x41,
        0x54, 0x78, 0x01, 0x01,
        0x54, 0x00, 0xab, 0xff,
        0x00, 0x01, 0xa4, 0x07,
        0x00, 0x7d, 0x8d, 0xf7,
        0x00, 0x45, 0x25, 0x17,
        0x00, 0x45, 0xad, 0x17,
        0x00, 0x45, 0xd5, 0x17,
        0x00, 0x7d, 0xf5, 0xf7,
        0x00, 0x01, 0x54, 0x07,
        0x00, 0xff, 0x27, 0xff,
        0x00, 0x10, 0x09, 0xdf,
        0x00, 0xce, 0xfd, 0xcf,
        0x00, 0x40, 0x57, 0x77,
        0x00, 0xdf, 0xfd, 0xc7,
        0x00, 0xf1, 0x15, 0x57,
        0x00, 0xff, 0x6a, 0xaf,
        0x00, 0x01, 0x68, 0x8f,
        0x00, 0x7d, 0x42, 0x2f,
        0x00, 0x45, 0x48, 0x97,
        0x00, 0x45, 0xfd, 0xcf,
        0x00, 0x45, 0x77, 0x77,
        0x00, 0x7d, 0x1d, 0xc7,
        0x00, 0x01, 0x35, 0x57,
        0xc6, 0x3f, 0x1d, 0x47,
        0xcc, 0xe2, 0x9d, 0xf4,
        0x00, 0x00, 0x00, 0x00,
        0x49, 0x45, 0x4e, 0x44,
        0xae, 0x42, 0x60, 0x82
    };

    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin));

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(out), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__invalid_version__false)
{
    data_chunk out;
    stream::out::data sink(out);

    BOOST_REQUIRE(!qr_code::encode(sink, "X", qr_code::maximum_version + 1u));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__maximum_version__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", qr_code::maximum_version));
    BOOST_REQUIRE_EQUAL(out.size(), 254263u);
}

BOOST_AUTO_TEST_CASE(qr_code__encode__excessive_margin__false)
{
    data_chunk out;
    stream::out::data sink(out);

    // At scale 0, (2 * (2^16 - 1))^2 > (2^32 - 1), the TIFF pixel limit. 
    BOOST_REQUIRE(!qr_code::encode(sink, "X", default_version, no_scale, bc::max_uint16));
}

BOOST_AUTO_TEST_CASE(qr_code__encode__excessive_scale__false)
{
    data_chunk out;
    stream::out::data sink(out);

    // At margin 0, scaling even a single pixel to 2^16 overflows the TIFF pixel limit.
    BOOST_REQUIRE(!qr_code::encode(sink, "X", default_version, bc::max_uint16, no_margin));
}

// recovery_level

BOOST_AUTO_TEST_CASE(qr_code__encode__recovery_level_low__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, qr_code::recovery_level::low));
    BOOST_REQUIRE_EQUAL(out.size(), 152u);
}

BOOST_AUTO_TEST_CASE(qr_code__encode__recovery_level_medium__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, qr_code::recovery_level::medium));
    BOOST_REQUIRE_EQUAL(out.size(), 152u);
}

BOOST_AUTO_TEST_CASE(qr_code__encode__recovery_level_high__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, qr_code::recovery_level::high));
    BOOST_REQUIRE_EQUAL(out.size(), 152u);
}

BOOST_AUTO_TEST_CASE(qr_code__encode__recovery_level_highest__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, qr_code::recovery_level::highest));
    BOOST_REQUIRE_EQUAL(out.size(), 152u);
}

// encode_mode

BOOST_AUTO_TEST_CASE(qr_code__encode__encode_mode_eight_bit__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, low, qr_code::encode_mode::eight_bit));
    BOOST_REQUIRE_EQUAL(out.size(), 152u);
}

BOOST_AUTO_TEST_CASE(qr_code__encode__encode_mode_kanji__expected_size)
{
    data_chunk out;
    stream::out::data sink(out);
    BOOST_REQUIRE(qr_code::encode(sink, "X", default_version, 1, no_margin, low, qr_code::encode_mode::kanji));
    BOOST_REQUIRE_EQUAL(out.size(), 152u);
}

// to_modules

BOOST_AUTO_TEST_CASE(qr_code__to_modules__empty__empty_zero_width)
{
    size_t width{ 42 };
    BOOST_REQUIRE(qr_code::to_modules(width, "").empty());
    BOOST_REQUIRE_EQUAL(width, 0u);
}

BOOST_AUTO_TEST_CASE(qr_code__to_modules__invalid_version__empty_zero_width)
{
    size_t width{ 42 };
    BOOST_REQUIRE(qr_code::to_modules(width, "X", qr_code::maximum_version + 1u).empty());
    BOOST_REQUIRE_EQUAL(width, 0u);
}

BOOST_AUTO_TEST_CASE(qr_code__to_modules__one_character__version_1)
{
    size_t width{};
    const auto modules = qr_code::to_modules(width, "X");
    BOOST_REQUIRE_EQUAL(width, 21u);
    BOOST_REQUIRE_EQUAL(modules.size(), 441u);
}

BOOST_AUTO_TEST_CASE(qr_code__to_modules__minimum_version__minimum_version_width)
{
    size_t width{};
    const auto modules = qr_code::to_modules(width, "X", 7);
    BOOST_REQUIRE_EQUAL(width, 45u);
    BOOST_REQUIRE_EQUAL(modules.size(), 2025u);
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
    static const data_chunk expected{ 0x00, 0x00, 0x00, 0x00 };

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
    // creates a 9 pixel image with the 5th most significant bit set to 1,
    // plus row padding.
    static const data_chunk expected{ 0x00, 0x40, 0x00 };

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
    BOOST_REQUIRE_EQUAL(encode_base16(pixels), encode_base16(expected));
}


BOOST_AUTO_TEST_CASE(qr_code__to_pixels__size_mismatch__empty)
{
    static const data_chunk data{ 0xff };
    BOOST_REQUIRE(qr_code_accessor::to_pixels(data, 2, 8, 2).empty());
}

BOOST_AUTO_TEST_SUITE_END()
