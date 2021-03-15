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

BOOST_AUTO_TEST_SUITE_END()
