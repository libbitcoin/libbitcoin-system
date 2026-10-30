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

BOOST_AUTO_TEST_SUITE(base85_tests)

using namespace bc::system::config;

BOOST_AUTO_TEST_CASE(base85__option_value__empty_token__empty)
{
    std::any value{};
    const std::unique_ptr<const options::value_semantic> semantic{ options::value<base85>() };
    semantic->parse(value, string_list{ "" }, "base85");
    BOOST_REQUIRE(std::any_cast<base85>(value).to_string().empty());
}

BOOST_AUTO_TEST_CASE(base85__option_value__multiple_tokens__throws)
{
    std::any value{};
    const std::unique_ptr<const options::value_semantic> semantic{ options::value<base85>() };
    BOOST_REQUIRE_THROW(semantic->parse(value, string_list{ "foo", "bar" }, "base85"), validation_exception);
}

BOOST_AUTO_TEST_CASE(base85__construct__default__empty_and_aligned)
{
    const config::base85 instance{};
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE((const data_chunk&)instance == data_chunk{});
}

BOOST_AUTO_TEST_CASE(base85__construct__copy_chunk__expected)
{
    const data_chunk value{ 0x01, 0x02, 0x03, 0x04 };
    const config::base85 instance(value);
    BOOST_REQUIRE(instance);
    BOOST_REQUIRE((const data_chunk&)instance == value);
}

BOOST_AUTO_TEST_CASE(base85__construct__move_chunk__expected)
{
    data_chunk value{ 0x01, 0x02, 0x03, 0x04 };
    const data_chunk expected{ 0x01, 0x02, 0x03, 0x04 };
    const config::base85 instance(std::move(value));
    BOOST_REQUIRE((const data_chunk&)instance == expected);
}

BOOST_AUTO_TEST_CASE(base85__construct__unaligned_chunk__false)
{
    const config::base85 instance(data_chunk{ 0x01, 0x02, 0x03 });
    BOOST_REQUIRE(!instance);
}

BOOST_AUTO_TEST_CASE(base85__construct__string__round_trips)
{
    const data_chunk value{ 0x01, 0x02, 0x03, 0x04 };
    const config::base85 instance(value);
    const config::base85 parsed(instance.to_string());
    BOOST_REQUIRE((const data_chunk&)parsed == value);
}

BOOST_AUTO_TEST_CASE(base85__construct__invalid_string__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(config::base85("\x01\x01\x01\x01\x01"), istream_exception);
}

BOOST_AUTO_TEST_CASE(base85__construct__unaligned_string__throws_istream_exception)
{
    BOOST_REQUIRE_THROW(config::base85("abc"), istream_exception);
}

BOOST_AUTO_TEST_CASE(base85__stream_out__unaligned__throws_ostream_exception)
{
    const config::base85 instance(data_chunk{ 0x01, 0x02, 0x03 });
    std::ostringstream output{};
    BOOST_REQUIRE_THROW(output << instance, ostream_exception);
}

BOOST_AUTO_TEST_SUITE_END()
