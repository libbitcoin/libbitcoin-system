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

BC_AUTO_TEST_SUITE(qr_encoder_tests)

using namespace bc::system::wallet;

class accessor
  : public qr_encoder
{
public:
    using qr_encoder::segment_mode;
    using qr_encoder::segment;
    using qr_encoder::segments;
    using qr_encoder::error_codewords;
    using qr_encoder::blocks;
    using qr_encoder::length_bits;
    using qr_encoder::payload_bits;
    using qr_encoder::minimum_fit;
    using qr_encoder::classify;
    using qr_encoder::to_upper;
    using qr_encoder::split;
    using qr_encoder::estimate_bits;
    using qr_encoder::stream_bits;
    using qr_encoder::select_version;
    using qr_encoder::to_data;
    using qr_encoder::to_error;
    using qr_encoder::interleave;
};

using mode = accessor::segment_mode;
using level = qr_encoder::recovery_level;
using segments = accessor::segments;

// Shift-JIS kanji characters (0x935f, 0xe4aa).
static const std::string kanji_pair{ "\x93\x5f\xe4\xaa" };

// Twenty three alternating eight bit and kanji segments.
#define UNIT "a\x93\x5f"
static const std::string alternating{ UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT UNIT };
#undef UNIT

// "HELLO WORLD" at version 1, recovery level medium.
static const data_chunk hello_data{ 0x20, 0x5b, 0x0b, 0x78, 0xd1, 0x72, 0xdc, 0x4d, 0x43, 0x40, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11 };
static const data_chunk hello_error{ 0xc4, 0x23, 0x27, 0x77, 0xeb, 0xd7, 0xe7, 0xe2, 0x5d, 0x17 };

// codewords

BC_AUTO_TEST_CASE(qr_encoder__codewords__invalid_versions__zero)
{
    BC_REQUIRE_EQUAL(qr_encoder::codewords(0), 0u);
    BC_REQUIRE_EQUAL(qr_encoder::codewords(41), 0u);
}

BC_AUTO_TEST_CASE(qr_encoder__codewords__valid_versions__expected)
{
    BC_REQUIRE_EQUAL(qr_encoder::codewords(1), 26u);
    BC_REQUIRE_EQUAL(qr_encoder::codewords(2), 44u);
    BC_REQUIRE_EQUAL(qr_encoder::codewords(7), 196u);
    BC_REQUIRE_EQUAL(qr_encoder::codewords(40), 3706u);
}

// data_codewords

BC_AUTO_TEST_CASE(qr_encoder__data_codewords__invalid_version__zero)
{
    BC_REQUIRE_EQUAL(qr_encoder::data_codewords(0, level::low), 0u);
    BC_REQUIRE_EQUAL(qr_encoder::data_codewords(41, level::low), 0u);
}

BC_AUTO_TEST_CASE(qr_encoder__data_codewords__valid_versions__expected)
{
    BC_REQUIRE_EQUAL(qr_encoder::data_codewords(1, level::low), 19u);
    BC_REQUIRE_EQUAL(qr_encoder::data_codewords(1, level::medium), 16u);
    BC_REQUIRE_EQUAL(qr_encoder::data_codewords(1, level::high), 13u);
    BC_REQUIRE_EQUAL(qr_encoder::data_codewords(1, level::highest), 9u);
    BC_REQUIRE_EQUAL(qr_encoder::data_codewords(40, level::low), 2956u);
    BC_REQUIRE_EQUAL(qr_encoder::data_codewords(40, level::highest), 1276u);
}

// encode

BC_AUTO_TEST_CASE(qr_encoder__encode__empty__empty_version_unchanged)
{
    uint8_t version{ 3 };
    BC_REQUIRE(qr_encoder::encode(version, "").empty());
    BC_REQUIRE_EQUAL(version, 3u);
}

BC_AUTO_TEST_CASE(qr_encoder__encode__invalid_version__empty_version_unchanged)
{
    uint8_t version{ 41 };
    BC_REQUIRE(qr_encoder::encode(version, "X").empty());
    BC_REQUIRE_EQUAL(version, 41u);
}

BC_AUTO_TEST_CASE(qr_encoder__encode__excessive_value__empty_version_unchanged)
{
    uint8_t version{};
    BC_REQUIRE(qr_encoder::encode(version, std::string(max_uint16, 'B')).empty());
    BC_REQUIRE_EQUAL(version, 0u);
}

BC_AUTO_TEST_CASE(qr_encoder__encode__hello_world_medium__expected)
{
    uint8_t version{};
    BC_REQUIRE_EQUAL(qr_encoder::encode(version, "HELLO WORLD", level::medium), splice(hello_data, hello_error));
    BC_REQUIRE_EQUAL(version, 1u);
}

BC_AUTO_TEST_CASE(qr_encoder__encode__minimum_version__expected_version)
{
    uint8_t version{ 5 };
    BC_REQUIRE_EQUAL(qr_encoder::encode(version, "X").size(), 134u);
    BC_REQUIRE_EQUAL(version, 5u);
}

BC_AUTO_TEST_CASE(qr_encoder__encode__maximum_capacity__maximum_version)
{
    uint8_t version{};
    BC_REQUIRE_EQUAL(qr_encoder::encode(version, std::string(2953, 'x')).size(), 3706u);
    BC_REQUIRE_EQUAL(version, 40u);
}

BC_AUTO_TEST_CASE(qr_encoder__encode__above_maximum_capacity__empty)
{
    uint8_t version{};
    BC_REQUIRE(qr_encoder::encode(version, std::string(2954, 'x')).empty());
}

BC_AUTO_TEST_CASE(qr_encoder__encode__case_insensitive__upper_case)
{
    uint8_t version1{};
    uint8_t version2{};
    BC_REQUIRE_EQUAL(qr_encoder::encode(version1, "hello world", level::medium, qr_encoder::encode_mode::eight_bit, false), qr_encoder::encode(version2, "HELLO WORLD", level::medium));
}

BC_AUTO_TEST_CASE(qr_encoder__encode__kanji_mode__kanji_segment)
{
    uint8_t version1{};
    uint8_t version2{};
    const auto kanji = qr_encoder::encode(version1, kanji_pair, level::low, qr_encoder::encode_mode::kanji);
    const auto eight = qr_encoder::encode(version2, kanji_pair, level::low, qr_encoder::encode_mode::eight_bit);
    BC_REQUIRE_EQUAL(kanji.size(), 26u);
    BC_REQUIRE_EQUAL(kanji.front(), 0x80u);
    BC_REQUIRE_EQUAL(eight.front(), 0x40u);
}

// error_codewords

BC_AUTO_TEST_CASE(qr_encoder__error_codewords__versions__expected)
{
    BC_REQUIRE_EQUAL(accessor::error_codewords(1, level::low), 7u);
    BC_REQUIRE_EQUAL(accessor::error_codewords(5, level::high), 72u);
    BC_REQUIRE_EQUAL(accessor::error_codewords(40, level::highest), 2430u);
}

// blocks

BC_AUTO_TEST_CASE(qr_encoder__blocks__versions__expected)
{
    BC_REQUIRE_EQUAL(accessor::blocks(1, level::low), 1u);
    BC_REQUIRE_EQUAL(accessor::blocks(5, level::high), 4u);
    BC_REQUIRE_EQUAL(accessor::blocks(40, level::highest), 81u);
}

// length_bits

BC_AUTO_TEST_CASE(qr_encoder__length_bits__version_ranges__expected)
{
    BC_REQUIRE_EQUAL(accessor::length_bits(mode::numeric, 0), 10u);
    BC_REQUIRE_EQUAL(accessor::length_bits(mode::numeric, 9), 10u);
    BC_REQUIRE_EQUAL(accessor::length_bits(mode::numeric, 10), 12u);
    BC_REQUIRE_EQUAL(accessor::length_bits(mode::numeric, 26), 12u);
    BC_REQUIRE_EQUAL(accessor::length_bits(mode::numeric, 27), 14u);
    BC_REQUIRE_EQUAL(accessor::length_bits(mode::alpha_numeric, 1), 9u);
    BC_REQUIRE_EQUAL(accessor::length_bits(mode::eight_bit, 40), 16u);
    BC_REQUIRE_EQUAL(accessor::length_bits(mode::kanji, 40), 12u);
}

// payload_bits

BC_AUTO_TEST_CASE(qr_encoder__payload_bits__modes__expected)
{
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::numeric, 0), 0u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::numeric, 1), 4u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::numeric, 2), 7u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::numeric, 3), 10u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::numeric, 7), 24u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::alpha_numeric, 1), 6u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::alpha_numeric, 2), 11u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::alpha_numeric, 3), 17u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::eight_bit, 3), 24u);
    BC_REQUIRE_EQUAL(accessor::payload_bits(mode::kanji, 4), 26u);
}

// minimum_fit

BC_AUTO_TEST_CASE(qr_encoder__minimum_fit__sizes__expected)
{
    BC_REQUIRE_EQUAL(accessor::minimum_fit(0, level::low), 1u);
    BC_REQUIRE_EQUAL(accessor::minimum_fit(152, level::low), 1u);
    BC_REQUIRE_EQUAL(accessor::minimum_fit(153, level::low), 2u);
    BC_REQUIRE_EQUAL(accessor::minimum_fit(153, level::highest), 3u);
    BC_REQUIRE_EQUAL(accessor::minimum_fit(max_uint32, level::low), 40u);
}

// classify

BC_AUTO_TEST_CASE(qr_encoder__classify__characters__expected)
{
    BC_REQUIRE(accessor::classify("5", 1, false) == mode::end);
    BC_REQUIRE(accessor::classify("5", 0, false) == mode::numeric);
    BC_REQUIRE(accessor::classify("A", 0, false) == mode::alpha_numeric);
    BC_REQUIRE(accessor::classify(":", 0, false) == mode::alpha_numeric);
    BC_REQUIRE(accessor::classify("a", 0, false) == mode::eight_bit);
    BC_REQUIRE(accessor::classify(std::string(1, '\0'), 0, false) == mode::eight_bit);
}

BC_AUTO_TEST_CASE(qr_encoder__classify__kanji__expected)
{
    BC_REQUIRE(accessor::classify(kanji_pair, 0, true) == mode::kanji);
    BC_REQUIRE(accessor::classify(kanji_pair, 2, true) == mode::kanji);
    BC_REQUIRE(accessor::classify(kanji_pair, 0, false) == mode::eight_bit);
    BC_REQUIRE(accessor::classify(kanji_pair, 3, true) == mode::eight_bit);
    BC_REQUIRE(accessor::classify("\xa0\x40", 0, true) == mode::eight_bit);
    BC_REQUIRE(accessor::classify("\x81\x3f", 0, true) == mode::eight_bit);
    BC_REQUIRE(accessor::classify("\xeb\xc0", 0, true) == mode::eight_bit);
}

// to_upper

BC_AUTO_TEST_CASE(qr_encoder__to_upper__mixed__upper_case)
{
    BC_REQUIRE_EQUAL(accessor::to_upper("abc123xyz:{}", false), "ABC123XYZ:{}");
}

BC_AUTO_TEST_CASE(qr_encoder__to_upper__kanji__kanji_unchanged)
{
    BC_REQUIRE_EQUAL(accessor::to_upper("\x8a\x61", true), "\x8a\x61");
    BC_REQUIRE_EQUAL(accessor::to_upper("\x8a\x61", false), "\x8a\x41");
    BC_REQUIRE_EQUAL(accessor::to_upper("a\x8a\x61", true), "A\x8a\x61");
}

// split

BC_AUTO_TEST_CASE(qr_encoder__split__empty__empty)
{
    BC_REQUIRE(accessor::split("", 0, false).empty());
}

BC_AUTO_TEST_CASE(qr_encoder__split__numeric__numeric)
{
    const segments expected{ { mode::numeric, 0, 5 } };
    BC_REQUIRE(accessor::split("12345", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__short_numeric_eight_bit__eight_bit)
{
    const segments expected{ { mode::eight_bit, 0, 2 } };
    BC_REQUIRE(accessor::split("1a", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__long_numeric_eight_bit__numeric_eight_bit)
{
    const segments expected{ { mode::numeric, 0, 9 }, { mode::eight_bit, 9, 1 } };
    BC_REQUIRE(accessor::split("123456789a", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__short_numeric_alpha_numeric__alpha_numeric)
{
    const segments expected{ { mode::alpha_numeric, 0, 2 } };
    BC_REQUIRE(accessor::split("1A", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__long_numeric_alpha_numeric__numeric_alpha_numeric)
{
    const segments expected{ { mode::numeric, 0, 9 }, { mode::alpha_numeric, 9, 1 } };
    BC_REQUIRE(accessor::split("123456789A", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__numeric_kanji__numeric_kanji)
{
    const segments expected{ { mode::numeric, 0, 1 }, { mode::kanji, 1, 4 } };
    BC_REQUIRE(accessor::split("1" + kanji_pair, 0, true) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__alpha_numeric__alpha_numeric)
{
    const segments expected{ { mode::alpha_numeric, 0, 4 } };
    BC_REQUIRE(accessor::split("AB1C", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__alpha_numeric_long_numeric__alpha_numeric_numeric)
{
    const segments expected{ { mode::alpha_numeric, 0, 1 }, { mode::numeric, 1, 13 } };
    BC_REQUIRE(accessor::split("A1234567890123", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__short_alpha_numeric_eight_bit__eight_bit)
{
    const segments expected{ { mode::eight_bit, 0, 2 } };
    BC_REQUIRE(accessor::split("Aa", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__long_alpha_numeric_eight_bit__alpha_numeric_eight_bit)
{
    const segments expected{ { mode::alpha_numeric, 0, 8 }, { mode::eight_bit, 8, 1 } };
    BC_REQUIRE(accessor::split("ABCDEFGHa", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__kanji__kanji)
{
    const segments expected{ { mode::kanji, 0, 4 } };
    BC_REQUIRE(accessor::split(kanji_pair, 0, true) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__kanji_not_hinted__eight_bit)
{
    const segments expected{ { mode::eight_bit, 0, 4 } };
    BC_REQUIRE(accessor::split(kanji_pair, 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__eight_bit_kanji__eight_bit_kanji)
{
    const segments expected{ { mode::eight_bit, 0, 1 }, { mode::kanji, 1, 4 } };
    BC_REQUIRE(accessor::split("a" + kanji_pair, 0, true) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__eight_bit_short_numeric__eight_bit)
{
    const segments expected{ { mode::eight_bit, 0, 3 } };
    BC_REQUIRE(accessor::split("a1b", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__eight_bit_long_numeric__eight_bit_numeric)
{
    const segments expected{ { mode::eight_bit, 0, 1 }, { mode::numeric, 1, 9 } };
    BC_REQUIRE(accessor::split("a123456789", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__eight_bit_short_alpha_numeric__eight_bit)
{
    const segments expected{ { mode::eight_bit, 0, 3 } };
    BC_REQUIRE(accessor::split("aBc", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__eight_bit_long_alpha_numeric__eight_bit_alpha_numeric)
{
    const segments expected{ { mode::eight_bit, 0, 1 }, { mode::alpha_numeric, 1, 16 } };
    BC_REQUIRE(accessor::split("aABCDEFGHIJKLMNOP", 0, false) == expected);
}

BC_AUTO_TEST_CASE(qr_encoder__split__version_length_bits__expected)
{
    const segments small{ { mode::alpha_numeric, 0, 1 }, { mode::numeric, 1, 8 } };
    const segments large{ { mode::alpha_numeric, 0, 9 } };
    BC_REQUIRE(accessor::split("A12345678", 0, false) == small);
    BC_REQUIRE(accessor::split("A12345678", 27, false) == large);
}

// estimate_bits

BC_AUTO_TEST_CASE(qr_encoder__estimate_bits__segments__expected)
{
    BC_REQUIRE_EQUAL(accessor::estimate_bits({}, 0), 0u);
    BC_REQUIRE_EQUAL(accessor::estimate_bits({ { mode::numeric, 0, 3 } }, 0), 24u);
    BC_REQUIRE_EQUAL(accessor::estimate_bits({ { mode::numeric, 0, 3 } }, 10), 26u);
    BC_REQUIRE_EQUAL(accessor::estimate_bits({ { mode::kanji, 0, 4 } }, 1), 38u);
}

BC_AUTO_TEST_CASE(qr_encoder__estimate_bits__chunked__chunk_headers)
{
    BC_REQUIRE_EQUAL(accessor::estimate_bits({ { mode::eight_bit, 0, 256 } }, 1), 2060u);
    BC_REQUIRE_EQUAL(accessor::estimate_bits({ { mode::eight_bit, 0, 257 } }, 1), 2080u);
}

// stream_bits

BC_AUTO_TEST_CASE(qr_encoder__stream_bits__segments__expected)
{
    BC_REQUIRE_EQUAL(accessor::stream_bits({}, 1), 0u);
    BC_REQUIRE_EQUAL(accessor::stream_bits({ { mode::numeric, 0, 3 } }, 1), 24u);
    BC_REQUIRE_EQUAL(accessor::stream_bits({ { mode::numeric, 0, 3 }, { mode::eight_bit, 3, 1 } }, 1), 44u);
    BC_REQUIRE_EQUAL(accessor::stream_bits({ { mode::kanji, 0, 4 } }, 1), 38u);
}

BC_AUTO_TEST_CASE(qr_encoder__stream_bits__maximum_words__unchunked)
{
    BC_REQUIRE_EQUAL(accessor::stream_bits({ { mode::eight_bit, 0, 255 } }, 1), 2052u);
    BC_REQUIRE_EQUAL(accessor::stream_bits({ { mode::kanji, 0, 510 } }, 1), 3327u);
}

BC_AUTO_TEST_CASE(qr_encoder__stream_bits__excess_words__chunked)
{
    BC_REQUIRE_EQUAL(accessor::stream_bits({ { mode::eight_bit, 0, 256 } }, 1), 2072u);
    BC_REQUIRE_EQUAL(accessor::stream_bits({ { mode::eight_bit, 0, 510 } }, 1), 4104u);
    BC_REQUIRE_EQUAL(accessor::stream_bits({ { mode::kanji, 0, 512 } }, 1), 3352u);
}

// select_version

BC_AUTO_TEST_CASE(qr_encoder__select_version__minimal__one)
{
    BC_REQUIRE_EQUAL(accessor::select_version({ { mode::eight_bit, 0, 1 } }, 0, level::low), 1u);
}

BC_AUTO_TEST_CASE(qr_encoder__select_version__minimum__minimum)
{
    BC_REQUIRE_EQUAL(accessor::select_version({ { mode::eight_bit, 0, 1 } }, 5, level::low), 5u);
}

BC_AUTO_TEST_CASE(qr_encoder__select_version__length_bits_change__expected)
{
    BC_REQUIRE_EQUAL(accessor::select_version({ { mode::eight_bit, 0, 300 } }, 0, level::low), 11u);
}

BC_AUTO_TEST_CASE(qr_encoder__select_version__minimum_length_bits_overflow__increased)
{
    BC_REQUIRE_EQUAL(accessor::split(alternating, 10, true).size(), 46u);
    BC_REQUIRE_EQUAL(accessor::select_version(accessor::split(alternating, 10, true), 0, level::high), 9u);
    BC_REQUIRE_EQUAL(accessor::select_version(accessor::split(alternating, 10, true), 10, level::high), 11u);
}

BC_AUTO_TEST_CASE(qr_encoder__select_version__excessive__maximum)
{
    BC_REQUIRE_EQUAL(accessor::select_version({ { mode::eight_bit, 0, 3000 } }, 0, level::low), 40u);
}

// to_data

BC_AUTO_TEST_CASE(qr_encoder__to_data__excessive__empty)
{
    BC_REQUIRE(accessor::to_data(std::string(20, 'a'), { { mode::eight_bit, 0, 20 } }, 1, level::low).empty());
}

BC_AUTO_TEST_CASE(qr_encoder__to_data__alpha_numeric__expected)
{
    BC_REQUIRE_EQUAL(accessor::to_data("HELLO WORLD", { { mode::alpha_numeric, 0, 11 } }, 1, level::medium), hello_data);
}

BC_AUTO_TEST_CASE(qr_encoder__to_data__numeric__expected)
{
    const data_chunk expected{ 0x10, 0x20, 0x0c, 0x56, 0x61, 0x80, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11 };
    BC_REQUIRE_EQUAL(accessor::to_data("01234567", { { mode::numeric, 0, 8 } }, 1, level::medium), expected);
}

BC_AUTO_TEST_CASE(qr_encoder__to_data__eight_bit__expected)
{
    const data_chunk expected{ 0x40, 0x16, 0x10, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11 };
    BC_REQUIRE_EQUAL(accessor::to_data("a", { { mode::eight_bit, 0, 1 } }, 1, level::low), expected);
}

BC_AUTO_TEST_CASE(qr_encoder__to_data__kanji__expected)
{
    const data_chunk expected{ 0x80, 0x26, 0xcf, 0xea, 0xa8, 0x00, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xec };
    BC_REQUIRE_EQUAL(accessor::to_data(kanji_pair, { { mode::kanji, 0, 4 } }, 1, level::low), expected);
}

BC_AUTO_TEST_CASE(qr_encoder__to_data__short_terminator__unpadded)
{
    const data_chunk expected{ 0x10, 0xa0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    BC_REQUIRE_EQUAL(accessor::to_data(std::string(40, '0'), { { mode::numeric, 0, 40 } }, 1, level::low), expected);
}

BC_AUTO_TEST_CASE(qr_encoder__to_data__full__unterminated)
{
    const auto value = std::string(17, 'a');
    const auto expected = splice(splice(data_chunk{ 0x41 }, data_chunk(17, 0x16)), data_chunk{ 0x10 });
    BC_REQUIRE_EQUAL(accessor::to_data(value, { { mode::eight_bit, 0, 17 } }, 1, level::low), expected);
}

// to_error

BC_AUTO_TEST_CASE(qr_encoder__to_error__hello_world__expected)
{
    BC_REQUIRE_EQUAL(accessor::to_error(hello_data, 10), hello_error);
}

BC_AUTO_TEST_CASE(qr_encoder__to_error__zeros__zeros)
{
    BC_REQUIRE_EQUAL(accessor::to_error(data_chunk(19, 0x00), 7), data_chunk(7, 0x00));
}

// interleave

BC_AUTO_TEST_CASE(qr_encoder__interleave__single_block__data_then_error)
{
    BC_REQUIRE_EQUAL(accessor::interleave(hello_data, 1, level::medium), splice(hello_data, hello_error));
}

BC_AUTO_TEST_CASE(qr_encoder__interleave__unequal_blocks__expected)
{
    // Version 5-Q: two blocks of 15 data codewords, two blocks of 16.
    data_chunk data(62);
    std::iota(data.begin(), data.end(), uint8_t{ 0 });
    const auto out = accessor::interleave(data, 5, level::high);

    BC_REQUIRE_EQUAL(out.size(), 134u);
    BC_REQUIRE_EQUAL(out[0], 0u);
    BC_REQUIRE_EQUAL(out[1], 15u);
    BC_REQUIRE_EQUAL(out[2], 30u);
    BC_REQUIRE_EQUAL(out[3], 46u);
    BC_REQUIRE_EQUAL(out[4], 1u);
    BC_REQUIRE_EQUAL(out[59], 60u);
    BC_REQUIRE_EQUAL(out[60], 45u);
    BC_REQUIRE_EQUAL(out[61], 61u);
    BC_REQUIRE_EQUAL(out[62], accessor::to_error({ data.cbegin(), std::next(data.cbegin(), 15) }, 18).front());
    BC_REQUIRE_EQUAL(out[65], accessor::to_error({ std::next(data.cbegin(), 46), data.cend() }, 18).front());
    BC_REQUIRE_EQUAL(out[133], accessor::to_error({ std::next(data.cbegin(), 46), data.cend() }, 18).back());
}

BC_AUTO_TEST_SUITE_END()
