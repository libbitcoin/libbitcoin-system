/**
 * Copyright (c) 2011-2026 libbitcoin developers (see AUTHORS)
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

using namespace boost;
using namespace bc::system::chain;

BOOST_AUTO_TEST_SUITE(witness_tests)

BOOST_AUTO_TEST_CASE(witness___json__conversions__expected)
{
    const std::string_view text
    {
        R"("[424344] [454647] [48494a]")"
    };

    const chain::witness instance
    {
        data_stack
        {
            { 0x42, 0x43, 0x44 },
            { 0x45, 0x46, 0x47 },
            { 0x48, 0x49, 0x4a }
        }
    };

    const auto value = json::value_from(instance);

    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
    BOOST_REQUIRE(json::parse(text) == value);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<chain::witness>(value) == instance);
}

BOOST_AUTO_TEST_SUITE_END()
