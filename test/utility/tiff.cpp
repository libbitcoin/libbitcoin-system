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
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test_suite.hpp>

#include <bitcoin/system.hpp>

using namespace bc::system;

BOOST_AUTO_TEST_SUITE(tiff_tests)

BOOST_AUTO_TEST_CASE(tiff__to_image_data__mismatched_image_width__false)
{
    // A square with sides of 5 pixels is [(5^2 + 7) / 8 = 4] bytes (not 42).
    static const auto byte_size = 42u;
    static const auto pixel_width = 5u;

    data_chunk bitmap(byte_size, 0);
    data_chunk tiff;
    data_sink stream(tiff);
    BOOST_REQUIRE(!tiff::to_image(stream, bitmap, pixel_width));
}

BOOST_AUTO_TEST_CASE(tiff__to_image_data__matched_image_width_and_size__true)
{
    // A square with sides of 5 pixels is [(5^2 + 7) / 8 = 4] bytes.
    static const auto byte_size = 4u;
    static const auto pixel_width = 5u;

    // Set valid byte_size, other data is arbitrary.
    data_chunk bitmap(byte_size, 0);
    data_chunk tiff;
    data_sink stream(tiff);
    BOOST_REQUIRE(tiff::to_image(stream, bitmap, pixel_width));
}

BOOST_AUTO_TEST_CASE(tiff__to_image_data__zero_area__empty_true)
{
    static const data_chunk bitmap;
    data_chunk tiff;
    data_sink stream(tiff);
    BOOST_REQUIRE(tiff::to_image(stream, bitmap, 0));
    BOOST_REQUIRE_EQUAL(tiff.size(), tiff::image_offset);
}

BOOST_AUTO_TEST_CASE(tiff__to_image_data__max_width__expected_size_true)
{
    static const auto width = bc::max_uint16;
    static const auto expected_image_bytes = tiff::max_image_bytes;

    // 511MB memory allocation here.
    data_chunk bitmap(expected_image_bytes, 0);
    data_chunk tiff;
    data_sink stream(tiff);
    BOOST_REQUIRE(tiff::to_image(stream, bitmap, width));
    BOOST_REQUIRE_EQUAL(tiff.size(), tiff::image_offset + expected_image_bytes);
}

BOOST_AUTO_TEST_CASE(tiff__to_image_data__perfect_square__expected_true)
{
    // A square with sides of 4 pixels is [(4^2 + 7) / 8 = 2] bytes.
    static const data_chunk bitmap
    {
        0x20, 0xa0,
        0x20, 0xb0
    };

    // Manually-contructed expectation of tiff encoding.
    static const data_chunk expected
    {
        // Header
        0x4d, 0x4d,             // "MM" Motorola encoding (big endian)
        0x00, 0x2a,             // magic number (42)
        0x00, 0x00, 0x00, 0x08, // IFD0 offset

        // [Each directory may be anywhere in the file]

        // IFD0, word boundary required.
        0x00, 0x0a,             // count of entries

        // IFDE0
        0x01, 0x00,             // image_width
        0x00, 0x04,             // long value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x00, 0x00, 0x04, // side

        // IFDE1
        0x01, 0x01,             // image_length
        0x00, 0x04,             // long value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x00, 0x00, 0x04, // side

        // IFDE2
        0x01, 0x03,             // compression
        0x00, 0x03,             // short value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x01,             // none
        0x00, 0x00,             // pad

        // IFDE3
        0x01, 0x06,             // photometric_interpretation
        0x00, 0x03,             // short value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x00,             // zero is white
        0x00, 0x00,             // pad

        // IFDE4
        0x01, 0x11,             // strip_offsets
        0x00, 0x04,             // long value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x00, 0x00, 0x9c, // value (not offset)

        // IFDE5
        0x01, 0x16,             // rows_per_strip
        0x00, 0x04,             // long value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x00, 0x00, 0x04, // side

        // IFDE6
        0x01, 0x17,             // strip_byte_counts
        0x00, 0x04,             // long value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x00, 0x00, 0x04, // value (not offset)

        // IFDE7
        0x01, 0x1a,             // x_resolution
        0x00, 0x05,             // ratio value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x00, 0x00, 0x88, // value (offset)

        // IFDE8
        0x01, 0x1b,             // y_resolution
        0x00, 0x05,             // ratio value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x00, 0x00, 0x90, // value (offset)

        // IFDE9
        0x01, 0x28,             // resolution_unit
        0x00, 0x03,             // short value
        0x00, 0x00, 0x00, 0x01, // count
        0x00, 0x01,             // inch
        0x00, 0x00,             // pad

        // IFD0 Terminator
        0x00, 0x00, 0x00, 0x00, // null teminated linked list

        // pad
        0x00, 0x00,             // pad

        // [Each field value may be anywhere in the file.]

        // Offset field values, word boundaries required.
        0x00, 0x00, 0x00, 0x48, // horizontal resolution numerator
        0x00, 0x00, 0x00, 0x01, // horizontal resolution denominator
        0x00, 0x00, 0x00, 0x48, // vertical resolution numerator
        0x00, 0x00, 0x00, 0x01, // vertical resolution denominator

        // Libbitcoin proprietary data delimiter.
        0xba, 0xad, 0xf0, 0x0d,

        // [Image strips may be anywhere in the file.]

        // Image (strip) - each row padded.
        // 0010 0000
        // 1010 0000
        // 0010 0000
        // 1011 0000
        0x20, 0xa0,
        0x20, 0xb0
    };

    data_chunk tiff;
    data_sink stream(tiff);
    BOOST_REQUIRE(tiff::to_image(stream, bitmap, 4));

    // Encode as base16 so that failure message is intelligible.
    BOOST_REQUIRE_EQUAL(encode_base16(tiff), encode_base16(expected));
}

BOOST_AUTO_TEST_SUITE_END()
