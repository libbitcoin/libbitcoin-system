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
#include <sstream>

BC_AUTO_TEST_SUITE(config_witness_tests)

using namespace bc::system::config;

BC_AUTO_TEST_CASE(witness__construct__default__invalid)
{
    const config::witness instance{};
    BC_REQUIRE(!instance.is_valid());
}

BC_AUTO_TEST_CASE(witness__construct__two_elements__expected)
{
    const config::witness instance("[0102] [abcdef]");
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE_EQUAL(instance.stack().size(), 2u);
    BC_REQUIRE_EQUAL(*instance.stack().front(), base16_chunk("0102"));
    BC_REQUIRE_EQUAL(*instance.stack().back(), base16_chunk("abcdef"));
}

BC_AUTO_TEST_CASE(witness__construct__empty_element__expected)
{
    const config::witness instance("[]");
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE_EQUAL(instance.stack().size(), 1u);
    BC_REQUIRE(instance.stack().front()->empty());
}

BC_AUTO_TEST_CASE(witness__construct__empty_string__valid_empty)
{
    const config::witness instance("");
    BC_REQUIRE(instance.is_valid());
    BC_REQUIRE(instance.stack().empty());
}

BC_AUTO_TEST_CASE(witness__construct__unbracketed__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::witness("0102"), istream_exception);
}

BC_AUTO_TEST_CASE(witness__construct__invalid_base16__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::witness("[0g]"), istream_exception);
}

BC_AUTO_TEST_CASE(witness__construct__chain_witness__expected)
{
    const chain::witness value{ "[0102] [abcdef]" };
    const config::witness instance(value);
    BC_REQUIRE(instance == value);
}

BC_AUTO_TEST_CASE(witness__stream__round_trip__expected)
{
    const auto text = "[0102] [] [abcdef]";
    const config::witness instance(text);

    std::ostringstream output{};
    output << instance;
    BC_REQUIRE_EQUAL(output.str(), text);

    config::witness parsed{};
    std::istringstream(output.str()) >> parsed;
    BC_REQUIRE(parsed == instance);
}

BC_AUTO_TEST_SUITE_END()
