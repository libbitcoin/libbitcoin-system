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

BC_AUTO_TEST_SUITE(token_tests)

using selection = chain::selection;

struct word_vector
{
    selection value;
    std::string_view word;
};

static const std::vector<word_vector> word_vectors
{
    { selection::mainnet, "mainnet" },
    { selection::testnet3, "testnet3" },
    { selection::testnet4, "testnet4" },
    { selection::regtest, "regtest" },
    { selection::signet, "signet" },
    { selection::testnet5, "testnet5" }
};

BC_AUTO_TEST_CASE(token__construct__default__zero_value)
{
    BC_REQUIRE(config::selection{} == selection::none);
}

BC_AUTO_TEST_CASE(token__construct__value__expected)
{
    BC_REQUIRE(config::selection{ selection::regtest } == selection::regtest);
}

BC_AUTO_TEST_CASE(token__construct__string_word__expected)
{
    BC_REQUIRE(config::selection{ "testnet4" } == selection::testnet4);
}

BC_AUTO_TEST_CASE(token__construct__string_empty__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::selection{ "" }, istream_exception);
}

BC_AUTO_TEST_CASE(token__construct__string_unknown__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::selection{ "testnet" }, istream_exception);
}

BC_AUTO_TEST_CASE(token__construct__string_wrong_case__throws_istream_exception)
{
    BC_REQUIRE_THROW(config::selection{ "Mainnet" }, istream_exception);
}

BC_AUTO_TEST_CASE(token__to_word__words__expected)
{
    for (const auto& vector: word_vectors)
    {
        BC_REQUIRE_EQUAL(config::selection::to_word(vector.value), vector.word);
    }
}

BC_AUTO_TEST_CASE(token__to_word__empty_word__empty)
{
    BC_REQUIRE(config::selection::to_word(selection::none).empty());
}

BC_AUTO_TEST_CASE(token__from_word__words__expected)
{
    for (const auto& vector: word_vectors)
    {
        auto out = selection::none;
        BC_REQUIRE(config::selection::from_word(out, vector.word));
        BC_REQUIRE(out == vector.value);
    }
}

BC_AUTO_TEST_CASE(token__from_word__empty__false_unchanged)
{
    auto out = selection::signet;
    BC_REQUIRE(!config::selection::from_word(out, ""));
    BC_REQUIRE(out == selection::signet);
}

BC_AUTO_TEST_CASE(token__from_word__unknown__false_unchanged)
{
    auto out = selection::signet;
    BC_REQUIRE(!config::selection::from_word(out, "mainnet "));
    BC_REQUIRE(out == selection::signet);
}

BC_AUTO_TEST_CASE(token__constexpr__words__expected)
{
    constexpr auto value = []() NOEXCEPT
    {
        auto out = selection::none;
        config::selection::from_word(out, "regtest");
        return out;
    }();

    constexpr auto word = config::selection::to_word(selection::signet);
    BC_REQUIRE(value == selection::regtest);
    BC_REQUIRE_EQUAL(word, "signet");
}

BC_AUTO_TEST_CASE(token__names__selection__parsed_words)
{
    BC_REQUIRE_EQUAL(config::selection::names(), "mainnet|testnet3|testnet4|regtest|signet|testnet5");
}

BC_AUTO_TEST_CASE(token__stream__round_trip__expected)
{
    config::selection instance{};
    std::istringstream("signet") >> instance;
    BC_REQUIRE(instance == selection::signet);

    std::ostringstream output{};
    output << instance;
    BC_REQUIRE_EQUAL(output.str(), "signet");
}

BC_AUTO_TEST_CASE(token__stream__empty_word__empty)
{
    std::ostringstream output{};
    output << config::selection{};
    BC_REQUIRE(output.str().empty());
}

BC_AUTO_TEST_CASE(token__stream__word__expected)
{
    config::selection instance{};
    std::istringstream("testnet3") >> instance;
    BC_REQUIRE(instance == selection::testnet3);
}

BC_AUTO_TEST_CASE(token__stream__unknown__throws_istream_exception)
{
    config::selection instance{};
    BC_REQUIRE_THROW(std::istringstream("testnet") >> instance, istream_exception);
}

BC_AUTO_TEST_SUITE_END()
