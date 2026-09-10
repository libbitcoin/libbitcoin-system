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

using namespace boost;
using namespace bc::system::chain;

BOOST_AUTO_TEST_SUITE(input_json_tests)

BOOST_AUTO_TEST_CASE(input__json__conversions__expected)
{
    const std::string_view text
    {
        "{"
            R"("point":)"
            "{"
                R"("hash":"0000000000000000000000000000000000000000000000000000000000000001",)"
                R"("index":42)"
            "},"
            R"("script":"pick roll return",)"
            R"("witness":"[424242]",)"
            R"("sequence":24)"
        "}"
    };

    const input instance
    {
        point{ one_hash, 42 },
        script
        {
            operations
            {
                { opcode::pick },
                { opcode::roll },
                { opcode::op_return }
            }
        },
        witness{ "[424242]" },
        24
    };

    const auto value = json::value_from(instance);

    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
    BOOST_REQUIRE(json::parse(text) == value);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<input>(value) == instance);
}

BOOST_AUTO_TEST_CASE(input__json__pointer_conversions__expected)
{
    const input expected
    {
        point{ one_hash, 42 },
        script{ operations{ { opcode::pick } } },
        witness{ "[424242]" },
        24
    };

    const input::cptr instance{ to_shared(expected) };
    const auto value = json::value_from(instance);
    BOOST_REQUIRE(value == json::value_from(*instance));
    BOOST_REQUIRE(*json::value_to<input::cptr>(value) == *instance);
}

// A witness-less input serializes without the witness element.
BOOST_AUTO_TEST_CASE(input__json__invalid_witness__no_witness_element)
{
    const std::string_view text
    {
        "{"
            R"("point":)"
            "{"
                R"("hash":"0000000000000000000000000000000000000000000000000000000000000001",)"
                R"("index":42)"
            "},"
            R"("script":"pick",)"
            R"("sequence":24)"
        "}"
    };

    const input instance
    {
        point{ one_hash, 42 },
        script{ operations{ { opcode::pick } } },
        24
    };

    BOOST_REQUIRE(!instance.witness().is_valid());
    const auto value = json::value_from(instance);
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);
}

BOOST_AUTO_TEST_SUITE_END()
