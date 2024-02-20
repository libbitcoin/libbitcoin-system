/**
 * Copyright (c) 2011-2023 libbitcoin developers (see AUTHORS)
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

BOOST_AUTO_TEST_SUITE(checkpoint_tests)

namespace json = boost::json;
using namespace system::chain;

BOOST_AUTO_TEST_CASE(checkpoint__constructor_default__always__null_hash_zero)
{
    const checkpoint instance;
    BOOST_REQUIRE_EQUAL(instance.hash(), null_hash);
    BOOST_REQUIRE_EQUAL(instance.height(), zero);
}

BOOST_AUTO_TEST_CASE(checkpoint__constructor_copy__always__expected)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2(instance1);
    BOOST_REQUIRE_EQUAL(instance1, instance2);
    BOOST_REQUIRE_EQUAL(instance1.hash(), instance2.hash());
    BOOST_REQUIRE_EQUAL(instance1.height(), instance2.height());
}

BOOST_AUTO_TEST_CASE(checkpoint__constructor_move__always__expected)
{
    checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2(instance1);
    const checkpoint instance3(std::move(instance1));
    BOOST_REQUIRE_EQUAL(instance2.hash(), instance3.hash());
    BOOST_REQUIRE_EQUAL(instance2.height(), instance3.height());
}

BOOST_AUTO_TEST_CASE(checkpoint__constructor_text__valid__expected)
{
    const auto height = 42u;
    const char text[] = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const checkpoint instance(text, height);
    BOOST_REQUIRE_EQUAL(instance.hash(), base16_hash(text));
    BOOST_REQUIRE_EQUAL(instance.height(), height);
}

BOOST_AUTO_TEST_CASE(checkpoint__constructor_text__invalid__expected)
{
    const auto height = 42u;
    const char text[] = "zzzz030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const checkpoint instance(text, height);
    BOOST_REQUIRE_EQUAL(instance.hash(), null_hash);
    BOOST_REQUIRE_EQUAL(instance.height(), zero);
}

BOOST_AUTO_TEST_CASE(checkpoint__constructor_string__valid__expected)
{
    const auto height = 42u;
    const char text[] = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const std::string value{ text };
    const checkpoint instance(value, height);
    BOOST_REQUIRE_EQUAL(instance.hash(), base16_hash(text));
    BOOST_REQUIRE_EQUAL(instance.height(), height);
}

BOOST_AUTO_TEST_CASE(checkpoint__constructor_string__invalid__expected)
{
    const auto height = 42u;
    const std::string value{ "dfjgsdrhugidsnfvinsdfvi" };
    const checkpoint instance(value, height);
    BOOST_REQUIRE_EQUAL(instance.hash(), null_hash);
    BOOST_REQUIRE_EQUAL(instance.height(), zero);
}

BOOST_AUTO_TEST_CASE(checkpoint__constructor_copy_hash__always__expected)
{
    const auto height = 42u;
    const char text[] = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const auto hash = base16_hash(text);
    const checkpoint instance(hash, height);
    BOOST_REQUIRE_EQUAL(instance.hash(), hash);
    BOOST_REQUIRE_EQUAL(instance.height(), height);
}

BOOST_AUTO_TEST_CASE(checkpoint__constructor_move_hash__always__expected)
{
    const auto height = 42u;
    const char text[] = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const checkpoint instance(base16_hash(text), height);
    BOOST_REQUIRE_EQUAL(instance.hash(), base16_hash(text));
    BOOST_REQUIRE_EQUAL(instance.height(), height);
}

BOOST_AUTO_TEST_CASE(checkpoint__assign_copy__always__expected)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2 = instance1;
    BOOST_REQUIRE_EQUAL(instance1, instance2);
    BOOST_REQUIRE_EQUAL(instance1.hash(), instance2.hash());
    BOOST_REQUIRE_EQUAL(instance1.height(), instance2.height());
}

BOOST_AUTO_TEST_CASE(checkpoint__assign_move__always__expected)
{
    checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2(instance1);
    const checkpoint instance3 = std::move(instance1);
    BOOST_REQUIRE_EQUAL(instance2.hash(), instance3.hash());
    BOOST_REQUIRE_EQUAL(instance2.height(), instance3.height());
}

BOOST_AUTO_TEST_CASE(checkpoint__lesser__same__false)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BOOST_REQUIRE(!(instance1 < instance2));
}

BOOST_AUTO_TEST_CASE(checkpoint__lesser__lesser_hash_same_height__false)
{
    const checkpoint instance1(null_hash, 42);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BOOST_REQUIRE(!(instance1 < instance2));
}

BOOST_AUTO_TEST_CASE(checkpoint__lesser__same_hash_lesser_height__true)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 41);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BOOST_REQUIRE(instance1 < instance2);
}

BOOST_AUTO_TEST_CASE(checkpoint__lesser__same_hash_greater_height__false)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 43);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BOOST_REQUIRE(!(instance1 < instance2));
}

BOOST_AUTO_TEST_CASE(checkpoint__equality__same__expected)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BOOST_REQUIRE(instance1 == instance2);
    BOOST_REQUIRE(!(instance1 != instance2));
}

BOOST_AUTO_TEST_CASE(checkpoint__equality__different__expected)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2;
    BOOST_REQUIRE(instance1 != instance2);
    BOOST_REQUIRE(!(instance1 == instance2));
}

BOOST_AUTO_TEST_CASE(checkpoint__is_at__empty__false)
{
    const checkpoints points{};
    BOOST_REQUIRE(!checkpoint::is_at(points, 0));
    BOOST_REQUIRE(!checkpoint::is_at(points, 42));
}

BOOST_AUTO_TEST_CASE(checkpoint__is_at__non_empty__expected)
{
    const checkpoints points
    {
        { "0000000000000000000000000000000000000000000000000000000000000000", 17 },
        { "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 },
        { "1111111111111111111111111111111111111111111111111111111111111111", 1000 }
    };

    BOOST_REQUIRE(!checkpoint::is_at(points, 0));
    BOOST_REQUIRE(!checkpoint::is_at(points, 1));
    BOOST_REQUIRE(checkpoint::is_at(points, 17));
    BOOST_REQUIRE(checkpoint::is_at(points, 42));
    BOOST_REQUIRE(checkpoint::is_at(points, 1000));
    BOOST_REQUIRE(!checkpoint::is_at(points, 1001));
}

BOOST_AUTO_TEST_CASE(checkpoint__is_under__empty__false)
{
    const checkpoints points{};
    BOOST_REQUIRE(!checkpoint::is_under(points, 0));
    BOOST_REQUIRE(!checkpoint::is_under(points, 42));
}

BOOST_AUTO_TEST_CASE(checkpoint__is_under__non_empty__expected)
{
    const checkpoints points
    {
        { "0000000000000000000000000000000000000000000000000000000000000000", 17 },
        { "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 },
        { "1111111111111111111111111111111111111111111111111111111111111111", 1000 }
    };

    BOOST_REQUIRE(checkpoint::is_under(points, 0));
    BOOST_REQUIRE(checkpoint::is_under(points, 1));
    BOOST_REQUIRE(checkpoint::is_under(points, 17));
    BOOST_REQUIRE(checkpoint::is_under(points, 42));
    BOOST_REQUIRE(checkpoint::is_under(points, 1000));
    BOOST_REQUIRE(!checkpoint::is_under(points, 1001));
}

BOOST_AUTO_TEST_CASE(checkpoint__is_conflict__empty__false)
{
    const checkpoints points{};
    const checkpoint point("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BOOST_REQUIRE(!checkpoint::is_conflict(points, point.hash(), 0));
    BOOST_REQUIRE(!checkpoint::is_conflict(points, point.hash(), point.height()));
}

BOOST_AUTO_TEST_CASE(checkpoint__is_conflict__non_empty__expected)
{
    const checkpoints points
    {
        { "0000000000000000000000000000000000000000000000000000000000000000", 17 },
        { "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 },
        { "1111111111111111111111111111111111111111111111111111111111111111", 1000 }
    };
    const auto hash = base16_hash("0102030405060708090a0102030405060708090a0102030405060708090a0b0c");

    BOOST_REQUIRE(!checkpoint::is_conflict(points, hash, 0));
    BOOST_REQUIRE(checkpoint::is_conflict(points, hash, 17));
    BOOST_REQUIRE(!checkpoint::is_conflict(points, hash, 42));
    BOOST_REQUIRE(checkpoint::is_conflict(points, hash, 1000));
    BOOST_REQUIRE(!checkpoint::is_conflict(points, hash, 1001));
}

// json
// ----------------------------------------------------------------------------

BOOST_AUTO_TEST_CASE(checkpoint__json__conversions__expected)
{
    const std::string text
    {
        "{"
            "\"hash\":\"0000000000000000000000000000000000000000000000000000000000000001\","
            "\"height\":42"
        "}"
    };

    const chain::checkpoint instance
    {
        one_hash,
        42
    };

    const auto value = json::value_from(instance);

    BOOST_REQUIRE(json::parse(text) == value);
    BOOST_REQUIRE_EQUAL(json::serialize(value), text);

    BOOST_REQUIRE(json::value_from(instance) == value);
    BOOST_REQUIRE(json::value_to<chain::checkpoint>(value) == instance);
}

BOOST_AUTO_TEST_SUITE_END()
