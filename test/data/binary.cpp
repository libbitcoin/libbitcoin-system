/**
 * Copyright (c) 2011-2021 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(binary_tests)

// TODO: test:
// construct(default)
// construct(move/copy)
// assign (move/copy)
// bytes
// >>
// []
// <
// ==/!=

BOOST_AUTO_TEST_CASE(binary__construct__bytes_32__expected_encoded)
{
    const data_chunk blocks{ { 0xba, 0xad, 0xf0, 0x0d } };
    const binary prefix(32, blocks);
    BOOST_REQUIRE_EQUAL(prefix.encoded(), "10111010101011011111000000001101");
}

BOOST_AUTO_TEST_CASE(binary__construct__bytes_32__expected_data)
{
    const data_chunk data{ { 0xba, 0xad, 0xf0, 0x0d } };
    const binary prefix(32, data);
    BOOST_REQUIRE(prefix.data() == data);
}

BOOST_AUTO_TEST_CASE(binary__construct__bytes_32__expected_out)
{
    const data_chunk blocks{ { 0xba, 0xad, 0xf0, 0x0d } };
    const binary prefix(32, blocks);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(stream.str(), "10111010101011011111000000001101");
}

BOOST_AUTO_TEST_CASE(binary__construct__uint32_8__expected_encoded)
{
    const binary prefix(8, uint32_t(0x0df0adba));
    BOOST_REQUIRE_EQUAL(prefix.encoded(), "10111010");
}

BOOST_AUTO_TEST_CASE(binary__construct__uint32_32__expected_encoded)
{
    const binary prefix(32, uint32_t(0x0df0adba));
    BOOST_REQUIRE_EQUAL(prefix.encoded(), "10111010101011011111000000001101");
}

BOOST_AUTO_TEST_CASE(binary__construct__string__expected_data)
{
    const data_chunk data{ { 0xba, 0xad, 0xf0, 0x0d } };
    const binary prefix("10111010101011011111000000001101");
    BOOST_REQUIRE(prefix.data() == data);
}

BOOST_AUTO_TEST_CASE(binary__construct__empty__empty)
{
    const data_chunk bytes;
    const binary prefix(0, bytes);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.bits(), 0u);
    BOOST_REQUIRE_EQUAL(prefix.data().size(), 0u);
    BOOST_REQUIRE(stream.str().empty());
}

BOOST_AUTO_TEST_CASE(binary__construct__zero_bits__expected)
{
    const data_chunk blocks{ { 0x00, 0x00, 0x00, 0x00 } };
    const binary prefix(0, blocks);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.bits(), 0u);
    BOOST_REQUIRE_EQUAL(prefix.data().size(), 0u);
    BOOST_REQUIRE(stream.str().empty());
}

BOOST_AUTO_TEST_CASE(binary__construct__one_bit__expected)
{
    data_chunk bytes{ { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } };
    auto prefix = binary(1, bytes);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.bits(), 1u);
    BOOST_REQUIRE_EQUAL(prefix.data().size(), 1u);
    BOOST_REQUIRE_EQUAL(stream.str(), "1");
}

BOOST_AUTO_TEST_CASE(binary__construct__two_same_bits__expected)
{
    const data_chunk bytes{ { 0x01, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42 } };
    const auto prefix = binary(2, bytes);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.bits(), 2u);
    BOOST_REQUIRE_EQUAL(prefix.data().size(), 1u);
    BOOST_REQUIRE_EQUAL(stream.str(), "00");
}

BOOST_AUTO_TEST_CASE(binary__construct__two_different_bits__expected)
{
    const data_chunk blocks{ { 0x42, 0x42, 0x42, 0x01 } };
    const binary prefix(2, blocks);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.bits(), 2u);
    BOOST_REQUIRE_EQUAL(prefix.data().size(), 1u);
    BOOST_REQUIRE_EQUAL(stream.str(), "01");
}

BOOST_AUTO_TEST_CASE(binary__construct__16_zero_bits__expected)
{
    const data_chunk blocks{ { 0x00, 0x00 } };
    const binary prefix(16, blocks);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.bits(), 16u);
    BOOST_REQUIRE_EQUAL(prefix.data().size(), 2u);
    BOOST_REQUIRE_EQUAL(stream.str(), "0000000000000000");
}

BOOST_AUTO_TEST_CASE(binary__construct__16_non_zero_bits__expected)
{
    const data_chunk bytes{ { 0xFF, 0x00 } };
    const auto prefix = binary(16, bytes);
    std::stringstream stream;
    stream << prefix;
    BOOST_REQUIRE_EQUAL(prefix.bits(), 16u);
    BOOST_REQUIRE_EQUAL(prefix.data().size(), 2u);
    BOOST_REQUIRE_EQUAL(stream.str(), "1111111100000000");
}

BOOST_AUTO_TEST_SUITE_END()
