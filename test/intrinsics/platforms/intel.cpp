/**
 * Copyright (c) 2011-2025 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(intrinsics_platforms_intel_tests)

#if defined(HAVE_SSE4)

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm_srli_epi8__expected)
{
    const data_array<16> expected
    {
        0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20,
        0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x3f
    };
    const auto instance = _mm_setr_epi8
    (
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8
    );

    data_array<16> result{};
    const auto shifted = mm_srli_epi8<2>(instance);
    _mm_storeu_si128(pointer_cast<xint128_t>(result.data()), shifted);

    BOOST_CHECK_EQUAL(result[0], expected[0]);
    BOOST_CHECK_EQUAL(result[1], expected[1]);
    BOOST_CHECK_EQUAL(result[2], expected[2]);
    BOOST_CHECK_EQUAL(result[3], expected[3]);
    BOOST_CHECK_EQUAL(result[4], expected[4]);
    BOOST_CHECK_EQUAL(result[5], expected[5]);
    BOOST_CHECK_EQUAL(result[6], expected[6]);
    BOOST_CHECK_EQUAL(result[7], expected[7]);
    BOOST_CHECK_EQUAL(result[8], expected[8]);
    BOOST_CHECK_EQUAL(result[9], expected[9]);
    BOOST_CHECK_EQUAL(result[10], expected[10]);
    BOOST_CHECK_EQUAL(result[11], expected[11]);
    BOOST_CHECK_EQUAL(result[12], expected[12]);
    BOOST_CHECK_EQUAL(result[13], expected[13]);
    BOOST_CHECK_EQUAL(result[14], expected[14]);
    BOOST_CHECK_EQUAL(result[15], expected[15]);
}

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm_slli_epi8__expected)
{
    const data_array<16> expected
    {
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0x00,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0xfc
    };
    const auto instance = _mm_setr_epi8
    (
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8
    );

    data_array<16> result{};
    const auto shifted = mm_slli_epi8<2>(instance);
    _mm_storeu_si128(pointer_cast<xint128_t>(result.data()), shifted);

    BOOST_CHECK_EQUAL(result[0], expected[0]);
    BOOST_CHECK_EQUAL(result[1], expected[1]);
    BOOST_CHECK_EQUAL(result[2], expected[2]);
    BOOST_CHECK_EQUAL(result[3], expected[3]);
    BOOST_CHECK_EQUAL(result[4], expected[4]);
    BOOST_CHECK_EQUAL(result[5], expected[5]);
    BOOST_CHECK_EQUAL(result[6], expected[6]);
    BOOST_CHECK_EQUAL(result[7], expected[7]);
    BOOST_CHECK_EQUAL(result[8], expected[8]);
    BOOST_CHECK_EQUAL(result[9], expected[9]);
    BOOST_CHECK_EQUAL(result[10], expected[10]);
    BOOST_CHECK_EQUAL(result[11], expected[11]);
    BOOST_CHECK_EQUAL(result[12], expected[12]);
    BOOST_CHECK_EQUAL(result[13], expected[13]);
    BOOST_CHECK_EQUAL(result[14], expected[14]);
    BOOST_CHECK_EQUAL(result[15], expected[15]);
}

#endif // HAVE_SSE4

#if defined(HAVE_AVX2)

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm256_srli_epi8__expected)
{
    const data_array<32> expected
    {
        0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20,
        0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x3f,
        0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20,
        0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x3f
    };
    const auto instance = _mm256_setr_epi8
    (
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8,
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8
    );

    data_array<32> result{};
    const auto shifted = mm256_srli_epi8<2>(instance);
    _mm256_storeu_si256(pointer_cast<xint256_t>(result.data()), shifted);

    BOOST_CHECK_EQUAL(result[0], expected[0]);
    BOOST_CHECK_EQUAL(result[1], expected[1]);
    BOOST_CHECK_EQUAL(result[2], expected[2]);
    BOOST_CHECK_EQUAL(result[3], expected[3]);
    BOOST_CHECK_EQUAL(result[4], expected[4]);
    BOOST_CHECK_EQUAL(result[5], expected[5]);
    BOOST_CHECK_EQUAL(result[6], expected[6]);
    BOOST_CHECK_EQUAL(result[7], expected[7]);
    BOOST_CHECK_EQUAL(result[8], expected[8]);
    BOOST_CHECK_EQUAL(result[9], expected[9]);
    BOOST_CHECK_EQUAL(result[10], expected[10]);
    BOOST_CHECK_EQUAL(result[11], expected[11]);
    BOOST_CHECK_EQUAL(result[12], expected[12]);
    BOOST_CHECK_EQUAL(result[13], expected[13]);
    BOOST_CHECK_EQUAL(result[14], expected[14]);
    BOOST_CHECK_EQUAL(result[15], expected[15]);
    BOOST_CHECK_EQUAL(result[16], expected[16]);
    BOOST_CHECK_EQUAL(result[17], expected[17]);
    BOOST_CHECK_EQUAL(result[18], expected[18]);
    BOOST_CHECK_EQUAL(result[19], expected[19]);
    BOOST_CHECK_EQUAL(result[20], expected[20]);
    BOOST_CHECK_EQUAL(result[21], expected[21]);
    BOOST_CHECK_EQUAL(result[22], expected[22]);
    BOOST_CHECK_EQUAL(result[23], expected[23]);
    BOOST_CHECK_EQUAL(result[24], expected[24]);
    BOOST_CHECK_EQUAL(result[25], expected[25]);
    BOOST_CHECK_EQUAL(result[26], expected[26]);
    BOOST_CHECK_EQUAL(result[27], expected[27]);
    BOOST_CHECK_EQUAL(result[28], expected[28]);
    BOOST_CHECK_EQUAL(result[29], expected[29]);
    BOOST_CHECK_EQUAL(result[30], expected[30]);
    BOOST_CHECK_EQUAL(result[31], expected[31]);
}

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm256_slli_epi8__expected)
{
    const data_array<32> expected
    {
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0x00,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0xfc,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0x00,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0xfc
    };
    const auto instance = _mm256_setr_epi8
    (
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8,
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8
    );

    data_array<32> result{};
    const auto shifted = mm256_slli_epi8<2>(instance);
    _mm256_storeu_si256(pointer_cast<xint256_t>(result.data()), shifted);

    BOOST_CHECK_EQUAL(result[0], expected[0]);
    BOOST_CHECK_EQUAL(result[1], expected[1]);
    BOOST_CHECK_EQUAL(result[2], expected[2]);
    BOOST_CHECK_EQUAL(result[3], expected[3]);
    BOOST_CHECK_EQUAL(result[4], expected[4]);
    BOOST_CHECK_EQUAL(result[5], expected[5]);
    BOOST_CHECK_EQUAL(result[6], expected[6]);
    BOOST_CHECK_EQUAL(result[7], expected[7]);
    BOOST_CHECK_EQUAL(result[8], expected[8]);
    BOOST_CHECK_EQUAL(result[9], expected[9]);
    BOOST_CHECK_EQUAL(result[10], expected[10]);
    BOOST_CHECK_EQUAL(result[11], expected[11]);
    BOOST_CHECK_EQUAL(result[12], expected[12]);
    BOOST_CHECK_EQUAL(result[13], expected[13]);
    BOOST_CHECK_EQUAL(result[14], expected[14]);
    BOOST_CHECK_EQUAL(result[15], expected[15]);
    BOOST_CHECK_EQUAL(result[16], expected[16]);
    BOOST_CHECK_EQUAL(result[17], expected[17]);
    BOOST_CHECK_EQUAL(result[18], expected[18]);
    BOOST_CHECK_EQUAL(result[19], expected[19]);
    BOOST_CHECK_EQUAL(result[20], expected[20]);
    BOOST_CHECK_EQUAL(result[21], expected[21]);
    BOOST_CHECK_EQUAL(result[22], expected[22]);
    BOOST_CHECK_EQUAL(result[23], expected[23]);
    BOOST_CHECK_EQUAL(result[24], expected[24]);
    BOOST_CHECK_EQUAL(result[25], expected[25]);
    BOOST_CHECK_EQUAL(result[26], expected[26]);
    BOOST_CHECK_EQUAL(result[27], expected[27]);
    BOOST_CHECK_EQUAL(result[28], expected[28]);
    BOOST_CHECK_EQUAL(result[29], expected[29]);
    BOOST_CHECK_EQUAL(result[30], expected[30]);
    BOOST_CHECK_EQUAL(result[31], expected[31]);
}

#endif // HAVE_AVX2

#if defined(HAVE_AVX512)

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm512_srli_epi8__expected)
{
    const data_array<64> expected
    {
        0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20,
        0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x3f,
        0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20,
        0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x3f,
        0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20,
        0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x3f,
        0x04, 0x08, 0x0c, 0x10, 0x14, 0x18, 0x1c, 0x20,
        0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x3f
    };
    const auto instance = _mm512_setr_epi8
    (
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8,
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8,
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8,
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8
    );

    data_array<64> result{};
    const auto shifted = mm512_srli_epi8<2>(instance);
    _mm512_storeu_si512(pointer_cast<xint512_t>(result.data()), shifted);

    BOOST_CHECK_EQUAL(result[0], expected[0]);
    BOOST_CHECK_EQUAL(result[1], expected[1]);
    BOOST_CHECK_EQUAL(result[2], expected[2]);
    BOOST_CHECK_EQUAL(result[3], expected[3]);
    BOOST_CHECK_EQUAL(result[4], expected[4]);
    BOOST_CHECK_EQUAL(result[5], expected[5]);
    BOOST_CHECK_EQUAL(result[6], expected[6]);
    BOOST_CHECK_EQUAL(result[7], expected[7]);
    BOOST_CHECK_EQUAL(result[8], expected[8]);
    BOOST_CHECK_EQUAL(result[9], expected[9]);
    BOOST_CHECK_EQUAL(result[10], expected[10]);
    BOOST_CHECK_EQUAL(result[11], expected[11]);
    BOOST_CHECK_EQUAL(result[12], expected[12]);
    BOOST_CHECK_EQUAL(result[13], expected[13]);
    BOOST_CHECK_EQUAL(result[14], expected[14]);
    BOOST_CHECK_EQUAL(result[15], expected[15]);
    BOOST_CHECK_EQUAL(result[16], expected[16]);
    BOOST_CHECK_EQUAL(result[17], expected[17]);
    BOOST_CHECK_EQUAL(result[18], expected[18]);
    BOOST_CHECK_EQUAL(result[19], expected[19]);
    BOOST_CHECK_EQUAL(result[20], expected[20]);
    BOOST_CHECK_EQUAL(result[21], expected[21]);
    BOOST_CHECK_EQUAL(result[22], expected[22]);
    BOOST_CHECK_EQUAL(result[23], expected[23]);
    BOOST_CHECK_EQUAL(result[24], expected[24]);
    BOOST_CHECK_EQUAL(result[25], expected[25]);
    BOOST_CHECK_EQUAL(result[26], expected[26]);
    BOOST_CHECK_EQUAL(result[27], expected[27]);
    BOOST_CHECK_EQUAL(result[28], expected[28]);
    BOOST_CHECK_EQUAL(result[29], expected[29]);
    BOOST_CHECK_EQUAL(result[30], expected[30]);
    BOOST_CHECK_EQUAL(result[31], expected[31]);
    BOOST_CHECK_EQUAL(result[32], expected[32]);
    BOOST_CHECK_EQUAL(result[33], expected[33]);
    BOOST_CHECK_EQUAL(result[34], expected[34]);
    BOOST_CHECK_EQUAL(result[35], expected[35]);
    BOOST_CHECK_EQUAL(result[36], expected[36]);
    BOOST_CHECK_EQUAL(result[37], expected[37]);
    BOOST_CHECK_EQUAL(result[38], expected[38]);
    BOOST_CHECK_EQUAL(result[39], expected[39]);
    BOOST_CHECK_EQUAL(result[40], expected[40]);
    BOOST_CHECK_EQUAL(result[41], expected[41]);
    BOOST_CHECK_EQUAL(result[42], expected[42]);
    BOOST_CHECK_EQUAL(result[43], expected[43]);
    BOOST_CHECK_EQUAL(result[44], expected[44]);
    BOOST_CHECK_EQUAL(result[45], expected[45]);
    BOOST_CHECK_EQUAL(result[46], expected[46]);
    BOOST_CHECK_EQUAL(result[47], expected[47]);
    BOOST_CHECK_EQUAL(result[48], expected[48]);
    BOOST_CHECK_EQUAL(result[49], expected[49]);
    BOOST_CHECK_EQUAL(result[50], expected[50]);
    BOOST_CHECK_EQUAL(result[51], expected[51]);
    BOOST_CHECK_EQUAL(result[52], expected[52]);
    BOOST_CHECK_EQUAL(result[53], expected[53]);
    BOOST_CHECK_EQUAL(result[54], expected[54]);
    BOOST_CHECK_EQUAL(result[55], expected[55]);
    BOOST_CHECK_EQUAL(result[56], expected[56]);
    BOOST_CHECK_EQUAL(result[57], expected[57]);
    BOOST_CHECK_EQUAL(result[58], expected[58]);
    BOOST_CHECK_EQUAL(result[59], expected[59]);
    BOOST_CHECK_EQUAL(result[60], expected[60]);
    BOOST_CHECK_EQUAL(result[61], expected[61]);
    BOOST_CHECK_EQUAL(result[62], expected[62]);
    BOOST_CHECK_EQUAL(result[63], expected[63]);
}

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm512_slli_epi8__expected)
{
    const data_array<64> expected
    {
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0x00,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0xfc,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0x00,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0xfc,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0x00,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0xfc,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0x00,
        0x40, 0x80, 0xc0, 0x00, 0x40, 0x80, 0xc0, 0xfc
    };
    const auto instance = _mm512_setr_epi8
    (
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8,
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8,
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8,
        0x10_u8, 0x20_u8, 0x30_u8, 0x40_u8, 0x50_u8, 0x60_u8, 0x70_u8, 0x80_u8,
        0x90_u8, 0xa0_u8, 0xb0_u8, 0xc0_u8, 0xd0_u8, 0xe0_u8, 0xf0_u8, 0xff_u8
    );

    data_array<64> result{};
    const auto shifted = mm512_slli_epi8<2>(instance);
    _mm512_storeu_si512(pointer_cast<xint512_t>(result.data()), shifted);

    BOOST_CHECK_EQUAL(result[0], expected[0]);
    BOOST_CHECK_EQUAL(result[1], expected[1]);
    BOOST_CHECK_EQUAL(result[2], expected[2]);
    BOOST_CHECK_EQUAL(result[3], expected[3]);
    BOOST_CHECK_EQUAL(result[4], expected[4]);
    BOOST_CHECK_EQUAL(result[5], expected[5]);
    BOOST_CHECK_EQUAL(result[6], expected[6]);
    BOOST_CHECK_EQUAL(result[7], expected[7]);
    BOOST_CHECK_EQUAL(result[8], expected[8]);
    BOOST_CHECK_EQUAL(result[9], expected[9]);
    BOOST_CHECK_EQUAL(result[10], expected[10]);
    BOOST_CHECK_EQUAL(result[11], expected[11]);
    BOOST_CHECK_EQUAL(result[12], expected[12]);
    BOOST_CHECK_EQUAL(result[13], expected[13]);
    BOOST_CHECK_EQUAL(result[14], expected[14]);
    BOOST_CHECK_EQUAL(result[15], expected[15]);
    BOOST_CHECK_EQUAL(result[16], expected[16]);
    BOOST_CHECK_EQUAL(result[17], expected[17]);
    BOOST_CHECK_EQUAL(result[18], expected[18]);
    BOOST_CHECK_EQUAL(result[19], expected[19]);
    BOOST_CHECK_EQUAL(result[20], expected[20]);
    BOOST_CHECK_EQUAL(result[21], expected[21]);
    BOOST_CHECK_EQUAL(result[22], expected[22]);
    BOOST_CHECK_EQUAL(result[23], expected[23]);
    BOOST_CHECK_EQUAL(result[24], expected[24]);
    BOOST_CHECK_EQUAL(result[25], expected[25]);
    BOOST_CHECK_EQUAL(result[26], expected[26]);
    BOOST_CHECK_EQUAL(result[27], expected[27]);
    BOOST_CHECK_EQUAL(result[28], expected[28]);
    BOOST_CHECK_EQUAL(result[29], expected[29]);
    BOOST_CHECK_EQUAL(result[30], expected[30]);
    BOOST_CHECK_EQUAL(result[31], expected[31]);
    BOOST_CHECK_EQUAL(result[32], expected[32]);
    BOOST_CHECK_EQUAL(result[33], expected[33]);
    BOOST_CHECK_EQUAL(result[34], expected[34]);
    BOOST_CHECK_EQUAL(result[35], expected[35]);
    BOOST_CHECK_EQUAL(result[36], expected[36]);
    BOOST_CHECK_EQUAL(result[37], expected[37]);
    BOOST_CHECK_EQUAL(result[38], expected[38]);
    BOOST_CHECK_EQUAL(result[39], expected[39]);
    BOOST_CHECK_EQUAL(result[40], expected[40]);
    BOOST_CHECK_EQUAL(result[41], expected[41]);
    BOOST_CHECK_EQUAL(result[42], expected[42]);
    BOOST_CHECK_EQUAL(result[43], expected[43]);
    BOOST_CHECK_EQUAL(result[44], expected[44]);
    BOOST_CHECK_EQUAL(result[45], expected[45]);
    BOOST_CHECK_EQUAL(result[46], expected[46]);
    BOOST_CHECK_EQUAL(result[47], expected[47]);
    BOOST_CHECK_EQUAL(result[48], expected[48]);
    BOOST_CHECK_EQUAL(result[49], expected[49]);
    BOOST_CHECK_EQUAL(result[50], expected[50]);
    BOOST_CHECK_EQUAL(result[51], expected[51]);
    BOOST_CHECK_EQUAL(result[52], expected[52]);
    BOOST_CHECK_EQUAL(result[53], expected[53]);
    BOOST_CHECK_EQUAL(result[54], expected[54]);
    BOOST_CHECK_EQUAL(result[55], expected[55]);
    BOOST_CHECK_EQUAL(result[56], expected[56]);
    BOOST_CHECK_EQUAL(result[57], expected[57]);
    BOOST_CHECK_EQUAL(result[58], expected[58]);
    BOOST_CHECK_EQUAL(result[59], expected[59]);
    BOOST_CHECK_EQUAL(result[60], expected[60]);
    BOOST_CHECK_EQUAL(result[61], expected[61]);
    BOOST_CHECK_EQUAL(result[62], expected[62]);
    BOOST_CHECK_EQUAL(result[63], expected[63]);
}

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm512_extract_epi8__expected)
{
    constexpr auto expected = 0x02_u8;
    const auto instance = _mm512_setr_epi8
    (
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f,
        0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
        0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
    );
    BOOST_CHECK_EQUAL(mm512_extract_epi8<2>(instance), expected);
}

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm512_extract_epi16__expected)
{
    constexpr auto expected = 0x0302_u16;
    const auto instance = _mm512_setr_epi16
    (
        0x0100_u16, 0x0302_u16, 0x0504_u16, 0x0706_u16,
        0x0908_u16, 0x0b0a_u16, 0x0d0c_u16, 0x0f0e_u16,
        0x1110_u16, 0x1312_u16, 0x1514_u16, 0x1716_u16,
        0x1918_u16, 0x1b1a_u16, 0x1d1c_u16, 0x1f1e_u16,
        0x2120_u16, 0x2322_u16, 0x2524_u16, 0x2726_u16,
        0x2928_u16, 0x2b2a_u16, 0x2d2c_u16, 0x2f2e_u16,
        0x3130_u16, 0x3332_u16, 0x3534_u16, 0x3736_u16,
        0x3938_u16, 0x3b3a_u16, 0x3d3c_u16, 0x3f3e_u16
    );
    BOOST_CHECK_EQUAL(mm512_extract_epi16<1>(instance), expected);
}

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm512_extract_epi32__expected)
{
    constexpr auto expected = 0x07060504_u32;
    const auto instance = _mm512_setr_epi32
    (
        0x03020100_u32, 0x07060504_u32, 0x0b0a0908_u32, 0x0f0e0d0c_u32,
        0x13121110_u32, 0x17161514_u32, 0x1b1a1918_u32, 0x1f1e1d1c_u32,
        0x23222120_u32, 0x27262524_u32, 0x2b2a2928_u32, 0x2f2e2d2c_u32,
        0x33323130_u32, 0x37363534_u32, 0x3b3a3938_u32, 0x3f3e3d3c_u32
    );
    BOOST_CHECK_EQUAL(mm512_extract_epi32<1>(instance), expected);
}

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm512_extract_epi64__expected)
{
    constexpr auto expected = 0x0f0e0d0c0b0a0908_u64;
    const auto instance = _mm512_setr_epi64
    (
        0x0706050403020100_u64, 0x0f0e0d0c0b0a0908_u64,
        0x1716151413121110_u64, 0x1f1e1d1c1b1a1918_u64,
        0x2726252423222120_u64, 0x2f2e2d2c2b2a2928_u64,
        0x3736353433323130_u64, 0x3f3e3d3c3b3a3938_u64
    );
    BOOST_CHECK_EQUAL(mm512_extract_epi64<1>(instance), expected);
}

#endif // HAVE_AVX512

#if defined(HAVE_SSE4)

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm_extract_epi64__expected)
{
    constexpr auto expected = 0x0f0e0d0c0b0a0908_u64;
    const auto instance = _mm_setr_epi8
    (
        0x00_u8, 0x01_u8, 0x02_u8, 0x03_u8, 0x04_u8, 0x05_u8, 0x06_u8, 0x07_u8,
        0x08_u8, 0x09_u8, 0x0A_u8, 0x0B_u8, 0x0C_u8, 0x0D_u8, 0x0E_u8, 0x0F_u8
    );
    BOOST_CHECK_EQUAL(mm_extract_epi64<1>(instance), expected);
}

#endif

#if defined(HAVE_AVX2)

BOOST_AUTO_TEST_CASE(intrinsics__intel__mm256_extract_epi64__expected)
{
    constexpr auto expected = 0x0f0e0d0c0b0a0908_u64;
    const auto instance = _mm256_setr_epi8
    (
        0x00_u8, 0x01_u8, 0x02_u8, 0x03_u8, 0x04_u8, 0x05_u8, 0x06_u8, 0x07_u8,
        0x08_u8, 0x09_u8, 0x0A_u8, 0x0B_u8, 0x0C_u8, 0x0D_u8, 0x0E_u8, 0x0F_u8,
        0x10_u8, 0x11_u8, 0x12_u8, 0x13_u8, 0x14_u8, 0x15_u8, 0x16_u8, 0x17_u8,
        0x18_u8, 0x19_u8, 0x1A_u8, 0x1B_u8, 0x1C_u8, 0x1D_u8, 0x1E_u8, 0x1F_u8
    );
    BOOST_CHECK_EQUAL(mm256_extract_epi64<1>(instance), expected);
}

#endif

BOOST_AUTO_TEST_SUITE_END()
