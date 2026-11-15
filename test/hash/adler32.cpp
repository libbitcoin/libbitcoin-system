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
#include "../test.hpp"

BC_AUTO_TEST_SUITE(adler32_tests)

struct adler32_vector
{
    std::string text;
    uint32_t adler;
};

const std_vector<adler32_vector> adler32_vectors
{
    { "", 0x00000001 },
    { "a", 0x00620062 },
    { "abc", 0x024d0127 },
    { "Wikipedia", 0x11e60398 }
};

BC_AUTO_TEST_CASE(adler32__text__vectors__expected)
{
    for (const auto& vector: adler32_vectors)
    {
        const data_chunk data(vector.text.begin(), vector.text.end());
        BC_REQUIRE_EQUAL(adler32(data), vector.adler);
    }
}

BC_AUTO_TEST_CASE(adler32__ones__beyond_reduction__expected)
{
    const data_chunk ones(6000, 0xff);
    BC_REQUIRE_EQUAL(adler32(ones), 0xa49759ea_u32);
}

BC_AUTO_TEST_SUITE_END()
