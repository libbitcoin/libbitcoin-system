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

BOOST_AUTO_TEST_SUITE(qr_matrix_tests)

using namespace bc::system::wallet;

class accessor
  : public qr_matrix
{
public:
    using qr_matrix::to_frame;
    using qr_matrix::format_bits;
    using qr_matrix::version_bits;
    using qr_matrix::place;
    using qr_matrix::is_masked;
    using qr_matrix::apply_mask;
    using qr_matrix::write_format;
    using qr_matrix::penalty;
    using qr_matrix::penalty_runs;
};

using level = qr_matrix::recovery_level;

constexpr uint8_t light = 0x00;
constexpr uint8_t dark = 0x01;
constexpr uint8_t reserved_light = 0x80;
constexpr uint8_t reserved_dark = 0x81;

// "HELLO WORLD" at version 1, recovery level medium.
static const data_chunk hello_codewords{ 0x20, 0x5b, 0x0b, 0x78, 0xd1, 0x72, 0xdc, 0x4d, 0x43, 0x40, 0xec, 0x11, 0xec, 0x11, 0xec, 0x11, 0xc4, 0x23, 0x27, 0x77, 0xeb, 0xd7, 0xe7, 0xe2, 0x5d, 0x17 };

// width

BOOST_AUTO_TEST_CASE(qr_matrix__width__invalid_versions__zero)
{
    BOOST_REQUIRE_EQUAL(qr_matrix::width(0), 0u);
    BOOST_REQUIRE_EQUAL(qr_matrix::width(41), 0u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__width__valid_versions__expected)
{
    BOOST_REQUIRE_EQUAL(qr_matrix::width(1), 21u);
    BOOST_REQUIRE_EQUAL(qr_matrix::width(2), 25u);
    BOOST_REQUIRE_EQUAL(qr_matrix::width(40), 177u);
}

// encode

BOOST_AUTO_TEST_CASE(qr_matrix__encode__invalid_version__empty)
{
    BOOST_REQUIRE(qr_matrix::encode(hello_codewords, 0, level::medium).empty());
    BOOST_REQUIRE(qr_matrix::encode(hello_codewords, 41, level::medium).empty());
}

BOOST_AUTO_TEST_CASE(qr_matrix__encode__codewords_size_mismatch__empty)
{
    BOOST_REQUIRE(qr_matrix::encode({}, 1, level::medium).empty());
    BOOST_REQUIRE(qr_matrix::encode(hello_codewords, 2, level::medium).empty());
}

BOOST_AUTO_TEST_CASE(qr_matrix__encode__hello_world__expected)
{
    const auto modules = qr_matrix::encode(hello_codewords, 1, level::medium);
    BOOST_REQUIRE_EQUAL(modules.size(), 441u);
    BOOST_REQUIRE(std::all_of(modules.cbegin(), modules.cend(), [](uint8_t module) { return module <= dark; }));
    BOOST_REQUIRE_EQUAL(modules[0], dark);
    BOOST_REQUIRE_EQUAL(modules[7], light);
}

// to_frame

BOOST_AUTO_TEST_CASE(qr_matrix__to_frame__version_1__function_patterns)
{
    const auto frame = accessor::to_frame(1);
    BOOST_REQUIRE_EQUAL(frame.size(), 441u);

    // Finders and separators.
    BOOST_REQUIRE_EQUAL(frame[0 * 21 + 0], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[1 * 21 + 1], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[3 * 21 + 3], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[7 * 21 + 7], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[0 * 21 + 20], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[7 * 21 + 13], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[20 * 21 + 0], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[13 * 21 + 7], reserved_light);

    // Format areas, timing and dark module.
    BOOST_REQUIRE_EQUAL(frame[8 * 21 + 8], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[8 * 21 + 20], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[20 * 21 + 8], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[6 * 21 + 8], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[6 * 21 + 9], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[9 * 21 + 6], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[12 * 21 + 6], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[13 * 21 + 8], reserved_dark);

    // Data area.
    BOOST_REQUIRE_EQUAL(frame[20 * 21 + 20], light);
    BOOST_REQUIRE_EQUAL(frame[9 * 21 + 9], light);
    BOOST_REQUIRE_EQUAL(std::count(frame.cbegin(), frame.cend(), light), 208);
}

BOOST_AUTO_TEST_CASE(qr_matrix__to_frame__version_2__alignment_pattern)
{
    const auto frame = accessor::to_frame(2);
    BOOST_REQUIRE_EQUAL(frame.size(), 625u);
    BOOST_REQUIRE_EQUAL(frame[18 * 25 + 18], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[17 * 25 + 18], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[16 * 25 + 16], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[15 * 25 + 15], light);
    BOOST_REQUIRE_EQUAL(std::count(frame.cbegin(), frame.cend(), light), 359);
}

BOOST_AUTO_TEST_CASE(qr_matrix__to_frame__version_7__version_information)
{
    // 0x07c94: bit 0 light, bit 2 dark.
    const auto frame = accessor::to_frame(7);
    BOOST_REQUIRE_EQUAL(frame.size(), 2025u);
    BOOST_REQUIRE_EQUAL(frame[0 * 45 + 34], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[34 * 45 + 0], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[0 * 45 + 36], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[36 * 45 + 0], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[6 * 45 + 22], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[22 * 45 + 22], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[38 * 45 + 38], reserved_dark);
    BOOST_REQUIRE_EQUAL(std::count(frame.cbegin(), frame.cend(), light), 1568);
}

// format_bits

BOOST_AUTO_TEST_CASE(qr_matrix__format_bits__levels_masks__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::format_bits(level::low, 0), 0x77c4u);
    BOOST_REQUIRE_EQUAL(accessor::format_bits(level::low, 7), 0x6976u);
    BOOST_REQUIRE_EQUAL(accessor::format_bits(level::medium, 0), 0x5412u);
    BOOST_REQUIRE_EQUAL(accessor::format_bits(level::medium, 2), 0x5e7cu);
    BOOST_REQUIRE_EQUAL(accessor::format_bits(level::high, 7), 0x2bedu);
    BOOST_REQUIRE_EQUAL(accessor::format_bits(level::highest, 5), 0x0255u);
}

// version_bits

BOOST_AUTO_TEST_CASE(qr_matrix__version_bits__versions__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::version_bits(7), 0x07c94u);
    BOOST_REQUIRE_EQUAL(accessor::version_bits(21), 0x15683u);
    BOOST_REQUIRE_EQUAL(accessor::version_bits(40), 0x28c69u);
}

// place

BOOST_AUTO_TEST_CASE(qr_matrix__place__first_codeword__bottom_right_upward)
{
    auto frame = accessor::to_frame(1);
    accessor::place(frame, 21, { 0xa5 });
    BOOST_REQUIRE_EQUAL(frame[20 * 21 + 20], dark);
    BOOST_REQUIRE_EQUAL(frame[20 * 21 + 19], light);
    BOOST_REQUIRE_EQUAL(frame[19 * 21 + 20], dark);
    BOOST_REQUIRE_EQUAL(frame[19 * 21 + 19], light);
    BOOST_REQUIRE_EQUAL(frame[18 * 21 + 20], light);
    BOOST_REQUIRE_EQUAL(frame[18 * 21 + 19], dark);
    BOOST_REQUIRE_EQUAL(frame[17 * 21 + 20], light);
    BOOST_REQUIRE_EQUAL(frame[17 * 21 + 19], dark);
    BOOST_REQUIRE_EQUAL(frame[16 * 21 + 20], light);
}

BOOST_AUTO_TEST_CASE(qr_matrix__place__all_ones__all_data_modules_dark)
{
    auto frame = accessor::to_frame(1);
    accessor::place(frame, 21, data_chunk(26, 0xff));
    BOOST_REQUIRE_EQUAL(std::count(frame.cbegin(), frame.cend(), dark), 208);
    BOOST_REQUIRE_EQUAL(std::count(frame.cbegin(), frame.cend(), light), 0);
}

BOOST_AUTO_TEST_CASE(qr_matrix__place__column_six__skipped)
{
    auto frame = accessor::to_frame(1);
    accessor::place(frame, 21, data_chunk(26, 0xff));
    BOOST_REQUIRE_EQUAL(frame[9 * 21 + 5], dark);
    BOOST_REQUIRE_EQUAL(frame[9 * 21 + 6], reserved_light);
}

// is_masked

BOOST_AUTO_TEST_CASE(qr_matrix__is_masked__masks__expected)
{
    BOOST_REQUIRE(accessor::is_masked(0, 0, 0));
    BOOST_REQUIRE(!accessor::is_masked(0, 0, 1));
    BOOST_REQUIRE(accessor::is_masked(1, 0, 5));
    BOOST_REQUIRE(!accessor::is_masked(1, 1, 5));
    BOOST_REQUIRE(accessor::is_masked(2, 4, 3));
    BOOST_REQUIRE(!accessor::is_masked(2, 4, 4));
    BOOST_REQUIRE(accessor::is_masked(3, 1, 2));
    BOOST_REQUIRE(!accessor::is_masked(3, 1, 1));
    BOOST_REQUIRE(accessor::is_masked(4, 0, 0));
    BOOST_REQUIRE(!accessor::is_masked(4, 0, 3));
    BOOST_REQUIRE(accessor::is_masked(5, 2, 3));
    BOOST_REQUIRE(!accessor::is_masked(5, 1, 1));
    BOOST_REQUIRE(accessor::is_masked(6, 1, 1));
    BOOST_REQUIRE(!accessor::is_masked(6, 1, 5));
    BOOST_REQUIRE(accessor::is_masked(7, 0, 0));
    BOOST_REQUIRE(!accessor::is_masked(7, 0, 1));
}

// apply_mask

BOOST_AUTO_TEST_CASE(qr_matrix__apply_mask__unreserved__flipped)
{
    data_chunk out{};
    accessor::apply_mask(out, data_chunk(441, light), 21, 1);
    BOOST_REQUIRE_EQUAL(out.size(), 441u);
    BOOST_REQUIRE_EQUAL(out[0], dark);
    BOOST_REQUIRE_EQUAL(out[20], dark);
    BOOST_REQUIRE_EQUAL(out[21], light);
    BOOST_REQUIRE_EQUAL(std::count(out.cbegin(), out.cend(), dark), 231);
}

BOOST_AUTO_TEST_CASE(qr_matrix__apply_mask__reserved__unchanged)
{
    const auto frame = accessor::to_frame(1);
    data_chunk out{};
    accessor::apply_mask(out, frame, 21, 1);
    BOOST_REQUIRE_EQUAL(out[0], reserved_dark);
    BOOST_REQUIRE_EQUAL(out[1 * 21 + 1], reserved_light);
    BOOST_REQUIRE_EQUAL(out[10 * 21 + 10], dark);
    BOOST_REQUIRE_EQUAL(out[11 * 21 + 10], light);
}

// write_format

BOOST_AUTO_TEST_CASE(qr_matrix__write_format__low_mask_0__expected)
{
    // 0x77c4: bits 0 light, 2, 6, 7, 8 and 14 dark.
    data_chunk frame(441, light);
    accessor::write_format(frame, 21, level::low, 0);
    BOOST_REQUIRE_EQUAL(frame[8 * 21 + 20], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[0 * 21 + 8], reserved_light);
    BOOST_REQUIRE_EQUAL(frame[8 * 21 + 18], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[2 * 21 + 8], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[8 * 21 + 8], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[7 * 21 + 8], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[6 * 21 + 8], light);
    BOOST_REQUIRE_EQUAL(frame[14 * 21 + 8], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[8 * 21 + 7], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[8 * 21 + 6], light);
    BOOST_REQUIRE_EQUAL(frame[20 * 21 + 8], reserved_dark);
    BOOST_REQUIRE_EQUAL(frame[8 * 21 + 0], reserved_dark);
    BOOST_REQUIRE_EQUAL(std::count(frame.cbegin(), frame.cend(), light), 441 - 30);
}

// penalty

BOOST_AUTO_TEST_CASE(qr_matrix__penalty__all_light__expected)
{
    // 400 blocks * 3 + 42 runs * (3 + 16).
    BOOST_REQUIRE_EQUAL(accessor::penalty(data_chunk(441, light), 21), 1998u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty__all_dark__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty(data_chunk(441, dark), 21), 1998u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty__checkerboard__zero)
{
    data_chunk modules(9, light);
    modules[1] = modules[3] = modules[5] = modules[7] = dark;
    BOOST_REQUIRE_EQUAL(accessor::penalty(modules, 3), 0u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty__reserved_flags__ignored)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty(data_chunk(441, reserved_light), 21), 1998u);
}

// penalty_runs

BOOST_AUTO_TEST_CASE(qr_matrix__penalty_runs__empty__zero)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({}), 0u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty_runs__long_runs__expected)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 4 }), 0u);
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 5 }), 3u);
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 0, 7, 6 }), 9u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty_runs__leading_finder__finder_penalty)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 0, 1, 1, 3, 1, 1, 4 }), 40u);
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 10, 1, 1, 3, 1, 1 }), 48u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty_runs__preceded_by_light__finder_penalty)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 1, 1, 4, 1, 1, 3, 1, 1, 0 }), 40u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty_runs__trailing_finder__finder_penalty)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 0, 2, 1, 1, 1, 3, 1, 1 }), 40u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty_runs__followed_by_light__finder_penalty)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 0, 2, 1, 1, 1, 3, 1, 1, 4, 1 }), 40u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty_runs__unbounded_finder__zero)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 0, 2, 1, 1, 1, 3, 1, 1, 1, 1 }), 0u);
}

BOOST_AUTO_TEST_CASE(qr_matrix__penalty_runs__unequal_ratio__zero)
{
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 0, 1, 1, 3, 1, 2 }), 0u);
    BOOST_REQUIRE_EQUAL(accessor::penalty_runs({ 0, 1, 1, 4, 1, 1 }), 0u);
}

BOOST_AUTO_TEST_SUITE_END()
