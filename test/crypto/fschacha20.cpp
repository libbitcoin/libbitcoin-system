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

BOOST_AUTO_TEST_SUITE(fschacha20_tests)

BOOST_AUTO_TEST_CASE(fschacha20__crypt__rekey_rotation__expected)
{
    struct fschacha20_vector
    {
        data_chunk plain;
        chacha20::secret key;
        uint32_t interval;
        data_chunk cipher;
    };

    const std_vector<fschacha20_vector> vectors
    {
        {
            base16_chunk("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"),
            base16_array("0000000000000000000000000000000000000000000000000000000000000000"),
            256,
            base16_chunk("a93df4ef03011f3db95f60d996e1785df5de38fc39bfcb663a47bb5561928349")
        },
        {
            base16_chunk("01"),
            base16_array("000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f"),
            5,
            base16_chunk("ea")
        },
        {
            base16_chunk("e93fdb5c762804b9a706816aca31e35b11d2aa3080108ef46a5b1f1508819c0a"),
            base16_array("8ec4c3ccdaea336bdeb245636970be01266509b33f3d2642504eaf412206207a"),
            4096,
            base16_chunk("8bfaa4eacff308fdb4a94a5ff25bd9d0c1f84b77f81239f67ff39d6e1ac280c9")
        }
    };

    for (const auto& vector: vectors)
    {
        fschacha20 cipher{ vector.key, vector.interval };
        data_chunk out(vector.plain.size());

        // Crypt through one full rekey interval, then compare the next.
        for (uint32_t chunk{}; chunk < vector.interval; ++chunk)
            cipher.crypt(vector.plain, out);

        cipher.crypt(vector.plain, out);
        BOOST_REQUIRE_EQUAL(out, vector.cipher);
    }
}

BOOST_AUTO_TEST_SUITE_END()
