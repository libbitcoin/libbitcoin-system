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

using namespace bc;
using namespace bc::system;
using namespace bc::system::message;

BOOST_AUTO_TEST_SUITE(heading_tests)

BOOST_AUTO_TEST_CASE(heading__constructor_1__always__initialized_invalid)
{
    heading instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
}

BOOST_AUTO_TEST_CASE(heading__constructor_2__always__equals_params)
{
    uint32_t magic = 123u;
    const std::string command = "foo";
    uint32_t payload_size = 3454u;
    uint32_t checksum = 35746u;
    heading instance(magic, command, payload_size, checksum);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(magic, instance.magic());
    BOOST_REQUIRE_EQUAL(command, instance.command());
    BOOST_REQUIRE_EQUAL(payload_size, instance.payload_size());
    BOOST_REQUIRE_EQUAL(checksum, instance.checksum());
}

BOOST_AUTO_TEST_CASE(heading__constructor_3__always__equals_params)
{
    uint32_t magic = 123u;
    const std::string command = "foo";
    uint32_t payload_size = 3454u;
    uint32_t checksum = 35746u;
    heading instance(magic, "foo", payload_size, checksum);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(magic, instance.magic());
    BOOST_REQUIRE_EQUAL(command, instance.command());
    BOOST_REQUIRE_EQUAL(payload_size, instance.payload_size());
    BOOST_REQUIRE_EQUAL(checksum, instance.checksum());
}

BOOST_AUTO_TEST_CASE(heading__constructor_4__always__equals_params)
{
    heading expected(453u, "bar", 436u, 5743u);
    heading instance(expected);
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE(expected == instance);
}

BOOST_AUTO_TEST_CASE(heading__constructor_5__always__equals_params)
{
    uint32_t magic = 123u;
    const std::string command = "foo";
    uint32_t payload_size = 3454u;
    uint32_t checksum = 35746u;
    heading value(magic, command, payload_size, checksum);
    heading instance(std::move(value));
    BOOST_REQUIRE(instance.is_valid());
    BOOST_REQUIRE_EQUAL(magic, instance.magic());
    BOOST_REQUIRE_EQUAL(command, instance.command());
    BOOST_REQUIRE_EQUAL(payload_size, instance.payload_size());
    BOOST_REQUIRE_EQUAL(checksum, instance.checksum());
}

BOOST_AUTO_TEST_CASE(heading__to_data__checksum_variations__success)
{
    heading instance
    {
        32414u,
        "foo",
        56731u,
        0u
    };

    const auto zero_checksum = instance.to_data();
    BOOST_REQUIRE_EQUAL(zero_checksum.size(), heading::satoshi_fixed_size());

    instance.set_checksum(123u);
    const auto nonzero_checksum = instance.to_data();
    BOOST_REQUIRE_EQUAL(nonzero_checksum.size(), heading::satoshi_fixed_size());
}

BOOST_AUTO_TEST_CASE(heading__from_data__insufficient_bytes__failure)
{
    static const data_chunk raw
    {
        0xab, 0xcd
    };

    heading instance;
    BOOST_REQUIRE(!instance.from_data(raw));
}

BOOST_AUTO_TEST_CASE(heading__factory_1__valid_input__success)
{
    static const heading expected
    {
        32414u,
        "foo",
        56731u,
        0u
    };

    const auto data = expected.to_data();
    const auto result = heading::factory(data);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), heading::satoshi_fixed_size());
}

BOOST_AUTO_TEST_CASE(heading__factory_2__valid_input__success)
{
    static const heading expected
    {
        29145u,
        "bar",
        79531u,
        0u
    };

    const auto data = expected.to_data();
    data_source istream(data);
    const auto result = heading::factory(istream);
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
    BOOST_REQUIRE_EQUAL(data.size(), heading::satoshi_fixed_size());
}

BOOST_AUTO_TEST_CASE(heading__factory_3__valid_input__success)
{
    static const heading expected
    {
        1u,
        "bazbazbazbaz",
        2u,
        0u
    };

    const auto data = expected.to_data();
    data_source istream(data);
    istream_reader source(istream);
    const auto result = heading::factory(source);
    BOOST_REQUIRE_EQUAL(data.size(), heading::satoshi_fixed_size());
    BOOST_REQUIRE(result.is_valid());
    BOOST_REQUIRE(expected == result);
}

BOOST_AUTO_TEST_CASE(heading__magic_accessor__always__returns_initialized_value)
{
    uint32_t expected = 3574u;
    message::heading instance(expected, "baz", 4356u, 7923u);
    BOOST_REQUIRE_EQUAL(expected, instance.magic());
}

BOOST_AUTO_TEST_CASE(heading__magic_setter__roundtrip__success)
{
    const uint32_t expected = 3574u;
    message::heading instance;
    BOOST_REQUIRE_EQUAL(0, instance.magic());
    instance.set_magic(expected);
    BOOST_REQUIRE_EQUAL(expected, instance.magic());
}

BOOST_AUTO_TEST_CASE(heading__command_accessor_1__always__returns_initialized_value)
{
    const std::string expected = "asdge";
    message::heading instance(545u, expected, 4356u, 7923u);
    BOOST_REQUIRE_EQUAL(expected, instance.command());
}

BOOST_AUTO_TEST_CASE(heading__command_accessor_2__always__returns_initialized_value)
{
    const std::string expected = "asdge";
    const message::heading instance(545u, expected, 4356u, 7923u);
    BOOST_REQUIRE_EQUAL(expected, instance.command());
}

BOOST_AUTO_TEST_CASE(heading__command_setter_1__roundtrip__success)
{
    const std::string expected = "gdasd";
    message::heading instance;
    BOOST_REQUIRE(expected != instance.command());
    instance.set_command(expected);
    BOOST_REQUIRE(expected == instance.command());
}

BOOST_AUTO_TEST_CASE(heading__command_setter_2__roundtrip__success)
{
    std::string expected = "eytyry";
    message::heading instance;
    BOOST_REQUIRE_EQUAL(false, expected == instance.command());
    instance.set_command("eytyry");
    BOOST_REQUIRE_EQUAL(expected, instance.command());
}

BOOST_AUTO_TEST_CASE(heading__payload_size_accessor__always__returns_initialized_value)
{
    const uint32_t expected = 4356u;
    message::heading instance(3574u, "baz", expected, 7923u);
    BOOST_REQUIRE_EQUAL(expected, instance.payload_size());
}

BOOST_AUTO_TEST_CASE(heading__payload_size_setter__roundtrip__success)
{
    const uint32_t expected = 3574u;
    message::heading instance;
    BOOST_REQUIRE_EQUAL(0, instance.payload_size());
    instance.set_payload_size(expected);
    BOOST_REQUIRE_EQUAL(expected, instance.payload_size());
}

BOOST_AUTO_TEST_CASE(heading__checksum_accessor__always__returns_initialized_value)
{
    uint32_t expected = 7923u;
    message::heading instance(3574u, "baz", 4356u, expected);
    BOOST_REQUIRE_EQUAL(expected, instance.checksum());
}

BOOST_AUTO_TEST_CASE(heading__checksum_setter__roundtrip__success)
{
    const uint32_t expected = 3574u;
    message::heading instance;
    BOOST_REQUIRE_EQUAL(0, instance.checksum());
    instance.set_checksum(expected);
    BOOST_REQUIRE_EQUAL(expected, instance.checksum());
}

BOOST_AUTO_TEST_CASE(heading__operator_assign_equals__always__matches_equivalent)
{
    message::heading value(1u, "foobar", 2u, 3u);
    BOOST_REQUIRE(value.is_valid());
    message::heading instance;
    BOOST_REQUIRE_EQUAL(false, instance.is_valid());
    instance = std::move(value);
    BOOST_REQUIRE(instance.is_valid());
}

BOOST_AUTO_TEST_CASE(heading__operator_boolean_equals__duplicates__returns_true)
{
    const message::heading expected(1u, "foobar", 2u, 3u);
    message::heading instance(expected);
    BOOST_REQUIRE(instance == expected);
}

BOOST_AUTO_TEST_CASE(heading__operator_boolean_equals__differs__returns_false)
{
    const message::heading expected(1u, "foobar", 2u, 3u);
    message::heading instance;
    BOOST_REQUIRE_EQUAL(false, instance == expected);
}

BOOST_AUTO_TEST_CASE(heading__operator_boolean_not_equals__duplicates__returns_false)
{
    const message::heading expected(1u, "foobar", 2u, 3u);
    message::heading instance(expected);
    BOOST_REQUIRE_EQUAL(false, instance != expected);
}

BOOST_AUTO_TEST_CASE(heading__operator_boolean_not_equals__differs__returns_true)
{
    const message::heading expected(1u, "foobar", 2u, 3u);
    message::heading instance;
    BOOST_REQUIRE(instance != expected);
}

BOOST_AUTO_TEST_CASE(heading__type__all_cases__match_expected)
{
    message::heading instance;
    BOOST_REQUIRE(message::message_type::unknown == instance.type());
    instance.set_command(message::address::command);
    BOOST_REQUIRE(message::message_type::address == instance.type());
    instance.set_command(message::alert::command);
    BOOST_REQUIRE(message::message_type::alert == instance.type());
    instance.set_command(message::block::command);
    BOOST_REQUIRE(message::message_type::block == instance.type());
    instance.set_command(message::block_transactions::command);
    BOOST_REQUIRE(message::message_type::block_transactions == instance.type());
    instance.set_command(message::compact_block::command);
    BOOST_REQUIRE(message::message_type::compact_block == instance.type());
    instance.set_command(message::filter_add::command);
    BOOST_REQUIRE(message::message_type::filter_add == instance.type());
    instance.set_command(message::filter_clear::command);
    BOOST_REQUIRE(message::message_type::filter_clear == instance.type());
    instance.set_command(message::filter_load::command);
    BOOST_REQUIRE(message::message_type::filter_load == instance.type());
    instance.set_command(message::get_address::command);
    BOOST_REQUIRE(message::message_type::get_address == instance.type());
    instance.set_command(message::get_block_transactions::command);
    BOOST_REQUIRE(message::message_type::get_block_transactions == instance.type());
    instance.set_command(message::get_blocks::command);
    BOOST_REQUIRE(message::message_type::get_blocks == instance.type());
    instance.set_command(message::get_data::command);
    BOOST_REQUIRE(message::message_type::get_data == instance.type());
    instance.set_command(message::get_headers::command);
    BOOST_REQUIRE(message::message_type::get_headers == instance.type());
    instance.set_command(message::headers::command);
    BOOST_REQUIRE(message::message_type::headers == instance.type());
    instance.set_command(message::inventory::command);
    BOOST_REQUIRE(message::message_type::inventory == instance.type());
    instance.set_command(message::memory_pool::command);
    BOOST_REQUIRE(message::message_type::memory_pool == instance.type());
    instance.set_command(message::merkle_block::command);
    BOOST_REQUIRE(message::message_type::merkle_block == instance.type());
    instance.set_command(message::not_found::command);
    BOOST_REQUIRE(message::message_type::not_found == instance.type());
    instance.set_command(message::ping::command);
    BOOST_REQUIRE(message::message_type::ping == instance.type());
    instance.set_command(message::pong::command);
    BOOST_REQUIRE(message::message_type::pong == instance.type());
    instance.set_command(message::reject::command);
    BOOST_REQUIRE(message::message_type::reject == instance.type());
    instance.set_command(message::send_compact::command);
    BOOST_REQUIRE(message::message_type::send_compact == instance.type());
    instance.set_command(message::send_headers::command);
    BOOST_REQUIRE(message::message_type::send_headers == instance.type());
    instance.set_command(message::transaction::command);
    BOOST_REQUIRE(message::message_type::transaction == instance.type());
    instance.set_command(message::verack::command);
    BOOST_REQUIRE(message::message_type::verack == instance.type());
    instance.set_command(message::version::command);
    BOOST_REQUIRE(message::message_type::version == instance.type());
}

BOOST_AUTO_TEST_CASE(heading__maximum_size__always__matches_satoshi_fixed_size)
{
    BOOST_REQUIRE_EQUAL(heading::satoshi_fixed_size(), heading::maximum_size());
}

BOOST_AUTO_TEST_CASE(heading__maximum_payload_size__non_witness__matches_expected)
{
    static const size_t expected = 3u + (sizeof(uint32_t) + hash_size) * 50000u;
    BOOST_REQUIRE_EQUAL(expected, heading::maximum_payload_size(0u, false));
}

BOOST_AUTO_TEST_CASE(heading__maximum_payload_size__witness__matches_expected)
{
    BOOST_REQUIRE_EQUAL(max_block_weight, heading::maximum_payload_size(0u, true));
}

BOOST_AUTO_TEST_SUITE_END()
