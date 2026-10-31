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

BC_AUTO_TEST_SUITE(checkpoint_tests)

namespace json = system::json;
using namespace system::chain;

BC_AUTO_TEST_CASE(checkpoint__constructor_default__always__null_hash_zero)
{
    const checkpoint instance;
    BC_REQUIRE_EQUAL(instance.hash(), null_hash);
    BC_REQUIRE_EQUAL(instance.height(), zero);
}

BC_AUTO_TEST_CASE(checkpoint__constructor_copy__always__expected)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2(instance1);
    BC_REQUIRE_EQUAL(instance1, instance2);
    BC_REQUIRE_EQUAL(instance1.hash(), instance2.hash());
    BC_REQUIRE_EQUAL(instance1.height(), instance2.height());
}

BC_AUTO_TEST_CASE(checkpoint__constructor_move__always__expected)
{
    checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2(instance1);
    const checkpoint instance3(std::move(instance1));
    BC_REQUIRE_EQUAL(instance2.hash(), instance3.hash());
    BC_REQUIRE_EQUAL(instance2.height(), instance3.height());
}

BC_AUTO_TEST_CASE(checkpoint__constructor_text__valid__expected)
{
    const auto height = 42u;
    const char text[] = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const checkpoint instance(text, height);
    BC_REQUIRE_EQUAL(instance.hash(), base16_hash(text));
    BC_REQUIRE_EQUAL(instance.height(), height);
}

BC_AUTO_TEST_CASE(checkpoint__constructor_text__invalid__expected)
{
    const auto height = 42u;
    const char text[] = "zzzz030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const checkpoint instance(text, height);
    BC_REQUIRE_EQUAL(instance.hash(), null_hash);
    BC_REQUIRE_EQUAL(instance.height(), zero);
}

BC_AUTO_TEST_CASE(checkpoint__constructor_string__valid__expected)
{
    const auto height = 42u;
    const char text[] = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const std::string value{ text };
    const checkpoint instance(value, height);
    BC_REQUIRE_EQUAL(instance.hash(), base16_hash(text));
    BC_REQUIRE_EQUAL(instance.height(), height);
}

BC_AUTO_TEST_CASE(checkpoint__constructor_string__invalid__expected)
{
    const auto height = 42u;
    const std::string value{ "dfjgsdrhugidsnfvinsdfvi" };
    const checkpoint instance(value, height);
    BC_REQUIRE_EQUAL(instance.hash(), null_hash);
    BC_REQUIRE_EQUAL(instance.height(), zero);
}

BC_AUTO_TEST_CASE(checkpoint__constructor_copy_hash__always__expected)
{
    const auto height = 42u;
    const char text[] = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const auto hash = base16_hash(text);
    const checkpoint instance(hash, height);
    BC_REQUIRE_EQUAL(instance.hash(), hash);
    BC_REQUIRE_EQUAL(instance.height(), height);
}

BC_AUTO_TEST_CASE(checkpoint__constructor_move_hash__always__expected)
{
    const auto height = 42u;
    const char text[] = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c";
    const checkpoint instance(base16_hash(text), height);
    BC_REQUIRE_EQUAL(instance.hash(), base16_hash(text));
    BC_REQUIRE_EQUAL(instance.height(), height);
}

BC_AUTO_TEST_CASE(checkpoint__assign_copy__always__expected)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2 = instance1;
    BC_REQUIRE_EQUAL(instance1, instance2);
    BC_REQUIRE_EQUAL(instance1.hash(), instance2.hash());
    BC_REQUIRE_EQUAL(instance1.height(), instance2.height());
}

BC_AUTO_TEST_CASE(checkpoint__assign_move__always__expected)
{
    checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2(instance1);
    const checkpoint instance3 = std::move(instance1);
    BC_REQUIRE_EQUAL(instance2.hash(), instance3.hash());
    BC_REQUIRE_EQUAL(instance2.height(), instance3.height());
}

BC_AUTO_TEST_CASE(checkpoint__lesser__same__false)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BC_REQUIRE(!(instance1 < instance2));
    BC_REQUIRE(!std::less<system::chain::checkpoint>{}(instance1, instance2));
}

BC_AUTO_TEST_CASE(checkpoint__lesser__lesser_hash_same_height__false)
{
    const checkpoint instance1(null_hash, 42);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BC_REQUIRE(!(instance1 < instance2));
    BC_REQUIRE(!std::less<system::chain::checkpoint>{}(instance1, instance2));
}

BC_AUTO_TEST_CASE(checkpoint__lesser__same_hash_lesser_height__true)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 41);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BC_REQUIRE(instance1 < instance2);
    BC_REQUIRE(std::less<system::chain::checkpoint>{}(instance1, instance2));
}

BC_AUTO_TEST_CASE(checkpoint__lesser__same_hash_greater_height__false)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 43);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BC_REQUIRE(!(instance1 < instance2));
    BC_REQUIRE(!std::less<system::chain::checkpoint>{}(instance1, instance2));
}

BC_AUTO_TEST_CASE(checkpoint__equality__same__expected)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BC_REQUIRE(instance1 == instance2);
    BC_REQUIRE(!(instance1 != instance2));
}

BC_AUTO_TEST_CASE(checkpoint__equality__different__expected)
{
    const checkpoint instance1("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    const checkpoint instance2;
    BC_REQUIRE(instance1 != instance2);
    BC_REQUIRE(!(instance1 == instance2));
}

BC_AUTO_TEST_CASE(checkpoint__sort__non_empty__expected)
{
    checkpoints points
    {
        { "1111111111111111111111111111111111111111111111111111111111111111", 1000 },
        { "0000000000000000000000000000000000000000000000000000000000000000", 17 },
        { "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 }
    };

    const checkpoints expected
    {
        { "0000000000000000000000000000000000000000000000000000000000000000", 17 },
        { "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 },
        { "1111111111111111111111111111111111111111111111111111111111111111", 1000 }
    };

    BC_REQUIRE_EQUAL(system::sort_copy(points), expected);

    system::sort(points);
    BC_REQUIRE_EQUAL(points, expected);
}

BC_AUTO_TEST_CASE(checkpoint__is_at__empty__false)
{
    const checkpoints points{};
    BC_REQUIRE(!checkpoint::is_at(points, 0));
    BC_REQUIRE(!checkpoint::is_at(points, 42));
}

BC_AUTO_TEST_CASE(checkpoint__is_at__non_empty__expected)
{
    const checkpoints points
    {
        { "0000000000000000000000000000000000000000000000000000000000000000", 17 },
        { "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 },
        { "1111111111111111111111111111111111111111111111111111111111111111", 1000 }
    };

    BC_REQUIRE(!checkpoint::is_at(points, 0));
    BC_REQUIRE(!checkpoint::is_at(points, 1));
    BC_REQUIRE(checkpoint::is_at(points, 17));
    BC_REQUIRE(checkpoint::is_at(points, 42));
    BC_REQUIRE(checkpoint::is_at(points, 1000));
    BC_REQUIRE(!checkpoint::is_at(points, 1001));
}

BC_AUTO_TEST_CASE(checkpoint__is_under__empty__false)
{
    const checkpoints points{};
    BC_REQUIRE(!checkpoint::is_under(points, 0));
    BC_REQUIRE(!checkpoint::is_under(points, 42));
}

BC_AUTO_TEST_CASE(checkpoint__is_under__non_empty__expected)
{
    const checkpoints points
    {
        { "0000000000000000000000000000000000000000000000000000000000000000", 17 },
        { "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 },
        { "1111111111111111111111111111111111111111111111111111111111111111", 1000 }
    };

    BC_REQUIRE(checkpoint::is_under(points, 0));
    BC_REQUIRE(checkpoint::is_under(points, 1));
    BC_REQUIRE(checkpoint::is_under(points, 17));
    BC_REQUIRE(checkpoint::is_under(points, 42));
    BC_REQUIRE(checkpoint::is_under(points, 1000));
    BC_REQUIRE(!checkpoint::is_under(points, 1001));
}

BC_AUTO_TEST_CASE(checkpoint__is_conflict__empty__false)
{
    const checkpoints points{};
    const checkpoint point("0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42);
    BC_REQUIRE(!checkpoint::is_conflict(points, point.hash(), 0));
    BC_REQUIRE(!checkpoint::is_conflict(points, point.hash(), point.height()));
}

BC_AUTO_TEST_CASE(checkpoint__is_conflict__non_empty__expected)
{
    const checkpoints points
    {
        { "0000000000000000000000000000000000000000000000000000000000000000", 17 },
        { "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 },
        { "1111111111111111111111111111111111111111111111111111111111111111", 1000 }
    };
    const auto hash = base16_hash("0102030405060708090a0102030405060708090a0102030405060708090a0b0c");

    BC_REQUIRE(!checkpoint::is_conflict(points, hash, 0));
    BC_REQUIRE(checkpoint::is_conflict(points, hash, 17));
    BC_REQUIRE(!checkpoint::is_conflict(points, hash, 42));
    BC_REQUIRE(checkpoint::is_conflict(points, hash, 1000));
    BC_REQUIRE(!checkpoint::is_conflict(points, hash, 1001));
}

// is_valid

BC_AUTO_TEST_CASE(checkpoint__is_valid__default__false)
{
    BC_REQUIRE(!checkpoint{}.is_valid());
}

BC_AUTO_TEST_CASE(checkpoint__is_valid__string__true)
{
    const checkpoint instance{ "0102030405060708090a0102030405060708090a0102030405060708090a0b0c", 42 };
    BC_REQUIRE(instance.is_valid());
}

BC_AUTO_TEST_CASE(checkpoint__is_valid__invalid_string__false)
{
    const checkpoint instance{ "42", 42 };
    BC_REQUIRE(!instance.is_valid());
}

// serialization

BC_AUTO_TEST_CASE(checkpoint__to_string__populated__expected)
{
    const auto text = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c:42";
    const checkpoint instance{ base16_hash("0102030405060708090a0102030405060708090a0102030405060708090a0b0c"), 42 };
    BC_REQUIRE_EQUAL(instance.to_string(), text);
}

BC_AUTO_TEST_CASE(checkpoint__ostream__populated__expected)
{
    const auto text = "0102030405060708090a0102030405060708090a0102030405060708090a0b0c:42";
    const checkpoint instance{ base16_hash("0102030405060708090a0102030405060708090a0102030405060708090a0b0c"), 42 };
    std::ostringstream output{};
    output << instance;
    BC_REQUIRE_EQUAL(output.str(), text);
}

// deserialization

BC_AUTO_TEST_CASE(checkpoint__istream__valid__expected)
{
    checkpoint instance{};
    std::istringstream("0102030405060708090a0102030405060708090a0102030405060708090a0b0c:42") >> instance;
    BC_REQUIRE_EQUAL(instance.hash(), base16_hash("0102030405060708090a0102030405060708090a0102030405060708090a0b0c"));
    BC_REQUIRE_EQUAL(instance.height(), 42u);
}

BC_AUTO_TEST_CASE(checkpoint__istream__no_delimiter__throws_istream_exception)
{
    checkpoint instance{};
    BC_REQUIRE_THROW(std::istringstream("0102030405060708090a0102030405060708090a0102030405060708090a0b0c") >> instance, istream_exception);
}

BC_AUTO_TEST_CASE(checkpoint__istream__invalid_hash__throws_istream_exception)
{
    checkpoint instance{};
    BC_REQUIRE_THROW(std::istringstream("42:42") >> instance, istream_exception);
}

BC_AUTO_TEST_CASE(checkpoint__istream__invalid_height__throws_istream_exception)
{
    checkpoint instance{};
    BC_REQUIRE_THROW(std::istringstream("0102030405060708090a0102030405060708090a0102030405060708090a0b0c:xx") >> instance, istream_exception);
}

BC_AUTO_TEST_SUITE_END()
