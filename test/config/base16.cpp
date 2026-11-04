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

BC_AUTO_TEST_SUITE(base16_tests)

using namespace bc::system::config;

BC_AUTO_TEST_CASE(base16__construct__valid_string__decodes)
{
    const data_chunk expected{ 0x01, 0x02, 0xab, 0xff };
    const config::base16 instance("0102abff");
    BC_REQUIRE_EQUAL((const data_chunk&)instance, expected);
}

BC_AUTO_TEST_CASE(base16__construct__empty_string__empty)
{
    const config::base16 instance("");
    BC_REQUIRE(((const data_chunk&)instance).empty());
}

BC_AUTO_TEST_CASE(base16__construct__invalid_string__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::base16("0g"), istream_exception);
}

BC_AUTO_TEST_CASE(base16__construct__odd_string__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::base16("012"), istream_exception);
}

BC_AUTO_TEST_CASE(base16__stream__round_trip__expected)
{
    const data_chunk value{ 0x01, 0x02, 0x03, 0x04 };
    const config::base16 instance(value);

    std::ostringstream output{};
    output << instance;

    config::base16 parsed{};
    std::istringstream input(output.str());
    input >> parsed;
    BC_REQUIRE(!input.fail());
    BC_REQUIRE_EQUAL((const data_chunk&)parsed, value);
}

BC_AUTO_TEST_CASE(base16__stream__empty__empty)
{
    config::base16 parsed{ data_chunk{ 0x42 } };
    std::istringstream input{};
    input >> parsed;
    BC_REQUIRE(!input.fail());
    BC_REQUIRE(((const data_chunk&)parsed).empty());
}

BC_AUTO_TEST_CASE(base16__stream__failed__fails)
{
    config::base16 parsed{};
    std::istringstream input("0102");
    input.setstate(std::ios::failbit);
    input >> parsed;
    BC_REQUIRE(input.fail());
}

BC_AUTO_TEST_CASE(base16__deserialize__empty__true_empty)
{
    config::base16 parsed{ data_chunk{ 0x42 } };
    BC_REQUIRE(deserialize(parsed, std::string_view{ "" }));
    BC_REQUIRE(((const data_chunk&)parsed).empty());
}

BC_AUTO_TEST_CASE(base16__deserialize__whitespace__true_empty)
{
    config::base16 parsed{ data_chunk{ 0x42 } };
    BC_REQUIRE(deserialize(parsed, std::string_view{ "\n" }));
    BC_REQUIRE(((const data_chunk&)parsed).empty());
}

BC_AUTO_TEST_SUITE_END()
