/**
 * Copyright (c) 2011-2018 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(ec_point_tests)

#define POINTX "0245dbb7e2cd3a5de19fde8d556fd567a036f9c377ecf69a9202aa4affce41c623"
#define POINTY "02cfc43e064c50cfd1896766ef70e7da82b16e8cfebd8d5dec618212d0db1e6d12"
#define SUM "03332bf6821c7c0e1080efc131d2b745760a8245c0b91a05f13308ff8600d30525"

BOOST_AUTO_TEST_CASE(ec_point__sum__valid__expected)
{
    const ec_point x{ base16_literal(POINTX) };
    const ec_point y{ base16_literal(POINTY) };
    const auto sum = x + y;
    BOOST_REQUIRE(sum);
    BOOST_REQUIRE_EQUAL(encode_base16(sum.point()), SUM);
}

BOOST_AUTO_TEST_SUITE_END()
