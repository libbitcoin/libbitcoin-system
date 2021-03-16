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

#include <bitcoin/system.hpp>

using namespace bc::system;

BOOST_AUTO_TEST_SUITE(qr_code_tests)

#ifdef WITH_QRENCODE

// This test may be sensitive to changes in qrencode conversion formatting.
BOOST_AUTO_TEST_CASE(qr_code__encode__always__expected)
{
    ////static const data_chunk expected
    ////{
    ////    // TODO: generate from qrencode.
    ////};

    ////data_chunk out;
    ////data_sink sink(out);
    ////BOOST_REQUIRE(!qr_code::encode(sink, "bitcoin:1L4M4obtbpexxuKpLrDimMEYWB2Rx2yzus"));
    ////BOOST_REQUIRE_EQUAL(out.size(), expected.size());

    ////// Encode as base16 so that failure message is intelligible.
    ////BOOST_REQUIRE_EQUAL(encode_base16(out), encode_base16(expected));
    BOOST_REQUIRE(true);
}

#else

BOOST_AUTO_TEST_CASE(qr_code__encode__not_implemented__false)
{
    data_chunk out;
    data_sink sink(out);
    BOOST_REQUIRE(!qr_code::encode(sink, "bitcoin:1L4M4obtbpexxuKpLrDimMEYWB2Rx2yzus"));
}

#endif // WITH_QRENCODE

BOOST_AUTO_TEST_CASE(qr_code__to_image_data__partial_header__empty)
{
    static const data_chunk data
    {
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00
    };

    // Must be at least 8 bytes of data (header), so empty result (failure).
    static const data_chunk expected{};

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(qr_code::to_image_data(data)), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_image_data__no_image_scale_0_margin_0__empty)
{
    static const data_chunk data
    {
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00
    };

    // Empty image with no margin creates empty result (success).
    static const data_chunk expected{};

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(qr_code::to_image_data(data, 0, 0)), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_image_data__no_image_scale_8_margin_2__expected)
{
    static const data_chunk data
    {
        0xff, 0xff, 0xff, 0xff,
        0x00, 0x00, 0x00, 0x00
    };

    // A 2 pixel margin on an empty image creates 16 zeroed pixels.
    static const data_chunk expected{ 0x00, 0x00 };

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(qr_code::to_image_data(data, 8, 2)), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_image_data__one_pixel_scale_0_margin_0__empty)
{
    static const data_chunk data
    {
        0xff, 0xff, 0xff, 0xff,
        0x01, 0x00, 0x00, 0x00,
        0xff
    };

    // Scale zero creates an empty image if there is no margin specified.
    static const data_chunk expected{};

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(qr_code::to_image_data(data, 0, 0)), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_image_data__one_pixel_scale_1_margin_0__expected)
{
    static const data_chunk data
    {
        0xff, 0xff, 0xff, 0xff,
        0x01, 0x00, 0x00, 0x00,

        // // This pixel state is true.
        0x01
    };

    // Scale 1 does not expand. A single pixel image with no margin creates a
    // one byte image with only the most significant bit set to 1.
    static const data_chunk expected{ 0x80 };

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(qr_code::to_image_data(data, 1, 0)), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_image_data__one_pixel_scale_1_margin_1__expected)
{
    static const data_chunk data
    {
        0xff, 0xff, 0xff, 0xff,
        0x01, 0x00, 0x00, 0x00,

        // This pixel state is true.
        0x01
    };

    // Scale 1 does not expand. A single pixel image with single pixel margin
    // creates a 9 pixel image with the 5th most significant bit set.
    static const data_chunk expected{ 0x08, 0x00 };

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(qr_code::to_image_data(data, 1, 1)), encode_base16(expected));
}

BOOST_AUTO_TEST_CASE(qr_code__to_image_data__four_pixel_scale_2_margin_2__expected)
{
    static const data_chunk data
    {
        0xff, 0xff, 0xff, 0xff,
        0x02, 0x00, 0x00, 0x00,

        // The pixel states are true, true, false, false.
        0x01, 0xff,
        0xfe, 0xfe
    };

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

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(qr_code::to_image_data(data, 2, 2)), encode_base16(expected));
}

// TODO: determine if TIFF encoding renders bits high-to-low => left-to-right.

BOOST_AUTO_TEST_SUITE_END()
